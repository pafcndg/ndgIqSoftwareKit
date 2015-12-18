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

#include "os/os.h"
#include "drivers/led/lp5562_led.h"
#include "drivers/lp5562.h"
#include "infra/log.h"
#include <string.h>

/* Set instruction config */
#define SET_CMD_MASK 0x40

/* lp5562 instruction define tool */
#define RAMP_UP_SIGN     (0<<7)
#define RAMP_DOWN_SIGN   (1<<7)
#define SHIFT_STEP_TIME(x) (x << 8)
#define _15_6_MS_CYCLE_TIME (1 << 14)
#define _0_49_MS_CYCLE_TIME 0 /* (0 << 14) */
#define SHIFT_LOOP_COUNT(x) (x << 7)
#define VAL 0 /* value to set */

/* basic lp5562 instruction */
#define RAMP_WAIT   0x0000
#define SET_PWM     0x4000
#define GO_TO_START 0x0000
#define BRANCH      0xA000
/* END: Set end status bit and set PWM value to 0 */
#define END         0xD800
#define TRIGGER     0xE000

/* lp5562 insctruction */
#define RAMP(w, x, y, z)   (RAMP_WAIT | w | SHIFT_STEP_TIME(x) | (y) | (z)) /* w=sign, x= steptime, y=increment, z=cycle_time*/
#define RAMP_UP(x, y, z)   (RAMP_WAIT | RAMP_UP_SIGN | SHIFT_STEP_TIME(x) | (y) | (z)) /* x= steptime, y=increment, z=cycle_time*/
#define RAMP_DOWN(x, y, z) (RAMP_WAIT | RAMP_DOWN_SIGN | SHIFT_STEP_TIME(x) | (y) | (z)) /* x= steptime, y=increment, z=cycle_time */
#define WAIT(x, y)     (RAMP_WAIT | SHIFT_STEP_TIME(x) | (y) | RAMP_DOWN_SIGN) /* wait x * 0.49 or 15.6ms (depending of y) */

#define SET_PWM_TO(x)  (SET_PWM | (x))
#define BRANCH_TO(x, y)      (BRANCH | SHIFT_LOOP_COUNT(y) | (x)) /* x: PC, y: iteration number */
#define SET_TRIGGER(x, y, z) (TRIGGER | ((x) << 1) | ((y) << 2) | ((z) << 3)) /* x: eng1, y: eng2, z: eng3*/
#define WAIT_TRIGGER(x, y, z) (TRIGGER | ((x) << 7) | ((y) << 8) | ((z) << 9)) /* x: eng1, y: eng2, z: eng3*/

/* Color index*/
const uint8_t colorIndex[3][3] = {
	{0, 6, 12},
	{0, 6, 10},
	{0, 4, 10}
};

/* End index*/
const uint8_t endIndex[3][3] = {
	{5, 9, 15},
	{3, 9, 15},
	{3, 9, 13}
};

struct programIndex {
	uint8_t engine;
	uint8_t instruction;
};

/* Time index*/
#define T1_ON_INDEX   1
#define T1_OFF_INDEX  3
#define T2_ON_INDEX   5
#define T2_OFF_INDEX  9
#define T3_ON_INDEX   11

#define NB_ENGINES           3
#define NB_MAX_INSTRUCTIONS  16

/* The patterns used in this driver are generic.
 * The configurable instructions must:
 * - all set instructions are configured to 0
 * - all ramp instructions have a null step time, with slow clock bit set
 * - all branch instructions have a null loop count
 */

