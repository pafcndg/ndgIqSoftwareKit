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

#include "util/cunit_test.h"
#include "infra/pm.h"
#include "soc_config.h"
#include "infra/port.h"
#include <errno.h>
#include <stdlib.h>

#define PRIV_KEY 0xdeadbeef

static volatile int wl_tst_counter = 0;

static void test_cb(void* priv)
{
    ((*((uint32_t*)priv)) == PRIV_KEY) ? wl_tst_counter++ : wl_tst_counter;
}

static uint8_t get_own_shared_variable(void){
    switch(get_cpu_id()){
    case CPU_ID_QUARK:
        return shared_data->any_lmt_wakelock_taken;
    case CPU_ID_ARC:
        return shared_data->any_arc_wakelock_taken;
    }
    CU_ASSERT("CPU ID not recognized", 0);
    return 0;
}

void wakelock_test(void)
{
    int ret;
    uint32_t key = PRIV_KEY;

    // Declare wakelock for test
    struct pm_wakelock pm0;

    cu_print("##################################################\n");
    cu_print("# Purpose of wakelock tests (No HW cfg needed):  #\n");
    cu_print("# - Init & acquire wakelock                      #\n");
    cu_print("# - Try to acquire a locked wakelock             #\n");
    cu_print("# - Release a valid wakelock and check all WL are freed #\n");
    cu_print("# - Try release already released wakelocks       #\n");
    cu_print("##################################################\n");

    // Init test locks
    pm_wakelock_init(&pm0, 1000);

    // Register callback function
    pm_wakelock_set_list_empty_cb(test_cb, &key);
    wl_tst_counter = 0;

    // Acquire test locks
    ret = pm_wakelock_acquire(&pm0);
    CU_ASSERT("acquire pm0 failed", !ret);
    CU_ASSERT("checking own shared variable: released", get_own_shared_variable() == true);

    // Acquire test lock already acquired
    ret = pm_wakelock_acquire(&pm0);
    CU_ASSERT("acquire pm1 ok. It should have failed", ret == -EINVAL);
    CU_ASSERT("checking own shared variable: released", get_own_shared_variable() == true);

    // Release infinite wakelock
    ret = pm_wakelock_release(&pm0);
    CU_ASSERT("release pm0 failed", !ret);
    CU_ASSERT("IRQ not triggered", wl_tst_counter == 1);

    // Try to release already released wakelock
    ret = pm_wakelock_release(&pm0);
    CU_ASSERT("release pm0 ok. It should have failed", ret == -EINVAL);
    CU_ASSERT("checking own shared variable: taken", get_own_shared_variable() == false);

    // Unregister callback function
    pm_wakelock_set_list_empty_cb(NULL, NULL);

    // Flush circular buffer
    local_task_sleep_ms(100);
}
