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

#include "drivers/soc_gpio.h"
#include "machine.h"
#include "util/cunit_test.h"
#include "scss_registers.h"

#define TXDELAY 500000000
#define PRIV_KEY 0xdeadbeef

#if defined (CONFIG_BOARD_CURIE_APP)
#define TST_SOC_32_INPUT_PIN  5
#define TST_SOC_32_OUTPUT_PIN 6
#define TST_SOC_AON_INPUT_PIN  4
#define TST_SOC_AON_OUTPUT_PIN 5
#endif

#if defined (CONFIG_BOARD_CURIE_CTB)
#define TST_SOC_32_INPUT_PIN  12
#define TST_SOC_32_OUTPUT_PIN 13
#define TST_SOC_AON_INPUT_PIN  1
#define TST_SOC_AON_OUTPUT_PIN 2
#endif

#define TST_SOC_32_PORT        SOC_GPIO_32_ID
#define TST_SOC_AON_PORT       SOC_GPIO_AON_ID

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

static DRIVER_API_RC soc_gpio_test_pin(struct device *dev, unsigned int input_pin, unsigned int output_pin)
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

    if((ret =  soc_gpio_set_config(dev, input_pin, &in_pin_cfg)) != DRV_RC_OK) {
        cu_print("Error pin %d config (%d)\n", input_pin, ret);
        return ret;
    }
    if((ret =  soc_gpio_set_config(dev, output_pin, &out_pin_cfg)) != DRV_RC_OK) {
        cu_print("Error pin %d config (%d)\n", output_pin, ret);
        soc_gpio_deconfig(dev, input_pin);
        return ret;
    }

    // Test LOW
    if((ret =  soc_gpio_write(dev, output_pin, 0)) != DRV_RC_OK) {
        cu_print("Error pin %d write 1 (%d)\n", output_pin, ret);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return ret;
    }
    trans_delay(); // Delay a little bit
    if((ret =  soc_gpio_read(dev, input_pin, &value)) != DRV_RC_OK) {
        cu_print("Error pin %d read 1 (%d)\n", input_pin, ret);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return ret;
    }
    if(value) {
        cu_print("Error pin %d is at 1, 0 expected\n", input_pin);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return DRV_RC_FAIL;
    }

    // Test HIGH
    if((ret =  soc_gpio_write(dev, output_pin, 1)) != DRV_RC_OK) {
        cu_print("Error pin %d write 2 (%d)\n", output_pin, ret);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return ret;
    }
    trans_delay(); // Delay a little bit
    if((ret =  soc_gpio_read(dev, input_pin, &value)) != DRV_RC_OK) {
        cu_print("Error pin %d read 2 (%d)\n", input_pin, ret);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return ret;
    }
    if(!value) {
        cu_print("Error pin %d is at 0, 1 expected\n", input_pin);
        soc_gpio_deconfig(dev, output_pin);
        soc_gpio_deconfig(dev, input_pin);
        return DRV_RC_FAIL;
    }

    soc_gpio_deconfig(dev, output_pin);
    soc_gpio_deconfig(dev, input_pin);

    return DRV_RC_OK;
}

static DRIVER_API_RC soc_gpio_test_port(struct device *dev, unsigned int input_pin, unsigned int output_pin)
{
    DRIVER_API_RC ret;
    uint32_t value;

    gpio_port_cfg_data_t port_cfg;

    /* fill in settings and defaults for un-needed settings */
    port_cfg.gpio_type = 1<<output_pin; // All input except output bit
    port_cfg.is_interrupt = 0; // No interrupts
    // Interrupts are not enabled here. We don t care about next parameters
    port_cfg.int_type = 0; // this is default
    port_cfg.int_bothedge = 0; // this is default
    port_cfg.int_polarity = 0; // this is default
    port_cfg.int_debounce = 0; // this is default
    port_cfg.int_ls_sync = 0; // this is default

    if((ret =  soc_gpio_set_port_config(dev, &port_cfg)) != DRV_RC_OK) {
        cu_print("Error port config (%d)\n", ret);
        return ret;
    }

    // Test LOW
    value = 0;
    if((ret =  soc_gpio_write_port(dev, value)) != DRV_RC_OK) {
        cu_print("Error port write 1 (%d)\n", ret);
        soc_gpio_port_deconfig(dev);
        return ret;
    }
    trans_delay();
    if((ret =  soc_gpio_read_port(dev, &value)) != DRV_RC_OK) {
        cu_print("Error port read 1 (%d)\n", ret);
        soc_gpio_port_deconfig(dev);
        return ret;
    }
    if(value & (1<<input_pin)) {
        cu_print("Error port is 0x%x, 0x%x expected (Test Low)\n", value, (value & ~(1<<input_pin)));
        soc_gpio_port_deconfig(dev);
        return DRV_RC_FAIL;
    }

    // Test HIGH
    value = (1<<output_pin);
    if((ret =  soc_gpio_write_port(dev, value)) != DRV_RC_OK) {
        cu_print("Error port write 2 (%d)\n", ret);
        soc_gpio_port_deconfig(dev);
        return ret;
    }
    trans_delay();
    if((ret =  soc_gpio_read_port(dev, &value)) != DRV_RC_OK) {
        cu_print("Error port read 2 (%d)\n", ret);
        soc_gpio_port_deconfig(dev);
        return ret;
    }
    if(!(value & (1<<input_pin)) ) {
        cu_print("Error port is 0x%x, 0x%x expected (Test High)\n", value, (value | (1<<input_pin)));
        soc_gpio_port_deconfig(dev);
        return DRV_RC_FAIL;
    }

    soc_gpio_port_deconfig(dev);

    return DRV_RC_OK;
}