const uint16_t pattern_blink_eng[NB_ENGINES][NB_MAX_INSTRUCTIONS]={{
/* engine blue LED*/
		SET_PWM_TO(0),         /* 0 */
		WAIT(0, 0),            /* 1 */
		BRANCH_TO(1, 0),       /* 2 */
		SET_TRIGGER(0, 1, 1),  /* 3 */
		SET_PWM_TO(0),         /* 4 */
		WAIT_TRIGGER(0, 1, 0), /* 5 */
		SET_PWM_TO(0),         /* 6 */
		WAIT_TRIGGER(0, 0, 1), /* 7 */
		SET_PWM_TO(0),         /* 8 */
		WAIT(0, 0),            /* 9 */
		BRANCH_TO(9, 0),       /* 10 */
		SET_TRIGGER(0, 1, 1),  /* 11 */
		SET_PWM_TO(0),         /* 12 */
		WAIT_TRIGGER(0, 1, 0), /* 13 */
		SET_PWM_TO(0),         /* 14 */
		END,                   /* 15 */
	},
/* engine green LED*/
	{
		SET_PWM_TO(0),                  /* 0 */
		WAIT_TRIGGER(1, 0, 0),          /* 1 */
		SET_PWM_TO(0),                  /* 2 */
		WAIT(0, 0),                     /* 3 */
		BRANCH_TO(3, 0),                /* 4 */
		SET_TRIGGER(1, 0, 1),           /* 5 */
		SET_PWM_TO(0),                  /* 6 */
		WAIT_TRIGGER(0, 0, 1),          /* 7 */
		SET_PWM_TO(0),                  /* 8 */
		WAIT_TRIGGER(1, 0, 0),          /* 9 */
		SET_PWM_TO(0),                  /* 10 */
		WAIT(0, 0),                     /* 11 */
		BRANCH_TO(11, 0),               /* 12 */
		SET_TRIGGER(1, 0, 1),           /* 13 */
		SET_PWM_TO(0),                  /* 14 */
		END                             /* 15 */
	},
/* engine red LED*/
	{
		SET_PWM_TO(0),         /* 0 */
		WAIT_TRIGGER(1, 0, 0), /* 1 */
		SET_PWM_TO(0),         /* 2 */
		WAIT_TRIGGER(0, 1, 0), /* 3 */
		SET_PWM_TO(0),         /* 4 */
		WAIT(0, 0),            /* 5 */
		BRANCH_TO(5, 0),       /* 6 */
		SET_TRIGGER(1, 1, 0),  /* 7 */
		SET_PWM_TO(0),         /* 8 */
		WAIT_TRIGGER(1, 0, 0), /* 9 */
		SET_PWM_TO(0),         /* 10 */
		WAIT_TRIGGER(0, 1, 0), /* 11 */
		SET_PWM_TO(0),         /* 12 */
		END,                   /* 13 */
		END,                   /* 14 */
		END,                   /* 15 */
	}
};

/*Ramp instruction config*/
#define BASE_CLOCK 32768
#define FAST_CLOCK 2048
#define SLOW_CLOCK 64
#define FAST_CLOCK_DIV (BASE_CLOCK/FAST_CLOCK)
#define SLOW_CLOCK_DIV (BASE_CLOCK/SLOW_CLOCK)
#define MS_TO_TICK(delay) ((delay)*((BASE_CLOCK+500/2)/1000))
#define LOOP_TICK 16 /* Clock cycles wasted for a branching */
#define WAIT_MAX_CMD 63 /* Command on 7 bits */
#define FAST_CLOCK_MAX_DELAY_MS (WAIT_MAX_CMD*1000/FAST_CLOCK)
#define SLOW_CLOCK_MAX_DELAY_MS (WAIT_MAX_CMD*1000/SLOW_CLOCK)
#define STATUS_PATTERN_ENDED 0x07

struct lp5562_led {
	void (*callback) (uint8_t, uint8_t);
	volatile bool is_enable;
	T_TIMER timer;
	struct device *dev;
	uint8_t repetition_remaining;
	uint16_t pattern_duration;
	uint16_t pattern_last_duration;
};

static struct lp5562_led led_handler;

