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

#include "infra/log.h"
#include "os/os.h"

/* Sensing module */
#include "iq/sensing.h"

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_encode.h"
#include "pb_decode.h"
#include "IntelBodyIQ.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

/* Time */
#include "infra/time.h"

/* Flash storage */
#include "project_mapping.h"
#include "services/ll_storage_service/ll_storage_service.h"
#include "services/ll_storage_service/ll_storage_service_api.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_client.h"
#include "drivers/data_type.h"

#include "iq/body_iq.h"

#define BODY_TOPIC_INDEX  0
#define BODY_SETTINGS_TOPIC_INDEX  1
#define BROADCAST_CON_ID 0xFF

/* Activities */
#define BODY_IQ_NO_ACTIVITY            0
#define BODY_IQ_ACTIVITY_WALKING       1
#define BODY_IQ_ACTIVITY_RUNNING       2
#define BODY_IQ_ACTIVITY_BIKING        3
#define BODY_IQ_ACTIVITY_SLEEPING      4

#define BODY_IQ_NB_MAX_REGISTERED_ACTIVITIES 10

const uint8_t _body_topic[] = ACTIVITY_UUID;
const uint8_t _body_settings_topic[] = ACTIVITY_SETTINGS_UUID;

static uint8_t const * const _body_topics[] = {
	[BODY_TOPIC_INDEX] = _body_topic,
	[BODY_SETTINGS_TOPIC_INDEX] = _body_settings_topic,
	NULL
};

static int _app_id = 0xFF;

static const intel_Activity_activityType bodyIQ_type_proto_to_svc[] = {
	[(uint8_t)intel_Activity_activityType_WALKING] = BODY_IQ_ACTIVITY_WALKING,
	[(uint8_t)intel_Activity_activityType_RUNNING] = BODY_IQ_ACTIVITY_RUNNING,
	[(uint8_t)intel_Activity_activityType_BIKING] = BODY_IQ_ACTIVITY_BIKING,
};

static const intel_Activity_activityType svc_to_bodyIQ_type_proto[] = {
	[BODY_IQ_ACTIVITY_WALKING] = (uint8_t)intel_Activity_activityType_WALKING,
	[BODY_IQ_ACTIVITY_RUNNING] = (uint8_t)intel_Activity_activityType_RUNNING,
	[BODY_IQ_ACTIVITY_BIKING] = (uint8_t)intel_Activity_activityType_BIKING,
};

static uint32_t step_count = 0;
static cir_storage_t *storage = NULL;
static intel_Activity cur_activity;
static bool device_connected = false;
static bool classification_enabled = true;
static cfw_client_t * ll_storage_client;
static cfw_service_conn_t *ll_storage_service_conn = NULL;

/* Activity settings */
#define MIN_GRANULARITY 5  /* minimum granularity is set to 5 mn */
#define MAX_GRANULARITY 60 /* maximum granularity is set to 60 mn */
static T_TIMER body_timer;
static uint32_t last_duration_update_date;
static uint8_t body_iq_interval_reports_duration;
static bool timer_duration_has_to_be_change;

static int publish_activity(intel_Activity *activity);
static void body_iq_timer_callback(void *data);
static void body_iq_stepcounter_cb(uint32_t steps, uint32_t activity_type, uint32_t timestamp);

static void _on_itm_subscribed(uint8_t con_id, uint8_t topic_ix)
{
	device_connected = true;
	/* We publish all stored activities */
	ll_storage_service_peek(ll_storage_service_conn,
				sizeof(intel_Activity),
				0,
				storage,
				NULL);
}

static void _on_itm_disconnect(uint8_t con_id)
{
	device_connected = false;
}

static void _on_itm_unsubscribe_req(uint8_t con_id, uint8_t topic_ix)
{
	device_connected = false;
}

