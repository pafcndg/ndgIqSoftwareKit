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
#include "infra/log.h"
#include "drivers/serial_bus_access.h"
#include "drivers/haptic/drv2605.h"
#include "drivers/haptic/drv2605_regs.h"
#include "infra/pm.h"
#include "infra/wakelock_ids.h"

/* According DRV2605 datasheet : ERM Library table
 * The longest wave (number 16) take 1s */
#define MAX_WAVE_DURATION    1000



/******************************************/
/************* LOCAL FUNCTIONS ************/
/******************************************/

static void timer_step_callback(struct device *dev);
static void program_one_wave(uint8_t wave, struct device *dev);
static void vibr_reset(struct device *dev);
static int8_t vibr_pattern_program_special(struct device *dev);
static int8_t vibr_pattern_program_square_x2(struct device *dev);
static void vibr_exit(int8_t status, struct device *dev);

/**
 * Function to wait the end of I2C bus communication for 2s
 * @param  vibr haptic device private info
 * @return none
 */
static void wait_i2c_complete(haptic_info_t *vibr)
{
	if (semaphore_take(vibr->sema_wait_i2c, DELAY_I2C_RSP) != E_OS_OK)
		panic(E_OS_ERR_TIMEOUT);
}

/**
 * Function to read in DRV2605 register
 * @param  reg register to read
 * @param  err pointer to read status
 * @param  haptic device
 * @return register value
 */
static int8_t drv260x_read_reg(uint8_t reg, DRIVER_API_RC * err, struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	uint8_t trx_buff = reg;
	uint8_t rx_buff = 0;
	*err = DRV_RC_FAIL;

	vibr->trx_request->tx_buff = &trx_buff;
	vibr->trx_request->rx_buff = &rx_buff;
	if (sba_exec_dev_request((struct sba_device*)dev, vibr->trx_request) == DRV_RC_OK) {
		/* Wait for transfert to complete */
		wait_i2c_complete(vibr);
		if (vibr->trx_request->status == 0)
			*err = DRV_RC_OK;
	}

	return rx_buff;
}

/**
 * Function to write in DRV2605 register
 * @param  reg register to write
 * @param  val value to write
 * @param  haptic device
 * @return write status
 */
static DRIVER_API_RC drv260x_write_reg(uint8_t reg, uint8_t val, struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	DRIVER_API_RC rc = DRV_RC_FAIL;
	uint8_t tx_buff[1][2] = { {reg, val} };

	vibr->tx_request->tx_buff = tx_buff[0];
	rc = sba_exec_dev_request((struct sba_device*)dev, vibr->tx_request);
	if (rc == DRV_RC_OK) {
		/* Wait for write to complete */
		wait_i2c_complete(vibr);
		if (vibr->tx_request->status == 0)
			return DRV_RC_OK;
	}
	return rc;
}

/**
 * I2C callback function.
 * This function is called when an i2c communication is complete
 * @param request pointer to request data
 */
static void drv2650_i2c_callback(sba_request_t * request)
{
	OS_ERR_TYPE err;
	T_SEMAPHORE sema = (T_SEMAPHORE) request->priv_data;
	semaphore_give(sema, &err);
}


/**
 * I2C bus initialisation function
 * @param  vibr haptic device private info
 * @return request status
 */
static void init_requests_i2c(haptic_info_t *vibr)
{
	vibr->tx_request = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
	vibr->trx_request = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);

	/* Creation of a Tx request */
	vibr->tx_request->request_type = SBA_TX;
	vibr->tx_request->tx_len = 2;
	vibr->tx_request->addr.slave_addr = DEVICE_ADDR;
	vibr->tx_request->callback = drv2650_i2c_callback;
	vibr->tx_request->priv_data = vibr->sema_wait_i2c;

	/* Creation of a Transaction request */
	vibr->trx_request->request_type = SBA_TRANSFER;
	vibr->trx_request->tx_len = 1;
	vibr->trx_request->rx_len = 1;
	vibr->trx_request->addr.slave_addr = DEVICE_ADDR;
	vibr->trx_request->callback = drv2650_i2c_callback;
	vibr->trx_request->priv_data = vibr->sema_wait_i2c;

}