static DRIVER_API_RC soc_gpio_test_pin_int(struct device *dev, unsigned int input_pin, unsigned int output_pin)
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
    soc_gpio_set_config(dev, output_pin, &out_pin_cfg);

    // -----------------------------
    // Test ACTIVE_LOW interrupt
    // -----------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = EDGE;
    in_pin_cfg.int_polarity = ACTIVE_LOW;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;
    in_pin_cfg.gpio_cb_arg = &priv;

    soc_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    my_callback_counter = 0;
    soc_gpio_set_config(dev, input_pin, &in_pin_cfg);
    trans_delay(); // Delay a little bit
    soc_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit

    // Check that interrupt is not triggered
    if(my_callback_counter != 0) {
        cu_print("Error: interrupt occured %d\n", my_callback_counter);
        goto fail;
    }

    // Trigger interrupt (active low)
    soc_gpio_write(dev, output_pin, 0);
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
    soc_gpio_deconfig(dev, input_pin);

    // -----------------------------
    // Test ACTIVE_HIGH interrupt
    // -----------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = EDGE;
    in_pin_cfg.int_polarity = ACTIVE_HIGH;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;

    // Test LOW
    soc_gpio_write(dev, output_pin, 1);
    trans_delay(); // Delay a little bit
    my_callback_counter = 0;
    soc_gpio_set_config(dev, input_pin, &in_pin_cfg);
    soc_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    // Check that interrupt is not triggered
    if(my_callback_counter != 0) {
        cu_print("Error: interrupt test for ACTIVE_HIGH returned %d (should be 0)\n", my_callback_counter);
        goto fail;
    }
    // Check that interrupt is triggered (active high)
    soc_gpio_write(dev, output_pin, 1);
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
    soc_gpio_deconfig(dev, input_pin);

    // -----------------------------
    // Test BOTH_EDGE interrupt
    // -----------------------------
    in_pin_cfg.gpio_type = GPIO_INTERRUPT;
    in_pin_cfg.int_type = DOUBLE_EDGE;
    in_pin_cfg.int_polarity = ACTIVE_HIGH;
    in_pin_cfg.int_debounce = DEBOUNCE_OFF;
    in_pin_cfg.gpio_cb = my_callback;

    // Test LOW
    soc_gpio_write(dev, output_pin, 0);
    trans_delay(); // Delay a little bit
    soc_gpio_set_config(dev, input_pin, &in_pin_cfg);
    my_callback_counter = 0;
    soc_gpio_write(dev, output_pin, 1);
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
    soc_gpio_write(dev, output_pin, 0);
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

    soc_gpio_deconfig(dev, output_pin);
    soc_gpio_deconfig(dev, input_pin);
    return DRV_RC_OK;

fail:
    soc_gpio_deconfig(dev, output_pin);
    soc_gpio_deconfig(dev, input_pin);
    return DRV_RC_FAIL;
}

