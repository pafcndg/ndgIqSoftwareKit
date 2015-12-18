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
#include "os/os_types.h"
#include "infra/log.h"

/* Services */
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_client.h"
#include "services/ui_service/ui_svc_api.h"

/* Sensing module */
#include "iq/sensing.h"

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_encode.h"
#include "IntelUserEvents.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

#define USER_EVENT_TOPIC_INDEX 0
#define BROADCAST_CON_ID 0xFF

const uint8_t _user_event_topic[] = USER_EVENT_UUID;

static uint8_t const * const _user_event_topics[] = {
	[USER_EVENT_TOPIC_INDEX] = _user_event_topic,
	NULL
};

static int _app_id = 0xFF;

/* CFW handlers */

/* Client handle */
static cfw_client_t * _client;

void user_events_iq_tapping_cb (int16_t tapping_cnt){
	uint8_t event_buffer[intel_UserEvent_size];
	size_t event_length;
	bool event_status;
	intel_UserEvent eventToBeEncoded = intel_UserEvent_init_default;
	/* Create a stream that will write to our buffer. */
	pb_ostream_t event_ostream =
			pb_ostream_from_buffer(event_buffer, sizeof(event_buffer));

	/* Fill in the notification data */
	eventToBeEncoded.has_tapping_event = true;
	if (tapping_cnt == 2)
		eventToBeEncoded.tapping_event.type = intel_UserEvent_tappingEventType_DOUBLE_TAP;
	else if (tapping_cnt == 3)
		eventToBeEncoded.tapping_event.type = intel_UserEvent_tappingEventType_TRIPLE_TAP;
	else {
		pr_error(LOG_MODULE_IQ, "Wrong Tapping Count");
		return;
	}

	/* encoding data */
	event_status = pb_encode(&event_ostream, intel_UserEvent_fields, &eventToBeEncoded);
	event_length = event_ostream.bytes_written;
	if (!event_status)
		pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _user_event_topic);
	else
		itm_publish(_app_id, BROADCAST_CON_ID, USER_EVENT_TOPIC_INDEX, event_buffer, event_length);
}

static void _on_ui_svc_connected(cfw_service_conn_t *c, void * param)
{
	UNUSED(param);
	UNUSED(c);
	static struct topic_application app_desc =
	{
		.topic_array = _user_event_topics,
	};
	/* Interface with the Topic Manager */
	_app_id = itm_add_application(&app_desc);
	if (_app_id >= 0) {
		itm_start_application(_app_id);
		sensing_set_tapping_cb(user_events_iq_tapping_cb);
	}
	init_done();
}

static void _on_cfw_message(struct cfw_message * msg, void * param)
{
	UNUSED(param);
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_UI_BTN_SINGLE_EVT:
	case MSG_ID_UI_BTN_DOUBLE_EVT:
	case MSG_ID_UI_BTN_MAX_EVT:{
		uint8_t event_buffer[intel_UserEvent_size];
		size_t event_length;
		bool event_status;
		intel_UserEvent eventToBeEncoded = intel_UserEvent_init_default;
		/* Create a stream that will write to our buffer. */
		pb_ostream_t event_ostream =
				pb_ostream_from_buffer(event_buffer, sizeof(event_buffer));

		/* Fill in the notification data */
		eventToBeEncoded.has_button_event = true;
		if (CFW_MESSAGE_ID(msg) == MSG_ID_UI_BTN_SINGLE_EVT){
			union ui_drv_evt *broadcast_evt = (union ui_drv_evt *)msg;
			if (!broadcast_evt->btn_evt.param)
				eventToBeEncoded.button_event.type =
						intel_UserEvent_buttonEventType_SINGLE_PRESS;
			else
				eventToBeEncoded.button_event.type =
						intel_UserEvent_buttonEventType_LONG_PRESS;
		}
		else if (CFW_MESSAGE_ID(msg) == MSG_ID_UI_BTN_DOUBLE_EVT)
			eventToBeEncoded.button_event.type =
					intel_UserEvent_buttonEventType_DOUBLE_PRESS;
		else
			eventToBeEncoded.button_event.type =
					intel_UserEvent_buttonEventType_LONG_PRESS;

		/* encoding data */
		event_status = pb_encode(&event_ostream, intel_UserEvent_fields, &eventToBeEncoded);
		event_length = event_ostream.bytes_written;
		if (!event_status)
			pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _user_event_topic);
		else
			itm_publish(_app_id, BROADCAST_CON_ID, USER_EVENT_TOPIC_INDEX, event_buffer, event_length);
		break;
	}
	default:
	{
		break;
	}
	}
	message_free(&msg->m);
}

void user_events_iq_init(T_QUEUE queue)
{
	/* Get a client handle */
	_client = cfw_client_init(queue, _on_cfw_message, NULL);

	int client_events[3] = {MSG_ID_UI_BTN_SINGLE_EVT,
			MSG_ID_UI_BTN_DOUBLE_EVT,
			MSG_ID_UI_BTN_MAX_EVT};
	cfw_open_service_helper_evt(_client, UI_SVC_SERVICE_ID, client_events,
			sizeof(client_events)/sizeof(client_events[0]),
			_on_ui_svc_connected, NULL);

	sensing_iq_init(queue);
}