/**
  * Set the DRV2605 register
  * When the function is complete, the DRV2605 is ready to play pattern
  * @param  haptic device
  * @return configuration status
  */
static DRIVER_API_RC config_drv2605(struct device *dev, vibration_type type)
{
	DRIVER_API_RC error, rc = DRV_RC_OK;
	uint8_t ctrl_3_val;
	uint8_t ctrl_2_val;
	uint8_t rated_voltage_val;

	/* out of stand by + internal trig => MODE_REG = 0 */
	rc = drv260x_write_reg(MODE_REG, 0, dev);

	/* set mode */
	/* No over drive => OVERDRIVE_REG = 0 */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(OVERDRIVE_REG, 0, dev);
	/* No sustain pos => SUSTAIN_POS_REG = 0 */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(SUSTAIN_POS_REG, 0, dev);
	/* No sustain neg => SUSTAIN_NEG_REG = 0 */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(SUSTAIN_NEG_REG, 0, dev);
	/* No BREAK => BREAK_REG = 0 */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(BREAK_REG, 0, dev);
	/* no LRA => FEEDBACK_REG & 0x7F */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(FEEDBACK_REG,
				       drv260x_read_reg(FEEDBACK_REG,
							&error, dev) & 0x7F, dev);
	/* lib 4 : wave duration is about 140ms */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(LIBRARY_SELECTION_REG, CURRENT_LIBRARY, dev);

	if (type == VIBRATION_SPECIAL_EFFECTS) {
		/* Use default value */
		ctrl_3_val = DEFAULT_CTRL_3;
		ctrl_2_val = DEFAULT_CTRL_2;
		rated_voltage_val = DEFAULT_RATED_VOLTAGE;
	} else {
		/* ERM close loop and RTP IN unsigned value */
		ctrl_3_val = ERM_CLOSE_LOOP;
		/* unidirectional input mode */
		ctrl_2_val = UNIDIRECTIONAL_INPUT;
		/* output voltage for full scale */
		rated_voltage_val = RATED_VOLTAGE_STRONGER;
	}
	/* Write register */
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(CONTROL3_REG, ctrl_3_val, dev);
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(CONTROL2_REG, ctrl_2_val, dev);
	if (rc == DRV_RC_OK)
		rc = drv260x_write_reg(RATED_VOLTAGE_REG, rated_voltage_val, dev);

	return rc;
}

/**
  * This function initialise one wave register
  * @param  wave vibration wave id to play (1 < id < 127)
  * @param haptic device
  * @return none
  */
static void program_one_wave(uint8_t wave, struct device *dev)
{
	/* launch the wave */
	drv260x_write_reg(WAVEQ1_REG, wave, dev);
	drv260x_write_reg(WAVEQ2_REG, 0, dev);
}


/**
  * Timer callback function
  * This function is called each time a pattern duration is over
  * It allow to continue to play the pattern
  * @param haptic device
  */
static void timer_step_callback(struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	int8_t err_code = DRV_RC_OK;
	pm_wakelock_acquire(&vibr->wakelock);

	timer_stop(vibr->t_step, NULL);

	switch (vibr->type) {
	case VIBRATION_NONE:
		pr_debug(LOG_MODULE_VIBR,
			 "timer step callback : no pattern selected");
		/* can be used to just reset the driver. */
		break;

	case VIBRATION_SPECIAL_EFFECTS:
		/* Increase pattern step to perform next special effect step */
		vibr->pattern_step++;
		/* Check if the pattern is finished */
		if (vibr->pattern_step <= SPECIAL_EFFECT_5)
			err_code = vibr_pattern_program_special(dev);
		else
			vibr->type = VIBRATION_NONE;
		break;

	case VIBRATION_SQUARE_X2:
		vibr->pattern_step++;

		if (vibr->pattern_step > SQUARE_T_OFF_2){
			/* reset pattern step for next repetition */
			vibr->pattern_step = SQUARE_T_ON_1;
		}
		/* Check repetion number */
		if (vibr->current_count <=
		    vibr->pattern->square_x2.repetition_count) {
			err_code = vibr_pattern_program_square_x2(dev);
		} else
			vibr->type = VIBRATION_NONE;
		break;

	default:
		err_code = DRV_RC_FAIL;
		pr_debug(LOG_MODULE_VIBR,
			 "timer step callback : step pattern undefined");
		break;
	}

	if (err_code != DRV_RC_OK) {
		pr_error(LOG_MODULE_VIBR,
			 "timer step callback : err_code with pattern %d",
			 vibr->type);
		vibr_exit(err_code, dev);
		return;
	}

	if (vibr->type == VIBRATION_NONE) {
		vibr_exit(err_code, dev);
		return;
	}
	pm_wakelock_release(&vibr->wakelock);
}

