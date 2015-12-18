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

#include "drivers/ss_gpio_iface.h"

#include <stddef.h>
#include <stdlib.h>

#include "nanokernel.h"

#include "machine.h"

#include "drivers/io_config.h"
#include "drivers/eiaextensions.h"

#include "infra/device.h"
#include "infra/log.h"

/* EIA GPIO device registers  */
#define     SWPORTA_DR      (0x00)  /* GPIO Port A Data Register*/
#define     SWPORTA_DDR     (0x01)  /* GPIO Port A Data Direction Register */
#define     INTEN           (0x03)  /* GPIO Interrupt Enable Register */
#define     INTMASK         (0x04)  /* GPIO Interrupt Mask Register */
#define     INTTYPE_LEVEL   (0x05)  /* GPIO Interrupt Type Register */
#define     INT_POLARITY    (0x06)  /* GPIO Interrupt Polarity Register */
#define     INTSTATUS       (0x07)  /* GPIO Interrupt Status Register */
#define     DEBOUNCE        (0x08)  /* GPIO Debounce Enable Register */
#define     PORTA_EOI       (0x09)  /* GPIO Port A Clear Interrupt Register */
#define     EXT_PORTA       (0x0a)  /* GPIO External Port A Register */
#define     LS_SYNC         (0x0b)  /* GPIO Level-Sensitive Sync Enable and Clock Enable Register */

#define GPIO_CLKENA_POS         (31)
#define GPIO_CLKENA_MSK         (0x1 << GPIO_CLKENA_POS)
#define GPIO_LS_SYNC_POS        (0)
#define GPIO_LS_SYNC_MSK        (0x1 << GPIO_LS_SYNC_POS)

static int ss_gpio_init(struct device *dev);
static int ss_gpio_resume(struct device *dev);
static int ss_gpio_suspend(struct device *dev, PM_POWERSTATE state);

// FIXME: remove when IRQs are called with private parameter
static struct device *ss_gpio_8b0_irq_dev;
static struct device *ss_gpio_8b1_irq_dev;

struct driver ss_gpio_driver = {
    .init = ss_gpio_init,
    .resume = ss_gpio_resume,
    .suspend = ss_gpio_suspend
};

static int ss_gpio_init(struct device *dev)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    // FIXME: remove when IRQs are called with private parameter
    switch (dev->id) {
        case SS_GPIO_8B0_ID:
            ss_gpio_8b0_irq_dev = dev;
            break;
        case SS_GPIO_8B1_ID:
            ss_gpio_8b1_irq_dev = dev;
            break;
        default:
            pr_debug(LOG_MODULE_DRV, "init unknown ss gpio %d", dev->id);
            return -1;
    }

    pr_debug(LOG_MODULE_DRV, "init ss gpio %d", dev->id);

    /* enable peripheral clock */
    SET_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+LS_SYNC), GPIO_CLKENA_POS);
    SET_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+LS_SYNC), GPIO_LS_SYNC_POS);
    /* Clear any existing interrupts */
    WRITE_ARC_REG(~(0), gpio_dev->reg_base + PORTA_EOI);
    /* enable interrupt for this GPIO block */
    SET_INTERRUPT_HANDLER( gpio_dev->vector, gpio_dev->gpio_isr);
    /* Enable GPIO  Interrupt into SSS  */
    SOC_UNMASK_INTERRUPTS(gpio_dev->gpio_int_mask);

    return 0;
}

static int ss_gpio_suspend(struct device *dev, PM_POWERSTATE state)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    /* Save Config */
    gpio_dev->pm_context.gpio_type =      READ_ARC_REG(gpio_dev->reg_base + SWPORTA_DDR);
    gpio_dev->pm_context.int_type =       READ_ARC_REG(gpio_dev->reg_base + INTTYPE_LEVEL);
    gpio_dev->pm_context.int_polarity =   READ_ARC_REG(gpio_dev->reg_base + INT_POLARITY);
    gpio_dev->pm_context.int_debounce =   READ_ARC_REG(gpio_dev->reg_base + DEBOUNCE);
    gpio_dev->pm_context.int_ls_sync =    READ_ARC_REG(gpio_dev->reg_base + LS_SYNC);
    gpio_dev->pm_context.output_values =  READ_ARC_REG(gpio_dev->reg_base + SWPORTA_DR);
    gpio_dev->pm_context.mask_interrupt = READ_ARC_REG(gpio_dev->reg_base + INTMASK);
    gpio_dev->pm_context.mask_inten =     READ_ARC_REG(gpio_dev->reg_base + INTEN);

    pr_debug(LOG_MODULE_DRV, "ss gpio suspend device %d (%d)", dev->id,
            state);

    return DRV_RC_OK;
}

