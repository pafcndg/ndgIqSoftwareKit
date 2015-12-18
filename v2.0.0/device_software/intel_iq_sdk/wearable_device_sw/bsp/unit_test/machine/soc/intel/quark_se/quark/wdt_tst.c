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

#include <stdbool.h>
#include "util/cunit_test.h"
#include "drivers/intel_qrk_wdt.h"
#include "machine.h"

volatile bool test_interrupt_result = false;
volatile bool test_boot = false;

void wdt_interrupt_routine(void) {

    if (test_boot == false) {
        qrk_cxxxx_wdt_tickle();
        cu_print(" Wdt kicked by interrupt \n");
    }
    test_interrupt_result = true;
}

void qrk_wdt_config(uint32_t timeout)
{
    struct qrk_cxxxx_wdt_config config;
    uint8_t res;

    config.timeout = timeout;
    config.response_mode = MODE_INTERRUPT_RESET;
    config.interrupt_fn = wdt_interrupt_routine;
    res = qrk_cxxxx_wdt_set_config(&config);
    CU_ASSERT("Wdt configuration failure", res == DRV_RC_OK);
}

void tickle_wdt_test(uint32_t timeout)
{
    cu_print(" Wait WDT timeout [interrupt test] ...\n");
    while(test_interrupt_result != true);
    qrk_cxxxx_wdt_clk_disable();
}

void tickle_wdt_boot_test(uint32_t timeout) {

    cu_print(" Wait WDT timeout [boot test ] ");

    while (test_interrupt_result != true) {
        if (MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE,SCSS_GPS1) == false) {
            MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE,SCSS_GPS1) = true;
        }
    }
    while(1){
        cu_print(".");
    }
}

void qrk_wdt_test(void)
{
    /*
     * Time out = 2^(timeout_multiplier + 16)/Freq_CPU;
     *
     */
    uint32_t i;
    uint32_t timeout_ms;
    if (MMIO_REG_VAL_FROM_BASE ( SCSS_REGISTER_BASE , SCSS_GPS1 ) == false) {

        cu_print("################################################\n");
        cu_print("# Watchdog BOOT test ...                       #\n");
        cu_print("################################################\n");

        test_boot = true;
        timeout_ms = 25;
        qrk_wdt_config(timeout_ms);
        tickle_wdt_boot_test(timeout_ms);

    } else {
        test_boot = false;
        CU_ASSERT("Wdt BOOT test Failed", MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE,SCSS_GPS1) == true);
        MMIO_REG_VAL_FROM_BASE (SCSS_REGISTER_BASE,SCSS_GPS1) = false;

        cu_print("##################################################\n");
        cu_print("# Purpose of Watchdog tests :                    #\n");
        cu_print("#            Start watchdog                      #\n");
        cu_print("#            Wait timeout                        #\n");
        cu_print("#            kicked watchdog with interruption   #\n");
        cu_print("##################################################\n");

        for (i = 5; i < 10; i++) {
            test_interrupt_result = false;
            timeout_ms = (1 << (i + 16)) / (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / 1000);
            cu_print("#############################\n");
            cu_print(" Test <tickle interrupt> with Timeout = %u [ms]# \n", timeout_ms);
            qrk_wdt_config(timeout_ms);
            tickle_wdt_test(timeout_ms);
            CU_ASSERT("Wdt <tickle interrupt> test Failed", test_interrupt_result == true);
            cu_print("#############################\n");
        }
    }

}
