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

#include <string.h>
#include "pinmux.h"
#include "util/cunit_test.h"
#include "test_task.h"
#include "utility.h"
#include "test_queue.h"
#include "infra/log.h"
#include "machine/soc/intel/quark_se/quark/log_backend_uart.h"

#include "test_stub.h"
#include "nanokernel.h"

#ifdef CONFIG_QUARK_SE_QUARK
#include "zephyr/common.h"
#include <zephyr.h>
#else
#include <misc/printk.h>
static void os_printk_puts(const char *s, uint16_t len)
{
    uint16_t i;
    char c[2];

    c[1] = '\0';

    for(i = 0; i < len ; i++) {
        c[0] = s[i];
        printk("%s", c);
    }
}
struct log_backend log_backend_os_printk = { os_printk_puts };
#endif


#define UNIT_TEST_FIBER_STACK_SIZE   670
#define UNIT_TEST_FIBER_PRIORITY      50
#define UNIT_TEST_FIBER_OPTIONS        0
T_SEMAPHORE sema_test_task;

static void test_sync (void)
{
    cu_print(" Test of synchronization objects\n");

    /* Mutex */
    CU_RUN_TEST(test_mutex_functions);
    CU_RUN_TEST(test_mutex_allocation);
    CU_RUN_TEST(test_mutex_simple_lock);
    CU_RUN_TEST(test_recursive_mutex);
    CU_RUN_TEST(test_mutex_priority_inversion);
#ifndef CONFIG_ARC
    CU_RUN_TEST(test_mutex_in_interruption_ctx);
#endif

    /* Semaphore */
    CU_RUN_TEST(test_sema_functions);
    CU_RUN_TEST(test_sema_allocation);
    CU_RUN_TEST(test_sema_used_as_mutex);
    CU_RUN_TEST(test_sema_producer_consumer);
#ifndef CONFIG_ARC
    CU_RUN_TEST(test_sema_in_interruption_ctx);
#endif
    /* Section Critic */
#ifndef ZEPHYR_NANO
    CU_RUN_TEST(test_disable_scheduling);
#endif
    cu_print("======================\n");
}

static void test_queue (void)
{
    cu_print(" Test of message queues\n");
    CU_RUN_TEST(test_queue_unit_testing); /* important: must be run before other queue tests */
    CU_RUN_TEST(test_queue_functional_testing_overflow_one_queue); /* important: must be run before test_queue_functional_testing_message_order */
    CU_RUN_TEST(test_queue_functional_testing_message_order); /* important: must be run after test_queue_functional_testing_overflow */
    CU_RUN_TEST(test_queue_functional_testing_overflow_all_queues);
    CU_RUN_TEST(test_queue_functional_testing_different_tasks);
#ifndef CONFIG_ARC
    CU_RUN_TEST(test_queue_interrupt);
#endif
    cu_print("======================\n");
}

static void test_timer (void)
{
    cu_print(" Test of timers\n");
    CU_RUN_TEST(test_timer_functions);
    CU_RUN_TEST(test_timer_allocation);
    CU_RUN_TEST(test_timer_callback);
    CU_RUN_TEST(test_timer_callback_with_timer_stop);
    CU_RUN_TEST(test_timer_restart);
    CU_RUN_TEST(test_timer_millisecond_timer_incrementation);
    CU_RUN_TEST(test_timer_microsecond_timer_incrementation);
    CU_RUN_TEST(test_timer_stat);
    cu_print("======================\n");
}

static void test_interrupt (void)
{
    cu_print(" Test interrupts\n");
#ifndef CONFIG_ARC
    CU_RUN_TEST(test_interrupt_simple_case);
    CU_RUN_TEST(test_interrupt_lock_unlock);
    CU_RUN_TEST(test_interrupt_enable_disable);
#endif
    cu_print("======================\n");
}

static void test_malloc(void)
{
    cu_print(" Test balloc");
    CU_RUN_TEST(test_malloc_and_free_1);
    CU_TEST_DISABLED(test_malloc_and_free_2);
#ifndef CONFIG_ARC
    CU_RUN_TEST(test_malloc_in_interruption_ctx);
#endif
    cu_print("======================\n");
}



/**
 * \brief Main function of the CUnit tests
 * This task is defined with priority = 10 (high priority
 *
 */
#ifdef  ZEPHYR_NANO
void test_task(int i1, int i2)
#else
void test_task(void * param)
#endif
{
#ifdef  ZEPHYR_NANO
UNUSED(i1);
UNUSED(i2);
#endif

    /* initialize test variables and start test tasks */
#ifdef CONFIG_QUARK_SE_QUARK
    pinmux_config();
    cu_set_log_backend(&log_backend_uart);
#else
    cu_set_log_backend(&log_backend_os_printk);
#endif

    /* initialize the OS abstraction */
#ifdef ZEPHYR_MICRO
    os_init();
    CU_ASSERT("Test: os_init - panic", did_panic() == false);
#endif

    clear_panic();

    test_interrupt_init();

    task1_fct_id = TK1_IDLE;
    task2_fct_id = TK2_IDLE;
    cunit_start_tasks();

    /* tests initialization */
    test_queue_init();

    cu_print("======================\n");
    cu_print(" OS Abstraction Tests");
#ifdef  ZEPHYR_NANO
    cu_print(" OS: ZEPHYR_NANO\n");
#endif
#ifdef  ZEPHYR_MICRO
    cu_print(" OS: ZEPHYR_MICRO\n");
#endif
    cu_print("======================\n");

    /* run test sequences */
    test_malloc();
    test_sync();
    test_queue();
    test_timer();
    test_interrupt();

#if defined (CONFIG_MATHLIB)
    cu_print("======================\n");
    cu_print(" Lib Mathematics Tests");
    CU_RUN_TEST(lib_math_test);
#endif

    semaphore_give(sema_test_task, NULL);
}

void run_os_test_suite()
{
    OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;

    /* Wait the end of Viper OS nano test suite */
    sema_test_task = semaphore_create(0, &err);

#ifdef  ZEPHYR_NANO
    /* allocate stack for the Unit Test fiber */
    static uint32_t g_UnitTestFiberStack [UNIT_TEST_FIBER_STACK_SIZE/sizeof(uint32_t)] ;


    /* start unit_test_main in a new fiber */
    task_fiber_start (
        (char*) g_UnitTestFiberStack,
        UNIT_TEST_FIBER_STACK_SIZE,
        (nano_fiber_entry_t)test_task,
        0,0,
        UNIT_TEST_FIBER_PRIORITY,
        UNIT_TEST_FIBER_OPTIONS );
#else
    test_task(NULL);
#endif

    err = semaphore_take(sema_test_task, OS_WAIT_FOREVER);
    semaphore_delete(sema_test_task, NULL);

}
