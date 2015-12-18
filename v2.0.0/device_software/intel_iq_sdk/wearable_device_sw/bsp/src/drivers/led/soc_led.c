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

#include "os/os.h"
#include "drivers/led/soc_led.h"
#include "drivers/intel_qrk_pwm.h"
#include "infra/log.h"
#include "infra/pm.h"
#include "infra/wakelock_ids.h"
#include "machine.h"


#define NB_LED_AVAILABLE        2
#define MAX_REPEAT_COUNT        63
#define PWM_PERIOD              20000000
#define PWM_PERIOD_MS           20
/* T_ON_FACTOR is computed like this
 * duty-cycle = 19 900 000 * intensity_x_100 / 10000
 * duty-cycle = 1 990 * intensity_x_100
 * */
#define T_ON_FACTOR             1990
#define MIN_WAVE_INTENSITY      100
#define CONFIG_AND_START        0
#define RESTART                 1
#define STOP                    2

/* Blink X2 pattern step */
#define BLINK_ON_1              0
#define BLINK_OFF_1             1
#define BLINK_ON_2              2
#define BLINK_OFF_2             3

/* Wave pattern step */
#define WAVE_FIRST_HALF_ON_1    0
#define WAVE_SECOND_HALF_ON_1   1
#define WAVE_OFF_1              2
#define WAVE_FIRST_HALF_ON_2    3
#define WAVE_SECOND_HALF_ON_2   4
#define WAVE_OFF_2              5

#define PWM_RAISING_EDGE(p) ((p % 2) == 0 )

enum pwm_num {
	PWM0,
	PWM1,
	PWM2,
	PWM3
};

typedef struct {
	enum led_type type;
	led_s *pattern;
	uint8_t ledNB;
	uint8_t current_count;
	uint8_t pattern_step;
	uint32_t intensity_x_100;
	T_TIMER timer_led;
} led_t;

typedef struct {
	uint8_t pwm;
	uint16_t intensity_step_ton1;
	uint16_t intensity_step_ton2;
	int32_t current_intensity;
	uint16_t callback_count;
	struct pm_wakelock wakelock;
} pwm_interrupt_data;

led_t g_led_data[NB_LED_AVAILABLE];
pwm_interrupt_data g_wave_config;
struct soc_pwm_channel_config config;
enum pwm_num pwm;

static led_s g_led_Pattern_Blink_x2;
static led_s g_led_Pattern_Wave_x2;
static int8_t led_reset(led_t * led);
static void timer_callback(void *data);
static int8_t led_pattern_program_blink_x1(led_t * led);
static int8_t led_pattern_program_blink_x2(led_t * led);
static int8_t led_pattern_program_wave_x2(led_t * led);
static void compute_data_wave_x2(uint32_t intensity_to_reach);
static void led_pwm_interrupt(void);
static DRIVER_API_RC intensity_led_setting(uint8_t led_id, int32_t intensity);
static DRIVER_API_RC led_parameter_setting(led_t * led, uint8_t flag);
static void (*led_callback_func_ptr) (uint8_t, uint8_t);

/******************************************/
/************* LOCAL FUNCTIONS ************/
/******************************************/

static void compute_data_wave_x2(uint32_t intensity_to_reach)
{
	/* compute nb cycle to reach half T_ON */
	uint16_t nb_cycle_ton1 =
	    (g_led_Pattern_Wave_x2.duration[0].duration_on / 2) / PWM_PERIOD_MS;
	uint16_t nb_cycle_ton2 =
	    (g_led_Pattern_Wave_x2.duration[1].duration_on / 2) / PWM_PERIOD_MS;

	/* compute intensity step to reach expected intensity during half T_ON */
	g_wave_config.intensity_step_ton1 = intensity_to_reach / nb_cycle_ton1;
	g_wave_config.intensity_step_ton2 = intensity_to_reach / nb_cycle_ton2;
	if (g_wave_config.intensity_step_ton1 == 0)
		g_wave_config.intensity_step_ton1 = MIN_WAVE_INTENSITY;
	if (g_wave_config.intensity_step_ton2 == 0)
		g_wave_config.intensity_step_ton1 = MIN_WAVE_INTENSITY;
}