/**
  * Set the driver to perform a special effect pattern.
  * @param  haptic device
  * @return pattern execution status
  */
static int8_t vibr_pattern_program_special(struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	OS_ERR_TYPE localErr = E_OS_OK;
	int8_t err_code = DRV_RC_OK;

	uint16_t tab_pattern[] = {
		vibr->pattern->special_effect.effect_1,
		vibr->pattern->special_effect.duration_off_1,
		vibr->pattern->special_effect.effect_2,
		vibr->pattern->special_effect.duration_off_2,
		vibr->pattern->special_effect.effect_3,
		vibr->pattern->special_effect.duration_off_3,
		vibr->pattern->special_effect.effect_4,
		vibr->pattern->special_effect.duration_off_4,
		vibr->pattern->special_effect.effect_5};

	if (vibr->pattern_step%2) {
		/* odd patern step: wait for a duration off */
		timer_start(vibr->t_step,
			    tab_pattern[vibr->pattern_step],
			    &localErr);
	}
	else {
		/* even pattern step: start wave */
		if (tab_pattern[vibr->pattern_step] >= FIRST_LIBRARY_WAVE_NUM
		    && tab_pattern[vibr->pattern_step] <= LAST_LIBRARY_WAVE_NUM) {
			program_one_wave(tab_pattern[vibr->pattern_step], dev);
			/* launch the wave */
			if ((err_code = drv260x_write_reg(GO_REG, 1, dev)) != DRV_RC_OK) {
				vibr->type = VIBRATION_NONE;
				return err_code;
			}
			/* Wait for wave duration */
			timer_start(vibr->t_step, MAX_WAVE_DURATION, &localErr);
		} else if (tab_pattern[vibr->pattern_step] == 0) {
			/* If effect is 0 => pattern is stopped */
			vibr->type = VIBRATION_NONE;
			return DRV_RC_OK;
		} else {
			/* Invalid effect effect => pattern is stopped */
			vibr->type = VIBRATION_NONE;
			return ((int8_t) DRV_RC_INVALID_OPERATION);
		}
	}
	if (localErr != E_OS_OK)
		err_code = DRV_RC_FAIL;

	return (err_code);
}

/**
  * Set the driver to perform a square_x2 pattern.
  * @param haptic device
  * @return pattern execution status
  */
