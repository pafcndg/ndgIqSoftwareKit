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

#include <gpio/soc_gpio.h>

#define GPIO_CLKENA_POS         (31)
#define GPIO_LS_SYNC_POS        (0)


typedef void (*ISR) ();

/*! GPIO management structure */
typedef struct gpio_info_struct
{
    /* static settings */
    uint32_t           reg_base;       /*!< base address of device register set */
    uint8_t            no_bits;        /*!< no of gpio bits in this entity */
} gpio_info_t, *gpio_info_pt;

static const gpio_info_t gpio_ports_devs[] = {
    {
        .reg_base = SOC_GPIO_BASE_ADDR,
        .no_bits = SOC_GPIO_32_BITS,
    },
    {
        .reg_base = SOC_GPIO_AON_BASE_ADDR,
        .no_bits = SOC_GPIO_AON_BITS,
    },
};

uint8_t soc_gpio_set_config(uint8_t port_id, uint8_t bit, gpio_cfg_data_t *config)
{
    const gpio_info_t dev = gpio_ports_devs[port_id];
    switch(config->gpio_type)
    {
    case GPIO_INPUT:
        /* configure as input */
        CLEAR_MMIO_BIT((volatile uint32_t *)(dev.reg_base+SOC_GPIO_SWPORTA_DDR), (uint32_t)bit);
        break;
    case GPIO_OUTPUT:
        /* configure as output */
        SET_MMIO_BIT((volatile uint32_t *)(dev.reg_base+SOC_GPIO_SWPORTA_DDR), (uint32_t)bit);
        break;
    default:
        return DRV_RC_FAIL;
    }

    return DRV_RC_OK;
}

uint8_t soc_gpio_write(uint8_t port_id, uint8_t bit, uint8_t value)
{
    const gpio_info_t dev = gpio_ports_devs[port_id];
    /* read/modify/write bit */
    if (value) {
        SET_MMIO_BIT((volatile uint32_t *)(dev.reg_base+SOC_GPIO_SWPORTA_DR), (uint32_t)bit);
    } else {
        CLEAR_MMIO_BIT((volatile uint32_t *)(dev.reg_base+SOC_GPIO_SWPORTA_DR), (uint32_t)bit);
    }
    return DRV_RC_OK;
}

uint8_t soc_gpio_write_port(uint8_t port_id, uint32_t value)
{
    const gpio_info_t dev = gpio_ports_devs[port_id];
    MMIO_REG_VAL_FROM_BASE(dev.reg_base, SOC_GPIO_SWPORTA_DR) = value;
    return DRV_RC_OK;
}

uint8_t soc_gpio_read(uint8_t port_id, uint8_t bit)
{
    const gpio_info_t dev = gpio_ports_devs[port_id];
    return !!(MMIO_REG_VAL_FROM_BASE(dev.reg_base, SOC_GPIO_EXT_PORTA) & (1 << bit));
}
