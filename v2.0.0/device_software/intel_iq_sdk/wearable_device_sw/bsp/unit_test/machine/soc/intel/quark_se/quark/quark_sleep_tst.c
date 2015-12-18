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

#include "drivers/intel_qrk_rtc.h"
#include "util/cunit_test.h"

#include "machine.h"
#include "util/compiler.h"

#define SLEEP_KEY      0xdeadbeef
static volatile uint32_t tst_sleep_key __section(".noinit");
#define SLEEP_DELAY    5         // RTC alarm delay in seconds
extern struct pm_wakelock rtc_wakelock;
// Because we cannot enable RTC alarm without setting a new value for RTC ...
#define RTC_INIT  1416571200 // EPOCH 21/11/2014 12:00:00

#define SEC_TO_EPOCH 1

#define SLEEP_CU_ASSERT(cdt, msg, ...) \
    do { CU_ASSERT("", (cdt)); \
    if(!(cdt)) { \
        cu_print(msg "\n", ## __VA_ARGS__); \
        return; } \
    } while(0)

// Internal test funtions
static DRIVER_API_RC quark_config_rtc_alarm(unsigned int delay);
static void rtc_cb_fn(uint32_t val);

/**
 * \brief Callback function for RTC in case sleep fails
 */
static void rtc_cb_fn(uint32_t val) {}

/**
 * \brief Configure RTC alarm
 */
static DRIVER_API_RC quark_config_rtc_alarm(unsigned int delay)
{
    struct qrk_cxxxx_rtc_alarm alarm;
    DRIVER_API_RC ret;

    // Configure RTC alarm
    alarm.alarm_enable = true;
    alarm.alarm_rtc_val = qrk_cxxxx_rtc_read() + (delay*SEC_TO_EPOCH);
    alarm.callback_fn = rtc_cb_fn;
    uint32_t t =  qrk_cxxxx_rtc_read();
    ret = qrk_cxxxx_rtc_set_alarm(&alarm);
    while ((t + SLEEP_DELAY) !=  MMIO_REG_VAL_FROM_BASE(QRK_RTC_BASE_ADDR, QRK_RTC_CCVR));
    pm_wakelock_release(&rtc_wakelock);
    CU_ASSERT("Error during RTC configuration", ret == DRV_RC_OK);
    return ret;
}

void quark_sleep_test(void)
{
    DRIVER_API_RC ret;

    cu_print("##################################################\n");
    cu_print("# Purpose of quark sleep test:                   #\n");
    cu_print("# - Configure RTC for wakeup event               #\n");
    cu_print("# - Put quark in sleep mode (reboot)             #\n");
    cu_print("# - Check that device is woken up by RTC         #\n");
    cu_print("##################################################\n");
    cu_print("Warning : address 0x%x must not be used\n", &tst_sleep_key);

    // Check RAM key for resume detection
    if (tst_sleep_key != SLEEP_KEY) {
        // Device has just boot, go to sleep mode
        tst_sleep_key = SLEEP_KEY;
        // Configure RTC alarm
        ret = quark_config_rtc_alarm(SLEEP_DELAY);
        SLEEP_CU_ASSERT(ret == DRV_RC_OK, "RTC alarm configuration failed (%d)", ret);
        // Reset quark and wait for RTC wakeup event
        // We need to use directly the low level function
        // because quark_deep_sleep function will resume context after resume
        // and we need in this test to reset platform after deep sleep
        uint32_t sleep_cfg = SLP_CFG_VRET_SEL_135; // All disabled except RTC
        SCSS_REG_VAL(SLP_CFG_BASE) = sleep_cfg;
        SET_MMIO_BIT(((volatile uint32_t*)PM1C_BASE), PM1C_SLPEN_BIT);

        // We should never reach this point as quark is in sleep mode
        SLEEP_CU_ASSERT(0, "fatal error, platform should sleep now !");
    }
    tst_sleep_key = 0;
    // sleep key matches, quark just resumed from sleep mode, test ok
    // Just disable RTC for next tests
    qrk_cxxxx_rtc_clk_disable();
}
