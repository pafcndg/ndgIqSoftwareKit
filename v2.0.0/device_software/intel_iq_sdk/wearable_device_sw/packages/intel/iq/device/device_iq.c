/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "infra/log.h"
#include "os/os_types.h"

/* Services */
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_client.h"
#include "services/ble_service/ble_service_api.h"

/* Time */
#include "infra/time.h"
#include "drivers/intel_qrk_rtc.h"

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq Initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_decode.h"
#include "pb_encode.h"
#include "IntelDevice.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

/* PUPDR */
#include "infra/pm.h"

#define DATETIME_TOPIC_INDEX 0
#define FACTORY_RESET_INDEX  1
#define NAME_TOPIC_INDEX     2

const uint8_t _datetime_topic[]      = DATETIME_UUID;
const uint8_t _factory_reset_topic[] = FACTORY_RESET_UUID;
const uint8_t _name_topic[]          = NAME_UUID;

static uint8_t const * const _device_topics[] = {
	[DATETIME_TOPIC_INDEX] = _datetime_topic,
	[FACTORY_RESET_INDEX]  = _factory_reset_topic,
	[NAME_TOPIC_INDEX]     = _name_topic,
	NULL
};

/* Client handle */
static cfw_client_t * _client;
/* BLE service handle */
static cfw_service_conn_t *_ble_service_conn;

static int _app_id = 0xFF;

/* New phone connection callback */
static void _on_itm_connect(uint8_t con_id)
{
	if(itm_subscribe_req(_app_id, con_id, _device_topics) < 0){
		pr_error(LOG_MODULE_IQ, "subscribe failed");
	}
}

static void _on_itm_publish(uint8_t con_id, const uint8_t * topic, const uint8_t * data, uint16_t len){
	UNUSED(con_id);
	bool decoding_success = true;
	/* Deserialize protobuf to get notification data */
	if(!strcmp(topic, _datetime_topic)) {
		intel_DateTime proto = intel_DateTime_init_default;
		pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data,len);
		if (pb_decode(&stream, intel_DateTime_fields, &proto)) {
			if (proto.has_datetime) {
				set_time(proto.datetime);
				/* Save epoch time on RTC to be able to restore absolute time after a reboot */
				struct qrk_cxxxx_rtc_config config = {0};
				config.initial_rtc_val = proto.datetime;
				qrk_cxxxx_rtc_set_config(&config);
			}
		} else
			decoding_success = false;
	} else if(!strcmp(topic, _factory_reset_topic)) {
		intel_FactoryReset proto = intel_FactoryReset_init_default;
		pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data,len);
		if (pb_decode(&stream, intel_FactoryReset_fields, &proto)) {
			if (proto.has_factory_reset) {
				if(proto.factory_reset) {
					/* Give the BLE stack some time to ack the request */
					local_task_sleep_ms(1000);
					reboot(TARGET_FACTORY);
				}
			}
		} else
			decoding_success = false;
	} else if(!strcmp(topic, _name_topic)) {
		intel_DeviceName proto = intel_DeviceName_init_default;
		pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data,len);
		if (pb_decode(&stream, intel_DeviceName_fields, &proto)) {
				if (proto.has_name) {
					ble_set_name(_ble_service_conn, (const uint8_t *) proto.name, NULL);
				}
		} else
			decoding_success = false;
	}

	if (!decoding_success)
		pr_error(LOG_MODULE_IQ, ERROR_MSG_DECODING_FAILURE_FOR_UUID, topic);
}

static void _on_itm_request(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len){
	UNUSED(topic_ix);
	UNUSED(data);
	UNUSED(len);
	uint8_t proto_buffer[intel_DateTime_size];
	intel_DateTime proto = intel_DateTime_init_default;
	size_t proto_length;
	bool proto_status;
	pb_ostream_t proto_ostream = pb_ostream_from_buffer(proto_buffer, sizeof(proto_buffer));
	proto.has_datetime = true;
	proto.datetime = time();
	proto_status = pb_encode(&proto_ostream, intel_DateTime_fields, &proto);
	proto_length = proto_ostream.bytes_written;

	/* Then just check for any errors.. */
	if (!proto_status)
	{
		pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _datetime_topic);
		itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
	}
	else {
		itm_response(con_id, req_id, TOPIC_STATUS_OK, proto_buffer, proto_length);
	}
}

static void _on_cfw_message(struct cfw_message * msg, void * param)
{
	UNUSED(param);
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_BLE_SET_NAME_RSP:
	{
		break;
	}
	default:
	{
		break;
	}
	}
	message_free(&msg->m);
}
void _on_ble_svc_connected(cfw_service_conn_t *c, void * param)
{
	UNUSED(param);

	/* Store BLE service handle */
	_ble_service_conn = c;

	/* Initialization of datetime_iq */
	static struct topic_application app_desc =
	{
		.topic_array = _device_topics,
		.on_connect  = _on_itm_connect,
		.on_publish  = _on_itm_publish,
		.on_request  = _on_itm_request
	};

	/* Interface with the Topic Manager */
	_app_id = itm_add_application(&app_desc);
	if (_app_id >= 0)
		itm_start_application(_app_id);
	init_done();
}

void device_iq_init(T_QUEUE queue)
{
	/* Initialization of absolute time using rtc if rtc not reseted at boot */
	if (qrk_cxxxx_rtc_read() > time())
		set_time(qrk_cxxxx_rtc_read());

	/* Get a client handle */
	_client = cfw_client_init(queue, _on_cfw_message, NULL);

	/* Open the BLE service */
	cfw_open_service_helper(_client,
			BLE_SERVICE_ID,
			_on_ble_svc_connected,
			NULL);
}
