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

#include "machine/soc/intel/quark_se/soc_config.h"
#include "drivers/intel_qrk_pwm.h"
#include "machine.h"
#include "drivers/clk_system.h"

/* Clock is 32 Mhz */
#define GRANULARITY_X_100           3125

/* One nanosecond in hertz */
#define NS_IN_HZ                        1000000000

/* TODO: DB - 64Bit calculation not working, causes issues setting slow speeds/high timers*/

static uint32_t soc_pwm_ioread(uint8_t channel, uint32_t offset);
static void soc_pwm_iowrite(uint8_t channel, uint32_t offset, uint32_t val);
static void soc_pwm_mask_interrupt(uint8_t channel);
static void soc_pwm_unmask_interrupt(uint8_t channel);
static DRIVER_API_RC soc_pwm_check_config(uint8_t channel, struct soc_pwm_channel_config *config);

bool one_shot[QRK_PWM_NPWM];
void (*callback_fn[QRK_PWM_NPWM]) (void);

/*! \fn     void soc_pwm_enable(void)
*
*  \brief   Function disable clock gating for the PWM device
*/
static void soc_pwm_enable(struct device *dev, uint8_t channel)
{
    uint32_t int_saved = interrupt_lock();
    struct pwm_pm_info *info = dev->priv;
    if(info->running_pwm == 0) {
        set_clock_gate(info->clk_gate_info, CLK_GATE_ON);
    }
    info->running_pwm |= (1 << channel);
    interrupt_unlock(int_saved);
}

/*! \fn     void soc_pwm_disable(void)
*
*  \brief   Function to enable clock gating for the PWM device
*/
static void soc_pwm_disable(struct device *dev, uint8_t channel)
{
    uint32_t int_saved = interrupt_lock();
    struct pwm_pm_info *info = dev->priv;
    info->running_pwm &= ~(1 << channel);
    if(info->running_pwm == 0) {
        set_clock_gate(info->clk_gate_info, CLK_GATE_OFF);
    }
    interrupt_unlock(int_saved);
}

/*
 * The channel specific registers are grouped in  sequential blocks of size
 * 0x14. However, the 4 LCNT2 registers are grouped seperately.
 * Therefore, the address calculation changes accordingly for LCNT2.
 */
static uint32_t soc_pwm_ioread(uint8_t channel, uint32_t offset)
{
    int regs_len = 0;

    if (QRK_PWM_N_LOAD_COUNT2 == offset)
    {
        regs_len = QRK_PWM_N_LCNT2_LEN;
    }

    else
    {
        regs_len = QRK_PWM_N_REGS_LEN;
    }

    return MMIO_REG_VAL_FROM_BASE(QRK_PWM_BASE_ADDR, ((channel * regs_len)
                                    + offset));
}

/*
 * The channel specific registers are grouped in 4 sequential blocks of size
 * 0x14. However, the 4 LCNT2 registers are grouped seperately.
 * Therefore, the address calculation changes accordingly for LCNT2.
 */
static void soc_pwm_iowrite(uint8_t channel, uint32_t offset, uint32_t val)
{
    int regs_len = 0;

    if (QRK_PWM_N_LOAD_COUNT2 == offset)
    {
        regs_len = QRK_PWM_N_LCNT2_LEN;
    }

    else
    {
        regs_len = QRK_PWM_N_REGS_LEN;
    }

    MMIO_REG_VAL_FROM_BASE(QRK_PWM_BASE_ADDR, ((channel * regs_len)
                            + offset)) = val;
}

