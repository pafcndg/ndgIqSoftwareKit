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

#include "lib/button/button.h"
#include "infra/log.h"

/**
 * Button states
 */
enum btn_state {
	IDLE,
	FIRST_PRESS,
	SECOND_PRESS
};

#define GET_BUTTON_ID(button) (button - button_list)

static struct button *button_list = NULL;
static uint8_t button_list_count = 0;
static uint32_t button_press_mask = 0;

/* Internal functions */
static void button_timer_callback(void *data);

void button_shutdown()
{
	int i;
	for (i=0; i<button_list_count; i++) {
		button_list[i].shutdown(&button_list[i]);
		timer_delete(button_list[i].timer, NULL);
	}
}

int button_init(struct button *button,
		uint8_t button_count,
		void (*cb)(uint8_t, uint8_t, uint32_t))
{
	int i, ret;
	button_list_count = button_count;
	button_list = button;

	for (i=0; i<button_count; i++) {
		button[i].state = IDLE;
		button[i].button_cb = cb;
#ifdef CONFIG_BUTTON_MULTIPLE_PRESS
		button[i].press_mask |= MULTIPLE_PRESS;
#endif
#ifdef CONFIG_BUTTON_FEEDBACK
		button[i].press_mask |= FEEDBACK_EVT;
#endif
		/* create timer to identify double press event */
		button[i].timer = timer_create(button_timer_callback,
				&button[i], 0, 0, 0, NULL);

		ret = button[i].init(&button[i]);
		if (ret) {
			pr_error(LOG_MODULE_DRV, "Failed to init button %d", i);
			return ret;
		}
	}
	return 0;
}

uint32_t button_get_port_state()
{
	return button_press_mask;
}

void button_set_press_mask(uint8_t button_id, uint8_t press_mask)
{
	button_list[button_id].press_mask = press_mask;
}

static void notify_event(struct button *button, uint8_t pattern, uint8_t param)
{
	if ((pattern & button->press_mask) && button->button_cb) {
		button->button_cb(GET_BUTTON_ID(button), pattern, param);
	}
}

static void button_timer_callback(void *data)
{
	struct button *button = (struct button *)data;
	switch (button->state) {
		case FIRST_PRESS:
#ifdef CONFIG_BUTTON_FEEDBACK
			if (button->action_index < button->action_count) {

				if ((button->action_index+1) == button->action_count) {
					timer_start(button->timer,
							button->timing.max_time -
							button->action_time[button->action_count], NULL);
				} else {
					timer_start(button->timer,
							button->action_time[button->action_index+1] -
							button->action_time[button->action_index],
							NULL);
				}

				notify_event(button, FEEDBACK_EVT, button->action_index);

				button->action_index ++;
				break;
			}
#endif
			// Max press detected
			button->state = IDLE;
			notify_event(button, MAX_PRESS, 0);
			break;
		case SECOND_PRESS:
			button->state = IDLE;
			// Short single press detected
			notify_event(button, SINGLE_PRESS, 0);
			break;
		default:
			break;
	}
}

void button_gpio_event(struct button *button, bool is_pressed, uint32_t timestamp)
{
	if (is_pressed) {
		button_press_mask |= (1<<GET_BUTTON_ID(button));
#ifdef CONFIG_BUTTON_MULTIPLE_PRESS
		if (button_press_mask & (button_press_mask-1)) {
			notify_event(button, MULTIPLE_PRESS, button_press_mask);
		}
#endif
	} else {
		button_press_mask &= ~(1<<GET_BUTTON_ID(button));

#ifdef CONFIG_BUTTON_MULTIPLE_PRESS
		if (button_press_mask) {
			notify_event(button, MULTIPLE_PRESS, button_press_mask);
		}
#endif
	}

	switch (button->state) {
		case IDLE:
			if (is_pressed) {
				button->state = FIRST_PRESS;
				button->time = timestamp;
#ifdef CONFIG_BUTTON_FEEDBACK
				button->action_index = 0;
				// Start feedback/max_time timer
				if (button->action_count > 0) {
					timer_start(button->timer,
							button->action_time[button->action_index],
							NULL);
					break;
				}
#endif
				timer_start(button->timer, button->timing.max_time, NULL);
			}
			break;
		case FIRST_PRESS: {
			uint32_t flags = interrupt_lock();
			if ((button->state == FIRST_PRESS) && (is_pressed == false)) {
				if ((button->press_mask & DOUBLE_PRESS) &&
					(timestamp - button->time) < button->timing.double_press_time) {
					// Avoid timer race condition
					button->state = IDLE;
					interrupt_unlock(flags);

					timer_stop(button->timer, NULL);
					button->state = SECOND_PRESS;

					button->time = timestamp - button->time;
					timer_start(button->timer, button->timing.double_press_time, NULL);
					break;
				}
				// Long single press
				button->state = IDLE;
				interrupt_unlock(flags);

				timer_stop(button->timer, NULL);
#ifdef CONFIG_BUTTON_FEEDBACK
				notify_event(button, SINGLE_PRESS,
						button->action_index);
#else
				{
					int i;
					int action = 0;

					for (i=0; i<button->action_count; i++) {
						if (button->action_time[i] >
								timestamp - button->time) {
							break;
						}
						action ++;
					}
					notify_event(button, SINGLE_PRESS, action);
				}
#endif
				break;
			}
			interrupt_unlock(flags);
			} break;
		case SECOND_PRESS: {
			uint32_t flags = interrupt_lock();
			if ((button->state == SECOND_PRESS) && is_pressed) {
				button->state = IDLE;
				interrupt_unlock(flags);
				timer_stop(button->timer, NULL);
				// Valid double press
				notify_event(button, DOUBLE_PRESS, 0);
				break;
			}
			interrupt_unlock(flags);
			} break;
		default:
		break;
	}
}
