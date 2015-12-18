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

#include <stdint.h>
#include <board.h>
#include "machine.h"
#include "infra/device.h"
#include "infra/log.h"
#include "drivers/ns16550_pm.h"

#define DRV_NAME "uart_pm"
#define LSR_RXRDY 0x01 /* receiver data available */

/* FIX ME: remove when IRQs are called with private parameter */
static void ns16550_pm_uart_ISR(struct device *dev);
static struct device *ns16550_pm_isr;

static void uart_console_init(struct device* dev)
{
	struct ns16550_pm_device* pmdev = dev->priv;

	//if (pmdev->init_info->irq) {
		/* Enable interrupt */
		SOC_UNMASK_INTERRUPTS(pmdev->uart_int_mask);
		interrupt_enable(pmdev->vector);
		/* Enable IRQ at controller level */
		UART_IRQ_RX_ENABLE(pmdev->uart_num);
		/* allow detecting uart break */
		UART_IRQ_ERR_ENABLE(pmdev->uart_num);
	//}
}

static void ns16550_pm_uart_init(struct device* dev)
{
	struct ns16550_pm_device* pmdev = dev->priv;
	uint8_t c;

	/* Re-init hardware */
	uart_init(pmdev->uart_num, pmdev->init_info);
	/* Drain RX FIFOs (no need to disable IRQ at this stage) */
	while (uart_poll_in(pmdev->uart_num, &c) != -1);
	uart_console_init(dev);
}

static int ns16550_pm_init(struct device* dev)
{
	/* Re-init uart with device settings */
	ns16550_pm_isr = dev;
	ns16550_pm_uart_init(dev);

	pr_debug(LOG_MODULE_DRV, DRV_NAME " %d init", dev->id);
	return 0;
}

static int ns16550_pm_suspend(struct device* dev, PM_POWERSTATE state)
{
	struct ns16550_pm_device* pmdev = dev->priv;
	/* Disable UART (set RTS/DTR) */
	uart_disable(pmdev->uart_num);

	pr_debug(LOG_MODULE_DRV, DRV_NAME " %d suspend", dev->id);
	if ((uart_line_status(pmdev->uart_num) & LSR_RXRDY) != 0x00) {
		/* enable UART RTS/DTR control signals */
		uart_enable(pmdev->uart_num, pmdev->init_info);
		return -1;
	}
	return 0;
}

static int ns16550_pm_resume(struct device* dev)
{
	ns16550_pm_uart_init(dev);
	pr_debug(LOG_MODULE_DRV, DRV_NAME " %d resume", dev->id);
	return 0;
}

void uart_rx_isr()
{
	ns16550_pm_uart_ISR(ns16550_pm_isr);
}

void ns16550_pm_uart_ISR(struct device *dev)
{
	struct ns16550_pm_device *pmdev = (struct ns16550_pm_device *)dev->priv;

	if (pmdev->uart_rx_callback)
		pmdev->uart_rx_callback();
}

struct driver ns16550_pm_driver = {
	.init = ns16550_pm_init,
	.suspend = ns16550_pm_suspend,
	.resume = ns16550_pm_resume,
};


