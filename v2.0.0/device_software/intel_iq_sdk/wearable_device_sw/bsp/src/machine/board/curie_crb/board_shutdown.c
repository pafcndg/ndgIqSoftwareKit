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
#include "drivers/soc_gpio.h"
#include "infra/log.h"
#include "soc_config.h"
#include "infra/pm.h"
#include "misc/printk.h"

#include "machine.h"

#define POWER_LATCH_GPIO	11

void board_poweroff_hook(void)
{
	printk("disconnect battery\n");
	//release the power latch.(GPIO[11])
	//configured as output during bootloader.
#ifdef CONFIG_SOC_GPIO_32
	while (1)
		soc_gpio_write(&pf_device_soc_gpio_32, POWER_LATCH_GPIO, 0);
#endif
}

void board_shutdown_hook(void)
{
	board_poweroff_hook();
}
