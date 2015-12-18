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
#include "test_stub.h"
#include "util/cunit_test.h"
#include "test_task.h"


/* -------------- global variable -------------- */
static T_MUTEX mutex = NULL;
static volatile bool b_task1_mutex_locked = false;
static volatile bool b_task2_mutex_locked = false;
static volatile bool b_task1_release_mutex = false;
static volatile bool b_task2_release_mutex = false;
static volatile bool b_task2_running = false;
static volatile bool b_isr_called = false;


void isr_clbk_test_mutex(void* data);



/* test the mutex functions call */
void test_mutex_functions(void)
{
    OS_ERR_TYPE err = E_OS_OK;

    mutex = mutex_create(NULL);
    CU_ASSERT("create mutex failed", mutex != NULL);
    mutex_delete(mutex, NULL);

    /* Normal use case ( create/ lock / unlock / delete) */
    mutex = mutex_create(&err);
    CU_ASSERT("create mutex failed", mutex != NULL && err == E_OS_OK);
    err = mutex_lock(mutex, OS_WAIT_FOREVER);
    CU_ASSERT("mutex lock failed", err == E_OS_OK);
    mutex_unlock (mutex, &err);
    CU_ASSERT("mutex unlock failed", err == E_OS_OK);
    mutex_delete(mutex, &err);
    CU_ASSERT("mutex lock failed", err == E_OS_OK);

    // check wrong call order (call unlock before lock)
    mutex = mutex_create(&err);
    CU_ASSERT("create mutex failed", mutex != NULL);
    mutex_unlock (mutex, &err);
    CU_ASSERT("try to unlock a mutex not locked", err == E_OS_ERR);
    mutex_delete(mutex, &err);
    CU_ASSERT("delete mutex failed", err == E_OS_OK);
}

/* test the allocation of 32 mutexes */
void test_mutex_allocation(void)
{
    OS_ERR_TYPE err = E_OS_OK;
#ifdef CONFIG_ARC_OS_UNIT_TESTS
	/* 32 mutexes allowed but only 28 mutexes are available:
	 * 1 is used in log_init */
    T_MUTEX mx[31] = {NULL};
#else
    T_MUTEX mx[32] = {NULL};
#endif
    T_MUTEX extra_mx = NULL;
    uint8_t i;

    for(i=0; i<DIM(mx); i++)
    {
        mx[i]= mutex_create(&err);
        CU_ASSERT("create mutex failed", mutex != NULL && err == E_OS_OK);
    }

    /* try to allocate an extra mutex and check it fail */
    extra_mx = mutex_create(&err);
    CU_ASSERT("create mutex failed", extra_mx == NULL && err == E_OS_ERR);
    mutex_delete(extra_mx, &err);
    CU_ASSERT("mutex lock failed", err == E_OS_ERR);

    for(i=0; i<DIM(mx); i++)
    {
        mutex_delete(mx[i], &err);
        CU_ASSERT("mutex lock failed", err == E_OS_OK);
    }
}
/*---------------------------------- simple mutex test -----------------------------*/

/* test simple mutex lock and release and makes sure
 * the resource locked cannot be accessed by someone else.
 *
 * test  is set with priority = 10
 * task1 is set with priority = 30
 * task2 is set with priority = 20
 */

void test_mutex_simple_lock(void)
{
    CU_ASSERT("check task1 not in use", task1_fct_id == TK1_IDLE);
    CU_ASSERT("check task2 not in use", task2_fct_id == TK2_IDLE);

    mutex = mutex_create(NULL);

    /* start Task1*/
    task1_fct_id = TK1_TEST_SIMPLE_MUTEX;

    /* wait until the mutex is locked by the Task 1 */
    while(!b_task1_mutex_locked)
    {
        local_task_sleep_ms(100);
    }

    /* start Task2*/
    task2_fct_id = TK2_TEST_SIMPLE_MUTEX;

    local_task_sleep_ms(100);

    CU_ASSERT("task2 mutex must not be locked (hold by task1)", b_task2_mutex_locked == false);

    /* tell Task1 to release the mutex */
    b_task1_release_mutex = true;

    /* wait until the mutex is locked by the Task 2 */
    while(!b_task2_mutex_locked)
    {
        local_task_sleep_ms(100);
    }

    /* tell Task2 to release the mutex */
    b_task2_release_mutex = true;

    /* wait until the mutex is unlocked by the Task 2 */
    while(b_task2_mutex_locked)
    {
        local_task_sleep_ms(100);
    }

    mutex_delete(mutex, NULL);
}

void fct_task1_simple_mutex(void)
{
    mutex_lock(mutex, OS_WAIT_FOREVER);
    b_task1_mutex_locked = true;
    /* wait until the the test ask me to release the mutex */
    while(!b_task1_release_mutex)
    {
        local_task_sleep_ms(100);
    }
    b_task1_release_mutex = false;
    mutex_unlock (mutex, NULL);
    b_task1_mutex_locked = false;
}

void fct_task2_simple_mutex(void)
{
    mutex_lock(mutex, OS_WAIT_FOREVER);
    b_task2_mutex_locked = true;
    /* wait until the the test ask me to release the mutex */
    while(!b_task2_release_mutex)
    {
        local_task_sleep_ms(100);
    }
    b_task2_release_mutex = false;
    mutex_unlock (mutex, NULL);
    b_task2_mutex_locked = false;
}

/*---------------------------------- recursive mutex test -----------------------------*/
/* test recursive mutex */
void test_recursive_mutex(void)
{
    /* not implemented for this release */
}

