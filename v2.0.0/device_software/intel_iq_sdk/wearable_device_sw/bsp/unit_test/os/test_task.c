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

#include "utility.h"
#include "test_task.h"
#include "os/os.h"
#include "test_mutex_task_fct.h"
#include "test_sema_task_fct.h"
#include "test_critical_section_task_fct.h"
#include "test_queue.h"

/* --------- Forward declarations */
static void fct_task_idle(void);

/* --------- Global variables */
void (*task1_fct_ptr[TK1_SIZE])(void) ={
                                            fct_task_idle,
                                            fct_task1_simple_mutex,
                                            fct_task1_priority_inversion_mutex,
                                            fct_task1_sema_used_as_mutex,
                                            fct_task1_sema_consumer,
                                            fct_task1_disable_scheduling,
                                            fct_task1_test_queue };




void (*task2_fct_ptr[TK2_SIZE])(void) ={
                                            fct_task_idle,
                                            fct_task2_simple_mutex,
                                            fct_task2_priority_inversion_mutex,
                                            fct_task2_sema_used_as_mutex,
                                            fct_task2_disable_scheduling,
                                            fct_task2_test_queue };


/**
 * \brief Idle task for test suite
 */
void fct_task_idle(void)
{
    local_task_sleep_ms(100);
}



/**
 * \brief Generic Task1
 */
#if defined ZEPHYR_NANO
void task1(int dummy1, int dummy2)
#elif defined ZEPHYR_MICRO
void task1(void)
#elif defined OS_FREERTOS
void task1(void)
#endif
{
#ifdef ZEPHYR_NANO
    UNUSED(dummy1);
    UNUSED(dummy2);
#endif
    while(1)
    {
        if(task1_fct_id == TK1_IDLE)
        {
            (*task1_fct_ptr[task1_fct_id])();
        }
        else if( (task1_fct_id >= 0) && (task1_fct_id < DIM(task1_fct_ptr))  )
        {
            (*task1_fct_ptr[task1_fct_id])();
            task1_fct_id = TK1_IDLE;    /* set back to idle */
        }
    }
}


/**
 * \brief Generic Task1
 */
#if defined ZEPHYR_NANO
void task2(int dummy1, int dummy2)
#elif defined ZEPHYR_MICRO
void task2(void)
#elif defined OS_FREERTOS
void task2(void)
#endif
{
#ifdef ZEPHYR_NANO
    UNUSED(dummy1);
    UNUSED(dummy2);
#endif

    while(1)
    {
        if(task2_fct_id == TK2_IDLE)
        {
            (*task2_fct_ptr[task2_fct_id])();
        }
        else if( (task2_fct_id >= 0) && (task2_fct_id < DIM(task2_fct_ptr)) )
        {
            (*task2_fct_ptr[task2_fct_id])();
            task2_fct_id = TK2_IDLE;    /* set back to idle */
        }
    }
}
