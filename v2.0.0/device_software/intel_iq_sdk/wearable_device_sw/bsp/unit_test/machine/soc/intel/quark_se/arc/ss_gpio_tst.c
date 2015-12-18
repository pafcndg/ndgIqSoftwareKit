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

#include "util/cunit_test.h"
#include "drivers/ss_gpio_iface.h"
#include "machine.h"

// Pin definition for ss GPIO service test

#define TST_SS_INPUT_PIN      2     //(GPIO SS 10)
#define TST_SS_OUTPUT_PIN     3     //(GPIO SS 11)
#define MUX_SS_GPIO_10_PWM0    63
#define MUX_SS_GPIO_11_PWM1    64

#define PRIV_KEY        0xdeadcafe

static void trans_delay(void)
{
    local_task_sleep_ms(100);
}

static volatile uint32_t my_callback_counter = 0;
static volatile bool     my_callback_state = false;

// Callback function for tests
static void my_callback(bool state, void* priv)
{
    if(*((uint32_t*)(priv)) == PRIV_KEY) {
        my_callback_counter += 1;
        my_callback_state = state;
    }
}

static DRIVER_API_RC ss_gpio_deconfig(struct device *dev, uint8_t bit)
{
    gpio_cfg_data_t config;
    // Default configuration (input pin without interrupt)
    config.gpio_type = GPIO_INPUT;
    config.int_type = EDGE;
    config.int_polarity = ACTIVE_LOW;
    config.int_debounce = DEBOUNCE_OFF;
    config.int_ls_sync = LS_SYNC_OFF;
    config.gpio_cb = NULL;
    config.gpio_cb_arg = NULL;

    return ss_gpio_set_config(dev, bit, &config);
}

static DRIVER_API_RC ss_gpio_test_pin(struct device *dev, unsigned int input_pin, unsigned int output_pin)
{
    DRIVER_API_RC ret;
    bool value;

    gpio_cfg_data_t in_pin_cfg;
    gpio_cfg_data_t out_pin_cfg;

    in_pin_cfg.gpio_type = GPIO_INPUT;
    in_pin_cfg.int_type = LEVEL;
    in_pin_cfg.int_polarity = ACTIVE_LOW;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.int_ls_sync = LS_SYNC_OFF;
    in_pin_cfg.gpio_cb = NULL;

    out_pin_cfg.gpio_type = GPIO_OUTPUT;
    out_pin_cfg.int_type = LEVEL;
    out_pin_cfg.int_polarity = ACTIVE_LOW;
    out_pin_cfg.int_debounce = DEBOUNCE_OFF;
    out_pin_cfg.int_ls_sync = LS_SYNC_OFF;
    out_pin_cfg.gpio_cb = NULL;

    if((ret =  ss_gpio_set_config(dev, input_pin, &in_pin_cfg)) != DRV_RC_OK) {
        cu_print("Error pin %d config (%d)\n", input_pin, ret);
        return ret;
    }
    if((ret =  ss_gpio_set_config(dev, output_pin, &out_pin_cfg)) != DRV_RC_OK) {
        cu_print("Error pin %d config (%d)\n", output_pin, ret);
        ss_gpio_deconfig(dev, input_pin);
        return ret;
    }

    // Test LOW
    if((ret =  ss_gpio_write(dev, output_pin, 0)) != DRV_RC_OK) {
        cu_print("Error pin %d write 1 (%d)\n", output_pin, ret);
        goto fail;
    }
    trans_delay(); // Delay a little bit
    if((ret =  ss_gpio_read(dev, input_pin, &value)) != DRV_RC_OK) {
        cu_print("Error pin %d read 1 (%d)\n", input_pin, ret);
        goto fail;
    }
    if(value) {
        cu_print("Error pin %d is at 1, 0 expected\n", input_pin);
        ret = DRV_RC_FAIL;
        goto fail;
    }

    // Test HIGH
    if((ret =  ss_gpio_write(dev, output_pin, 1)) != DRV_RC_OK) {
        cu_print("Error pin %d write 2 (%d)\n", output_pin, ret);
        goto fail;
    }
    trans_delay(); // Delay a little bit
    if((ret =  ss_gpio_read(dev, input_pin, &value)) != DRV_RC_OK) {
        cu_print("Error pin %d read 2 (%d)\n", input_pin, ret);
        goto fail;
    }
    if(!value) {
        cu_print("Error pin %d is at 0, 1 expected\n", input_pin);
        ret = DRV_RC_FAIL;
        goto fail;
    }

    ret = DRV_RC_OK;

fail:
    ss_gpio_deconfig(dev, output_pin);
    ss_gpio_deconfig(dev, input_pin);
    return ret;
}