/*---------------------------------- priority inversion mutex test -----------------------------*/

/*test a priority inversion and make sure we don't get a deadlock
 *
 * A low priority thread T1 holding a Lock L1 .
 * A thread T2 with an medium priority is started, to prevent T1 to run.
 * The test function with the highest priority ask for the mutex hold by T1
 * Verify that T1 is boosted to the high priority.
 */
void test_mutex_priority_inversion(void)
{
    OS_ERR_TYPE err = E_OS_OK;

    CU_ASSERT("check task1 not in use", task1_fct_id == TK1_IDLE);
    CU_ASSERT("check task2 not in use", task2_fct_id == TK2_IDLE);

    mutex = mutex_create(NULL);

    /* start Task1*/
    task1_fct_id = TK1_TEST_PRIORITY_INVERSION_MUTEX;

    /* wait until the mutex is locked by the Task 1 */
    while(!b_task1_mutex_locked)
    {
        local_task_sleep_ms(100);
    }

    /* verify that the mutex is hold by task1 */
    err = mutex_lock(mutex, OS_NO_WAIT);
    CU_ASSERT("mutex expected to be use by Task1", err == E_OS_ERR_BUSY);

    /* start Task2 (medium priority task, who prevent task1 to run) */
    task2_fct_id = TK2_TEST_PRIORITY_INVERSION_MUTEX;

    /* wait until the Task 2 run */
    while(!b_task2_running)
    {
        local_task_sleep_ms(50);
    }

    /* verify that the mutex is still hold by task1 */
    err = mutex_lock(mutex, OS_NO_WAIT);
    CU_ASSERT("mutex expected to be locked by T1", err == E_OS_ERR_BUSY);

    /* tell Task1 to release the mutex, even if the Task1 cannot do it immediately
     * because Task2 have an higher priority  and prevent Task1 to run. */
    b_task1_release_mutex = true;

    local_task_sleep_ms(50);
    CU_ASSERT("mutex must still be locked by Task1", b_task1_mutex_locked = true);

    /* trigger priority inversion by requesting the resource/mutex access */
    err = mutex_lock(mutex, 100);
    CU_ASSERT("mutex lock", err == E_OS_OK);
    mutex_unlock (mutex, NULL);

    /* check priority inversion is working */
    CU_ASSERT("check mutex release properly", b_task1_mutex_locked == false);

    /* stop Task2  and check it's done */
    b_task2_running = false;
    while(task2_fct_id != TK2_IDLE)
    {
        local_task_sleep_ms(100);
    }
    CU_ASSERT("check task2 finished", task2_fct_id == TK2_IDLE);
    CU_ASSERT("check task1 finished", task1_fct_id == TK1_IDLE);

    mutex_delete(mutex, NULL);
}


void fct_task1_priority_inversion_mutex(void)
{
    mutex_lock(mutex, OS_WAIT_FOREVER);
    b_task1_mutex_locked = true;

    while(!b_task1_release_mutex)
    {
        local_task_sleep_ms(100);
    }

    b_task1_release_mutex = false;
    b_task1_mutex_locked = false;
    mutex_unlock (mutex, NULL);
}


/* medium priority task to prevent Task1 to run without priority inversion */
void fct_task2_priority_inversion_mutex(void)
{
    volatile int i = 0;

    b_task2_running = true;
    while(b_task2_running)
    {
        i++;
#ifdef ZEPHYR_NANO
        /* Rem: NanoKernel is not a preemptive system */
        local_task_sleep_ms(10);
#endif
    }
}


/* check mutex functions in an interrupt context */
void test_mutex_in_interruption_ctx(void)
{
    OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
    interrupt_param_t it_parm ={E_CALLBACK_MUTEX, NULL};

    mutex = mutex_create(&err);
    CU_ASSERT("mutex_create", mutex != NULL && err == E_OS_OK);

    /* trigger ISR */
    trigger_mbx_isr(&it_parm);
    /* The following busy wait is to be sure to wait the ISR to be executed */
    while(!b_isr_called)
    {
        cu_print("Waiting for the interrupt...\n");
    };
    CU_ASSERT("ISR not called", b_isr_called == true);

    mutex_delete(mutex, &err);
    CU_ASSERT("mutex_delete", err == E_OS_OK);
}


/* test in isr context */
void isr_clbk_test_mutex(void* data)
{
    OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;

    /* check error returned */
    T_MUTEX mutex_isr = NULL;
    mutex_isr = mutex_create(&err);
    CU_ASSERT("create mutex must fail in ISR context", mutex_isr == NULL && err == E_OS_ERR_NOT_ALLOWED);
    err = mutex_lock(mutex, OS_WAIT_FOREVER);
    CU_ASSERT("mutex lock failed", err == E_OS_ERR_NOT_ALLOWED);
    mutex_unlock (mutex, &err);
    CU_ASSERT("mutex unlock failed", err == E_OS_ERR_NOT_ALLOWED);
    mutex_delete(mutex, &err);
    CU_ASSERT("mutex lock failed", err == E_OS_ERR_NOT_ALLOWED);


    /* check panic */
    mutex_isr = mutex_create(NULL);
    CU_ASSERT("panic excepted", mutex_isr == NULL && did_panic() );
    clear_panic();
    mutex_unlock (mutex, NULL);
    CU_ASSERT("panic excepted", did_panic() );
    clear_panic();
    mutex_delete(mutex, NULL);
    CU_ASSERT("panic excepted", did_panic() );
    clear_panic();

    b_isr_called = true;
}
