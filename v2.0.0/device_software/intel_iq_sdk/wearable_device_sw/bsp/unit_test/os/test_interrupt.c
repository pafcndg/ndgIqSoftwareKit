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

#include "os/os.h"
#include "utility.h"
#include "util/cunit_test.h"
#include "test_task.h"


#include "board.h"

#define TEST_IRQ    21 /* Mailbox IT */

/* -------------- global variable -------------- */
static volatile uint32_t isr_count = 0;

void isr_clbk_test_irq(void* data)
{
    isr_count++;
}

static void isr_test_setup(void)
{
#ifndef TARGET_QUARK
    OS_ERR_TYPE err = E_OS_ERR;
    /* NOTE: Zephyr uses static irq vectors, you can't set ISR like that. */
    interrupt_set_isr(TEST_IRQ, isr_clbk_test_irq, NULL , 3, &err);
    CU_ASSERT("interrupt_set_isr FAILED", err == E_OS_OK);
#endif
    isr_count = 0;
}

static void test_interrupt_delay(void)
{
    /* An UART print will allow enough time for the interrupt to react */
    cu_print("Wait (test interrupt)...\n");
}

void test_interrupt_simple_case(void)
{
    interrupt_param_t it_parm = {E_CALLBACK_IRQ, NULL};

    isr_test_setup();

    /* trigger ISR */
    trigger_mbx_isr(&it_parm);

    test_interrupt_delay();
    CU_ASSERT("ISR not called", isr_count == 1);

    trigger_mbx_isr(&it_parm);

    test_interrupt_delay();
    CU_ASSERT("ISR not called", isr_count == 2);
}

void test_interrupt_lock_unlock(void)
{
    uint32_t it_mask;
    interrupt_param_t it_parm = {E_CALLBACK_IRQ, NULL};
    isr_test_setup();

    it_mask = interrupt_lock();
    trigger_mbx_isr(&it_parm);
    test_interrupt_delay();
    CU_ASSERT("ISR called", isr_count == 0);
    interrupt_unlock(it_mask);
    test_interrupt_delay();
    CU_ASSERT("ISR not called", isr_count == 1);
}

void test_interrupt_enable_disable(void)
{
    interrupt_param_t it_parm = {E_CALLBACK_IRQ, NULL};
    isr_test_setup();

    interrupt_disable(TEST_IRQ);
    trigger_mbx_isr(&it_parm);
    test_interrupt_delay();
    CU_ASSERT("ISR called", isr_count == 0);

    mbx_status_reset();
    interrupt_enable(TEST_IRQ);
    trigger_mbx_isr(&it_parm);
    test_interrupt_delay();
    CU_ASSERT("ISR not called", isr_count == 1);
}

