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

#ifndef __TEST_TASK_H__
#define __TEST_TASK_H__

enum {
    TK1_IDLE = 0,                              /* default task to run IDLE */
    TK1_TEST_SIMPLE_MUTEX,
    TK1_TEST_PRIORITY_INVERSION_MUTEX,
    TK1_TEST_SEMA_USED_AS_MUTEX,
    TK1_TEST_SEMA_CONSUMER,
    TK1_TEST_DISABLE_SCHEDULING,
    TK1_TEST_QUEUES,
    /* end , do not put anything below */
    TK1_SIZE
} task1_fct_id;


enum {
    TK2_IDLE = 0,                              /* default task to run IDLE */
    TK2_TEST_SIMPLE_MUTEX,
    TK2_TEST_PRIORITY_INVERSION_MUTEX,
    TK2_TEST_SEMA_USED_AS_MUTEX,
    TK2_TEST_DISABLE_SCHEDULING,
    TK2_TEST_QUEUES,
    /* end , do not put anything below */
    TK2_SIZE
} task2_fct_id;


#if defined ZEPHYR_NANO

extern void task1(int dummy1, int dummy2);
extern void task2(int dummy1, int dummy2);

#elif defined ZEPHYR_MICRO

extern void task1(void);
extern void task2(void);

#elif defined OS_FREERTOS

#warning Fix prototypes of task entry points for unit test tasks
extern void task1(void);
extern void task2(void);

#endif

#endif    /* __TEST_TASK_H__  */
