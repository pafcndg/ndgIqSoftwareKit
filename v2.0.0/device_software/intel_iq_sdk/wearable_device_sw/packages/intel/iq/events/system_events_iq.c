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

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_encode.h"
#include "IntelSystemEvents.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

/* Boot */
#include "infra/boot.h"
#include "infra/time.h"

/* Power Management */
#include "infra/pm.h"

/* Soc Flash */
#include "drivers/soc_flash.h"
#include "project_mapping.h"
#include "panic_quark_se.h"

/* Flash storage */
#include "project_mapping.h"
#include "services/ll_storage_service/ll_storage_service.h"
#include "services/ll_storage_service/ll_storage_service_api.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_client.h"
#include "drivers/data_type.h"

#define SYS_EVENT_TOPIC_INDEX 0
#define BROADCAST_CON_ID 0xFF

#define PANIC_NVM_BASE (DEBUGPANIC_START_BLOCK * EMBEDDED_FLASH_BLOCK_SIZE)
/* Align address on 32bits (add 3 then clear LSBs) */
#define PANIC_ALIGN_32(x) (((uint32_t) (x) + 3) & ~(3))
#define DEFAULT_ADDRESS ~(0)

const uint8_t _sys_event_topic[] = SYSTEM_EVENT_UUID;

static uint8_t const * const _sys_event_topics[] = {
	[SYS_EVENT_TOPIC_INDEX] = _sys_event_topic,
	NULL
};

static int _app_id = 0xFF;
static uint32_t boot_timestamp = 0;
static bool device_connected = false;

static cir_storage_t *storage = NULL;
static cfw_client_t * ll_storage_client;
static cfw_service_conn_t *ll_storage_service_conn = NULL;

enum system_event_type {
	BOOT_EVENT,
	SHUTDOWN_EVENT,
	ARC_CRASH_EVENT,
	QUARK_CRASH_EVENT
};

struct system_event_storage {
	enum system_event_type type;
	uint32_t values[3];
	uint32_t timestamp;
};

static int send_system_event(struct system_event_storage event_to_publish)
{
	uint8_t event_buffer[intel_SystemEvent_size];
	size_t event_length;
	bool event_status;
	intel_SystemEvent eventToBeEncoded = intel_SystemEvent_init_default;
	/* Create a stream that will write to our buffer. */
	pb_ostream_t event_ostream = pb_ostream_from_buffer(event_buffer, sizeof(event_buffer));
	int err = -1;

	/* Fill in the system event data */

	/* timestamp of crash events does not correspond to datetime */
	if ((event_to_publish.type != ARC_CRASH_EVENT) &&
			(event_to_publish.type != QUARK_CRASH_EVENT)) {
		eventToBeEncoded.has_timestamp = true;
		eventToBeEncoded.timestamp.has_datetime = true;
		eventToBeEncoded.timestamp.datetime = event_to_publish.timestamp;
	}

	switch (event_to_publish.type) {
	case BOOT_EVENT:
		eventToBeEncoded.has_boot_event = true;
		enum boot_targets boot_event = (enum boot_targets)(event_to_publish.values[0]);

		if (boot_event == TARGET_MAIN)
			eventToBeEncoded.boot_event.type = intel_SystemEvent_bootType_MAIN;
		else if (boot_event == TARGET_RECOVERY)
			eventToBeEncoded.boot_event.type = intel_SystemEvent_bootType_OTA;
		else if (boot_event == TARGET_FACTORY)
			eventToBeEncoded.boot_event.type = intel_SystemEvent_bootType_FACTORY_RESET;
		else {
			/* Boot mode not supported */
			return 1;
		}
		break;
	case SHUTDOWN_EVENT:
		eventToBeEncoded.has_shutdown_event = true;

		switch ((enum shutdown_types)(event_to_publish.values[0])) {
		case SHUTDOWN:
			eventToBeEncoded.shutdown_event.shutdown_type =
					intel_SystemEvent_shutdownType_USER_SHUTDOWN;
			break;
		case BATTERY_SHUTDOWN:
			eventToBeEncoded.shutdown_event.shutdown_type =
					intel_SystemEvent_shutdownType_CRITICAL_BATTERY_SHUTDOWN;
			break;
		default:
			/* Shutdown mode not supported */
			return 1;
		}
		break;
	case ARC_CRASH_EVENT:
		eventToBeEncoded.has_crash_event = true;
		eventToBeEncoded.crash_event.has_arc_panic = true;
		eventToBeEncoded.crash_event.arc_panic.timestamp = event_to_publish.timestamp;
		eventToBeEncoded.crash_event.arc_panic.eret = event_to_publish.values[0];
		eventToBeEncoded.crash_event.arc_panic.ecr = event_to_publish.values[1];
		eventToBeEncoded.crash_event.arc_panic.efa = event_to_publish.values[2];
		break;
	case QUARK_CRASH_EVENT:
		eventToBeEncoded.has_crash_event = true;
		eventToBeEncoded.crash_event.has_quark_panic = true;
		eventToBeEncoded.crash_event.quark_panic.timestamp = event_to_publish.timestamp;
		eventToBeEncoded.crash_event.quark_panic.eip = event_to_publish.values[0];
		eventToBeEncoded.crash_event.quark_panic.type = event_to_publish.values[1];
		eventToBeEncoded.crash_event.quark_panic.error = event_to_publish.values[2];
		break;
	}

	/* encoding data */
	event_status = pb_encode(&event_ostream, intel_SystemEvent_fields, &eventToBeEncoded);
	event_length = event_ostream.bytes_written;
	if (!event_status)
		pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _sys_event_topic);
	else
		err = itm_publish(_app_id, BROADCAST_CON_ID, SYS_EVENT_TOPIC_INDEX, event_buffer, event_length);

	return err;
}

