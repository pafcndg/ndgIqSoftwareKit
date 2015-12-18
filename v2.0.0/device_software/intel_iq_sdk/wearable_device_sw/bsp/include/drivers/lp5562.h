/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential information of Intel
 * or its suppliers and licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual property right
 * is granted to or conferred upon you by disclosure or delivery of the Materials,
 * either expressly, by implication, inducement, estoppel or otherwise.
 *
 * Any license under such intellectual property rights must be express and approved by Intel in writing.
*/

#ifndef _LP5562_H_
#define _LP5562_H_

#include <stdint.h>
#include <stdbool.h>
#include "infra/pm.h"
/* Internal includes */
#include "os/os.h"
#include "drivers/serial_bus_access.h"

/**
 * @defgroup lp5562_driver lp5562 low level led driver
 * @{
 * @ingroup common_drivers
 */

/* LED id (for the set PWM/current commands) */
#define LED_B 0
#define LED_G 1
#define LED_R 2
#define LED_W 3

/* Config register bits */
#define REG_CONFIG_PWM_HF   (1<<6)
#define REG_CONFIG_PWR_SAVE (1<<5)
#define REG_CONFIG_EXT_CLK  (0)
#define REG_CONFIG_INT_CLK  (1)
#define REG_CONFIG_AUTO_CLK (2)

/* Start command masks */
/* Start program and run it until the end */
#define LED_EN1_RUN_MASK (2<<4)
#define LED_EN2_RUN_MASK (2<<2)
#define LED_EN3_RUN_MASK (2<<0)
/* Run one program command and increase PC */
#define LED_EN1_STEP_MASK (1<<4)
#define LED_EN2_STEP_MASK (1<<2)
#define LED_EN3_STEP_MASK (1<<0)
/* Run one program command */
#define LED_EN1_EXEC_MASK (0<<4)
#define LED_EN2_EXEC_MASK (0<<2)
#define LED_EN3_EXEC_MASK (0<<0)
/* Stop engine */
#define LED_EN1_HOLD_MASK (0<<4)
#define LED_EN2_HOLD_MASK (0<<2)
#define LED_EN3_HOLD_MASK (0<<0)

/* Set mode command masks */
/* Set engine in RUN mode */
#define LED_EN1_RUN_MODE (2<<4)
#define LED_EN2_RUN_MODE (2<<2)
#define LED_EN3_RUN_MODE (2<<0)
/* Set engine in Direct Control mode (PWM) */
#define LED_EN1_DC_MODE (3<<4)
#define LED_EN2_DC_MODE (3<<2)
#define LED_EN3_DC_MODE (3<<0)
/* Disable engine */
#define LED_EN1_STOP_MODE (0<<4)
#define LED_EN2_STOP_MODE (0<<2)
#define LED_EN3_STOP_MODE (0<<0)

/* Engine id for the LED_MAP macro */
#define LED_EN1 (1)
#define LED_EN2 (2)
#define LED_EN3 (3)
#define LED_PWM (0)
/* led map register bits */
#define REG_LEDMAP_SHIFT_B (0)
#define REG_LEDMAP_SHIFT_G (2)
#define REG_LEDMAP_SHIFT_R (4)
#define REG_LEDMAP_SHIFT_W (6)
#define LED_MAP(engine, RGB) (engine << REG_LEDMAP_SHIFT_ ## RGB)

/**
* LED driver structure
*/
extern struct driver led_lp5562_driver;

/**
* Structure to handle a LP5562 device
*/
struct lp5562_info {
	uint8_t led_en_pin;              /*!< gpio pin for the led enable feature */
	uint8_t led_map;                 /*!< default led map to use */
	uint8_t config;                  /*!< default configuration to use */
	struct pm_wakelock wakelock;     /*!< wakelock */
	/* Internal driver fields */
	struct sba_request req;          /*!< sba request object used to transfer i2c data */
	T_SEMAPHORE        i2c_sync_sem; /*!< Semaphore to wait for an i2c transfer to complete */
	struct device      *led_en_dev;  /*!< gpio device handler for led_enable */
};

/**
* Structure to program led patterns
*/
struct lp5562_pattern {
    uint16_t *pattern;
    uint8_t size;
};

/**
* Enable/disable lp5562 device.
*
* @param dev led device to use
* @param enable enable or disable action
*/
void led_lp5562_enable(struct device *dev, bool enable);

/**
* Reset lp5562 device.
*
* @param dev led device to use
*/
int led_lp5562_reset(struct device *dev);

/**
* Get lp5562 device status register.
*
* @param dev led device to use
*/
uint8_t led_lp5562_get_status(struct device *dev);

/**
* Set lp5562 device configuration
*
* @param dev led device to use
* @param config new configuration to send (default is internal clock, low power)
* @param led_map new led_map to send. default is
*                LED_MAP(LED_EN3, R) | LED_MAP(LED_EN2, G) | LED_MAP(LED_EN1, B) | LED_MAP(PWM, W)
*/
void led_lp5562_config(struct device *dev, uint8_t config, uint8_t led_map);

/**
* Set lp5562 device led current.
*
* @param dev led device to use
* @param led led to configure (LED_B, lED_G, lED_R or LED_W)
* @param current current value (0->255, 0mA->25.5mA)
*/
void led_lp5562_set_current(struct device *dev, uint8_t led, uint8_t current);

/**
* Set lp5562 device led PWM value.
*
* @param dev led device to use
* @param led led to configure (LED_B, lED_G, lED_R or LED_W)
* @param pwm pwm value (0->255)
*/
void led_lp5562_set_pwm(struct device *dev, uint8_t led, uint8_t pwm);

/**
* Send engine programs to lp5562.
*
* @param dev led device to use
* @param p_eng1 pattern to program for engine1
* @param p_eng2 pattern to program for engine2
* @param p_eng3 pattern to program for engine3
*/
void led_lp5562_program_pattern(struct device *dev,
                                struct lp5562_pattern *p_eng1,
                                struct lp5562_pattern *p_eng2,
                                struct lp5562_pattern *p_eng3);

/**
* Start engines on lp5562 device.
*
* @param dev led device to use
* @param run_mask engines to start
* 	(LED_EN1_RUN_MASK | LED_EN2_RUN_MASK | LED_EN3_RUN_MASK)
*/
void led_lp5562_start(struct device *dev, uint8_t run_mask);

/**
* Set engine mode on lp5562 device.
*
* @param dev led device to use
* @param mode_mask mode to operate device engines
* 	(LED_EN1_RUN_MODE | LED_EN2_DC_MODE | LED_EN3_STOP_MODE)
*/
void led_lp5562_set_mode(struct device *dev, uint8_t mode_mask);

/** @} */

#endif /* _LP5562_H_ */
