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

#include "ui_config.h"
#include "services/service_queue.h"
#include "util/misc.h"
#include "lib/button/button_gpio_svc.h"

#define BUTTON_ACTION(...) \
	action_time = (uint16_t[]) {__VA_ARGS__}, \
	.action_count = sizeof((uint16_t[]) {__VA_ARGS__})/sizeof(uint16_t)

struct ui_config ui_config_ref_app = {
#ifdef CONFIG_BUTTON
	.btns = (struct button[]) {
		[PWR_BTN_ID] = {
			.init = button_gpio_svc_init,
			.shutdown = button_gpio_svc_shutdown,

			.priv = &(struct button_gpio_svc_priv) {
				.gpio_service_id = AON_GPIO_SERVICE_ID,
				.pin = 0,
				.active_low = true,
			},
			.timing = {
				.double_press_time = 250,
				.max_time = 10000,
			},
			.press_mask = SINGLE_PRESS | DOUBLE_PRESS | MAX_PRESS,
			.BUTTON_ACTION(1000, 6000),
		}
	},
	.btn_count = UI_BUTTON_COUNT,
#endif
	.led_count = 1,
	.leds = NULL,
};

static cfw_service_conn_t * ui_conn = NULL;
/* We don't do anything in the callback... */
static inline void ui_connect_cb(cfw_service_conn_t * conn, void * param)
{
	ui_conn = conn;
	led_blink(0, 255, 0);
}

void ui_service_start_helper(cfw_client_t * client)
{
	int client_events[3] = {MSG_ID_UI_BTN_SINGLE_EVT,
			MSG_ID_UI_BTN_DOUBLE_EVT,
			MSG_ID_UI_BTN_MAX_EVT};
	ui_service_init(get_service_queue(), &ui_config_ref_app);

	cfw_open_service_helper_evt(client, UI_SVC_SERVICE_ID, client_events,
			ARRAY_SIZE(client_events),
			ui_connect_cb, NULL);
}

static led_s led_pattern = {
    .id = 0,
    .repetition_count = 0,
    .intensity = 255,
    .duration = {
        {1000,1000},
        {0,0},
        {0,0}
    },
    .rgb = {
            {0, 255, 0},
            {0, 0, 0},
            {0, 0, 0},
    }
};

void led_blink(uint8_t r, uint8_t g, uint8_t b) {
	if(ui_conn) {
		led_pattern.rgb[0].r = r;
		led_pattern.rgb[0].g = g;
		led_pattern.rgb[0].b = b;
		ui_play_led_pattern(ui_conn, 0, LED_BLINK_X1, &led_pattern, NULL);
	}
}
