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

/* Topic manager */
#include "iq/itm/itm.h"
#include "iq/itm/topic_application.h"

/* Iq initialization */
#include "iq/init_iq.h"

/* Protobufs */
#include "pb_decode.h"
#include "IntelNotifications.pb.h"

/* UUID */
#include "iq/uuid_iq.h"

#define NOTIFICATION_TOPIC_INDEX 0

const uint8_t _notication_topic[] = NOTIFICATION_UUID;

static uint8_t const * const _subscribed_topics[] = {
	[NOTIFICATION_TOPIC_INDEX] = _notication_topic,
	NULL
};

static int _app_id;

/* CFW handlers */

/* Client handle */
static cfw_client_t * _client;

/* UI service handle */
static cfw_service_conn_t *_ui_service_conn;

/* Data sent to timer callback to execute LED pattern after delay startup.*/
struct delay_startup_data_s {
	uint8_t led_id;
	enum led_type type;
	led_s pattern;
	T_TIMER timer;
};

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* New phone connection callback */
static void _on_itm_connect(uint8_t con_id)
{
	/* Blindly subscribe to phone notifications */
	itm_subscribe_req(_app_id, con_id, _subscribed_topics);
}
/* This array makes correspondance between protobuf pattern_type and color count to
 Led service type */
static const led_type led_type_proto_to_svc[][4]= {
	[(uint8_t)intel_Notification_ledPatternType_LED_BLINK][0] = LED_NONE,
	[(uint8_t)intel_Notification_ledPatternType_LED_WAVE][0] = LED_NONE,
	[(uint8_t)intel_Notification_ledPatternType_LED_BLINK][1] = LED_BLINK_X1,
	[(uint8_t)intel_Notification_ledPatternType_LED_BLINK][2] = LED_BLINK_X2,
	[(uint8_t)intel_Notification_ledPatternType_LED_BLINK][3] = LED_BLINK_X3,
	[(uint8_t)intel_Notification_ledPatternType_LED_WAVE][1] = LED_WAVE_X1,
	[(uint8_t)intel_Notification_ledPatternType_LED_WAVE][2] = LED_WAVE_X2,
	[(uint8_t)intel_Notification_ledPatternType_LED_WAVE][3] = LED_WAVE_X3,
};

/* Timer callback for LED delay startup */
static void led_delay_timer_cb(void* priv)
{
	struct delay_startup_data_s *delay_startup_data = (struct delay_startup_data_s*)priv;
	ui_play_led_pattern(_ui_service_conn,
			    delay_startup_data->led_id,
			    delay_startup_data->type,
			    &(delay_startup_data->pattern),
			    NULL);
	timer_delete(delay_startup_data->timer, NULL);
	bfree(delay_startup_data);
}

