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

/* Intel Quark Watchdog Timer driver
 *
 */

#include "drivers/intel_qrk_wdt.h"
#include "infra/device.h"
#include "infra/log.h"
#include "machine.h"
#include "drivers/clk_system.h"

#ifndef CONFIG_WDT_NMI
void (*callback_fn)(void);
#endif

void qrk_cxxxx_wdt_clk_enable(void)
{
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE,
			       QRK_SCSS_PERIPH_CFG0_OFFSET) |=
		QRK_SCSS_PERIPH_CFG0_WDT_ENABLE;
}

void qrk_cxxxx_wdt_gating_enable(struct clk_gate_info_s* clk_gate_info)
{
	set_clock_gate(clk_gate_info, CLK_GATE_ON);
}

void qrk_cxxxx_wdt_clk_disable(void)
{
	/* Disable the clock for the peripheral watchdog */
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE,
			       QRK_SCSS_PERIPH_CFG0_OFFSET) &=
		~QRK_SCSS_PERIPH_CFG0_WDT_ENABLE;
}

void qrk_cxxxx_wdt_gating_disable(struct clk_gate_info_s* clk_gate_info)
{
	set_clock_gate(clk_gate_info, CLK_GATE_OFF);
}

#ifndef CONFIG_WDT_NMI
DECLARE_INTERRUPT_HANDLER void wdt_isr(void)
{
	if (callback_fn)
		(*callback_fn)();
}
#endif

DRIVER_API_RC qrk_cxxxx_wdt_set_config(struct qrk_cxxxx_wdt_config *config)
{
	DRIVER_API_RC ret = DRV_RC_OK;
	/* Enables the clock for the peripheral watchdog */
	qrk_cxxxx_wdt_clk_enable();

	/*  Set timeout value
	 *  [7:4] TOP_INIT - the initial timeout value is hardcoded in silicon,
	 *  only bits [3:0] TOP are relevant.
	 *  Once tickled TOP is loaded at the next expiration.
	 */
	uint32_t i;
	uint32_t ref = (1 << 16) / (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / 1000000); /* 2^16/FREQ_CPU */
	uint32_t timeout = config->timeout * 1000;
	for(i = 0; i < 16; i++){
		if (timeout <= ref) break;
		ref = ref << 1;
	}
	if (i > 15){
		ret = DRV_RC_INVALID_CONFIG;
		i = 15;
	}
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_TORR) = i;

	/* Set response mode */
	if (MODE_RESET == config->response_mode) {
		MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR,
				       QRK_WDT_CR) &= ~QRK_WDT_CR_INT_ENABLE;
	} else {
#ifndef CONFIG_WDT_NMI
		if (config->interrupt_fn)
			callback_fn = config->interrupt_fn;
		else
			return DRV_RC_INVALID_CONFIG;
#endif
		MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR,
				       QRK_WDT_CR) |= QRK_WDT_CR_INT_ENABLE;
#ifdef CONFIG_WDT_NMI
		/* routed watchdog to NMI*/
		SET_INTERRUPT_NMI(SOC_WDT_INTERRUPT);
		interrupt_enable(SOC_WDT_INTERRUPT);
#else
		SET_INTERRUPT_HANDLER(SOC_WDT_INTERRUPT,wdt_isr);
#endif

		/* unmask WDT interrupts to quark  */
		QRK_UNMASK_INTERRUPTS(SCSS_INT_WATCHDOG_MASK_OFFSET);
	}

	/* Enable WDT, cannot be disabled until soc reset */
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR,
			       QRK_WDT_CR) |= QRK_WDT_CR_ENABLE;

	qrk_cxxxx_wdt_tickle();

	return ret;
}

void qrk_cxxxx_wdt_tickle(void)
{
	/* This register is used to restart the WDT
	 *  counter. as a safetly feature to pervent
	 *  accidenctal restarts the value 0x76 must be
	 *  written. A restart also clears the WDT
	 *  interrupt.*/
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR,
			       QRK_WDT_CRR) = QRK_WDT_CRR_VAL;
}

uint32_t qrk_cxxxx_wdt_read_counter(void)
{
	return MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_CCVR);
}

uint32_t qrk_cxxxx_wdt_timeout(void)
{
	return MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_TORR);
}

static int qrk_cxxxx_wdt_suspend(struct device *dev, PM_POWERSTATE state)
{
	struct wdt_pm_data *registers = (struct wdt_pm_data*)dev->priv;

	/* Save Config */
	registers->control_register =
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_CR);
	registers->timeout_range_register =
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_TORR);

	/* Disable WDT */
	qrk_cxxxx_wdt_gating_disable(registers->clk_gate_info);
	return DRV_RC_OK;
}

static int qrk_cxxxx_wdt_resume(struct device *dev)
{
	struct wdt_pm_data *registers = (struct wdt_pm_data*)dev->priv;

#ifdef CONFIG_WDT_NMI
	/* routed watchdog to NMI*/
	SET_INTERRUPT_NMI(SOC_WDT_INTERRUPT);
	interrupt_enable(SOC_WDT_INTERRUPT);
#else
	SET_INTERRUPT_HANDLER(SOC_WDT_INTERRUPT,wdt_isr);
#endif

	/* unmask WDT interrupts to quark  */
	QRK_UNMASK_INTERRUPTS(SCSS_INT_WATCHDOG_MASK_OFFSET);

	qrk_cxxxx_wdt_gating_enable(registers->clk_gate_info);

	/* Restore Config */
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_TORR) =
			registers->timeout_range_register;
	MMIO_REG_VAL_FROM_BASE(QRK_WDT_BASE_ADDR, QRK_WDT_CR) =
			registers->control_register;
	qrk_cxxxx_wdt_tickle();

	pr_debug(LOG_MODULE_DRV, "wdt resume device %d", dev->id);
	return DRV_RC_OK;
}

static int qrk_cxxxx_wdt_init(struct device *dev)
{
	struct wdt_pm_data *wdt_data = (struct wdt_pm_data*)dev->priv;
	qrk_cxxxx_wdt_gating_enable(wdt_data->clk_gate_info);
	return (!dev->priv);
}

struct driver watchdog_driver = {
	.init = qrk_cxxxx_wdt_init,
	.suspend = qrk_cxxxx_wdt_suspend,
	.resume = qrk_cxxxx_wdt_resume
};