static void led_pwm_interrupt(void)
{
	int flags = interrupt_lock();
	struct device *dev = &pf_device_pwm;
	pm_wakelock_release(&g_wave_config.wakelock);
	pm_wakelock_acquire(&g_wave_config.wakelock);
	interrupt_unlock(flags);
	/* update pwm parameter on raising edge interruption */
	if (PWM_RAISING_EDGE(g_wave_config.callback_count)) {
		switch (g_led_data[g_wave_config.pwm].pattern_step) {
			/* step to increase led intensity */
		case WAVE_FIRST_HALF_ON_1:
			/* stop pwm */
			led_parameter_setting(&g_led_data[g_wave_config.pwm],
					      STOP);
			g_wave_config.current_intensity =
			    g_wave_config.current_intensity +
			    g_wave_config.intensity_step_ton1;
			/* adjust to max intensity value */
			if (g_wave_config.current_intensity >
			    g_led_data[g_wave_config.pwm].intensity_x_100)
				g_wave_config.current_intensity =
				    g_led_data[g_wave_config.
					       pwm].intensity_x_100;
			config.pwm_duty_cycle_ns =
			    T_ON_FACTOR * g_wave_config.current_intensity;
			/* Start again pwm with new param */
			if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK)
				soc_pwm_start(dev, g_wave_config.pwm);
			else
				pr_error(LOG_MODULE_LED,
					 "PWM: Failed to set new config for wave pattern");
			break;
			/* step to decrease led intensity */
		case WAVE_SECOND_HALF_ON_1:
			/* stop pwm */
			led_parameter_setting(&g_led_data[g_wave_config.pwm],
					      STOP);
			g_wave_config.current_intensity =
			    g_wave_config.current_intensity -
			    g_wave_config.intensity_step_ton1;
			/* adjust to min intensity value */
			if (g_wave_config.current_intensity <
			    MIN_WAVE_INTENSITY)
				g_wave_config.current_intensity =
				    MIN_WAVE_INTENSITY;
			config.pwm_duty_cycle_ns =
			    T_ON_FACTOR * g_wave_config.current_intensity;
			/* Start again pwm with new param */
			if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK)
				soc_pwm_start(dev, g_wave_config.pwm);
			else
				pr_error(LOG_MODULE_LED,
					 "PWM: Failed to set new config for wave pattern");
			break;
			/* step where the led is off */
		case WAVE_OFF_1:
			/* nothing to do */
			break;
			/* step to increase led intensity */
		case WAVE_FIRST_HALF_ON_2:
			/* stop pwm */
			led_parameter_setting(&g_led_data[g_wave_config.pwm],
					      STOP);
			g_wave_config.current_intensity =
			    g_wave_config.current_intensity +
			    g_wave_config.intensity_step_ton2;
			/* adjust to max intensity value */
			if (g_wave_config.current_intensity >
			    g_led_data[g_wave_config.pwm].intensity_x_100)
				g_wave_config.current_intensity =
				    g_led_data[g_wave_config.
					       pwm].intensity_x_100;
			config.pwm_duty_cycle_ns =
			    T_ON_FACTOR * g_wave_config.current_intensity;
			/* Start again pwm with new param */
			if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK)
				soc_pwm_start(dev, g_wave_config.pwm);
			else
				pr_error(LOG_MODULE_LED,
					 "PWM: Failed to set new config for wave pattern");
			break;
			/* step to decrease led intensity */
		case WAVE_SECOND_HALF_ON_2:
			/* stop pwm */
			led_parameter_setting(&g_led_data[g_wave_config.pwm],
					      STOP);
			g_wave_config.current_intensity =
			    g_wave_config.current_intensity -
			    g_wave_config.intensity_step_ton2;
			/* adjust to min intensity value */
			if (g_wave_config.current_intensity <
			    MIN_WAVE_INTENSITY)
				g_wave_config.current_intensity =
				    MIN_WAVE_INTENSITY;
			config.pwm_duty_cycle_ns =
			    T_ON_FACTOR * g_wave_config.current_intensity;
			/* Start again pwm with new param */
			if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK)
				soc_pwm_start(dev, g_wave_config.pwm);
			else
				pr_error(LOG_MODULE_LED,
					 "PWM: Failed to set new config for wave pattern");
			break;
			/* step where the led is off */
		case WAVE_OFF_2:
			/* nothing to do */
			break;
		default:
			pr_error(LOG_MODULE_LED,
				 "pwm_callback : pattern step undefined");
			break;
		}
	}

	g_wave_config.callback_count++;
}