static int ss_gpio_resume(struct device *dev)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    WRITE_ARC_REG(gpio_dev->pm_context.int_type,       gpio_dev->reg_base+INTTYPE_LEVEL);
    WRITE_ARC_REG(gpio_dev->pm_context.int_polarity,   gpio_dev->reg_base+INT_POLARITY);
    WRITE_ARC_REG(gpio_dev->pm_context.int_debounce,   gpio_dev->reg_base+DEBOUNCE);
    WRITE_ARC_REG(gpio_dev->pm_context.int_ls_sync,    gpio_dev->reg_base+LS_SYNC);
    WRITE_ARC_REG(gpio_dev->pm_context.mask_interrupt, gpio_dev->reg_base+INTMASK);
    WRITE_ARC_REG(gpio_dev->pm_context.mask_inten,     gpio_dev->reg_base+INTEN);
    WRITE_ARC_REG(gpio_dev->pm_context.output_values,  gpio_dev->reg_base+SWPORTA_DR);
    WRITE_ARC_REG(gpio_dev->pm_context.gpio_type,      gpio_dev->reg_base+SWPORTA_DDR);

    /* enable interrupt for this GPIO block */
    SET_INTERRUPT_HANDLER(gpio_dev->vector, gpio_dev->gpio_isr);
    /* Enable GPIO  Interrupt into SSS  */
    SOC_UNMASK_INTERRUPTS(gpio_dev->gpio_int_mask);

    pr_debug(LOG_MODULE_DRV, "ss gpio resume device %d", dev->id);
    return DRV_RC_OK;
}

static void ss_gpio_ISR_proc(struct device *dev);

/*! \brief  Interrupt handler for GPIO port 0
*/
DECLARE_INTERRUPT_HANDLER void ss_gpio_8b0_ISR()
{
    ss_gpio_ISR_proc(ss_gpio_8b0_irq_dev);
}

/*! \brief  Interrupt handler for GPIO port 1
*/
DECLARE_INTERRUPT_HANDLER void ss_gpio_8b1_ISR()
{
    ss_gpio_ISR_proc(ss_gpio_8b1_irq_dev);
}

void* ss_gpio_get_callback_arg(struct device *dev, uint8_t pin)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    if (pin >= gpio_dev->no_bits) {
        return NULL;
    }

    return gpio_dev->gpio_cb_arg[pin];
}

