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
#include "stdint.h"

static volatile uint32_t saved_disturb_critical_section_counter1 = 0;
static volatile uint32_t saved_disturb_critical_section_counter2 = 0;
static volatile uint32_t saved_disturb_critical_section_counter3 = 0;
static volatile uint32_t saved_disturb_critical_section_counter4 = 0;
static volatile uint32_t disturb_critical_section_counter = 0;
static volatile bool b_end_disturbing_task = false;
static volatile bool b_task1_complete = false;
static volatile bool task2_started = false;

/* NB: this test is testing scheduling disabling which is not available in zephyr micro.
 * The function disable_scheduling disables all interrupts (see os.h)
 * */

void test_disable_scheduling(void)
{
#ifndef ZEPHYR_NANO

    disable_scheduling();
//    asm("nop");
    enable_scheduling();

    CU_ASSERT("check task1 not in use", task1_fct_id == TK1_IDLE);
    CU_ASSERT("check task2 not in use", task2_fct_id == TK2_IDLE);

    /* start Task2 (disturb task of medium priority)*/
    task2_fct_id = TK2_TEST_DISABLE_SCHEDULING;

    /* wait until the Task 2 started */
    while(!task2_started)
    {
        local_task_sleep_ms(20);
    }

    /* start Task1 (low priority task with critical section ) */
    task1_fct_id = TK1_TEST_DISABLE_SCHEDULING;

    /* wait until the Task 1 is complete  (critical section finished) */
    while(!b_task1_complete)
    {
        local_task_sleep_ms(200);
    }

    /* stop task2 */
    b_end_disturbing_task = true;
    /* wait until the Task 2 is finished */
    while(task2_started)
    {
        local_task_sleep_ms(10);
    }
    b_end_disturbing_task = false;

    /* Make sure the critical section haven't been interrupted by T2 */
    CU_ASSERT("critical section has been interrupted", saved_disturb_critical_section_counter1 == saved_disturb_critical_section_counter2);
    CU_ASSERT("critical section has been interrupted", saved_disturb_critical_section_counter3 == saved_disturb_critical_section_counter4);

#else

    CU_ASSERT("critical section test is disabled for Zephyr NanoKernel", false );

#endif
}


/* Low priority task with critical section.
 * make sure task2 of higher priority cannot interrupt the section protected by "disable_scheduling()"
 */
void fct_task1_disable_scheduling(void)
{
    volatile uint32_t i=0;

    /* start critical section */
    disable_scheduling();
    saved_disturb_critical_section_counter1 = disturb_critical_section_counter;
    cu_print("start section critic \n");

    /* wait for quite some time */
    i = 0x1FFFFF;
    while(--i)
    {
#ifdef ZEPHYR_NANO
        local_task_sleep_ms(10); /* allow task preemption */
#endif
    }

    cu_print("end section critic \n");
    saved_disturb_critical_section_counter2 = disturb_critical_section_counter;
    cu_print("Task 1 is going to sleep during 20ms => Task 2 shall log 'T2-' \n");
    enable_scheduling();

    local_task_sleep_ms(20); /* allow task preemption to be sure T2 is still alive */

     /* start a new critical section disabling scheduling 3 times */
    disable_scheduling();
    disable_scheduling();
    disable_scheduling();
    saved_disturb_critical_section_counter3 = disturb_critical_section_counter;
    cu_print("start new section critic with disable_scheduling_reenterable\n");

    /* wait for quite some time */
    i = 0x1FFFFF;
    while(--i)
    {
#ifdef ZEPHYR_NANO
        local_task_sleep_ms(10); /* allow task preemption */
#endif
    }

    /* enable scheduling 1st time => it should not have effect */
    enable_scheduling();
    /* wait for quite some time again to be sure T2 is not activated */
    i = 0x1FFFFF;
    while(--i)
    {
#ifdef ZEPHYR_NANO
        local_task_sleep_ms(10); /* allow task preemption */
#endif
    }

    /* enable scheduling 2nd time => it should not have effect */
    enable_scheduling();
    /* wait for quite some time again and again to be sure T2 is not activated */
    i = 0x1FFFFF;
    while(--i)
    {
#ifdef ZEPHYR_NANO
        local_task_sleep_ms(10); /* allow task preemption */
#endif
    }

    cu_print("end section critic \n");
    saved_disturb_critical_section_counter4 = disturb_critical_section_counter;
    /* enable scheduling 3rd time => scheduling is actived */
    enable_scheduling();

    b_task1_complete = true;
}

/* this task (T2) of higher priority as for goal to disturb the critical section run
 * by the lower priority task1 (T1)
 */
void fct_task2_disable_scheduling(void)
{
    task2_started = true;
    cu_print("\n");
    while(!b_end_disturbing_task)
    {
        /* update the watch variable */
        disturb_critical_section_counter ++;
        cu_print("T2-");
        local_task_sleep_ms(5);
    }
    cu_print("\n");
    task2_started = false;
}