static DRIVER_API_RC soc_pwm_check_config(uint8_t channel, struct soc_pwm_channel_config *config)
{
    DRIVER_API_RC ret = DRV_RC_OK;

    if (channel >= QRK_PWM_NPWM)
    {
        ret = DRV_RC_INVALID_CONFIG;
    }

    if (PWM_MODE == config->mode)
    {
        if (0 == config->pwm_period_ns ||
            config->pwm_duty_cycle_ns > config->pwm_period_ns ||
            config->pwm_duty_cycle_ns > DUTY_CYCLE_MAX_NS ||
            config->pwm_period_ns  > PERIOD_MAX_NS)
        {
             ret = DRV_RC_INVALID_CONFIG;
        }
    }

    else
    {
        if (0 == config->timer_timeout_ns || config->timer_timeout_ns > TIMER_TIMEOUT_MAX_NS)
        {
            ret = DRV_RC_INVALID_CONFIG;
        }
    }

    if (config->mode != PWM_MODE && config->mode != TIMER_MODE)
    {
        ret = DRV_RC_INVALID_CONFIG;
    }

    return ret;
}

/*! \fn     DRIVER_API_RC soc_pwm_set_config(struct soc_pwm_channel_config *config)
*
*  \brief   Function to configure a specified PWM channel
*
*  \param   config   : pointer to a channel configuration structure
*
*  \return  DRV_RC_OK on success\n
*           DRV_RC_FAIL otherwise
*/
DRIVER_API_RC soc_pwm_set_config(struct device *dev, uint8_t channel, struct soc_pwm_channel_config *config)
{
    uint64_t duty_cycle = 0, period = 0;
    uint32_t hcnt = 0, lcnt = 0, val = 0;
    /* Dont go any further if config is bad */
    if(soc_pwm_check_config(channel, config) != DRV_RC_OK)
    {
        return DRV_RC_INVALID_CONFIG;
    }

    soc_pwm_enable(dev, channel);
    /*  Manage differences between PWM and timer mode */
    val = soc_pwm_ioread(channel, QRK_PWM_N_CONTROL);

    if(PWM_MODE == config->mode)
    {
            duty_cycle = config->pwm_duty_cycle_ns;
            period = config->pwm_period_ns;

            /*  if PWM_MODE enable output */
            val |= QRK_PWM_CONTROL_PWM_OUT;
    }


    if(TIMER_MODE == config->mode)
    {
            duty_cycle = config->timer_timeout_ns;
            period = (config->timer_timeout_ns * 2);

            one_shot[channel] = config->timer_enable_oneshot;

            /*  if TIMER_MODE disable output */
            val &= ~QRK_PWM_CONTROL_PWM_OUT;
    }

    soc_pwm_iowrite(channel, QRK_PWM_N_CONTROL, val);


    /* Calculate value for count for LoadCount1 and LoadCount2 register */
    /* Values are multiplied by 100 to increase accuracy
    *  without the use of floats  */
    hcnt = (uint32_t) ((duty_cycle * 100 + (GRANULARITY_X_100 >> 1)) / GRANULARITY_X_100);
    lcnt = (uint32_t) (((period * 100) + (GRANULARITY_X_100 >> 1)) / GRANULARITY_X_100) - hcnt;

    /* A count of 0, equates to (1 * granularity), so adjust by -1 */
    if (hcnt > 0)
    {
        hcnt--;
    }

    if (lcnt > 0)
    {
        lcnt--;
    }
    /* Load counter value */
    soc_pwm_iowrite(channel, QRK_PWM_N_LOAD_COUNT2, hcnt);
    soc_pwm_iowrite(channel, QRK_PWM_N_LOAD_COUNT1, lcnt);

    /*  Interrupt masking/unmasking and channel specific interrupt functions */
    if (false != config->pwm_enable_interrupts || TIMER_MODE == config->mode)
    {
        soc_pwm_unmask_interrupt(channel);
        callback_fn[channel] = config->interrupt_fn;
    }
    else
    {
        soc_pwm_mask_interrupt(channel);
    }
    soc_pwm_disable(dev, channel);
    return DRV_RC_OK;
}

/*! \fn     void soc_pwm_start(int channel)
*
*  \brief   Function to start a pwm/timer channel
*
*  \param   channel   : Channel number
*/

