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

/*
 * Intel Quark Always On Timer driver.
 *
 *
 */
#include "drivers/intel_qrk_aonpt.h"
#include "machine.h"
#include "infra/time.h"

static volatile bool is_oneshot = false;
static void (*callback_fn)() = NULL;

void qrk_aonpt_start(void)
{
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_CTRL) =
		(AONPT_CLR | AONPT_RST);
	volatile uint32_t time = get_uptime_32k();
	while(get_uptime_32k() - time < 5);
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_STAT);
}

void qrk_aonpt_stop(void)
{
	uint32_t flags = interrupt_lock();
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_CFG) = 0;
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_CTRL) = (AONPT_CLR | AONPT_RST);
	volatile uint32_t time = get_uptime_32k();
	while(get_uptime_32k() - time < 5);
	interrupt_unlock(flags);
}

void aonpt_ISR(void)
{
	if (!is_oneshot) {
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE,SCSS_AONPT_CTRL) = AONPT_CLR;
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_STAT);
	}

	if (callback_fn != NULL)
		callback_fn();
}

static int qrk_aonpt_resume(struct device *dev)
{
	SET_INTERRUPT_HANDLER(SOC_AONPT_INTERRUPT, aonpt_ISR);
	SOC_UNMASK_INTERRUPTS(SCSS_AON_TIMER_MASK);
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_STAT);
	return 0;
}

static int qrk_aonpt_init(struct device *dev)
{
	/* Set counter to 0 and reset to set a clean initial state */
	qrk_aonpt_stop();
	return qrk_aonpt_resume(dev);
}

struct driver aonpt_driver = {
	.init = qrk_aonpt_init,
	.suspend = NULL,
	.resume = qrk_aonpt_resume
};

void qrk_aonpt_configure(uint32_t period, void (*on_timeout_cb)(), bool one_shot)
{
	/* stops the previous alarm if any */
	qrk_aonpt_stop();
	callback_fn = on_timeout_cb;
	is_oneshot = one_shot;
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_CFG) = period;
}

uint32_t qrk_aonpt_read(void)
{
	return MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONPT_CNT);
}