static void _on_itm_request(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len)
{
	if(topic_ix == BODY_TOPIC_INDEX) {
		if (!classification_enabled) {
			itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
			return;
		}
		if (cur_activity.activity_status ==
		    intel_Activity_activityStatus_ONGOING) {
			uint8_t proto_buffer[intel_Activity_size];
			intel_Activity proto = (intel_Activity)intel_Activity_init_default;
			proto = cur_activity;
			proto.timestamp_begin.datetime = uptime_to_epoch(proto.timestamp_begin.datetime);
			proto.timestamp_end.datetime = uptime_to_epoch(proto.timestamp_end.datetime);
			pb_ostream_t proto_ostream = pb_ostream_from_buffer(proto_buffer, sizeof(proto_buffer));
			bool proto_status = pb_encode(&proto_ostream, intel_Activity_fields, &proto);
			size_t proto_length = proto_ostream.bytes_written;
			if (proto_status) {
				itm_response(con_id, req_id, TOPIC_STATUS_OK,
					     proto_buffer, proto_length);
				return;
			}
		}
		itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
	} else if(topic_ix == BODY_SETTINGS_TOPIC_INDEX) {
		intel_ActivitySettings proto = (intel_ActivitySettings)intel_ActivitySettings_init_default;
		pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data,len);
		if (pb_decode(&stream, intel_ActivitySettings_fields, &proto)) {
			if(proto.has_activity_classification_enable_disable) {
				if (classification_enabled && !proto.activity_classification_enable_disable) {
					/* We stop timer periodic reports */
					timer_stop(body_timer, NULL);
					/* We stop collecting sensor activity data */
					sensing_set_stepcounter_cb(NULL);
				} else if (!classification_enabled &&
					   proto.activity_classification_enable_disable) {
					/* We start timer for periodic reports */
					body_timer = timer_create(body_iq_timer_callback,
							NULL,
							body_iq_interval_reports_duration * 60000,
							1,
							1,
							NULL);
					/* We start collecting sensor activity data */
					sensing_set_stepcounter_cb(body_iq_stepcounter_cb);
				}
				classification_enabled = proto.activity_classification_enable_disable;
			} else if (!classification_enabled) {
				/* If classification is not enabled, we drop change granularity requests */
				itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
				return;
			}
			if(proto.has_activity_report_granularity) {
				if(proto.activity_report_granularity >= MIN_GRANULARITY &&
				     proto.activity_report_granularity <= MAX_GRANULARITY) {
					uint32_t time_remaining_before_mutliple;

					pr_debug(LOG_MODULE_IQ, "receiving new interval duration:  %d s",
						 proto.activity_report_granularity);
					body_iq_interval_reports_duration =
						proto.activity_report_granularity;
					time_remaining_before_mutliple =
						body_iq_interval_reports_duration * 60000 -
						((get_uptime_ms() - last_duration_update_date) %
						 (body_iq_interval_reports_duration * 60000));
					timer_duration_has_to_be_change = true;

					if(time_remaining_before_mutliple) {
						timer_stop(body_timer, NULL);
						timer_start(body_timer,
							    time_remaining_before_mutliple,
							    NULL);
					} else {
						body_iq_timer_callback(NULL);
					}
				} else {
					itm_response(con_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
					return;
				}
			}
			else if (proto.has_activity_discard_stored_data) {
				pr_debug(LOG_MODULE_IQ, "receiving clear activity");
				/* Put itm informations in priv_data */
				void * priv_itm_info = (void *)(con_id | (req_id << 8));
				ll_storage_service_clear(ll_storage_service_conn, storage, 0, priv_itm_info);
			}
			itm_response(con_id, req_id, TOPIC_STATUS_OK, NULL, 0);
		}
	}
}

static int publish_activity(intel_Activity *activity)
{
	size_t body_length;
	bool body_status;
	uint8_t body_buffer[intel_Activity_size];
	int err = -1;

	/* Create a stream that will write to our buffer. */
	pb_ostream_t body_ostream =
		pb_ostream_from_buffer(body_buffer, sizeof(body_buffer));

	/* Encoding data */
	body_status = pb_encode(&body_ostream, intel_Activity_fields, activity);
	body_length = body_ostream.bytes_written;
	if (!body_status) {
		pr_error(LOG_MODULE_IQ, ERROR_MSG_ENCODING_FAILURE_FOR_UUID, _body_topic);
	} else {
		err = itm_publish(_app_id, BROADCAST_CON_ID, BODY_TOPIC_INDEX, body_buffer, body_length);
	}
	return err;
}

