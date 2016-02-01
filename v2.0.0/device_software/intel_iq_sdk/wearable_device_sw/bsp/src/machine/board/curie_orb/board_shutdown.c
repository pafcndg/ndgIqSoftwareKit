/* INTEL CONFIDENTIAL Copyright 2014-2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential information
 * of Intel or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 *
 ******************************************************************************/

#include <assert.h>
#include "infra/pm.h"
#include "infra/time.h"
#include "infra/soc_reboot.h"
#include "drivers/soc_gpio.h"
#include "drivers/serial_bus_access.h"
#include "drivers/usb_pm.h"
#include "soc_config.h"

/* GPIO[1] LOW = charger enable / HIGH = charger disable*/
#define GPIO_SOC_CD             (uint8_t)(1)

/* BQ25120 I2C address and register */
#define CHARGER_I2C_ADD 0x6A    /* Charger I2C address */
#define REG_STATUS_ADD 0x00     /* Status and Ship Mode control register */
#define REG_SHIPMODE_EN 0x20;   /* Enable ship mode */
#define CH_HIZ_ACTIVEBAT (1)    /* 1ms to pass from hi Z to active battery */

static void ch_cd_high(void)
{
	uint32_t start_time;

	/* We suppose the CD gpio has already been correctly configured as
	 * output */
	soc_gpio_write(&pf_device_soc_gpio_32, GPIO_SOC_CD, 1);
	start_time = get_uptime_ms();
	/* spin 1 ms to wait until the charger is connected */
	while ((get_uptime_ms() - start_time) < CH_HIZ_ACTIVEBAT) ;
}

/**@brief Function to enable ship mode:
 * => If USB plugged: board reboot
 * => If USB unplugged: board enter in ship mode
 */
void board_poweroff_hook(void)
{
	struct sba_request req;
	uint8_t tx_buff[2];

	if (usb_pm_is_plugged(&pf_device_usb_pm))
		soc_reboot();

	/* First raise the CD */
	ch_cd_high();
	/* Put the chager in shipment mode */
	tx_buff[0] = REG_STATUS_ADD;
	tx_buff[1] = REG_SHIPMODE_EN;
	req.bus_id = SBA_I2C_MASTER_1;
	req.addr.slave_addr = CHARGER_I2C_ADD;
	req.request_type = SBA_TX;
	req.tx_len = 2;
	req.tx_buff = tx_buff;
	req.callback = NULL; /* IRQs are disabled, callback is useless */
	/* Note this rest will succeed even if IRQ are disabled because the
	 * first FIFO is filled immediately. */
	sba_exec_request(&req);

	/* Battery will be disconnected soon! */
}

void board_shutdown_hook(void)
{
	board_poweroff_hook();
}
