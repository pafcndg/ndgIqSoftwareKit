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

#ifndef _NFC_STN54E_H_
#define _NFC_STN54E_H_

#include <stdint.h>
#include <stdbool.h>
#include "infra/pm.h"
// Internal includes
#include "os/os.h"
#include "drivers/gpio.h"
#include "drivers/serial_bus_access.h"

/**
 * @defgroup common_driver_nfc STN54 NFC Driver
 * This driver implements NFC STN54D/E chip support
 *
 * @ingroup common_drivers
 * @{
 */

/**
* NFC driver structure.
*/
extern struct driver nfc_stn54e_driver;

/**
* Internal variables used by the STN54E device driver
*/
struct nfc_stn54e_info {
	struct pm_wakelock wakelock;                   /*!< wakelock */
	uint8_t stn_reset_pin;                         /*!< STN reset pin number */
	uint8_t stn_pwr_en_pin;                        /*!< STN power enable pin number */
	uint8_t stn_irq_pin;                           /*!< STN IRQ_OUT pin number */
	uint8_t booster_reset_pin;                     /*!< Optional RF Booster reset pin */
	/* Internal driver fields */
	struct device *gpio_dev;                       /*!< GPIO device to use */
	struct sba_request req;                        /*!< sba request object used to transfer i2c data */
	T_SEMAPHORE        i2c_sync_sem;               /*!< Semaphore to wait for an i2c transfer to complete */
};

/**
 *  RX handler callback function signature
 *
 *  @param pointer to the sba_device of the NFC driver.
 */
typedef void (* nfc_stn54e_rx_handler_t)(void*);

/**
 *  Set the callback to handle rx events triggered on the IRQ_OUT pin by the
 *  NFC controller.
 *
 *  @param   handler pointer to the callback function
 */
void nfc_stn54e_set_rx_handler(nfc_stn54e_rx_handler_t handler);

/**
 *  Clear the rx data callback.
 */
void nfc_stn54e_clear_rx_handler();

/**
 *  Power up and reset the NFC controller and booster (if available).
 *  Same function can be used as a reset only.
 */
void nfc_stn54e_power_up(void);

/**
 *  Power down the NFC controller and booster (if available).
 */
void nfc_stn54e_power_down(void);

/**
 *  Read data from the NFC controller over the I2C bus.
 *
 *  @param   buf    pointer to destination buffer (max 255 bytes)
 *  @param   size   number of bytes to be read (max 255 bytes)
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 */
DRIVER_API_RC nfc_stn54e_read(uint8_t * buf,  uint8_t size);

/**
 *  Write data to the NFC controller over the I2C bus.
 *
 *  @param   buf    pointer to data source buffer (max 255 bytes)
 *  @param   size   number of bytes to be written (max 255 bytes)
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 */
DRIVER_API_RC nfc_stn54e_write(uint8_t * buf, uint8_t size);

/** @} */

#endif /* _NFC_STN54E_H_ */
