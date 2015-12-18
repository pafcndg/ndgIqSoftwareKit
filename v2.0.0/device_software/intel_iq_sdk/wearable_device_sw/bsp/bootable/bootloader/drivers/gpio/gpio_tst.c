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

#include "gpio/gpio.h"

void delay(void){
    uint32_t   stamp=0;
    while (stamp < 1000000){
        stamp++;
        __asm__ volatile("nop");
    }
}

void gpio_tst(void)
{
    gpio_cfg_data_t pin_cfg;
    uint8_t value_pin;

    SET_PIN_MODE(15, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(20, QRK_PMUX_SEL_MODEA);

    pin_cfg.gpio_type = GPIO_OUTPUT;
    soc_gpio_set_config(SOC_GPIO_32, 15, &pin_cfg);

    pin_cfg.gpio_type = GPIO_INPUT;
    soc_gpio_set_config(SOC_GPIO_32, 20, &pin_cfg);
    value_pin = soc_gpio_read_port(20);

    if (!value_pin) {
        soc_gpio_write(SOC_GPIO_32, 15, GPIO_PIN_RESET);
        while (1) {
            soc_gpio_write(SOC_GPIO_32, 15, GPIO_PIN_SET);
            delay();
            soc_gpio_write(SOC_GPIO_32, 15, GPIO_PIN_RESET);
            delay();
        }
    }
}