/* New notification callback */
static void _on_itm_publish(uint8_t con_id,
                            const uint8_t * topic_ix,
                            const uint8_t * data,
                            uint16_t len)
{
	UNUSED(con_id);
	/* Check if the topic is the one expected */
	if(strcmp(_notication_topic,topic_ix)) return;

	/* Deserialize protobuf to get notification data */
	intel_Notification notif = intel_Notification_init_default;
	pb_istream_t stream = pb_istream_from_buffer((uint8_t*)data,len);
	if (pb_decode(&stream, intel_Notification_fields, &notif)) {
		if(notif.has_vibra_notif || notif.has_led_notif) {
			if(notif.has_vibra_notif) {
				/* Extract Vibra pattern { and call UI service*/
				vibration_u vib_pattern;
				switch (notif.vibra_notif.type) {
				case intel_Notification_vibraPatternType_VIBRA_SQUARE:
					/* Square pattern */
					vib_pattern.square_x2.amplitude =
						notif.vibra_notif.amplitude;
					vib_pattern.square_x2.duration_on_1 =
						MAX(5, notif.vibra_notif.duration[0].duration_on);
					vib_pattern.square_x2.duration_off_1 =
						MAX(5, notif.vibra_notif.duration[0].duration_off);
					vib_pattern.square_x2.duration_on_2 =
						MAX(5, notif.vibra_notif.duration[1].duration_on);
					vib_pattern.square_x2.duration_off_2 =
						MAX(5, notif.vibra_notif.duration[1].duration_off);
					vib_pattern.square_x2.repetition_count =
						notif.vibra_notif.repetition_count;
					ui_play_vibr_pattern(_ui_service_conn,
							     VIBRATION_SQUARE_X2,
							     &vib_pattern,
							     NULL);
					break;
				case intel_Notification_vibraPatternType_VIBRA_SPECIAL_EFFECTS:
					/* Special effect */
					vib_pattern.special_effect.effect_1 =
						notif.vibra_notif.effect[0];
					vib_pattern.special_effect.duration_off_1 =
						MAX(5, notif.vibra_notif.duration[0].duration_off);
					vib_pattern.special_effect.effect_2 =
						notif.vibra_notif.effect[1];
					vib_pattern.special_effect.duration_off_2 =
						MAX(5, notif.vibra_notif.duration[0].duration_off);
					vib_pattern.special_effect.effect_3 =
						notif.vibra_notif.effect[2];
					vib_pattern.special_effect.duration_off_3 =
						MAX(5, notif.vibra_notif.duration[2].duration_off);
					vib_pattern.special_effect.effect_4 =
						notif.vibra_notif.effect[3];
					vib_pattern.special_effect.duration_off_4 =
						MAX(5, notif.vibra_notif.duration[3].duration_off);
					vib_pattern.special_effect.effect_5 =
						notif.vibra_notif.effect[4];
					ui_play_vibr_pattern(_ui_service_conn,
							     VIBRATION_SPECIAL_EFFECTS,
							     &vib_pattern,
							     NULL);
					break;
				default:
					pr_error(LOG_MODULE_IQ,
						 "Unsupported vibration pattern %d",
						 notif.vibra_notif.type);
					break;
				}
			}
			if(notif.has_led_notif) {
				led_s led_pattern = {0};
				int i;
#ifdef CONFIG_LED_MULTICOLOR
				for(i = 0 ; i < notif.led_notif.rgb_count ; i++) {
					led_pattern.rgb[i].r = notif.led_notif.rgb[i].red;
					led_pattern.rgb[i].g = notif.led_notif.rgb[i].green;
					led_pattern.rgb[i].b = notif.led_notif.rgb[i].blue;
				}
#endif
				for(i = 0 ; i < notif.led_notif.duration_count ; i++) {
					led_pattern.duration[i].duration_on =
						notif.led_notif.duration[i].duration_on;
					led_pattern.duration[i].duration_off =
						notif.led_notif.duration[i].duration_off;
				}
				led_pattern.intensity = notif.led_notif.intensity;
				led_pattern.repetition_count = notif.led_notif.repetition_count;
				if(notif.led_notif.has_id) {
					led_pattern.id = notif.led_notif.id;
				} else {
					led_pattern.id = 0;
				}
				if(notif.has_delay_startup && notif.delay_startup) {
					struct delay_startup_data_s *delay_startup_data =
						balloc(sizeof(struct delay_startup_data_s), NULL);
					delay_startup_data->led_id = led_pattern.id;
					delay_startup_data->type =
						led_type_proto_to_svc[notif.led_notif.type]
						[notif.led_notif.duration_count];
					memcpy(&(delay_startup_data->pattern),
					       &led_pattern,
					       sizeof(led_s));
					delay_startup_data->timer =
						timer_create(led_delay_timer_cb,
							     delay_startup_data,
							     notif.delay_startup,
							     false,
							     true,
							     NULL);
				} else {
					ui_play_led_pattern(_ui_service_conn,
							    led_pattern.id,
							    led_type_proto_to_svc[notif.led_notif.type]
							    [notif.led_notif.duration_count],
							    &led_pattern,
							    NULL);
				}
			}
		} else {
			pr_error(LOG_MODULE_IQ, "Empty notification");
		}
	} else {
		pr_error(LOG_MODULE_IQ, ERROR_MSG_DECODING_FAILURE_FOR_UUID, _notication_topic);
	}
}

static void _on_ui_svc_connected(cfw_service_conn_t *c, void * param)
{
	UNUSED(param);
	/* Store UI service handle */
	_ui_service_conn = c;
	static struct topic_application app_desc =
	{
		.on_connect = _on_itm_connect,
		.on_publish = _on_itm_publish
	};
	/* Interface with the Topic Manager */
	_app_id = itm_add_application(&app_desc);
	if (_app_id >= 0)
		itm_start_application(_app_id);
	init_done();
}

static void _on_cfw_message(struct cfw_message * msg, void * param)
{
	UNUSED(param);
	switch(CFW_MESSAGE_ID(msg)) {
		case MSG_ID_UI_VIBR_RSP:
		{
			break;
		}
	case MSG_ID_UI_LED_RSP:
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

void notif_iq_init(T_QUEUE queue)
{
	/* Get a client handle */
	_client = cfw_client_init(queue, _on_cfw_message, NULL);

	/* Open the UI service */
	cfw_open_service_helper(_client,
	                        UI_SVC_SERVICE_ID,
	                        _on_ui_svc_connected,
	                        NULL);
}