static bool complete_crash_event_to_store(struct system_event_storage* event_to_store,
                                 struct panic_data_flash_header* header,
                                 uint32_t panic_addr)
{
	unsigned int retlen;
	DRIVER_API_RC ret;
	bool event_completed = false;

	if (header->arch == ARC_CORE){
		struct arcv2_panic_arch_data* arc_info = balloc(sizeof(struct arcv2_panic_arch_data),NULL);
		ret = soc_flash_read(panic_addr+ header->struct_size - sizeof(struct arcv2_panic_arch_data),
				sizeof(struct arcv2_panic_arch_data)/sizeof(uint32_t),
				&retlen,
				(uint32_t *)arc_info);
		/* If read status is success, complete protobuf for arc panic */
		if (!ret){
			event_completed = true;
			event_to_store->type = ARC_CRASH_EVENT;
			event_to_store->values[0] = arc_info->eret;
			event_to_store->values[1] = arc_info->ecr;
			event_to_store->values[2] = arc_info->efa;
			event_to_store->timestamp = header->time;
		}
		bfree(arc_info);
	} else {
		struct x86_panic_arch_data* qrk_info = balloc(sizeof(struct x86_panic_arch_data),NULL);
		ret = soc_flash_read(panic_addr+ header->struct_size - sizeof(struct x86_panic_arch_data),
				sizeof(struct x86_panic_arch_data)/sizeof(uint32_t),
				&retlen,
				(uint32_t *)qrk_info);
		/* If read status is success, complete protobuf for quark panic */
		if(!ret){
			event_completed = true;
			event_to_store->type = QUARK_CRASH_EVENT;
			event_to_store->values[0] = qrk_info->eip;
			event_to_store->values[1] = qrk_info->type;
			event_to_store->values[2] = qrk_info->error;
			event_to_store->timestamp = header->time;
		}
		bfree(qrk_info);
	}
	return event_completed;
}

