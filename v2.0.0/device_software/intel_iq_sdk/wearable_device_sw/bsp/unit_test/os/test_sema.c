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
#include "test_stub.h"

/* -------------- global variable -------------- */
static T_SEMAPHORE sema = NULL;
static volatile bool b_task1_semaphore_taken = false;
static volatile bool b_task2_semaphore_taken = false;
static volatile bool b_task1_release_semaphre = false;        // release == give
static volatile bool b_task2_release_semaphore = false;       // release == give
static volatile bool b_task1_sema_consumer_started = true;
static volatile bool b_end_task1_sema_consumer = false;
static volatile uint8_t nb_of_msg_consumed = 0;



void isr_clbk_test_sema(void* data);


/* this function just test the function call */
void test_sema_functions(void)
{
    OS_ERR_TYPE err = E_OS_OK;
    uint8_t i=0;
    int count=0;

    sema = semaphore_create(0, NULL);
    CU_ASSERT("create semaphore failed", sema != NULL);
    semaphore_delete(sema, NULL);

    /* Normal use case ( create/ give / take / delete) */
    sema = semaphore_create(0, &err);
    CU_ASSERT("create semaphore failed", sema != NULL && err == E_OS_OK);
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);
    err = semaphore_take(sema, OS_WAIT_FOREVER);
    CU_ASSERT("semaphore take failed", err == E_OS_OK);
    err = semaphore_take(sema, 5);    /* timeout = 5ms */
    CU_ASSERT("semaphore take failed", err == E_OS_ERR_TIMEOUT);
    semaphore_delete(sema, &err);
    CU_ASSERT("semaphore lock failed", err == E_OS_OK);


    /* Test with initialCount set to 3 ( create / take x 3 / delete) */
    sema = semaphore_create(3, &err);

    CU_ASSERT("create semaphore failed", sema != NULL && err == E_OS_OK);
    for(i=0; i<3; i++)
    {
        count = semaphore_get_count(sema, NULL);
        CU_ASSERT("semaphore count invalid", count == (3-i));
        err = semaphore_take(sema, OS_WAIT_FOREVER);
        CU_ASSERT("semaphore take failed", err == E_OS_OK);
    }
    err = semaphore_take(sema, 5);    /* timeout = 5ms */
    CU_ASSERT("semaphore take failed", err == E_OS_ERR_TIMEOUT);
    semaphore_delete(sema, &err);
    CU_ASSERT("semaphore lock failed", err == E_OS_OK);


    /* Test with initialCount set to 3 ( create / take x 3 / delete) */
    sema = semaphore_create(3, &err);
    CU_ASSERT("create semaphore failed", sema != NULL && err == E_OS_OK);
    for(i=0; i<3; i++)
    {
        count = semaphore_get_count(sema, NULL);
        CU_ASSERT("semaphore count invalid", count == (3-i));
        err = semaphore_take(sema, 5);    /* timeout = 5ms */
        CU_ASSERT("semaphore take failed", err == E_OS_OK);
    }
    err = semaphore_take(sema, 5);    /* timeout = 5ms */
    CU_ASSERT("semaphore take failed", err == E_OS_ERR_TIMEOUT);
    semaphore_delete(sema, &err);
    CU_ASSERT("semaphore lock failed", err == E_OS_OK);

    /* try to use a deleted semaphore */
    err = semaphore_take(sema, 5);    /* timeout = 5ms */
    CU_ASSERT("semaphore take failed", err == E_OS_ERR);
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_ERR);
}