static DRIVER_API_RC soc_gpio_test_port_int(struct device *dev, unsigned int input_pin, unsigned int output_pin)
{
    uint32_t priv = PRIV_KEY;
    gpio_port_cfg_data_t port_cfg;

    // -----------------------------
    // Test ACTIVE_LOW interrupt
    // -----------------------------
    port_cfg.gpio_type = 1<<output_pin; // All input except output bit
    port_cfg.is_interrupt = 0; // No interrupts
    port_cfg.int_type = 1<<input_pin; // edge interrupt on input pin
    port_cfg.int_bothedge = 0; // Only one edge
    port_cfg.int_polarity = ACTIVE_LOW;
    port_cfg.int_debounce = DEBOUNCE_OFF;
    port_cfg.gpio_cb[input_pin] = my_callback;
    port_cfg.gpio_cb_arg[input_pin] = &priv;

    soc_gpio_set_port_config(dev, &port_cfg);
    soc_gpio_write_port(dev, 0);
    trans_delay(); // Delay a little bit

    // Enable interrupt
    my_callback_counter = 0;
    port_cfg.is_interrupt = 1<<input_pin;
    soc_gpio_set_port_config(dev, &port_cfg);
    trans_delay(); // Delay a little bit
    soc_gpio_write_port(dev, 1 << output_pin);
    trans_delay(); // Delay a little bit

    // Check that interrupt is not triggered
    if(my_callback_counter != 0) {
        cu_print("Error: interrupt occured %d\n", my_callback_counter);
        goto fail;
    }

    // Trigger interrupt (active low)
    soc_gpio_write_port(dev, 0);
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

    // deconfigure port for next test (active high)
    soc_gpio_port_deconfig(dev);

    // -----------------------------
    // Test ACTIVE_HIGH interrupt
    // -----------------------------
    port_cfg.gpio_type = 1<<output_pin; // All input except output bit
    port_cfg.is_interrupt = 0;
    port_cfg.int_type = 1<<input_pin; // edge interrupt on input pin
    port_cfg.int_bothedge = 0; // Only one edge
    port_cfg.int_polarity = 1<<input_pin; // ACTIVE_HIGH
    port_cfg.int_debounce = 0; // this is default
    port_cfg.gpio_cb[input_pin] = my_callback;
    port_cfg.gpio_cb_arg[input_pin] = &priv;

    soc_gpio_set_port_config(dev, &port_cfg);
    soc_gpio_write_port(dev, 1 << output_pin);
    trans_delay(); // Delay a little bit

    // Enable interrupt
    my_callback_counter = 0;
    port_cfg.is_interrupt = 1<<input_pin;
    soc_gpio_set_port_config(dev, &port_cfg);
    trans_delay(); // Delay a little bit
    soc_gpio_write_port(dev, 0);
    trans_delay(); // Delay a little bit

    if(my_callback_counter != 0) {
        cu_print("Error: interrupt test for ACTIVE_HIGH returned %d (should be 0)\n", my_callback_counter);
        goto fail;
    }
    // Check that interrupt is triggered (active high)
    soc_gpio_write_port(dev, 1 << output_pin);
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

    // deconfigure port for next test
    soc_gpio_port_deconfig(dev);

    // -----------------------------
    // Test BOTH_EDGE interrupt
    // -----------------------------
    port_cfg.gpio_type = 1<<output_pin; // All input except output bit
    port_cfg.is_interrupt = 0;
    port_cfg.int_type = 1<<input_pin; // edge interrupt on input pin
    port_cfg.int_bothedge = 1<<input_pin;
    port_cfg.int_polarity = 1<<input_pin; // ACTIVE_HIGH
    port_cfg.int_debounce = 0; // this is default
    port_cfg.gpio_cb[input_pin] = my_callback;
    port_cfg.gpio_cb_arg[input_pin] = &priv;

    soc_gpio_set_port_config(dev, &port_cfg);
    soc_gpio_write_port(dev, 1 << output_pin);
    trans_delay(); // Delay a little bit

    // Enable interrupt
    my_callback_counter = 0;
    port_cfg.is_interrupt = 1<<input_pin;
    soc_gpio_set_port_config(dev, &port_cfg);
    trans_delay(); // Delay a little bit
    soc_gpio_write_port(dev, 0);
    trans_delay(); // Delay a little bit

    // Check that interrupt is triggered
    if(my_callback_counter != 1) {
        cu_print("Error: interrupt test for DOUBLE_EDGE returned %d (should be 1)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != false) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }
    // Check that interrupt is triggered (double edge)
    soc_gpio_write_port(dev, 1<<output_pin);
    trans_delay(); // Delay a little bit
    // Check that interrupt is not triggered
    if(my_callback_counter != 2) {
        cu_print("Error: interrupt test for DOUBLE_EDGE returned %d (should be 2)\n", my_callback_counter);
        goto fail;
    }
    // Check that pin state returned in IRQ callback is valid
    if(my_callback_state != true) {
        cu_print("Error: pin state not valid\n");
        goto fail;
    }

    soc_gpio_port_deconfig(dev);
    return DRV_RC_OK;

fail:
    soc_gpio_port_deconfig(dev);
    return DRV_RC_FAIL;
}

#ifdef CONFIG_SOC_GPIO_32
void gpio_test(void)
{
    uint8_t init_tst_soc_input_pin_mode = GET_PIN_MODE(TST_SOC_32_INPUT_PIN);
    uint8_t init_tst_soc_output_pin_mode = GET_PIN_MODE(TST_SOC_32_OUTPUT_PIN);

    cu_print("##################################################\n");
    cu_print("#                                                #\n");
    cu_print("#    !!! Pins %d and %d must be connected !!!      #\n", TST_SOC_32_INPUT_PIN, TST_SOC_32_OUTPUT_PIN);
    cu_print("#                                                #\n");
    cu_print("#  Purpose of GPIOs tests on port %d:             #\n", TST_SOC_32_PORT);
    cu_print("#            Validate input/output ports         #\n");
    cu_print("#            Validate interrupts (edge low)      #\n");
    cu_print("#            Validate interrupts (edge high)     #\n");
    cu_print("#            Validate interrupts (double edge)   #\n");
    cu_print("##################################################\n");

    struct device *soc_dev = &pf_device_soc_gpio_32;

    /* Change Pin Mode of pin 5 and 6 to be as GPIO */
    SET_PIN_MODE(TST_SOC_32_INPUT_PIN, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(TST_SOC_32_OUTPUT_PIN, QRK_PMUX_SEL_MODEA);

    DRIVER_API_RC ret;
    ret = soc_gpio_test_pin(soc_dev, TST_SOC_32_INPUT_PIN, TST_SOC_32_OUTPUT_PIN);
    CU_ASSERT("Test for gpio pin failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_port(soc_dev, TST_SOC_32_INPUT_PIN, TST_SOC_32_OUTPUT_PIN);
    CU_ASSERT("Test for gpio port failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_pin_int(soc_dev, TST_SOC_32_INPUT_PIN, TST_SOC_32_OUTPUT_PIN);
    CU_ASSERT("Test for gpio pin interrupt failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_port_int(soc_dev, TST_SOC_32_INPUT_PIN, TST_SOC_32_OUTPUT_PIN);
    CU_ASSERT("Test for gpio port interrupt failed", ret == DRV_RC_OK);

    /* Restore default configuration for SOC_32 pins */
    SET_PIN_MODE(TST_SOC_32_INPUT_PIN, init_tst_soc_input_pin_mode);
    SET_PIN_MODE(TST_SOC_32_OUTPUT_PIN, init_tst_soc_output_pin_mode);
}
#endif
void gpio_aon_test(void)
{
    uint8_t init_tst_input_pin_mode = GET_PIN_MODE(TST_SOC_AON_INPUT_PIN);
    uint8_t init_tst_output_pin_mode = GET_PIN_MODE(TST_SOC_AON_OUTPUT_PIN);

    cu_print("##################################################\n");
    cu_print("#                                                #\n");
    cu_print("#    !!! Pins %d and %d must be connected !!!      #\n", TST_SOC_AON_INPUT_PIN, TST_SOC_AON_OUTPUT_PIN);
    cu_print("#                                                #\n");
    cu_print("#  Purpose of aon GPIOs tests on port %d:         #\n", TST_SOC_AON_PORT);
    cu_print("#            Validate input/output ports         #\n");
    cu_print("#            Validate interrupts (edge low)      #\n");
    cu_print("#            Validate interrupts (edge high)     #\n");
    cu_print("#            Validate interrupts (double edge)   #\n");
    cu_print("##################################################\n");

    DRIVER_API_RC ret;
    struct device *aon_dev = &pf_device_soc_gpio_aon;

    /* Change Pin Mode of pin 5 and 6 to be as GPIO */
    SET_PIN_MODE(TST_SOC_AON_INPUT_PIN, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(TST_SOC_AON_OUTPUT_PIN, QRK_PMUX_SEL_MODEA);

    ret = soc_gpio_test_pin(aon_dev, TST_SOC_AON_INPUT_PIN, TST_SOC_AON_OUTPUT_PIN);
    CU_ASSERT("Test for gpio pin failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_port(aon_dev, TST_SOC_AON_INPUT_PIN, TST_SOC_AON_OUTPUT_PIN);
    CU_ASSERT("Test for gpio port failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_pin_int(aon_dev, TST_SOC_AON_INPUT_PIN, TST_SOC_AON_OUTPUT_PIN);
    CU_ASSERT("Test for gpio pin interrupt failed", ret == DRV_RC_OK);

    ret = soc_gpio_test_port_int(aon_dev, TST_SOC_AON_INPUT_PIN, TST_SOC_AON_OUTPUT_PIN);
    CU_ASSERT("Test for gpio port interrupt failed", ret == DRV_RC_OK);

    /* Restore default configuration for AON pins */
    SET_PIN_MODE(TST_SOC_AON_INPUT_PIN, init_tst_input_pin_mode);
    SET_PIN_MODE(TST_SOC_AON_OUTPUT_PIN, init_tst_output_pin_mode);
}