static bool store_panics(uint32_t panic_addr)
{
	unsigned int retlen;
	DRIVER_API_RC ret;
	struct panic_data_flash_header* header = balloc(sizeof(struct panic_data_flash_header),NULL);
	bool panic_stored = false;

	/* Read flash to find panics */
	while ((soc_flash_read(panic_addr,
			sizeof(struct panic_data_flash_header)/sizeof(uint32_t),
			&retlen,
			(uint32_t *)header) == DRV_RC_OK) &&
			!panic_stored) {

		/* If no panic, stop to search panics in flash */
		if (header->magic != PANIC_DATA_MAGIC)
			break;

		/* Panic shall be sent if valid and not already pulled */
		if ((header->flags & PANIC_DATA_FLAG_FRAME_VALID) &&
				(header->flags & PANIC_DATA_FLAG_FRAME_BLE_AVAILABLE)){
			/* Update header to not store again panic data */
			header->flags = header->flags & ~PANIC_DATA_FLAG_FRAME_BLE_AVAILABLE;
			/* Erase not needed because only one bit is modified from 1 to 0 */
			ret = soc_flash_write(panic_addr,
					sizeof(struct panic_data_flash_header)/sizeof(uint32_t),
					&retlen,
					(uint32_t *)header);
			if (ret){
				pr_error(LOG_MODULE_IQ,"Header update failed");
				break;
			}

			/* Store panic */
			static struct system_event_storage event_to_store;
			if (complete_crash_event_to_store(&event_to_store, header, panic_addr)) {
				/* if crash event is completed, push it in spi flash */
				/* priv is equal to next panic address */
				ll_storage_service_push(ll_storage_service_conn,
							(uint8_t *)&event_to_store,
							sizeof(struct system_event_storage),
							storage,
							(void*) PANIC_ALIGN_32(panic_addr + header->struct_size));
				panic_stored = true;
			}
		}
		/* Here, panic magic found. Increment where ptr and start over */
		panic_addr = PANIC_ALIGN_32(panic_addr + header->struct_size);
	}

	bfree(header);
	return panic_stored;
}

static void store_boot_event (void)
{
	struct system_event_storage event_to_store =
		{BOOT_EVENT,
		{(uint32_t)(get_boot_target()),0,0},
		boot_timestamp};
	ll_storage_service_push(ll_storage_service_conn,
				(uint8_t *)&event_to_store,
				sizeof(struct system_event_storage),
				storage,
				NULL);
}

static void _on_itm_subscribed(uint8_t con_id, uint8_t topic_ix)
{
	UNUSED(con_id);
	UNUSED(topic_ix);

	device_connected = true;

	/* We publish all system events saved in SPI flash */
	ll_storage_service_peek(ll_storage_service_conn,
			       sizeof(struct system_event_storage),
			       0,
			       storage,
			       NULL);
}

static void shutdown_event(enum shutdown_types shutdown_type)
{
	struct system_event_storage shutdown_event =
		{SHUTDOWN_EVENT,
		{(uint32_t)shutdown_type,0,0},
		time()};
	if (device_connected) {
		if (send_system_event(shutdown_event) <= 0) {
			/* Store Shutdown Event in case of publish error */
			ll_storage_service_push(ll_storage_service_conn,
						(uint8_t *)&shutdown_event,
						sizeof(struct system_event_storage),
						storage,
						NULL);
		}
		/* Give the BLE stack some time to send the publish */
		local_task_sleep_ms(1000);
	} else {
		/* Store Shutdown Event */
		ll_storage_service_push(ll_storage_service_conn,
					(uint8_t *)&shutdown_event,
					sizeof(struct system_event_storage),
					storage,
					NULL);
	}
}

static void _on_itm_disconnect(uint8_t con_id)
{
	device_connected = false;
}

static void _on_itm_unsubscribe_req(uint8_t con_id, uint8_t topic_ix)
{
	device_connected = false;
}