/* this function just test the allocation of 32 semaphores */
void test_sema_allocation(void)
{
    OS_ERR_TYPE err = E_OS_OK;
#ifdef CONFIG_ARC_OS_UNIT_TESTS
    /* 32 semaphore allowed but only 29 semaphores are available:
     *      1 is used by first queue initialisation in ARC (see platform_init())
     *      2 used by log */
    T_SEMAPHORE sm[29]={NULL};
#else
    T_SEMAPHORE sm[32]={NULL};
#endif
    T_SEMAPHORE extra_sm = NULL;
    uint8_t i;

    for(i=0; i<DIM(sm); i++)
    {
        sm[i]= semaphore_create(i, &err);
        CU_ASSERT("create semaphore failed", sm[i] != NULL && err == E_OS_OK);
    }

    /* try to allocate an extra semaphore and check it fail */
    extra_sm = semaphore_create(0, &err);
    CU_ASSERT("create semaphore failed", extra_sm == NULL && err == E_OS_ERR);
    semaphore_delete(extra_sm, &err);
    CU_ASSERT("semaphore lock failed", err == E_OS_ERR);

    for(i=0; i<DIM(sm); i++)
    {
        semaphore_delete(sm[i], &err);
        CU_ASSERT("semaphore lock failed", err == E_OS_OK);
    }
}

/*---------------------------------- test semaphore used as mutex -----------------------------*/

/* this function test the semaphore used as mutex (binary semaphore)
 *
 * test  is set with priority = 10
 * task1 is set with priority = 30
 * task2 is set with priority = 20
 */

void test_sema_used_as_mutex(void)
{
    CU_ASSERT("check task1 not in use", task1_fct_id == TK1_IDLE);
    CU_ASSERT("check task2 not in use", task2_fct_id == TK2_IDLE);

    sema = semaphore_create(1,NULL); /* initial count set to 1 because used as mutex */

    /* start Task1*/
    task1_fct_id = TK1_TEST_SEMA_USED_AS_MUTEX;

    /* wait until the semaphore is taken by the Task 1 */
    while(!b_task1_semaphore_taken)
    {
        local_task_sleep_ms(100);
    }

    /* start Task2*/
    task2_fct_id = TK2_TEST_SEMA_USED_AS_MUTEX;

    local_task_sleep_ms(100);

    CU_ASSERT("task2 sema must not be taken (hold by task1)", b_task2_semaphore_taken == false);

    /* tell Task1 to release/give the semaphore */
    b_task1_release_semaphre = true;

    /* wait until the semaphore is taken by the Task 2 */
    while(!b_task2_semaphore_taken)
    {
        local_task_sleep_ms(100);
    }

    /* tell Task2 to release the semaphore */
    b_task2_release_semaphore = true;

    /* wait until the sema is unlocked by the Task 2 */
    while(b_task2_semaphore_taken)
    {
        local_task_sleep_ms(100);
    }

    semaphore_delete(sema, NULL);
}

void fct_task1_sema_used_as_mutex(void)
{
    OS_ERR_TYPE err = E_OS_OK;
    err = semaphore_take(sema, OS_WAIT_FOREVER);
    CU_ASSERT("semaphore take failed", err == E_OS_OK);

    b_task1_semaphore_taken = true;
    /* wait until the the test ask me to release/give the semaphore */
    while(!b_task1_release_semaphre)
    {
        local_task_sleep_ms(100);
    }
    semaphore_give(sema, NULL);
    b_task1_semaphore_taken = false;
}

void fct_task2_sema_used_as_mutex(void)
{
    semaphore_take(sema, OS_WAIT_FOREVER);
    b_task2_semaphore_taken = true;
    /* wait until the the test ask me to release/give the semaphore */
    while(!b_task2_release_semaphore)
    {
        local_task_sleep_ms(100);
    }
    semaphore_give(sema, NULL);
    b_task2_semaphore_taken = false;
}



/*---------------------------------- producer consumer semaphore test -----------------------------*/

/* test the producer consumer scenario
 *
 * test  (producer) is set with priority = 10
 * task1 (consumer) is set with priority = 30
 */