static void update_date_and_publish_or_store_current_activity()
{
	uint32_t timestamp_begin;
	uint32_t timestamp_end;
	int err = -1;

	timestamp_begin = cur_activity.timestamp_begin.datetime;
	timestamp_end = cur_activity.timestamp_end.datetime;
	/* Set timestamp to epoch date */
	cur_activity.timestamp_begin.datetime = uptime_to_epoch(cur_activity.timestamp_begin.datetime);
	cur_activity.timestamp_end.datetime = uptime_to_epoch(cur_activity.timestamp_end.datetime);

	/* If a device is connected, we publish the report */
	if (device_connected) {
		err = publish_activity(&cur_activity);
	}
	if ((cur_activity.activity_status != intel_Activity_activityStatus_STARTED) &&
	    (err <= 0)) {
		ll_storage_service_push(ll_storage_service_conn,
					(uint8_t *)&cur_activity,
					sizeof(intel_Activity),
					storage,
					NULL);
		if (device_connected) {
			/* We also trigger a peek to retry */
			ll_storage_service_peek(ll_storage_service_conn,
			                        sizeof(intel_Activity),
			                        0,
			                        storage,
			                        NULL);
		}
	}

	cur_activity.timestamp_begin.datetime = timestamp_begin;
	cur_activity.timestamp_end.datetime = timestamp_end;
}

static void body_iq_stepcounter_cb(uint32_t steps, uint32_t activity_type, uint32_t timestamp)
{
	static bool registered = true;     /* Is last activity registered */

	/* If last activity is not registered and current activity is different, last activity
	   is finished */
	if ((!registered) && (activity_type != bodyIQ_type_proto_to_svc[cur_activity.type])) {
		cur_activity.activity_status = intel_Activity_activityStatus_FINISHED;
		step_count = steps;
		/* Dispose of that activity */
		update_date_and_publish_or_store_current_activity();
		cur_activity = (intel_Activity)intel_Activity_init_default;
		registered = true;
	}

	/* If the last activity recorded has been registered, this event is the first of
	   the new activity */
	if (registered && ((activity_type == BODY_IQ_ACTIVITY_WALKING) ||
			  (activity_type == BODY_IQ_ACTIVITY_RUNNING) ||
			  (activity_type == BODY_IQ_ACTIVITY_BIKING))) {
		cur_activity.timestamp_begin.has_datetime = true;
		cur_activity.timestamp_begin.datetime = timestamp;
		cur_activity.timestamp_end.has_datetime = true;
		cur_activity.timestamp_end.datetime =
			cur_activity.timestamp_begin.datetime;
		cur_activity.has_activity_status = true;
		cur_activity.activity_status =
			intel_Activity_activityStatus_STARTED;
		cur_activity.type = svc_to_bodyIQ_type_proto[activity_type];
		/* Dispose of that activity */
		update_date_and_publish_or_store_current_activity();
		cur_activity.activity_status = intel_Activity_activityStatus_ONGOING;
		registered = false;
	}

	/* Fill in the notification data */
	switch (activity_type) {
		case BODY_IQ_ACTIVITY_WALKING:
			cur_activity.has_walking_activity = true;
			cur_activity.walking_activity.has_step_count = true;
			cur_activity.walking_activity.step_count = steps - step_count;
			cur_activity.timestamp_end.has_datetime = true;
			cur_activity.timestamp_end.datetime = timestamp;
		break;
		case BODY_IQ_ACTIVITY_RUNNING:
			cur_activity.has_running_activity = true;
			cur_activity.running_activity.has_step_count = true;
			cur_activity.running_activity.step_count = steps - step_count;
			cur_activity.timestamp_end.has_datetime = true;
			cur_activity.timestamp_end.datetime = timestamp;
		break;
		case BODY_IQ_ACTIVITY_BIKING:
			cur_activity.has_biking_activity = true;
			cur_activity.timestamp_end.has_datetime = true;
			cur_activity.timestamp_end.datetime = timestamp;
		break;
		case BODY_IQ_NO_ACTIVITY:
		case BODY_IQ_ACTIVITY_SLEEPING:
		default:
			/* Nothing to do */
		break;
	}
}

static void body_iq_timer_callback(void *data) {

	if (cur_activity.activity_status == intel_Activity_activityStatus_ONGOING) {
		/* Dispose of that activity */
		update_date_and_publish_or_store_current_activity();

		cur_activity.timestamp_begin.datetime = cur_activity.timestamp_end.datetime;
		if (cur_activity.has_walking_activity) {
			step_count += cur_activity.walking_activity.step_count;
			cur_activity.walking_activity.step_count = 0;
		} else if (cur_activity.has_running_activity) {
			step_count += cur_activity.running_activity.step_count;
			cur_activity.running_activity.step_count = 0;
		}
	}

	if(timer_duration_has_to_be_change) {
		timer_stop(body_timer, NULL);
		timer_start(body_timer, body_iq_interval_reports_duration * 60000, NULL);
		timer_duration_has_to_be_change = false;
	}
	last_duration_update_date = get_uptime_ms();
}