void soc_pwm_start(struct device *dev, uint8_t channel)
{
    uint32_t val = 0;

    /* Read/Write protection */
    /* Protect QRK_PWM_N_CONTROL using lock and unlock of interruptions */
    uint32_t saved = interrupt_lock();

    soc_pwm_enable(dev, channel);
    val = soc_pwm_ioread(channel, QRK_PWM_N_CONTROL);
    val |= QRK_PWM_CONTROL_ENABLE;
    soc_pwm_iowrite(channel, QRK_PWM_N_CONTROL, val);
    interrupt_unlock(saved);
}

/*! \fn     void soc_pwm_stop(int channel)
*
*  \brief   Function to stop a pwm/timer channel
*
*  \param   channel   : Channel number
*/
void soc_pwm_stop(struct device *dev, uint8_t channel)
{
    uint32_t val = 0;

    /* Read/Write protection */
    /* Protect QRK_PWM_N_CONTROL using lock and unlock of interruptions */
    uint32_t saved = interrupt_lock();

    soc_pwm_disable(dev, channel);
    val = soc_pwm_ioread(channel, QRK_PWM_N_CONTROL);
    val &= ~QRK_PWM_CONTROL_ENABLE;
    soc_pwm_iowrite(channel, QRK_PWM_N_CONTROL, val);
    interrupt_unlock(saved);
}

static void soc_pwm_mask_interrupt(uint8_t channel)
{
    uint32_t val = 0;

    /* Read/Write protection */
    /* Protect QRK_PWM_N_CONTROL using lock and unlock of interruptions */
    uint32_t saved = interrupt_lock();

    val = soc_pwm_ioread(channel, QRK_PWM_N_CONTROL);
    val |= QRK_PWM_CONTROL_INT_MASK;
    soc_pwm_iowrite(channel, QRK_PWM_N_CONTROL, val);
    interrupt_unlock(saved);
}

static void soc_pwm_unmask_interrupt(uint8_t channel)
{
    uint32_t val = 0;

    /* Read/Write protection */
    /* Protect QRK_PWM_N_CONTROL using lock and unlock of interruptions */
    uint32_t saved = interrupt_lock();

    val = soc_pwm_ioread(channel, QRK_PWM_N_CONTROL);
    val &= ~QRK_PWM_CONTROL_INT_MASK;
    soc_pwm_iowrite(channel, QRK_PWM_N_CONTROL, val);
    interrupt_unlock(saved);
}

/* ! \fn     void pwm_isr(void)
 *
 *  \brief   PWM ISR, if specified calls a user defined callback
 */
DECLARE_INTERRUPT_HANDLER void pwm_isr(struct device *dev)
{
    uint32_t pending = 0, pwm = 0;

    /*   Which pin (if any) triggered the interrupt */
    while ((pending = MMIO_REG_VAL_FROM_BASE(QRK_PWM_BASE_ADDR, QRK_PWMS_INT_STATUS)))
    {

        do {
            if (pending & 0x01)
            {
                pwm = 0;
                pending &= ~0x01;
            }

            else if (pending & 0x02)
            {
                pwm = 1;
                pending &= ~0x02;
            }

            else if (pending & 0x04)
            {
                pwm = 2;
                pending &= ~0x04;
            }

            else if (pending & 0x08)
            {
                pwm = 3;
                pending &= ~0x08;
            }

            if (callback_fn[pwm])
            {
                (*callback_fn[pwm])();
            }

            if (false != one_shot[pwm])
            {
                soc_pwm_stop(dev, pwm);
            }

        } while (pending);

         /*  Clear the interrupt  */
        MMIO_REG_VAL_FROM_BASE(QRK_PWM_BASE_ADDR, QRK_PWMS_EOI);
    }
}