void test_sema_producer_consumer(void)
{

    volatile int count;
    OS_ERR_TYPE err = E_OS_OK;

    CU_ASSERT("check task1 not in use", task1_fct_id == TK1_IDLE);

    sema = semaphore_create(0, NULL);
    count = semaphore_get_count(sema, NULL);
    CU_ASSERT("semaphore count invalid", count == 0);

    /* start Task1*/
    task1_fct_id = TK1_TEST_SEMA_CONSUMER;

    CU_ASSERT("number of msg consumed invalid", nb_of_msg_consumed==0);

    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);
    local_task_sleep_ms(100);

    CU_ASSERT("number of msg consumed invalid", nb_of_msg_consumed==1);

    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);

    /* give some time for the task 1 to consume the above semaphores */
    local_task_sleep_ms(100);

    b_end_task1_sema_consumer = true;
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);

    /* wait until task 1 ended */
    while(b_task1_sema_consumer_started)
    {
        local_task_sleep_ms(100);
    }

    CU_ASSERT("number of msg consumed invalid", nb_of_msg_consumed==5);

    count = semaphore_get_count(sema, NULL);
    CU_ASSERT("semaphore count invalid", count == 0);

    semaphore_delete(sema, NULL);
}


void fct_task1_sema_consumer(void)
{

    b_task1_sema_consumer_started = true;
    while(!b_end_task1_sema_consumer)
    {
        semaphore_take(sema, OS_WAIT_FOREVER);
        nb_of_msg_consumed++;
    }
    b_task1_sema_consumer_started = false;
}


/* trigger an interruption and call a semaphore create/give/take/delete
 * from the interrupt handler and check that E_OS_ERR_NOT_ALLOWED is returned
 * except for "semaphore_give()
 */
void test_sema_in_interruption_ctx(void)
{
    OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
    int32_t count;
    interrupt_param_t it_parm ={E_CALLBACK_SEMA, NULL};

    sema = semaphore_create(0, &err);
    CU_ASSERT("semaphore_create", sema != NULL && err == E_OS_OK);

    trigger_mbx_isr(&it_parm);

    count = semaphore_get_count(sema, &err);
    CU_ASSERT("semaphore_get_count", err == E_OS_OK);
    CU_ASSERT("semaphore_get_count", count == 2);   /* semaphore signal called 2 times by the ISR */

    semaphore_delete(sema, &err);
    CU_ASSERT("semaphore_delete", err == E_OS_OK);
}


/* test in isr context */
void isr_clbk_test_sema(void* data)
{
    OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
    T_SEMAPHORE sema_isr = NULL;
    int32_t count;

    /* check error returned */
    sema_isr = semaphore_create(0, &err);
    CU_ASSERT("create semaphore failed", sema_isr == NULL && err == E_OS_ERR_NOT_ALLOWED);
    semaphore_give(sema, &err);
    CU_ASSERT("semaphore give failed", err == E_OS_OK);
    err = semaphore_take(sema, OS_WAIT_FOREVER);
    CU_ASSERT("semaphore take failed", err == E_OS_ERR_NOT_ALLOWED);
    err = semaphore_take(sema, 5);
    CU_ASSERT("semaphore take failed", err == E_OS_ERR_NOT_ALLOWED);
    count = semaphore_get_count(sema, &err);
    CU_ASSERT("semaphore_get_count", err == E_OS_OK);
#ifndef OS_ZEPHYR
    CU_ASSERT("semaphore_get_count",  count ==1);   /* ZEPHYR (micro&nano) doesn't support semaphore_get_count() in ISR context, value returned is invalid but no error or panic raised */
#endif
    semaphore_delete(sema, &err);
    CU_ASSERT("semaphore_delete", err == E_OS_ERR_NOT_ALLOWED);


    /* check panic */
    sema_isr = semaphore_create(0, NULL);
    CU_ASSERT("panic excepted", sema_isr == NULL && did_panic() );
    clear_panic();
    semaphore_give(sema, NULL);
    CU_ASSERT("NO panic excepted", did_panic() == false );
    count = semaphore_get_count(sema, NULL);
    CU_ASSERT("NO panic excepted", did_panic() == false );
#ifndef OS_ZEPHYR
    CU_ASSERT("semaphore_get_count",  count == 2);   /* ZEPHYR (micro&nano) doesn't support semaphore_get_count() in ISR context, value returned is invalid but no error or panic raised */
#endif
    semaphore_delete(sema, NULL);
    CU_ASSERT("panic excepted", did_panic() );
    clear_panic();
}