/* Internal functions */
static void led_timer_callback(void *data);
static void update_pattern_duration(struct lp5562_led *led, led_s *p, uint8_t nbColor);
static void update_loop_cmd(uint16_t *cmd, uint8_t loop);
static void update_wait_generic(uint16_t *cmd, uint16_t delay, uint8_t loop);
static void update_wait_loop_cmd(uint16_t *cmd, uint16_t delay);
static void reverse_pattern(uint16_t data[][NB_MAX_INSTRUCTIONS]);
static void led_play_blink(struct lp5562_led *led, led_s *p, uint8_t nbColor);
#ifdef CONFIG_LED_WAVE_SUPPORT
static void adjust_and_fill_ramp(uint16_t data[][NB_MAX_INSTRUCTIONS], uint8_t nbColor);
static uint16_t set_ramp_instruction(uint16_t sign, uint16_t duration_ms, uint8_t pwm_value);
static void led_play_wave(struct lp5562_led *led, led_s *p, uint8_t nbColor);
#endif

static void led_timer_callback(void *data)
{
	struct lp5562_led *led = (struct lp5562_led*)data;
	if(led_lp5562_get_status(led->dev) != STATUS_PATTERN_ENDED) {
		/* wait for 10ms to check again if pattern has ended */
		timer_start(led->timer, 10, NULL);
		return;
	}
	if(led->repetition_remaining) {
		led->repetition_remaining--;
		if(!(led->repetition_remaining))
			led_handler.pattern_duration = led_handler.pattern_last_duration;
		/* execute the pattern again */
		timer_start(led->timer, led_handler.pattern_duration, NULL);
		led_lp5562_start(led->dev, LED_EN1_RUN_MASK|LED_EN2_RUN_MASK|LED_EN3_RUN_MASK);
	} else {
		/* Send LED disabled notification */
		if (led->callback) {
			led->callback(UI_LED1, 0);
		}
	}
}

static void update_pattern_duration(struct lp5562_led *led, led_s *p, uint8_t nbColor)
{
	/* Update colors and duration */
	led->pattern_duration = 0;
	for(uint8_t i = 0; i < (nbColor); i++) {
		led->pattern_duration += (p->duration[i].duration_on +
					  p->duration[i].duration_off);
	}

	/* delete last t_off duration for the last repetition */
	led->pattern_last_duration =
		led->pattern_duration - p->duration[nbColor - 1].duration_off;
	if(led->pattern_last_duration == 0) {
		led->pattern_last_duration = 0;
		return;
	}

	if(led->repetition_remaining == 0) {
		led->pattern_duration = led->pattern_last_duration;
	}
}

static void update_loop_cmd(uint16_t *cmd, uint8_t loop)
{
	if (loop == 0) {
		/* If no loop needed, replace it with a set 0 command
		 * This case should never happened */
		cmd[0] = SET_PWM_TO(0);
	} else {
		cmd[0] |= BRANCH_TO(0, loop);
	}
}

static void update_wait_generic(uint16_t *cmd, uint16_t delay, uint8_t loop)
{
	if (delay <= FAST_CLOCK_MAX_DELAY_MS*(1+loop)) {
		/* Short delay, switch to fast clock */
		cmd[0] = WAIT((MS_TO_TICK(delay/(1+loop))-LOOP_TICK+FAST_CLOCK_DIV/2)/(FAST_CLOCK_DIV), _0_49_MS_CYCLE_TIME);
	} else {
		/* Long delay, switch to slow clock */
		cmd[0] = WAIT((MS_TO_TICK(delay/(1+loop))-LOOP_TICK+SLOW_CLOCK_DIV/2)/(SLOW_CLOCK_DIV), _15_6_MS_CYCLE_TIME);
	}
}

static void update_wait_loop_cmd(uint16_t *cmd, uint16_t delay)
{
	/* We always want at least one loop
	 * because we don't have a nop command */
	uint8_t loop = 1;

	/* If one wait loop is not enough, loop again */
	if (delay > SLOW_CLOCK_MAX_DELAY_MS*(1+loop)) {
		loop = (delay+SLOW_CLOCK_MAX_DELAY_MS-1)/SLOW_CLOCK_MAX_DELAY_MS-1;
	}
	/* Update wait command */
	update_wait_generic(&cmd[0], delay, loop);
	/* Update loop command */
	update_loop_cmd(&cmd[1], loop);
}

