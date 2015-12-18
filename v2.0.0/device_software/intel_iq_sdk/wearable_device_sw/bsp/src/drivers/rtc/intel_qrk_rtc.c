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
 * Intel Quark Real Time Clock driver
 *
 */

#include "drivers/intel_qrk_rtc.h"

#include "machine.h"
#include "infra/device.h"
#include "infra/log.h"
#include "infra/wakelock_ids.h"
#include "drivers/clk_system.h"

#define RTC_WAKELOCK_DELAY            4000       /**< (ms) period to call rtc timer callback*/
static  T_TIMER rtc_wakelock_timer = NULL;
static void (*callback_fn) (uint32_t callback_param);
static uint32_t callback_param;

static void qrk_cxxxx_rtc_wakelock_timer_callback(void* data)
{
    pm_wakelock_release(&rtc_wakelock);
}

static void qrk_cxxxx_rtc_clock_frequency(uint32_t frequency)
{
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_CCU_SYS_CLK_CTL_OFFSET) &= ~SCSS_CCU_RTC_CLK_DIV_EN;
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_CCU_SYS_CLK_CTL_OFFSET) &= ~SCSS_RTC_CLK_DIV_MASK;
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_CCU_SYS_CLK_CTL_OFFSET) |= frequency;
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_CCU_SYS_CLK_CTL_OFFSET) |= SCSS_CCU_RTC_CLK_DIV_EN;
}

static int qrk_cxxxx_rtc_init(struct device* dev) {
    OS_ERR_TYPE err;
    const uint32_t expected_freq = SCSS_RTC_CLK_DIV_1_HZ | SCSS_CCU_RTC_CLK_DIV_EN;

    qrk_cxxxx_rtc_enable(dev);
    rtc_wakelock_timer = timer_create(qrk_cxxxx_rtc_wakelock_timer_callback,
            NULL,
            RTC_WAKELOCK_DELAY,
            false,
            false,
            &err);
    if (E_OS_OK != err)
        pr_error(LOG_MODULE_DRV,"rtc_wakelock_timer err");

    uint32_t curr_freq =
        MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_CCU_SYS_CLK_CTL_OFFSET) &
        (SCSS_CCU_RTC_CLK_DIV_EN | SCSS_RTC_CLK_DIV_MASK);
    pm_wakelock_init(&rtc_wakelock, RTC_WAKELOCK);

    // disable interrupt
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) &= ~QRK_RTC_INTERRUPT_ENABLE;
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_EOI);

    /* Reset initial value only if RTC wasn't enabled at right frequency at
     * beginning of init
     */
    if (expected_freq != curr_freq) {
        //  Set RTC divider 4096HZ for fast uptade
        qrk_cxxxx_rtc_clock_frequency(SCSS_RTC_CLK_DIV_4096_HZ);

        /* set intial RTC value 0 */
        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CLR) = 0;
        while (0 != MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCVR)) {
            MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CLR) = 0;
        }
    }
    //  Set RTC divider 1HZ
    qrk_cxxxx_rtc_clock_frequency(SCSS_RTC_CLK_DIV_1_HZ);
    return 0;
}

static int qrk_cxxxx_rtc_resume(struct device* dev) {
	qrk_cxxxx_rtc_enable(dev);
	return 0;
}
struct driver rtc_driver = {
    .init = qrk_cxxxx_rtc_init,
    .suspend = NULL,
    .resume = qrk_cxxxx_rtc_resume
};

/*! \fn     void qrk_cxxxx_rtc_enable(void)
*
*  \brief   Function to enable clock gating for the RTC
*/
void qrk_cxxxx_rtc_enable(struct device *dev)
{
    struct rtc_pm_data *info = dev->priv;
    set_clock_gate(info->clk_gate_info, CLK_GATE_ON);
}

/*! \fn     void qrk_cxxxx_rtc_disable(void)
*
*  \brief   Function to enable clock gating for the RTC
*/
void qrk_cxxxx_rtc_disable(struct device *dev)
{
    struct rtc_pm_data *info = dev->priv;
    set_clock_gate(info->clk_gate_info, CLK_GATE_OFF);
}

/*! \fn     void rtc_isr(void)
*
*  \brief   RTC alarm ISR, if specified calls a user defined callback
*/
void rtc_isr(void)
{
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_EOI);

    if (callback_fn)
    {
        (*callback_fn)(callback_param);
    }
}

/*! \fn     DRIVER_API_RC qrk_cxxxx_rtc_set_config(struct qrk_cxxxx_rtc_config *config)
*
*  \brief   Function to configure the RTC
*
*  \param   config   : pointer to a RTC configuration structure
*
*  \return  DRV_RC_OK on success\n
*           DRV_RC_FAIL otherwise
*/
DRIVER_API_RC qrk_cxxxx_rtc_set_config(struct qrk_cxxxx_rtc_config *config)
{
    OS_ERR_TYPE err;
    /*  Set RTC divider - 32.768khz / 32768 = 1 second.
    *   Note: Divider not implemented in standard emulation image.
    */
    qrk_cxxxx_rtc_clock_frequency(SCSS_RTC_CLK_DIV_1_HZ);

    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) |= QRK_RTC_INTERRUPT_MASK;

    /* set initial RTC value */
    while (config->initial_rtc_val != MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCVR)) {
        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CLR) = config->initial_rtc_val;
    }

    pm_wakelock_acquire(&rtc_wakelock);
    timer_start(rtc_wakelock_timer, RTC_WAKELOCK_DELAY, &err);
    if(err != E_OS_OK) {
        pm_wakelock_release(&rtc_wakelock);
        return DRV_RC_BUSY;
    }
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) &= ~QRK_RTC_INTERRUPT_MASK;

    return DRV_RC_OK;
}

DRIVER_API_RC qrk_cxxxx_rtc_set_alarm(struct qrk_cxxxx_rtc_alarm *alarm){
    OS_ERR_TYPE err;
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) &= ~QRK_RTC_INTERRUPT_ENABLE;

    if(false != alarm->alarm_enable)
    {
        if(alarm->callback_fn)
        {
            callback_fn = alarm->callback_fn;
            callback_param = alarm->callback_param;
        }
        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_EOI);
        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CMR) = alarm->alarm_rtc_val;

        SET_INTERRUPT_HANDLER(SOC_RTC_INTERRUPT, rtc_isr);

         /* unmask RTC interrupts to quark  */
        MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_INT_RTC_MASK_OFFSET) =
        QRK_INT_RTC_UNMASK_QRK;

        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) |= QRK_RTC_INTERRUPT_ENABLE;
        MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) &= ~QRK_RTC_INTERRUPT_MASK;
    }
    else
    {
        MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_INT_RTC_MASK_OFFSET) = ~(0);
    }
    pm_wakelock_acquire(&rtc_wakelock);
    timer_start(rtc_wakelock_timer, RTC_WAKELOCK_DELAY, &err);
    if(err != E_OS_OK) {
        pm_wakelock_release(&rtc_wakelock);
        return DRV_RC_BUSY;
    }
    return DRV_RC_OK;
}

/*! \fn     uint32_t qrk_cxxxx_rtc_read(void)
*
*  \brief   Function to read the RTC
*
*  \return  uint32_t - epoch time
*/
uint32_t qrk_cxxxx_rtc_read(void)
{
    return MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCVR);
}

/*! \fn     void qrk_cxxxx_rtc_clk_disable(void)
*
*  \brief   Function to disable RTC clock
*/
void qrk_cxxxx_rtc_clk_disable(void)
{
    MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCR) &= ~QRK_RTC_ENABLE;
}
