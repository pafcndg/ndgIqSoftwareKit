/****************************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************************/
#include <stdbool.h>
#include "drivers/intel_qrk_pwm.h"
#include "util/cunit_test.h"
#include "machine.h"
#include "infra/log.h"
#include "infra/wakelock_ids.h"
#include "infra/device.h"

#define TEST_PWM_PIN_1 (63)
#define TEST_PWM_PIN_2 (64)
#define TEST_PWM_PIN_3 (65)
#define TEST_PWM_PIN_4 (66)

int32_t test_fn_0 = 0;
int32_t test_fn_1 = 0;

void test_interrupt_fn_0(void);
void test_interrupt_fn_1(void);

void test_interrupt_fn_0(void)
{
    test_fn_0++;
}

void test_interrupt_fn_1(void)
{
    test_fn_1++;
}

void qrk_pwm_test(void)
{
    struct device *dev = &pf_device_pwm;
    struct soc_pwm_channel_config config0, config1;
    struct pm_wakelock pwm_wakelock;

    cu_print("##################################################\n");
    cu_print("# Purpose of PWM tests :                         #\n");
    cu_print("#            Start using PWM 1                   #\n");
    cu_print("#            Start using TIMER 0                 #\n");
    cu_print("##################################################\n");

    pm_wakelock_init(&pwm_wakelock, PWM_WAKELOCK);
    config0.mode = TIMER_MODE;
    config0.pwm_period_ns = 0;
    config0.pwm_duty_cycle_ns = 0;
    config0.pwm_enable_interrupts = true;
    config0.interrupt_fn = test_interrupt_fn_0;
    config0.timer_timeout_ns = 50000000;
    config0.timer_enable_oneshot = false;

    config1.mode = PWM_MODE;
    config1.pwm_period_ns = 20000000;
    config1.pwm_duty_cycle_ns = 5000000;
    config1.pwm_enable_interrupts = true;
    config1.interrupt_fn = test_interrupt_fn_1;
    config1.timer_timeout_ns = 0;

#ifdef FULL_BOARD
    config0.mode = PWM_MODE;
    config0.pwm_period_ns = 10000000;
    config0.pwm_duty_cycle_ns = 5000000;
    config0.pwm_enable_interrupts = false;
    config0.timer_timeout_ns = 0;

    config2.mode = PWM_MODE;
    config2.pwm_period_ns = 10000000;
    config2.pwm_duty_cycle_ns = 7500000;
    config2.pwm_enable_interrupts = false;
    config2.timer_timeout_ns = 0;

    config3.mode = PWM_MODE;
    config3.pwm_period_ns = 20000000;
    config3.pwm_duty_cycle_ns = 5000000;
    config3.pwm_enable_interrupts = false;
    config3.timer_timeout_ns = 0; */
#endif

    /*  Setup Pin mux for pwm, external pad 63->66 @TODO:
     *  find universal place for pin mux setups */
    uint8_t save_pin_mode_1 = GET_PIN_MODE(TEST_PWM_PIN_1);
    uint8_t save_pin_mode_2 = GET_PIN_MODE(TEST_PWM_PIN_2);
    uint8_t save_pin_mode_3 = GET_PIN_MODE(TEST_PWM_PIN_3);
    uint8_t save_pin_mode_4 = GET_PIN_MODE(TEST_PWM_PIN_4);
    SET_PIN_MODE(TEST_PWM_PIN_1, QRK_PMUX_SEL_MODEB);
    SET_PIN_MODE(TEST_PWM_PIN_2, QRK_PMUX_SEL_MODEB);
    SET_PIN_MODE(TEST_PWM_PIN_3, QRK_PMUX_SEL_MODEB);
    SET_PIN_MODE(TEST_PWM_PIN_4, QRK_PMUX_SEL_MODEB);

    if (soc_pwm_set_config(dev, 0, &config0) == DRV_RC_OK)
    {
        soc_pwm_start(dev, 0);
    }

    cu_print("Start waiting 2s => counter fn_0 should increase...\n");

    pm_wakelock_acquire(&pwm_wakelock);
    local_task_sleep_ms(2000);
    pm_wakelock_release(&pwm_wakelock);

    cu_print("test_fn_0:%d\n",test_fn_0);
    CU_ASSERT("TIMER 0 callback not reached", test_fn_0 > 0);

    soc_pwm_stop(dev, 0);

    if (soc_pwm_set_config(dev, 1, &config1) == DRV_RC_OK)
    {
        soc_pwm_start(dev, 1);
    }

    cu_print("Start waiting 2s => counter fn_1 should increase ");
    cu_print("the led linked to PWM 1 should blink...\n");

    pm_wakelock_acquire(&pwm_wakelock);
    local_task_sleep_ms(2000);
    pm_wakelock_release(&pwm_wakelock);

    soc_pwm_stop(dev, 1);
    cu_print("test_fn_1:%d\n",test_fn_1);
    CU_ASSERT("PWM 1 callback not reached", test_fn_1 > 0);

    SET_PIN_MODE(TEST_PWM_PIN_1, save_pin_mode_1);
    SET_PIN_MODE(TEST_PWM_PIN_2, save_pin_mode_2);
    SET_PIN_MODE(TEST_PWM_PIN_3, save_pin_mode_3);
    SET_PIN_MODE(TEST_PWM_PIN_4, save_pin_mode_4);
}