static void reverse_pattern(uint16_t data[][NB_MAX_INSTRUCTIONS])
{
	uint8_t i, j;
	for(i = 0; i < NB_ENGINES; i++) {
		for(j = 0; j < NB_MAX_INSTRUCTIONS; j++) {
			data[i][j] = ((data[i][j] & 0x00FF) << 8 | (data[i][j] & 0xFF00) >> 8);
		}
	}
}

#ifdef CONFIG_LED_WAVE_SUPPORT
/* Adjust ramp instruction to make them last the nearest duration possible,
 * even if we lost color accuracy */
static void adjust_and_fill_ramp(uint16_t data[][NB_MAX_INSTRUCTIONS], uint8_t nbColor)
{
	uint8_t i, j;
	uint16_t ramp_duration_steptime;
	uint16_t ramp_duration_ms;
	uint32_t increment;
	uint16_t minimum;

	for(i = 0; i < nbColor; i++)
	{
		minimum = 0xFFFF;
		for(j = 0; j < NB_ENGINES; j++)
		{
			/* check if it's not a simple delay or a "goto" sync instruction */
			if(data[j][i*7] == BRANCH_TO(i*7+6, 0) ||
			   data[j][i*7+2] == BRANCH_TO(i*7 + 7, 0)) {continue;}
			ramp_duration_steptime =
				((((data[j][i*7]) & 0x3F00) >> 8) *
				((data[j][i*7]) & 0x007F));
			ramp_duration_ms = ramp_duration_steptime * 1000 /
				((data[j][i*7] & _15_6_MS_CYCLE_TIME)? SLOW_CLOCK : FAST_CLOCK);
			if (minimum > ramp_duration_ms) minimum = ramp_duration_ms;
		}
		for(j = 0; j < NB_ENGINES; j++)
		{
			if(data[j][i*7] == BRANCH_TO(i*7+6, 0) ||
			   data[j][i*7+2] == BRANCH_TO(i*7 + 7, 0)) {continue;}
			increment = minimum *
				((data[j][i*7] & _15_6_MS_CYCLE_TIME)? SLOW_CLOCK : FAST_CLOCK) /
				(((data[j][i*7]) & 0x3F00) >> 8) / 1000;
			data[j][i*7] &= 0xFF80;
			data[j][i*7] |= increment & 0x7F;
			data[j][i*7 + 1] = data[j][i*7];
			data[j][i*7 + 2] = data[j][i*7 + 1] | RAMP_DOWN_SIGN;
			data[j][i*7 + 3] = data[j][i*7 + 2];
		}
	}
}

static uint16_t set_ramp_instruction(uint16_t sign,
				     uint16_t duration_ms,
				     uint8_t pwm_value)
{
	uint8_t steptime;
	uint32_t tmp_duration_us;
	tmp_duration_us = (duration_ms * 1000) / pwm_value;

	if(tmp_duration_us < (FAST_CLOCK_MAX_DELAY_MS*1000)) {
		steptime = (tmp_duration_us * FAST_CLOCK) / 1000000;
		return RAMP(sign, steptime, pwm_value, _0_49_MS_CYCLE_TIME);
	} else if(tmp_duration_us < (SLOW_CLOCK_MAX_DELAY_MS*1000)) {
		steptime = (tmp_duration_us * SLOW_CLOCK) / 1000000;
		return RAMP(sign, steptime, pwm_value, _15_6_MS_CYCLE_TIME);
	}
	else return 0;
}
#endif