DRIVER_API_RC ss_gpio_set_config(struct device *dev, uint8_t bit, gpio_cfg_data_t *config)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;
    uint32_t saved;

    // Check pin index
    if (bit >= gpio_dev->no_bits) {
        return DRV_RC_CONTROLLER_NOT_ACCESSIBLE;
    }

    // Validate config data
    if ((config->gpio_type != GPIO_INPUT) && (config->gpio_type != GPIO_OUTPUT) && (config->gpio_type != GPIO_INTERRUPT)) {
        return DRV_RC_INVALID_CONFIG;
    }

    // Check listen configuration
    if(config->gpio_type == GPIO_INTERRUPT) {
        if (gpio_dev->gpio_cb[bit] != NULL) {
            // pin is already in use
            return DRV_RC_CONTROLLER_IN_USE;
        }
        if ((config->int_type != LEVEL) &&
                (config->int_type != EDGE) &&
                (config->int_type != DOUBLE_EDGE)) {
            // invalid config for listen
            return DRV_RC_INVALID_CONFIG;
        }

    }

    gpio_dev->pm_context.int_bothedge &= (~(1 << bit));

    /* Disable Interrupts for this bit */
    CLEAR_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+INTEN), bit);

    // Set interrupt handler to NULL
    gpio_dev->gpio_cb[bit] = NULL;
    gpio_dev->gpio_cb_arg[bit] = NULL;

    switch(config->gpio_type)
    {
        case GPIO_INPUT:
            /* configure as input */
            CLEAR_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+SWPORTA_DDR), bit);
            break;
        case GPIO_OUTPUT:
            /* configure as output */
            SET_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+SWPORTA_DDR), bit);
            break;
        case GPIO_INTERRUPT:
            saved = interrupt_lock();
            // Configure interrupt handler
            gpio_dev->gpio_cb[bit] = config->gpio_cb;
            gpio_dev->gpio_cb_arg[bit] = config->gpio_cb_arg;
            WRITE_ARC_REG(READ_ARC_REG(gpio_dev->reg_base+SWPORTA_DDR) & ~(1 << bit),
                    gpio_dev->reg_base+SWPORTA_DDR);

            /* Set Level or Edge */
            if(config->int_type == LEVEL) {
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INTTYPE_LEVEL)) & ~(1 << bit),
                        gpio_dev->reg_base+INTTYPE_LEVEL);
            } else {
                /* EDGE or DOUBLE EDGE case */
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INTTYPE_LEVEL)) | (1 << bit),
                        gpio_dev->reg_base+INTTYPE_LEVEL);
                if(config->int_type == DOUBLE_EDGE) {
                    bool value;
                    ss_gpio_read(dev, bit, &value);
                    //setting double edge request
                    gpio_dev->pm_context.int_bothedge |= (1 << bit);
                    if(value){
                        //we are in high level...setting LOW POLARITY
                        config->int_polarity = ACTIVE_LOW;
                    } else{
                        config->int_polarity = ACTIVE_HIGH;
                    }
                }
            }

            /* Set Polarity - Active Low / High */
            if(ACTIVE_LOW == config->int_polarity) {
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INT_POLARITY)) & ~(1 << bit),
                        gpio_dev->reg_base+INT_POLARITY);
            } else {
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INT_POLARITY)) | (1 << bit),
                        gpio_dev->reg_base+INT_POLARITY);
            }

            /* Set Debounce - On / Off */
            if(config->int_debounce == DEBOUNCE_OFF) {
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+DEBOUNCE)) & ~(1 << bit),
                        gpio_dev->reg_base+DEBOUNCE);
            } else {
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+DEBOUNCE)) | (1 << bit),
                        gpio_dev->reg_base+DEBOUNCE);
            }

            /* Enable as Interrupt */
            WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INTEN)) | (1 << bit),
                    gpio_dev->reg_base+INTEN);

            /* Unmask Interrupt */
            WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INTMASK)) & ~(1 << bit),
                    gpio_dev->reg_base+INTMASK);

            interrupt_unlock(saved);

            break;
    }

    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_write(struct device *dev, uint8_t bit, bool value)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    // Check pin index
    if (bit >= gpio_dev->no_bits) {
        return DRV_RC_CONTROLLER_NOT_ACCESSIBLE;
    }
    /* read/modify/write bit */
    if (value) {
        SET_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+SWPORTA_DR), bit);
    } else {
        CLEAR_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+SWPORTA_DR), bit);
    }

    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_write_port(struct device *dev, uint32_t value)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;
    WRITE_ARC_REG(value, gpio_dev->reg_base + SWPORTA_DR);
    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_read(struct device *dev, uint8_t bit, bool *value)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;
    // Check pin index
    if (bit >= gpio_dev->no_bits) {
        return DRV_RC_CONTROLLER_NOT_ACCESSIBLE;
    }
    if (READ_ARC_REG(gpio_dev->reg_base + SWPORTA_DDR) & (1 << bit)) {
        return DRV_RC_INVALID_OPERATION;          /* not configured as input */
    }
    *value = !!(READ_ARC_REG(gpio_dev->reg_base + EXT_PORTA) & (1 << bit));
    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_read_port(struct device *dev, uint32_t *value)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    *value = READ_ARC_REG(gpio_dev->reg_base + EXT_PORTA);
    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_mask_interrupt(struct device *dev, uint8_t bit)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    /* Mask Interrupt */
    SET_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+INTMASK), bit);
    return DRV_RC_OK;
}

DRIVER_API_RC ss_gpio_unmask_interrupt(struct device *dev, uint8_t bit)
{
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    /* Unmask Interrupt */
    CLEAR_ARC_BIT((volatile uint32_t *)(gpio_dev->reg_base+INTMASK), bit);
    return DRV_RC_OK;
}

static void ss_gpio_ISR_proc(struct device *dev)
{
    unsigned int triggered_bit;
    uint32_t shifted_bit;
    gpio_info_pt gpio_dev = (gpio_info_pt)dev->priv;

    // Save interrupt status
    uint32_t status = READ_ARC_REG(gpio_dev->reg_base + INTSTATUS);
    // Clear interrupt flag (write 1 to clear)
    WRITE_ARC_REG(status, gpio_dev->reg_base + PORTA_EOI);

    for(triggered_bit = 0; triggered_bit < gpio_dev->no_bits; triggered_bit++) {
        shifted_bit = (1 << triggered_bit);
        if(status & shifted_bit) {
            if(gpio_dev->pm_context.int_bothedge & shifted_bit) {
                //toggling polarity
                WRITE_ARC_REG((READ_ARC_REG(gpio_dev->reg_base+INT_POLARITY)) ^ shifted_bit, gpio_dev->reg_base+INT_POLARITY);
            }
            if (gpio_dev->gpio_cb[triggered_bit]) {
                (gpio_dev->gpio_cb[triggered_bit])(
                    !!(READ_ARC_REG(gpio_dev->reg_base + EXT_PORTA) & (1 << triggered_bit)),
                    gpio_dev->gpio_cb_arg[triggered_bit]);
            }
        }
    }
}


