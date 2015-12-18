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

#ifndef _APDS9190_H_
#define _APDS9190_H_

#include <stdint.h>
#include <stdbool.h>
#include "infra/pm.h"
/* Internal includes */
#include "os/os.h"
#include "drivers/serial_bus_access.h"

/**
 * @defgroup apds9190 APDS9190 Driver
 * This driver implements APDS9190 chip support
 * @ingroup common_drivers
 * @{
 */

/**
* IR Proximity driver structure
*/
extern struct driver apds9190_driver;

/**
* Structure to handle and configure an apds9190 device
*/
struct apds9190_info {
	/* Configuration fields */
	uint8_t ptime;    /*!< Proximity ADC time */
	uint8_t wtime;    /*!< Wait time */
	uint8_t pers;     /*!< Interrupt persistence filter */
	uint8_t config;   /*!< Configuration */
	uint8_t ppcount;  /*!< Proximity pulse count */
	uint16_t pilt;    /*!< Proximity low threshold */
	uint16_t piht;    /*!< Proximity high threshold */
	/* Internal driver fields */
	struct sba_request req;          /*!< sba request object used to transfer i2c data */
	T_SEMAPHORE        i2c_sync_sem; /*!< Semaphore to wait for an i2c transfer to complete */
};


/**
 * Return the current proximity value
 *
 * \param dev the device structure
 * \return positive value of proximity if > 0 the value is the raw adc value
 *         read between 0 and 1023
 *         negative means error
 */
int apds9190_read_prox(struct device *dev);

/** @} */
#endif /* _LED_apds9190_H_ */