static void led_play_blink(struct lp5562_led *led, led_s *p, uint8_t nbColor)
{
	uint16_t pattern[NB_ENGINES][NB_MAX_INSTRUCTIONS];
	int i;

	/* blink */

	struct lp5562_pattern p_eng1 = {
		.pattern = pattern[0],
		.size = sizeof(pattern[0])};
	struct lp5562_pattern p_eng2 = {
		.pattern = pattern[1],
		.size = sizeof(pattern[1])};
	struct lp5562_pattern p_eng3 = {
		.pattern = pattern[2],
		.size = sizeof(pattern[2])};

	memcpy(pattern, pattern_blink_eng, sizeof(pattern_blink_eng));

	/* Update colors */
	led->pattern_duration = 0;
	for(i = 0; i < (nbColor); i++) {
		pattern[0][colorIndex[0][i]] = SET_PWM_TO(p->rgb[i].b);
		pattern[1][colorIndex[1][i]] = SET_PWM_TO(p->rgb[i].g);
		pattern[2][colorIndex[2][i]] = SET_PWM_TO(p->rgb[i].r);
	}

	update_pattern_duration(led, p, nbColor);

	/* Update Wait and loop */
	update_wait_loop_cmd(&pattern[0][T1_ON_INDEX], p->duration[0].duration_on);
	update_wait_loop_cmd(&pattern[1][T1_OFF_INDEX], p->duration[0].duration_off);
	update_wait_loop_cmd(&pattern[2][T2_ON_INDEX], p->duration[1].duration_on);
	update_wait_loop_cmd(&pattern[0][T2_OFF_INDEX], p->duration[1].duration_off);
	update_wait_loop_cmd(&pattern[1][T3_ON_INDEX], p->duration[2].duration_on);

	/* Update END*/
	for(i = 0; i < NB_ENGINES; i++) {
		pattern[i][endIndex[i][nbColor - 1]] = END;
	}

	reverse_pattern(pattern);

	led_lp5562_program_pattern(led->dev, &p_eng1, &p_eng2, &p_eng3);
	led_lp5562_start(led->dev, LED_EN1_RUN_MASK|LED_EN2_RUN_MASK|LED_EN3_RUN_MASK);
	return;
}

#ifdef CONFIG_LED_WAVE_SUPPORT
static void led_play_wave(struct lp5562_led *led, led_s *p, uint8_t nbColor)
{
	uint8_t i, j = 0;
	uint16_t pattern[NB_ENGINES][NB_MAX_INSTRUCTIONS] = {{0}};
	uint8_t color[NB_ENGINES][nbColor];

	struct lp5562_pattern p_eng1 = {
		.pattern = pattern[0],
		.size = sizeof(pattern[0])};
	struct lp5562_pattern p_eng2 = {
		.pattern = pattern[1],
		.size = sizeof(pattern[1])};
	struct lp5562_pattern p_eng3 = {
		.pattern = pattern[2],
		.size = sizeof(pattern[2])};

	for(i = 0; i < nbColor; i++) {
		color[0][i] = p->rgb[i].b;
		color[1][i] = p->rgb[i].g;
		color[2][i] = p->rgb[i].r;
	}

	for(i = 0; i < NB_ENGINES; i++) {
		for(j = 0; j < nbColor; j++) {

			if(color[0][j] < 2 && color[1][j] < 2 && color[2][j] < 2) {
				/* add a delay to simulate an empty ramp
				 * if there isn't any valid ramp. */
				pattern[i][j*7+1] = BRANCH_TO(j*7, 0);
				update_wait_loop_cmd(&pattern[i][j*7],
						     p->duration[j].duration_on +
						     p->duration[j].duration_on);
				pattern[i][j*7+2] = BRANCH_TO(j*7 + 7, 0);
				continue;
			}

			if(color[i][j] > 1) {
				pattern[i][j*7] =
					set_ramp_instruction(RAMP_UP_SIGN,
						       (p->duration[j].duration_on >> 2),
						       color[i][j] >> 1);
				pattern[i][j*7 + 5] = BRANCH_TO(j*7 + 4, 0);
				update_wait_loop_cmd(&pattern[i][j*7+4],
						     p->duration[j].duration_off);
				pattern[i][j*7+6] =
					SET_TRIGGER(!(i == 0) && (color[0][j] < 2),
						    !(i == 1) && (color[1][j] < 2),
						    !(i == 2) && (color[2][j] < 2));
			} else {
				pattern[i][j*7] = BRANCH_TO(j*7+6, 0);
				pattern[i][j*7+6] =
					WAIT_TRIGGER(!(i == 0) && (color[0][j] > 1),
						     !(i == 1) && (color[1][j] > 1),
						     !(i == 2) && (color[2][j] > 1));
			}
		}
		pattern[i][nbColor*7] = END;
	}

	adjust_and_fill_ramp(pattern, nbColor);

	update_pattern_duration(led, p, nbColor);

	reverse_pattern(pattern);

	led_lp5562_program_pattern(led->dev, &p_eng1, &p_eng2, &p_eng3);
	led_lp5562_start(led->dev, LED_EN1_RUN_MASK|LED_EN2_RUN_MASK|LED_EN3_RUN_MASK);
}
#endif