static DRIVER_API_RC intensity_led_setting(uint8_t led_id, int32_t intensity)
{
	/* stop pwm */
	uint8_t led_ident = g_wave_config.pwm;
	struct device *dev = &pf_device_pwm;

	led_parameter_setting(&g_led_data[led_ident], STOP);

	/* store the current intensity for pwm callback computation */
	g_wave_config.current_intensity = intensity;
	config.pwm_duty_cycle_ns = T_ON_FACTOR * intensity;
	if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK) {
		soc_pwm_start(dev, led_ident);
		return DRV_RC_OK;
	} else {
		pr_debug(LOG_MODULE_LED, "PWM: Failed to set minimun config");
		return DRV_RC_FAIL;
	}
}

static DRIVER_API_RC led_parameter_setting(led_t * led, uint8_t flag)
{

	uint32_t l_intensity;
	struct device *dev = &pf_device_pwm;

	/* flag = STOP : the associated LED is fully OFF
	 * flag != STOP and intensity = 0 : config error
	 * 1% <= pwm_value <= 100% : the brightness of the associated LED is set depending upon the PWM modulation value.
	 * pwm_value = 100% : the associated LED is fully ON. */
	if (((led->intensity_x_100 > 10000) || (led->intensity_x_100 == 0))
	    && (flag != STOP))
		goto invalid_config;

	/* UI_LED1 is link to PWM0
	 * UI_LED2 is link to PWM1 */
	if (led->ledNB == UI_LED1) {
		pwm = PWM0;
		g_wave_config.pwm = PWM0;
	} else if (led->ledNB == UI_LED2) {
		pwm = PWM1;
		g_wave_config.pwm = PWM1;
	} else
		goto invalid_config;

	if (flag == STOP) {
		soc_pwm_stop(dev, pwm);
		pm_wakelock_release(&g_wave_config.wakelock);
		return DRV_RC_OK;
	} else {
		l_intensity = led->intensity_x_100;
		if (flag == CONFIG_AND_START) {
			config.mode = PWM_MODE;
			config.timer_timeout_ns = 0;
			config.pwm_enable_interrupts = false;
			/* if pattern is wave => special config */
			if (led->type == LED_WAVE_X2) {
				config.pwm_enable_interrupts = true;
				config.interrupt_fn = led_pwm_interrupt;
				/* compute step according intensity to reach */
				compute_data_wave_x2(l_intensity);

				/* for step 0 and 3 the intensity shall start at min value
				 * for step 1 and 4 the intensity shall start at max value,
				 * this value is already set by input parameter intensity via
				 * function intensity_led_setting */
				if ((led->pattern_step == 0)
				    || (led->pattern_step == 3))
					l_intensity = MIN_WAVE_INTENSITY;

				/* store the current intensity for pwm callback computation */
				g_wave_config.current_intensity = l_intensity;
			}
			/* 20ms period = 50Hz frequency (good for the persistence of vision) */
			config.pwm_period_ns = PWM_PERIOD;
			/* full intensity when led is swith on during 19.9ms (100%) except for wave pattern */
			config.pwm_duty_cycle_ns = l_intensity * T_ON_FACTOR;
			if (soc_pwm_set_config(dev, pwm, &config) == DRV_RC_OK) {
				soc_pwm_start(dev, pwm);
				return DRV_RC_OK;
			} else {
				pr_debug(LOG_MODULE_LED,
					 "PWM: Failed to set config");
				return DRV_RC_FAIL;
			}
		} else if (flag == RESTART) {
			soc_pwm_start(dev, pwm);
			return DRV_RC_OK;
		}
	}

invalid_config:
	pr_debug(LOG_MODULE_LED,
		 "PWM: input parameter incorrect: led_id= %d, intensity= %d",
		 led->ledNB, led->intensity_x_100 / 100);
	return DRV_RC_INVALID_CONFIG;
}