static int8_t vibr_pattern_program_square_x2(struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	int8_t err_code = DRV_RC_OK;
	OS_ERR_TYPE localErr = E_OS_OK;

	uint16_t tab_pattern[] = {
		vibr->pattern->square_x2.duration_on_1,
		vibr->pattern->square_x2.duration_off_1,
		vibr->pattern->square_x2.duration_on_2,
		vibr->pattern->square_x2.duration_off_2};

	if (vibr->pattern_step%2) {
		/* odd pattern step: launch timer and stop wave */
		/* putting the devide in standby mode */
		if ((err_code = drv260x_write_reg(MODE_REG, STANDBY_MODE, dev)) != DRV_RC_OK) {
			vibr->type = VIBRATION_NONE;
			return err_code;
		}
		/* let off for duration_off_x ms */
		if (tab_pattern[vibr->pattern_step]) {
			timer_start(vibr->t_step,
			    tab_pattern[vibr->pattern_step],
			    &localErr);
		} else {
			err_code = DRV_RC_INVALID_OPERATION;
			pr_debug(LOG_MODULE_VIBR,
				 "pattern square_x2 : T_OFF = 0");
		}
		if (vibr->pattern_step == SQUARE_T_OFF_2) {
			/* Max number of pattern reached => exit */
			if (vibr->current_count == MAX_REPEAT_COUNT) {
				vibr->type = VIBRATION_NONE;
			} else
				/* Pattern is over -> increase the pattern counter */
				vibr->current_count++;
		}

	}
	else {
		/* even pattern step: launch timer and start wave */
		if (tab_pattern[vibr->pattern_step]) {
			timer_start(vibr->t_step,
			    tab_pattern[vibr->pattern_step],
			    &localErr);
			/* Start continuous wave */
			if ((err_code = drv260x_write_reg(MODE_REG, 5, dev)) != DRV_RC_OK) {
				vibr->type = VIBRATION_NONE;
				return err_code;
			}
		} else {
			err_code = DRV_RC_INVALID_OPERATION;
			pr_debug(LOG_MODULE_VIBR,
				 "pattern square_x2 : T_ON = 0");
		}
	}

	if (localErr != E_OS_OK)
		err_code = DRV_RC_FAIL;

	return (err_code);
}

/**
  * Initialise the global variable.
  * @param vibr pointer to vibration data
  */
static void vibr_vars_init(haptic_info_t * vibr)
{
	vibr->pattern_step = 0;
	vibr->current_count = 0;
	vibr->t_step = NULL;
	vibr->sema_wait_i2c = NULL;
	vibr->pattern = NULL;
	vibr->tx_request = NULL;
	vibr->trx_request = NULL;

}

/**
  * Reset the global resources and send status.
  * @param haptic device
  */
static void vibr_exit(int8_t status, struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	vibr_reset(dev);

	/* Stop and remove timer */
	if (vibr->t_step) {
		timer_stop(vibr->t_step, NULL);
		timer_delete(vibr->t_step, NULL);
	}
	/* remove semaphore */
	if (vibr->sema_wait_i2c)
		semaphore_delete(vibr->sema_wait_i2c, NULL);

	vibr->t_step = NULL;
	vibr->sema_wait_i2c = NULL;
	vibr->pattern = NULL;

	/* Send response */
	if (vibr->vibr_callback_func_ptr != NULL) {
		vibr->vibr_callback_func_ptr(status);
	}
	pm_wakelock_release(&vibr->wakelock);
}

/**
  * Free memory and put the device in standby mode.
  * @param haptic device
  */
static void vibr_reset(struct device *dev)
{
	haptic_info_t *vibr = (struct drv2605_info *)dev->priv;
	/* Put devide in standby mode */
	drv260x_write_reg(MODE_REG, STANDBY_MODE, dev);

	if (vibr->tx_request)
		bfree(vibr->tx_request);
	if (vibr->trx_request)
		bfree(vibr->trx_request);

}

/*******************************************/
/************* GLOBAL FUNCTIONS ************/
/*******************************************/

static int vibr_init(struct device *dev)
{
	struct drv2605_info *vibr = (struct drv2605_info *)dev->priv;
	pr_info(LOG_MODULE_VIBR, "DRV2605 Haptic driver initialization");
	vibr->vibr_callback_func_ptr = NULL;

	/* Create resources to put device in STANDBY_MODE */
	vibr->sema_wait_i2c = semaphore_create(0, NULL);
	init_requests_i2c(vibr);
	/* Put devide in standby mode */
	drv260x_write_reg(MODE_REG, STANDBY_MODE, dev);

	/* Reset resources used to put device in STANDBY_MODE */
	if (vibr->tx_request)
		bfree(vibr->tx_request);
	if (vibr->trx_request)
		bfree(vibr->trx_request);
	semaphore_delete(vibr->sema_wait_i2c, NULL);

	pm_wakelock_init(&vibr->wakelock, HAPTIC_WAKELOCK);
	return 0;
}

struct driver drv2605_driver = {
    .init = vibr_init,
    .suspend = NULL,
    .resume = NULL
};