static void systemevents_iq_handle_msg(struct cfw_message *msg, void *data)
{
	bool error = true;
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_LL_PUSH_RSP:
		if (((ll_storage_push_rsp_msg_t *)msg)->rsp_header.status == DRV_RC_OK) {
			error = false;
			/* If push contains priv, pushed data was a panic and
			 * priv is equal to next panic address. */
			if (CFW_MESSAGE_PRIV(msg)) {
				if (!store_panics((uint32_t)CFW_MESSAGE_PRIV(msg))) {
					/* Store Boot Event if no more panics to store */
					store_boot_event();
				}
			}
		}
		break;
	case MSG_ID_LL_POP_RSP:
		/* Not used */
		break;
	case MSG_ID_LL_CLEAR_RSP:
		error = (((ll_storage_clear_rsp_msg_t *)msg)->rsp_header.status != DRV_RC_OK);
		break;
	case MSG_ID_LL_PEEK_RSP: ;
		ll_storage_peek_rsp_msg_t *peek_resp = (ll_storage_peek_rsp_msg_t *)msg;
		if (device_connected) {
			if (peek_resp->rsp_header.status == DRV_RC_OK) {
				struct system_event_storage * event_to_publish =
					(struct system_event_storage *)peek_resp->buffer;

				if (send_system_event(*event_to_publish) > 0){
					/* Clear the event we just published */
					ll_storage_service_clear(ll_storage_service_conn,
								storage,
								sizeof(struct system_event_storage),
								NULL);
				}
			}
			/* If buffer is not empty, try to fetch another system event from the flash */
			if (peek_resp->rsp_header.status != DRV_RC_OUT_OF_MEM) {
				ll_storage_service_peek(ll_storage_service_conn,
							sizeof(struct system_event_storage),
							0,
							storage,
							NULL);
			}
		}

		error = (peek_resp->rsp_header.status != DRV_RC_OK);
		bfree(peek_resp->buffer);
		break;
	case MSG_ID_LL_CIR_STOR_INIT_RSP: ;
		ll_storage_cir_stor_init_rsp_msg_t *init_resp = (ll_storage_cir_stor_init_rsp_msg_t *)msg;
		if (init_resp->rsp_header.status == DRV_RC_OK) {
			storage = init_resp->storage;
			/* Store all panics saved in flash and not already stored */
			if (!store_panics(PANIC_NVM_BASE)) {
				/* Store Boot Event if no panics to store */
				store_boot_event();
			}

			static struct topic_application app_desc =
			{
				.topic_array    = _sys_event_topics,
				.on_subscribed  = _on_itm_subscribed,
				.on_disconnect = _on_itm_disconnect,
				.on_unsubscribe_req = _on_itm_unsubscribe_req
			};

			/* Interface with the Topic Manager */
			_app_id = itm_add_application(&app_desc);
			if (_app_id >= 0) {
				itm_start_application(_app_id);
				pm_set_shutdown_event_cb(shutdown_event);
			}
			init_done();
			error = false;
		}
		break;
	default: break;
	}
	if (error) {
		pr_debug(LOG_MODULE_IQ, "SYS_EVENT_IQ: Response error");
	}
	/* Free message once processed */
	message_free(&msg->m);
}

static void service_connection_cb(cfw_service_conn_t * handle, void * param)
{
	ll_storage_service_conn = handle;

	ll_storage_service_cir_stor_init(ll_storage_service_conn,
					 SPI_SYSTEM_EVENT_PARTITION_ID,
					 SPI_SYSTEM_EVENT_NB_BLOCKS*SERIAL_FLASH_BLOCK_SIZE,
					 SPI_SYSTEM_EVENT_START_BLOCK,
					 NULL);
}

void system_events_iq_init(T_QUEUE queue)
{
	boot_timestamp = time();

	/* Get a client handle */
	ll_storage_client = cfw_client_init(queue, systemevents_iq_handle_msg, NULL);

	/* Open the ll_storage service */
	cfw_open_service_helper(ll_storage_client,
				LL_STOR_SERVICE_ID,
				service_connection_cb,
				(void *)LL_STOR_SERVICE_ID);
}