/* Callback function which recall a LED pattern */
static void timer_callback(void *data)
{
	int8_t err_code = DRV_RC_OK;
	led_t *led = (led_t *) data;

	timer_stop(led->timer_led, NULL);

	switch (led->type) {
	case LED_NONE:
		pr_debug(LOG_MODULE_LED,
			 "timer_callback : no pattern selected");
		// can be used to just reset the driver.
		break;

	case LED_BLINK_X1:
		led_parameter_setting(led, STOP);
		led->type = LED_NONE;
		break;

	case LED_BLINK_X2:
		if (led->current_count <
		    g_led_Pattern_Blink_x2.repetition_count) {
			err_code = led_pattern_program_blink_x2(led);
		} else {
			led->type = LED_NONE;
		}
		break;

	case LED_WAVE_X2:
		/* for wave, the pattern step is managed into timer_callback :
		 * the pattern step is increase only if the previous is finished */
		led->pattern_step++;
		/* If current pattern step is upper than last pattern step then
		 * reset current pattern step */
		if (led->pattern_step > WAVE_OFF_2)
			led->pattern_step = 0;
		if (led->current_count < g_led_Pattern_Wave_x2.repetition_count) {
			err_code = led_pattern_program_wave_x2(led);
		} else {
			led->type = LED_NONE;
		}
		break;

	default:
		err_code = DRV_RC_FAIL;
		break;
	}

	if (err_code != DRV_RC_OK) {
		pm_wakelock_release(&g_wave_config.wakelock);
		pr_debug(LOG_MODULE_LED,
			 "timer_callback : err_code with pattern %d, driver %d",
			 led->type, led->ledNB);
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(led->ledNB, DRV_RC_FAIL);
		}
		return;
	}

	if (led->type == LED_NONE) {
		pm_wakelock_release(&g_wave_config.wakelock);
		timer_delete(led->timer_led, NULL);
		led->timer_led = NULL;
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(led->ledNB, DRV_RC_OK);
		}
	}
}

static int8_t led_pattern_program_blink_x1(led_t * led)
{
	DRIVER_API_RC err_code = DRV_RC_OK;
	OS_ERR_TYPE localErr = E_OS_OK;

	/* Program pwm */
	err_code = led_parameter_setting(led, CONFIG_AND_START);

	/* Wait for a duration if needed */
	if (err_code == DRV_RC_OK && led->pattern->duration[0].duration_on)
		/* let the led on for duration_on */
		timer_start(led->timer_led, led->pattern->duration[0].duration_on,
			    &localErr);
	else
		return ((int8_t) DRV_RC_FAIL);

	if ((localErr == E_OS_OK) || (localErr == E_OS_ERR_BUSY))
		err_code = ((int8_t) DRV_RC_OK);
	else
		err_code = ((int8_t) DRV_RC_FAIL);

	return err_code;
}

static int8_t step_setting(led_t * led, uint32_t duration, uint8_t flag)
{
	int8_t err_code = DRV_RC_OK;
	err_code = led_parameter_setting(led, flag);

	/* start timer until duration */
	if ((err_code == DRV_RC_OK) && duration)
		timer_start(led->timer_led, duration, NULL);

	return err_code;
}

static int8_t led_pattern_program_blink_x2(led_t * led)
{
	int8_t err_code = DRV_RC_OK;

	int flags = interrupt_lock();
	pm_wakelock_release(&g_wave_config.wakelock);
	pm_wakelock_acquire(&g_wave_config.wakelock);
	interrupt_unlock(flags);

	switch (led->pattern_step) {
	case BLINK_ON_1:
		/* Program led driver to turn on the led during ON_1 */
		err_code = step_setting(led,
					led->pattern->duration[0].duration_on,
					CONFIG_AND_START);
		led->pattern_step = 1;
		break;
	case BLINK_OFF_1:
		/* Program led driver to turn off the led during OFF_1 */
		err_code = step_setting(led,
					led->pattern->duration[0].duration_off,
					STOP);
		led->pattern_step = 2;
		break;
	case BLINK_ON_2:
		/* Program led driver to turn on the led during ON_2 */
		err_code = step_setting(led,
					led->pattern->duration[1].duration_on,
					RESTART);
		led->pattern_step = 3;
		break;
	case BLINK_OFF_2:
		/* Program led driver to turn off the led during OFF_2 */
		err_code = step_setting(led,
					led->pattern->duration[1].duration_off,
					STOP);
		led->pattern_step = 0;
		led->current_count++;
		break;
	default:
		err_code = DRV_RC_FAIL;
		pr_debug(LOG_MODULE_LED,
			 "led_pattern_program_blink_x2 : pattern failed");
		break;
	}
	return (err_code);
}