int8_t led_pattern_handler_config(enum led_type type, led_s *pattern,
				  uint8_t ledNb)
{
	uint8_t blinks = 0;
	uint8_t waves = 0;
	if (ledNb >= UI_LED_COUNT) {
		return 0;
	}

	timer_stop(led_handler.timer, NULL);

	if (!led_handler.is_enable) {
		/* LED disabled, enable it */
		led_lp5562_enable(led_handler.dev, true);
		led_handler.is_enable = true;
	}

	if ((type == LED_BLINK_X1) && (pattern->duration[0].duration_on == 0)) {
		led_lp5562_set_pwm(led_handler.dev,
				   LED_R, pattern->rgb[0].r);
		led_lp5562_set_pwm(led_handler.dev,
				   LED_G, pattern->rgb[0].g);
		led_lp5562_set_pwm(led_handler.dev,
				   LED_B, pattern->rgb[0].b);

		led_lp5562_set_mode(led_handler.dev,
				    LED_EN1_DC_MODE|LED_EN2_DC_MODE|LED_EN3_DC_MODE);

		return 0;
	}

	led_handler.pattern_duration = 0;

	switch (type) {
	case LED_NONE:
		led_handler.is_enable = false;
		led_lp5562_enable(led_handler.dev, false);
		return 0;
	case LED_BLINK_X1:
		blinks = 1;
		break;
	case LED_BLINK_X2:
		blinks = 2;
		break;
	case LED_BLINK_X3:
		blinks = 3;
		break;
	case LED_WAVE_X1:
		waves = 1;
		break;
	case LED_WAVE_X2:
		waves = 2;
		break;
	default:
		pr_error(LOG_MODULE_MAIN, "LED pattern %d not handled", type);
		led_handler.is_enable = false;
		led_lp5562_enable(led_handler.dev, false);
		return -1;
	}

	led_handler.repetition_remaining = pattern->repetition_count;
	if(blinks) led_play_blink(&led_handler, pattern, blinks);
	if(waves) {
#ifdef CONFIG_LED_WAVE_SUPPORT
		led_play_wave(&led_handler, pattern, waves);
#else
		pr_error(LOG_MODULE_MAIN, "LED wave pattern is not supported");
		led_handler.is_enable = false;
		led_lp5562_enable(led_handler.dev, false);
		return -1;
#endif
	}

	/* doesn't execute pattern is there is not t_on duration */
	if(led_handler.pattern_last_duration) {
		timer_start(led_handler.timer, led_handler.pattern_duration, NULL);
	} else {

		pr_error(LOG_MODULE_MAIN, "LED pattern has no duration", type);
		led_handler.is_enable = false;
		led_lp5562_enable(led_handler.dev, false);
		return -1;
	}
	return 0;
}

void led_set_pattern_callback(void (*evt_callback_func) (uint8_t, uint8_t),
			      uint8_t led_count, struct led led_config[])
{
	/* Register led event callback */
	led_handler.callback = evt_callback_func;

	/* Resolve LED device */
	led_handler.dev = (struct device *)&pf_sba_device_led_lp5562;
	led_handler.is_enable = false;

	/* Init led completion timer */
	if (!led_handler.timer) {
		led_handler.timer = timer_create(led_timer_callback,
						 &led_handler, 1000, 0, 0, NULL);
	}
}