/*******************************************/
/******** HAPTIC API IMPLEMENTATION ********/
/*******************************************/
// FIXME should be somewhere else

int8_t haptic_play(vibration_type type, vibration_u * pattern)
{
	struct device *dev = (struct device *)&pf_sba_device_drv2605;
	struct drv2605_info *vibr = (struct drv2605_info *)dev->priv;

	DRIVER_API_RC err_code = DRV_RC_OK;
	OS_ERR_TYPE localErr = E_OS_OK;
	bool start = false;
	bool repeat = false;
	int8_t delay = 0;
	int8_t status = 0;
	uint8_t device_id = 0;

	pm_wakelock_acquire(&vibr->wakelock);

	/* Reset value */
	vibr_vars_init(vibr);

	/* Create semaphore */
	vibr->sema_wait_i2c = semaphore_create(0, NULL);

	if (vibr->sema_wait_i2c == NULL) {
		err_code = DRV_RC_FAIL;
		goto ret_fail;
	}

	init_requests_i2c(vibr);

	/* Check status for devide_id = DRV2605 */
	pr_debug(LOG_MODULE_VIBR, "Check status drv2605");
	status = drv260x_read_reg(STATUS_REG, &err_code, dev);

	/* Read device ID */
	device_id = (status & DEV_ID_MASK);
	switch (device_id) {
	case DRV2605:
		pr_debug(LOG_MODULE_VIBR, "driver found: drv2605.");
		break;
	case DRV2604:
		pr_debug(LOG_MODULE_VIBR, "driver found: drv2604.");
		err_code = DRV_RC_MODE_NOT_SUPPORTED;
		goto ret_fail;
		break;
	default:
		pr_debug(LOG_MODULE_VIBR, "driver found: unknown. %x",status);
		err_code = DRV_RC_MODE_NOT_SUPPORTED;
		goto ret_fail;
		break;
	}

	err_code = config_drv2605(dev, type);
	if (err_code != DRV_RC_OK) {
		pr_debug(LOG_MODULE_VIBR,
			 "Could not initialise DRV2605 chipset");
		goto ret_fail;
	}

	/* Create step timer */
	vibr->t_step = timer_create((T_ENTRY_POINT) timer_step_callback,
					  dev, delay, repeat, start,
					  &localErr);

	if (vibr->t_step == NULL) {
		err_code = DRV_RC_FAIL;
		goto ret_fail;
	}

	/* Store pattern type */
	vibr->type = type;
	vibr->pattern = pattern;

	switch (type) {
	case VIBRATION_NONE:
		/* can be used to just reset the driver. */
		vibr_exit(DRV_RC_OK, dev);
		return DRV_RC_OK;
		break;

	case VIBRATION_SPECIAL_EFFECTS:
		vibr->pattern->special_effect = pattern->special_effect;
		/* Don't play a pattern with 1st effect null */
		if (vibr->pattern->special_effect.effect_1)
			err_code = vibr_pattern_program_special(dev);
		else
			err_code = DRV_RC_INVALID_OPERATION;
		break;

	case VIBRATION_SQUARE_X2:
		vibr->pattern->square_x2 = pattern->square_x2;
		/* Set pattern amplitude */
		if ((err_code = drv260x_write_reg(RTPIN_REG,
			vibr->pattern->square_x2.amplitude, dev)) == DRV_RC_OK) {
			err_code = vibr_pattern_program_square_x2(dev);
		}
		break;
	default:
		err_code = DRV_RC_FAIL;
		pr_debug(LOG_MODULE_VIBR,
			 "vibr_pattern_handler_config : pattern type unknown");
		break;
	}

	/* If configuration is OK => return OK */
	if (err_code == DRV_RC_OK) {
		pm_wakelock_release(&vibr->wakelock);
		return err_code;
	}

ret_fail:
	vibr_exit(err_code, dev);

	return err_code;
}

void haptic_init(void (*evt_callback_func) (int8_t))
{
	struct device *dev = (struct device *)&pf_sba_device_drv2605;
	struct drv2605_info *vibr = (struct drv2605_info *)dev->priv;
	vibr->vibr_callback_func_ptr = evt_callback_func;
}