static int8_t led_pattern_program_wave_x2(led_t * led)
{
	int8_t err_code = DRV_RC_FAIL;

	int flags = interrupt_lock();
	pm_wakelock_release(&g_wave_config.wakelock);
	pm_wakelock_acquire(&g_wave_config.wakelock);
	interrupt_unlock(flags);

	switch (led->pattern_step) {
		/* This step is the first part of TON 1 */
	case WAVE_FIRST_HALF_ON_1:
		/* Program led driver intensity to turn on the leds with upward intensity */
		err_code = led_parameter_setting(led, CONFIG_AND_START);

		/* let the led on for duration_on_1 ms / 2  */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[0].duration_on)
			timer_start(led->timer_led,
				    led->pattern->duration[0].duration_on / 2,
				    NULL);
		break;

		/* This step is the second part of TON 1 */
	case WAVE_SECOND_HALF_ON_1:
		/* Program led driver intensity to turn on the leds with downward intensity */
		err_code =
		    intensity_led_setting(led->ledNB, led->intensity_x_100);

		/* let the led on for duration_on_1 ms / 2  */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[0].duration_on)
			timer_start(led->timer_led,
				    led->pattern->duration[0].duration_on / 2,
				    NULL);
		break;

		/* This step is TOFF 1 */
	case WAVE_OFF_1:
		/* Program led intensity to turn off the led */
		err_code = led_parameter_setting(led, STOP);

		/* let the led off for duration_off_1 ms */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[0].duration_off)
			timer_start(led->timer_led,
				    led->pattern->duration[0].duration_off, NULL);
		break;

		/* This step is the first part of TON 2 */
	case WAVE_FIRST_HALF_ON_2:
		/* Program led driver intensity to turn on the leds with upward intensity */
		err_code = led_parameter_setting(led, CONFIG_AND_START);

		/* let the led on for duration_on_1 ms / 2  */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[1].duration_on)
			timer_start(led->timer_led,
				    led->pattern->duration[1].duration_on / 2,
				    NULL);
		break;

		/* This step is the second part of TON 2 */
	case WAVE_SECOND_HALF_ON_2:
		/* Program led driver intensity to turn on the leds with downward intensity */
		err_code =
		    intensity_led_setting(led->ledNB, led->intensity_x_100);

		/* let the led on for duration_on_1 ms / 2  */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[1].duration_on)
			timer_start(led->timer_led,
				    led->pattern->duration[1].duration_on / 2,
				    NULL);
		break;

		/* This step is TOFF 2 */
	case WAVE_OFF_2:
		/* Program led intensity to turn off the led */
		err_code = led_parameter_setting(led, STOP);

		/* let the led off for duration_off_1 ms */
		if ((err_code == DRV_RC_OK)
		    && led->pattern->duration[1].duration_off)
			timer_start(led->timer_led,
				    led->pattern->duration[1].duration_off, NULL);
		led->current_count++;
		break;
	default:
		err_code = DRV_RC_FAIL;
		pr_debug(LOG_MODULE_LED,
			 "led_pattern_program_wave_x2 : bad pattern step");
		break;
	}

	return (err_code);
}

static int8_t led_reset(led_t * led)
{
	led->pattern_step = 0;
	led->current_count = 0;
	led->intensity_x_100 = 0;
	led->type = 0;
	g_wave_config.callback_count = 0;
	/* Shutdown all PWM. */
	return led_shutdown();
}

/*******************************************/
/************* GLOBAL FUNCTIONS ************/
/*******************************************/

int8_t led_shutdown()
{
	DRIVER_API_RC ret_code = DRV_RC_OK;
	for (int led_num = 0; led_num < NB_LED_AVAILABLE; led_num++) {
		ret_code = led_parameter_setting(&g_led_data[led_num], STOP);
		if (ret_code != DRV_RC_OK)
			return DRV_RC_FAIL;
	}

	return ret_code;
}