static DRIVER_API_RC ss_gpio_test_pin_int(struct device *dev, unsigned int input_pin, unsigned int output_pin)
{
    uint32_t priv = PRIV_KEY;
    gpio_cfg_data_t in_pin_cfg;
    gpio_cfg_data_t out_pin_cfg;

    out_pin_cfg.gpio_type = GPIO_OUTPUT;
    out_pin_cfg.int_type = LEVEL;
    out_pin_cfg.int_polarity = ACTIVE_LOW;
    out_pin_cfg.int_debounce = DEBOUNCE_OFF;
    out_pin_cfg.gpio_cb = NULL;

    // TODO: test return value. It should be ok if previous tests worked
    ss_gpio_set_config(dev, output_pin, &out_pin_cfg);

    // -----------------------------
    // Test ACTIVE_LOW interrupt
    // -----------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = EDGE;
    in_pin_cfg.int_polarity = ACTIVE_LOW;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;
    in_pin_cfg.gpio_cb_arg = &priv;

    ss_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    my_callback_counter = 0;
    ss_gpio_set_config(dev, input_pin, &in_pin_cfg);
    trans_delay(); // Delay a little bit
    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit

    // Check that interrupt is not triggered
    if(my_callback_counter != 0) {
        cu_print("Error: interrupt occured %d\n", my_callback_counter);
        goto fail;
    }

    // Trigger interrupt (active low)
    ss_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    // Check that interrupt is triggered
    if(my_callback_counter != 1) {
        cu_print("Error: interrupt test for ACTIVE_LOW returned %d\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != false) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }

    // deconfigure input pin for next test (active high)
    ss_gpio_deconfig(dev, input_pin);
    // -----------------------------
    // Test ACTIVE_HIGH interrupt
    // -----------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = EDGE;
    in_pin_cfg.int_polarity = ACTIVE_HIGH;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;

    // Test LOW
    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    my_callback_counter = 0;
    ss_gpio_set_config(dev, input_pin, &in_pin_cfg);
    trans_delay(); // Delay a little bit
    ss_gpio_write(dev, output_pin, 0);

    // Check that interrupt is not triggered
    if(my_callback_counter != 0) {
        cu_print("Error: interrupt test for ACTIVE_HIGH returned %d (should be 0)\n", my_callback_counter);
        goto fail;
    }
    // Check that interrupt is triggered (active high)
    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    if(my_callback_counter != 1) {
        cu_print("Error: interrupt test for ACTIVE_HIGH returned %d (should be 1)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != true) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }

    // deconfigure input pin for next test (double edge)
    ss_gpio_deconfig(dev, input_pin);

    // ----------------------------------------------
    // Test BOTH_EDGE interrupt rising/falling edge
    // ----------------------------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = DOUBLE_EDGE;
    in_pin_cfg.int_polarity = ACTIVE_HIGH;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;

    // Test LOW
    ss_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    ss_gpio_set_config(dev, input_pin, &in_pin_cfg);
    my_callback_counter = 0;
    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    // Check that interrupt is triggered
    if(my_callback_counter != 1) {
        cu_print("Error: interrupt test for DOUBLE_EDGE returned %d (should be 1)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != true) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }
    // Check that interrupt is triggered (double edge)
    ss_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    // Check that interrupt is not triggered
    if(my_callback_counter != 2) {
        cu_print("Error: interrupt test for DOUBLE_EDGE returned %d (should be 2)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != false) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }

    // ----------------------------------------------
    // Test BOTH_EDGE interrupt falling/rising edge
    // ----------------------------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = DOUBLE_EDGE;
    in_pin_cfg.int_polarity = ACTIVE_HIGH;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;

    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    ss_gpio_set_config(dev, input_pin, &in_pin_cfg);
    my_callback_counter = 0;
    trans_delay(); // Delay a little bit
    ss_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    // Check that interrupt is triggered
    if(my_callback_counter != 1) {
        cu_print("Error: first interrupt test for DOUBLE_EDGE returned %d (should be 1)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != false) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }
    // Check that interrupt is triggered (double edge)
    ss_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    // Check that interrupt is not triggered
    if(my_callback_counter != 2) {
        cu_print("Error: second interrupt test for DOUBLE_EDGE returned %d (should be 2)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != true) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }

    ss_gpio_deconfig(dev, output_pin);
    ss_gpio_deconfig(dev, input_pin);
    return DRV_RC_OK;
fail:
    ss_gpio_deconfig(dev, output_pin);
    ss_gpio_deconfig(dev, input_pin);
    return DRV_RC_FAIL;

}

void gpio_ss_test(void)
{
    DRIVER_API_RC ret;

    cu_print("\n##################################################");
    cu_print("\n#      Testing SS GPIO                           #");
    cu_print("\n#    !!! Pins %d and %d must be connected !!!    #", TST_SS_INPUT_PIN, TST_SS_OUTPUT_PIN);
    cu_print("\n#                                                #");
    cu_print("\n#  Purpose of GPIOs tests on port:               #");
    cu_print("\n#            Validate input/output ports         #");
    cu_print("\n#            Validate interrupts (edge low)      #");
    cu_print("\n#            Validate interrupts (double edge)   #");
    cu_print("\n##################################################\n");

    struct device *ss_dev = &pf_device_ss_gpio_8b1;

    // retrieving previous mux configurations for TST_SS_INPUT_PIN and TST_SS_OUTPUT_PIN
    uint8_t init_tst_ss_input_pin_mode = GET_PIN_MODE(MUX_SS_GPIO_10_PWM0);
    uint8_t init_tst_ss_output_pin_mode = GET_PIN_MODE(MUX_SS_GPIO_11_PWM1);

    SET_PIN_MODE(MUX_SS_GPIO_10_PWM0, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(MUX_SS_GPIO_11_PWM1, QRK_PMUX_SEL_MODEA);

    ret = ss_gpio_test_pin(ss_dev, TST_SS_INPUT_PIN, TST_SS_OUTPUT_PIN);
    CU_ASSERT("Test for ss gpio pin failed", ret == DRV_RC_OK);

    ret = ss_gpio_test_pin_int(ss_dev, TST_SS_INPUT_PIN, TST_SS_OUTPUT_PIN);
    CU_ASSERT("Test for ss gpio pin interrupt failed", ret == DRV_RC_OK);

    SET_PIN_MODE(MUX_SS_GPIO_10_PWM0, init_tst_ss_input_pin_mode);
    SET_PIN_MODE(MUX_SS_GPIO_11_PWM1, init_tst_ss_output_pin_mode);
}