static void body_iq_handle_msg(struct cfw_message *msg, void *data)
{
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_LL_PUSH_RSP:
		break;
	case MSG_ID_LL_POP_RSP: ;
		/* Not used */
		break;
	case MSG_ID_LL_PEEK_RSP: ;
		ll_storage_peek_rsp_msg_t *peek_resp = (ll_storage_peek_rsp_msg_t *)msg;
		if (peek_resp->rsp_header.status == DRV_RC_OK) {
			if (publish_activity((intel_Activity *)peek_resp->buffer) > 0) {
				/* Clear the activity we just published */
				ll_storage_service_clear(ll_storage_service_conn,
				                         storage,
				                         sizeof(intel_Activity),
				                         NULL);
			}

		}
		if (device_connected && (peek_resp->rsp_header.status != DRV_RC_OUT_OF_MEM)) {
			/* Try to fetch another activity report from the flash */
			ll_storage_service_peek(ll_storage_service_conn,
			                        sizeof(intel_Activity),
			                        0,
			                        storage,
			                        NULL);
		}
		bfree(peek_resp->buffer);
		break;
	case MSG_ID_LL_CIR_STOR_INIT_RSP: ;
		ll_storage_cir_stor_init_rsp_msg_t *init_resp = (ll_storage_cir_stor_init_rsp_msg_t *)msg;
		if (init_resp->rsp_header.status == DRV_RC_OK) {
			storage = init_resp->storage;
		}
		break;
	case MSG_ID_LL_CLEAR_RSP:
		if (msg->priv != NULL) {
			ll_storage_clear_rsp_msg_t *clear_resp = (ll_storage_clear_rsp_msg_t *)msg;
			/* Retrieve itm information from msg priv_data */
			uint8_t conn_id = (uint32_t)(msg->priv);
			uint8_t req_id = ((uint32_t)(msg->priv)) >> 8;
			if (clear_resp->rsp_header.status == DRV_RC_OK)
				itm_response(conn_id, req_id, TOPIC_STATUS_OK, NULL, 0);
			else
				itm_response(conn_id, req_id, TOPIC_STATUS_FAIL, NULL, 0);
		}
	default: break;
	}
	if ((DRIVER_API_RC)msg->conn != DRV_RC_OK) {
		pr_debug(LOG_MODULE_IQ, "BODY_IQ: Response error");
	}
	cfw_msg_free(msg);
}

static void service_connection_cb(cfw_service_conn_t * handle, void * param)
{
	ll_storage_service_conn = handle;
	ll_storage_service_cir_stor_init(ll_storage_service_conn,
					 SPI_APPLICATION_DATA_PARTITION_ID,
					 CONFIG_ACTIVITY_STORAGE_SIZE,
					 SPI_APPLICATION_DATA_START_BLOCK,
					 NULL);

	/* Initialize current activity */
	cur_activity = (intel_Activity)intel_Activity_init_default;

	body_iq_interval_reports_duration = CONFIG_BODY_IQ_INTERVAL_REPORTS_DURATION;
	timer_duration_has_to_be_change = false;
	static struct topic_application app_desc =
	{
		.topic_array = _body_topics,
		.on_subscribed = _on_itm_subscribed,
		.on_request  = _on_itm_request,
		.on_disconnect = _on_itm_disconnect,
		.on_unsubscribe_req = _on_itm_unsubscribe_req
	};
	/* Interface with the Topic Manager */
	_app_id = itm_add_application(&app_desc);
	if (_app_id >= 0) {
		itm_start_application(_app_id);
		/* Set sensing callbacks */
		sensing_set_stepcounter_cb(body_iq_stepcounter_cb);
	}

	/* Timer duration is a build configuration parameter in minutes (*60*1000 to have it in ms) */
	body_timer = timer_create(body_iq_timer_callback,
				  NULL,
				  body_iq_interval_reports_duration * 60000,
				  1,
				  1,
				  NULL);

	/* Enable activity classification */
	classification_enabled = true;
	init_done();
}

void body_iq_init(T_QUEUE queue)
{
	/* Get a client handle */
	ll_storage_client = cfw_client_init(queue, body_iq_handle_msg, NULL);

	/* Open the ll_storage service */
	cfw_open_service_helper(ll_storage_client,
				LL_STOR_SERVICE_ID,
				service_connection_cb,
				(void *)LL_STOR_SERVICE_ID);

	/* Initialize sensing IQ on active queue */
	sensing_iq_init(queue);
}
