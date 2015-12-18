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

#include "scss_registers.h"

#include "machine.h"
#include "drivers/soc_comparator.h"
#include "drivers/soc_gpio.h"
#include "util/cunit_test.h"
#include "infra/device.h"
#include "infra/time.h"

#define MAGIC 0xdeadbeef

#define TEST_GPIO_PIN_OUT 5
#define COMPARATOR_PIN    6

/* 1 second for timeout duration */
#define TIMEOUT_DURATION 32000

static volatile int comparator_triggered = 0;

void comparator_callback(void *param)
{
	CU_ASSERT("Magic does not match", (int)param == MAGIC);
	comparator_triggered = 1;
}

void comparator_test()
{
        uint32_t timeout;
	uint8_t init_tst_soc_input_pin_mode = GET_PIN_MODE(COMPARATOR_PIN);
	uint8_t init_tst_soc_output_pin_mode = GET_PIN_MODE(TEST_GPIO_PIN_OUT);
	gpio_cfg_data_t pin_cfg;
	struct device* comparator_test = &pf_device_soc_comparator;
	cu_print("##################################################\n");
	cu_print("#                                                #\n");
	cu_print("#    !!! Pins %d and %d must be connected !!!      #\n",
	       COMPARATOR_PIN, TEST_GPIO_PIN_OUT);
	cu_print("#                                                #\n");
	cu_print("##################################################\n");

	struct device *gpio_dev = &pf_device_soc_gpio_32;

	SET_PIN_MODE(COMPARATOR_PIN, QRK_PMUX_SEL_MODEB);
	SET_PIN_MODE(TEST_GPIO_PIN_OUT, QRK_PMUX_SEL_MODEA);

	/* test configuration */
	pin_cfg.gpio_type = GPIO_OUTPUT;
	pin_cfg.int_type = LEVEL;
	pin_cfg.int_polarity = ACTIVE_LOW;
	pin_cfg.int_debounce = DEBOUNCE_OFF;
	pin_cfg.int_ls_sync = LS_SYNC_OFF;

	/*    VRef A is the tension on AREF_PAD (depend on R113 & R120) */
	/*    VRef B is VREF_OUT (adjustable between 0.98 and 1.2 V) */

	soc_gpio_set_config(gpio_dev, TEST_GPIO_PIN_OUT, &pin_cfg);
	soc_gpio_write(gpio_dev, TEST_GPIO_PIN_OUT, 1);
	/* test polarity negative, ref sel A  */
	comp_configure(comparator_test, COMPARATOR_PIN, CMP_REF_POL_NEG, CMP_REF_SEL_A,
		       comparator_callback, (void *)MAGIC);
	soc_gpio_write(gpio_dev, TEST_GPIO_PIN_OUT, 0);
	timeout = get_uptime_ms() + TIMEOUT_DURATION;
	while (!comparator_triggered && timeout < get_uptime_ms()) ;
	CU_ASSERT("Interrupt not triggered", comparator_triggered);

	/* test polarity positive, ref sel A */
	comparator_triggered = 0;
	comp_configure(comparator_test, COMPARATOR_PIN, CMP_REF_POL_POS, CMP_REF_SEL_A,
		       comparator_callback, (void *)MAGIC);
	soc_gpio_write(gpio_dev, TEST_GPIO_PIN_OUT, 1);
	timeout = get_uptime_ms() + TIMEOUT_DURATION;
	while (!comparator_triggered && timeout < get_uptime_ms()) ;
	CU_ASSERT("Interrupt not triggered", comparator_triggered);

	/* test polarity negative, ref sel B */
	comp_configure(comparator_test, COMPARATOR_PIN, CMP_REF_POL_NEG, CMP_REF_SEL_B,
	comparator_callback, (void *)MAGIC);
	soc_gpio_write(gpio_dev, TEST_GPIO_PIN_OUT, 0);
	timeout = get_uptime_ms() + TIMEOUT_DURATION;
	while (!comparator_triggered && timeout < get_uptime_ms()) ;
	CU_ASSERT("Interrupt not triggered", comparator_triggered);

	/* test polarity positive, ref sel B */
	comparator_triggered = 0;
	comp_configure(comparator_test, COMPARATOR_PIN, CMP_REF_POL_POS, CMP_REF_SEL_B,
	 	       comparator_callback, (void *)MAGIC);
	soc_gpio_write(gpio_dev, TEST_GPIO_PIN_OUT, 1);
	timeout = get_uptime_ms() + TIMEOUT_DURATION;
	while (!comparator_triggered && timeout < get_uptime_ms()) ;
	CU_ASSERT("Interrupt not triggered", comparator_triggered);

	SET_PIN_MODE(COMPARATOR_PIN, init_tst_soc_input_pin_mode);
	SET_PIN_MODE(TEST_GPIO_PIN_OUT, init_tst_soc_output_pin_mode);
}