static int soc_pwm_block_init(struct device *dev)
{
    int i = 0;
    uint32_t val = 0;
    /* Read/Write protection */
    /* Protect QRK_PWM_N_CONTROL using lock and unlock of interruptions */
    uint32_t saved = interrupt_lock();

    soc_pwm_enable(dev, 1); /* activate pwm to acces global register */
    for (i = 0; i < QRK_PWM_NPWM; i++)
    {
        val = soc_pwm_ioread(i, QRK_PWM_N_CONTROL);

        /*  Mask interrupts */
        val |= QRK_PWM_CONTROL_INT_MASK;

        /* Set timer mode periodic-
        * EAS "Free-running timer mode is not supported
        * (i.e. TimerXControlReg.”Timer Mode” must be set to b1)*/
        val |= QRK_PWM_CONTROL_MODE_PERIODIC;
        soc_pwm_iowrite(i, QRK_PWM_N_CONTROL, val);
    }
    soc_pwm_disable(dev, 1);
    interrupt_unlock(saved);

    SET_INTERRUPT_HANDLER(SOC_PWM_INTERRUPT, pwm_isr);

    /* unmask pwm ints to quark core */
    QRK_UNMASK_INTERRUPTS(SCSS_INT_PWM_TIMER_MASK_OFFSET);
    return 0;
}

static int soc_pwm_block_suspend(struct device *dev, PM_POWERSTATE state)
{
    struct pwm_pm_info *pwm_dev_priv = (struct pwm_pm_info*)dev->priv;
    uint8_t i ;
    uint32_t saved = interrupt_lock();

    soc_pwm_enable(dev, 1); /* activate pwm to acces global register */
    for (i = 0; i < QRK_PWM_NPWM; i++)
    {
        pwm_dev_priv->timer_info[i].timer_load_count = soc_pwm_ioread(i, QRK_PWM_N_LOAD_COUNT1);
        pwm_dev_priv->timer_info[i].timer_current_value = soc_pwm_ioread(i, QRK_PWM_N_CURRENT_VALUE);
        pwm_dev_priv->timer_info[i].timer_control_reg = soc_pwm_ioread(i, QRK_PWM_N_CONTROL);
        pwm_dev_priv->timer_info[i].timer_load_count_2 = soc_pwm_ioread(i, QRK_PWM_N_LOAD_COUNT2);
    }
    pwm_dev_priv->timers_comp_version = soc_pwm_ioread(i, QRK_PWMS_COMP_VERSION);
    soc_pwm_disable(dev, 1);
    interrupt_unlock(saved);

    return 0;
}

static int soc_pwm_block_resume(struct device *dev)
{
    struct pwm_pm_info *pwm_dev_priv = (struct pwm_pm_info*)dev->priv;
    uint8_t i ;
    uint8_t pwm_was_disabled = 0 ;
    uint32_t saved = interrupt_lock();

    set_clock_gate(pwm_dev_priv->clk_gate_info, CLK_GATE_ON);

    if (!(pwm_dev_priv->running_pwm)){
        pwm_was_disabled = 1;
    }

    for (i = 0; i < QRK_PWM_NPWM; i++)
    {
        soc_pwm_iowrite(i, QRK_PWM_N_LOAD_COUNT1, pwm_dev_priv->timer_info[i].timer_load_count);
        soc_pwm_iowrite(i, QRK_PWM_N_CURRENT_VALUE, pwm_dev_priv->timer_info[i].timer_current_value);
        soc_pwm_iowrite(i, QRK_PWM_N_CONTROL, pwm_dev_priv->timer_info[i].timer_control_reg);
        soc_pwm_iowrite(i, QRK_PWM_N_LOAD_COUNT2, pwm_dev_priv->timer_info[i].timer_load_count_2);
    }
    soc_pwm_iowrite(i, QRK_PWMS_COMP_VERSION, pwm_dev_priv->timers_comp_version);
    MMIO_REG_VAL_FROM_BASE(QRK_PWM_BASE_ADDR, QRK_PWMS_EOI);

    SET_INTERRUPT_HANDLER(SOC_PWM_INTERRUPT, pwm_isr);
    QRK_UNMASK_INTERRUPTS(SCSS_INT_PWM_TIMER_MASK_OFFSET);

    if(pwm_was_disabled == 1) {
        set_clock_gate(pwm_dev_priv->clk_gate_info, CLK_GATE_OFF);
    }

    interrupt_unlock(saved);

    return 0;
}

struct driver pwm_driver = {
    .init = soc_pwm_block_init,
    .suspend = soc_pwm_block_suspend,
    .resume = soc_pwm_block_resume
};
