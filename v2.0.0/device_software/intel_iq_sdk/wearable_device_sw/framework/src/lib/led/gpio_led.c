/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential information of Intel
 * or its suppliers and licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without Intels prior
 * express written permission.
 * No license under any patent, copyright, trade secret or other intellectual property right
 * is granted to or conferred upon you by disclosure or delivery of the Materials,
 * either expressly, by implication, inducement, estoppel or otherwise.
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing.
*/
#include "lib/led/gpio_led.h"
#include "lib/led/led_gpio_svc.h"

#include "os/os.h"
#include "drivers/led/led.h"
#include "infra/log.h"
#include "services/gpio_service/gpio_service.h"
#include "services/service_queue.h"
#include "cfw/cfw.h"
#include <string.h>

struct gpio_led {
	uint8_t id;
	void (*callback) (uint8_t, uint8_t);
	T_TIMER timer;
	struct led_gpio_svc_priv * priv;
	uint8_t remaining_repetition;
	led_s * current_pattern;
	cfw_service_conn_t *gpio_svc_client_conn; /* gpio service handler */
};

static struct gpio_led led_list[UI_LED_COUNT];

/* gpio service connection handler */
static cfw_client_t * gpio_msg_client = NULL;

/* Called when gpio_service has an event to send to the client, or when a call
 * to the gpio_service has been handled.
 * Here we are just a GPIO output controler, so there is nothing to do in this
 * callback. */
static void led_gpio_service_handler(struct cfw_message *msg, void *param)
{
	pr_debug(LOG_MODULE_LED, "%s: There is nothing to do msg_id: %x", __func__, CFW_MESSAGE_ID(msg));
	cfw_msg_free(msg);
}

/* Called when we are well registered to the gpio service. Here, we set the gpio
 * to output mode and to the */
static void led_service_connection_cb(cfw_service_conn_t * handle, void *param)
{
	struct gpio_led *led = (struct gpio_led *)param;

	pr_debug(LOG_MODULE_LED, "%s: led %d registered!", __func__,
		 led->id);
	led->gpio_svc_client_conn = handle;

	/* Configure GPIO output */
	gpio_configure(led->gpio_svc_client_conn, led->priv->pin, 1, NULL);
}

static void led_gpio_svc_init(struct gpio_led *led)
{
	if (!gpio_msg_client) {
		/* Init gpio service wrapper */
		gpio_msg_client = cfw_client_init(
				get_service_queue(),
				led_gpio_service_handler, NULL);
		pr_debug(LOG_MODULE_LED, "%s: cfw_client_init!", __func__);
	}

	cfw_open_service_helper(gpio_msg_client,
			led->priv->gpio_service_id,
			led_service_connection_cb, led);
	pr_debug(LOG_MODULE_LED, "%s: led %d init!", __func__, led->id);
}


/******************************************/
/******** led.h API implementation ********/
/******************************************/
int8_t led_pattern_handler_config(enum led_type type, led_s *pattern,
				  uint8_t ledNb)
{
	if (NULL == pattern || ledNb >= UI_LED_COUNT) {
		return -1;
	}
	pr_debug(LOG_MODULE_LED, "%s: !", __func__);

	struct gpio_led * led_handler = &led_list[ledNb];
	timer_stop(led_handler->timer, NULL);

	led_handler->remaining_repetition = pattern->repetition_count;
	led_handler->current_pattern = pattern;

	switch (type) {
		case LED_NONE:
			/* shutdown the led */
			/* Here, the ui service do not wait for a callback to be
			 * called. */
			pr_debug(LOG_MODULE_LED, "%s: Playing LED_NONE!", __func__);
			gpio_set_state (led_handler->gpio_svc_client_conn,
				    led_handler->priv->pin,
				    (uint8_t)!led_handler->priv->active_high,
				    NULL);
			break;
		case LED_BLINK_X1:
			gpio_set_state (led_handler->gpio_svc_client_conn,
				    led_handler->priv->pin,
				    (uint8_t)led_handler->priv->active_high,
				    NULL);
			if (pattern->duration[0].duration_on != 0) {
				/* Blink for duration_on milliseconds */
				pr_debug(LOG_MODULE_LED, "%s: Playing LED_BLINK_X1!", __func__);
				timer_start(led_handler->timer,
					    pattern->duration[0].duration_on,
					    NULL);
			} else {
				/* Background LED */
				pr_debug(LOG_MODULE_LED, "%s: Playing BACKGROUND LED_BLINK_X1!", __func__);
			}
			break;
		case LED_BLINK_X2:
			/* Not handled */
		case LED_BLINK_X3:
			/* Not handled */
		default:
			pr_error(LOG_MODULE_MAIN, "LED pattern %d not handled", type);
			return -1;
	}

	return 0;
}

static void led_timer_callback(void *data) {
	pr_debug(LOG_MODULE_LED, "%s: timer callback!", __func__);
	/* toggle LED value */
	struct gpio_led* handler = (struct gpio_led*)data;
	gpio_set_state (handler->gpio_svc_client_conn,
			handler->priv->pin,
			(uint8_t)!handler->priv->active_high,
			NULL);
	if (handler->remaining_repetition == 0) {
		handler->callback(0, 0);
	} else {
		/* For now, we do not handle repetition*/
		/* Need to re-call led_pattern_handler_config with the right
		 * arguments. in a timer */
		handler->callback(0, 0);
	}
}

/* Initialization of LED */
void led_set_pattern_callback(void (*evt_callback_func) (uint8_t, uint8_t),
			      uint8_t led_count, struct led led_config[])
{
	for (int i = 0; i < led_count; i++) {

		/* Register led event callback */
		led_list[i].callback = evt_callback_func;

		/* Init led completion timer */
		if (!led_list[i].timer) {
			led_list[i].timer = timer_create(led_timer_callback,
							 &led_list[i], 0, 0, 0, NULL);
		}
		led_list[i].id = led_config[i].id;
		led_list[i].priv = (struct led_gpio_svc_priv * )led_config[i].priv;
		led_list[i].current_pattern = NULL;
		led_gpio_svc_init(&led_list[i]);
		pr_debug(LOG_MODULE_LED, "%s: !", __func__);
	}
}

int8_t led_shutdown(void) {
	return 0;
}
