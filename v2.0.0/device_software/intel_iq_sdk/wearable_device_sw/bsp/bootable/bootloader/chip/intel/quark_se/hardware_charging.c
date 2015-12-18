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

#include "quark_se/board_intel.h"
#include "gpio/soc_gpio.h"
#include "reboot.h"
#include <stdbool.h>
#include <printk.h>

#define BATTERY_CHK 0x1

/* We use sticky register SCSS_GPS2 to know if we already run the COS */
static uint32_t battery_checked(void)
{
	return MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE, SCSS_GPS2) &
		BATTERY_CHK;
}

static void battery_checked_set(void)
{
	MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE, SCSS_GPS2) |= BATTERY_CHK;
}

static void battery_checked_clear(void)
{
	MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE, SCSS_GPS2) &= ~BATTERY_CHK;
}


static void fg_init(void)
{
	/* GPIO_SS 14, to enable fuel gauge */
	SET_PIN_MODE(67, QRK_PMUX_SEL_MODEA);
	/* AIN 4 is battery level */
	SET_PIN_MODE(4, QRK_PMUX_SEL_MODEB);
	cos_start();
}

static int battery_voltage(void)
{
	return batt_adc_to_mv(cos_adc_request(4));
}

static bool conductive_charger_available(void)
{
	return ((soc_gpio_read(0, 7) == 1) && (soc_gpio_read(1, 3) == 1));
}

static bool wireless_charger_available(void)
{
	return (soc_gpio_read(1, 3) == 0);
}

void hardware_charging(void)
{
	if (!battery_checked() && get_reset_reason() == RESET_HW
			&& conductive_charger_available() == false) {
		fg_init();

		while (battery_voltage() < CONFIG_BATT_THRESHOLD_MV) {
			if (conductive_charger_available())
				break;

			if (wireless_charger_available())
				mdelay(500);
			else
				shutdown(BATTERY_SHUTDOWN);
		}

		battery_checked_set();
		reboot(TARGET_MAIN);
	} else {
		battery_checked_clear();
	}
}
