/*******************************************************************************
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
 ******************************************************************************/

#include <microkernel.h>
#include "drivers/intel_qrk_rtc.h"
#include "machine.h"
#include "util/cunit_test.h"
#include "infra/time.h"

#define RTC_INIT 1416571200 // EPOCH 21/11/2014 12:00:00
#define RTC_ALARM 1416571210 // EPOCH  21/11/2014 12:00:10
#define RTC_TEST_END 1416571220 // EPOCH  21/11/2014 12:00:20

#define AON_TIMEOUT 6553509 //20 sec @ 32768 Hz

int test_rtc = 0;
int test_callback_param = 0;

void test_rtc_interrupt_fn(uint32_t val)
{
    test_rtc++;
    test_callback_param = val;
}

void rtc_no_alarm_test (void)
{
    struct qrk_cxxxx_rtc_config config;
    uint32_t rtc_time = 0;
    uint32_t init_time, ao_time;
    DRIVER_API_RC init = DRV_RC_OK;

    config.initial_rtc_val = RTC_INIT;
#ifdef DEBUG
    cu_print("#################################\n");
    cu_print ("RTC TEST WITHOUT ALARM\n");
    cu_print("#################################\n");
    cu_print("Configuring RTC\n");
#endif

    init = qrk_cxxxx_rtc_set_config(&config);
    CU_ASSERT ("Error during RTC configuration", init == DRV_RC_OK);
    if (init == DRV_RC_OK) {
#ifdef DEBUG
        cu_print("RTC successfully configured\n");
#endif
        init_time = get_uptime_32k();
        do
        {
            rtc_time = qrk_cxxxx_rtc_read();
            ao_time = get_uptime_32k();
        } while ((rtc_time == 0) && (ao_time <= init_time + AON_TIMEOUT));
        CU_ASSERT ("Error during RTC time read", rtc_time != 0);
#ifdef DEBUG
        cu_print("Current time (EPOCH): %d\n", rtc_time);
#endif
    } else {
#ifdef DEBUG
        cu_print("Failure to configure RTC.\n");
#endif
    }

#ifdef DEBUG
   cu_print("RTC disable\n");
#endif
   qrk_cxxxx_rtc_clk_disable ();
}


void rtc_alarm_test (void)
{
    struct qrk_cxxxx_rtc_config config;
    struct qrk_cxxxx_rtc_alarm alarm;
    uint32_t rtc_time = 0;
    uint32_t init_time, ao_time;
    DRIVER_API_RC init = DRV_RC_OK;

#ifdef DEBUG
   cu_print("###############################\n");
   cu_print("RTC TEST WITH ALARM\n");
   cu_print("###############################\n");
   cu_print("Configuring RTC\n");
#endif
    config.initial_rtc_val = RTC_INIT;
    alarm.alarm_enable = true;
    alarm.alarm_rtc_val = RTC_ALARM;
    alarm.callback_fn = test_rtc_interrupt_fn;
    alarm.callback_param = 1;

    qrk_cxxxx_rtc_set_config(&config);
    uint32_t cc = qrk_cxxxx_rtc_read();
    CU_ASSERT ("Initial RTC value not set", cc == RTC_INIT);
    local_task_sleep_ms(4005);

    init = qrk_cxxxx_rtc_set_alarm(&alarm);
    CU_ASSERT ("Error during RTC configuration", init == DRV_RC_OK);
    if (init == DRV_RC_OK) {
#ifdef DEBUG
        cu_print("RTC successfully configured\n");
#endif
        init_time = get_uptime_32k();
        do
        {
            rtc_time = qrk_cxxxx_rtc_read();
            ao_time = get_uptime_32k();
        } while ((rtc_time <= RTC_TEST_END) && (ao_time <= init_time + AON_TIMEOUT));
        CU_ASSERT ("Error during RTC time read", rtc_time != 0);
        CU_ASSERT ("Interrupt callback failure.", test_rtc != 0);
#ifdef DEBUG
        cu_print("Current time (EPOCH): %d\n", rtc_time);
#endif
    } else {
 #ifdef DEBUG
         cu_print("Failure to configure RTC.\n");
 #endif
     }

 #ifdef DEBUG
    cu_print("RTC disable\n");
 #endif
    qrk_cxxxx_rtc_clk_disable();
}


/* RTC driver test main function */
void rtc_test(void)
{
    cu_print("#######################################\n");
    cu_print("# Purpose of RTC tests :              #\n");
    cu_print("#         Enabled without alarm       #\n");
    cu_print("#         Enabled with an alarm       #\n");
    cu_print("#         Wait for alarm interrupt    #\n");
    cu_print("#######################################\n");

    rtc_no_alarm_test();
    local_task_sleep_ms(4005);
    rtc_alarm_test();
}