int8_t led_pattern_handler_config(enum led_type type, led_s * pattern,
				  uint8_t ledNumber)
{
	int8_t err_code = DRV_RC_OK;
	bool start = false;
	bool repeat = false;
	int8_t delay = 0;
	OS_ERR_TYPE localErr = E_OS_OK;

	if (ledNumber >= NB_LED_AVAILABLE) {
		pr_debug(LOG_MODULE_LED,
			 "led_pattern_handler_config : wrong led number %d",
			 ledNumber);
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(ledNumber, DRV_RC_FAIL);
		}

		return DRV_RC_FAIL;
	}

	/* reset led */
	err_code = led_reset(&g_led_data[ledNumber]);
	if (err_code != DRV_RC_OK) {
		pr_debug(LOG_MODULE_LED,
			 "led_pattern_handler_config : led_reset failed");
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(ledNumber, DRV_RC_FAIL);
		}

		return err_code;
	}

	g_led_data[ledNumber].type = type;
	g_led_data[ledNumber].pattern = pattern;
	g_led_data[ledNumber].timer_led =
	    timer_create((T_ENTRY_POINT) timer_callback, &g_led_data[ledNumber],
			 delay, repeat, start, &localErr);

	switch (type) {
	case LED_NONE:
		/* can be used to just reset the driver. */
		led_shutdown();
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(ledNumber, DRV_RC_OK);
		}
		return DRV_RC_OK;
		break;

	case LED_BLINK_X1:
		if (pattern->intensity) {
			/* Add 100 factor to increase accuracy for computation */
			g_led_data[ledNumber].intensity_x_100 =
			    pattern->intensity * 100;
			err_code =
			    led_pattern_program_blink_x1(&g_led_data[ledNumber]);
		}
		break;

	case LED_BLINK_X2:
		if ((pattern->intensity)
		    && (pattern->repetition_count)
		    && (pattern->repetition_count <= MAX_REPEAT_COUNT)) {
			/* Add 100 factor to increase accuracy for computation */
			g_led_data[ledNumber].intensity_x_100 =
			    pattern->intensity * 100;
			g_led_Pattern_Blink_x2 = *pattern;
			err_code =
			    led_pattern_program_blink_x2(&g_led_data
							 [ledNumber]);
		}
		break;

	case LED_BLINK_X3:
		if (led_callback_func_ptr != NULL) {
			led_callback_func_ptr(ledNumber, DRV_RC_MODE_NOT_SUPPORTED);
		}
		break;

	case LED_WAVE_X2:
		if ((pattern->intensity)
		    && (pattern->repetition_count)
		    && (pattern->repetition_count <= MAX_REPEAT_COUNT)) {
			/* Add 100 factor to increase accuracy for computation */
			g_led_data[ledNumber].intensity_x_100 =
			    pattern->intensity * 100;
			g_led_Pattern_Wave_x2 = *pattern;
			err_code =
			    led_pattern_program_wave_x2(&g_led_data[ledNumber]);
		}
		break;

	default:
		err_code = DRV_RC_FAIL;
		pr_debug(LOG_MODULE_MAIN,
			 "led_pattern_handler_config : pattern type unknown");
		break;
	}
	if ((err_code != DRV_RC_OK) && (led_callback_func_ptr != NULL)) {
		led_callback_func_ptr(ledNumber, DRV_RC_FAIL);
	}

	return 0;
}

void led_set_pattern_callback(void (*evt_callback_func) (uint8_t, uint8_t),
			      uint8_t led_count,
			      struct led led_config[])
{
	led_callback_func_ptr = evt_callback_func;
}

static int led_init(struct device *dev)
{
	pr_debug(LOG_MODULE_LED, "Led drivers initialization");
	led_callback_func_ptr = NULL;
	uint8_t led_num;

	for (led_num = 0; led_num < NB_LED_AVAILABLE; led_num++) {
		g_led_data[led_num].ledNB = led_num;
		g_led_data[led_num].timer_led = NULL;
	}

	pm_wakelock_init(&g_wave_config.wakelock, LED_WAKELOCK);

	return DRV_RC_OK;
}

struct driver soc_led_driver = {
	.init = led_init,
	.suspend = NULL,
	.resume = NULL
};
