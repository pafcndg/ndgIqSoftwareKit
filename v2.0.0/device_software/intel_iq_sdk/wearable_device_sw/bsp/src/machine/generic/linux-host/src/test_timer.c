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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "os/os.h"

struct timer_hal_data {
    void (*cb)(void *);
    void * cb_data;
    int pending;
    int current;
    int trigger;
} timer_hal_data;

int timer1_count = 0;
int timer2_count = 0;
int timer3_count = 0;
int timer4_count = 0;

int timer_hal_init(void (*cb)(void*), void * param)
{
    timer_hal_data.cb = cb;
    timer_hal_data.cb_data = param;
    timer_hal_data.current = 0;
    return 0;
}

int timer_hal_get_ms(void)
{
    printf("%s: return: %d\n", __func__, timer_hal_data.current);
    return timer_hal_data.current;
}

void timer_hal_trigger(int timeout)
{
    timer_hal_data.trigger = timeout + timer_hal_data.current;
    timer_hal_data.pending = 1;
    printf("%s: pend: %d time:%d \n", __func__, timer_hal_data.pending, timer_hal_data.trigger);
}

void loop()
{
    timer_hal_data.current++;
    printf("Current time:%d pending:%d expire:%d\n",
            timer_hal_data.current,
            timer_hal_data.pending,
            timer_hal_data.trigger);
    if (timer_hal_data.pending &&
            (timer_hal_data.current >= timer_hal_data.trigger)) {
        timer_hal_data.pending = 0;
        timer_hal_data.cb(timer_hal_data.cb_data);
    }
}

T_TIMER t4;

void test_timer_cb(void *param)
{
    printf("%s: %s\n", __func__, (char*)param);
    if (!strcmp((char*)param, "timer1")) {
        timer1_count++;
    } else if (!strcmp((char*)param, "timer2")) {
        timer2_count++;
    } else if (!strcmp((char*)param, "timer3")) {
        timer3_count++;
    } else {
        timer4_count++;
        timer_stop(t4, NULL);
        if (timer4_count < 3) {
            timer_start(t4, 5, NULL);
        }
    }
}

void test_timers()
{
    os_init();

    T_TIMER t1 = timer_create(test_timer_cb, "timer1", 10, 1, 1, NULL);
    T_TIMER t2 = timer_create(test_timer_cb, "timer2", 20, 1, 1, NULL);
    T_TIMER t3 = timer_create(test_timer_cb, "timer3", 40, 0, 1, NULL);
    t4 = timer_create(test_timer_cb, "timer4", 10, 0, 0, NULL);
    assert(t3 && t4);

    timer_start(t4, 5, NULL);
    while(timer_hal_data.current < 400) {
        loop();
        if (timer_hal_data.current == 50) {
            timer_start(t3, 10, NULL);
        }
    }

    printf("timer1 count: %d timer2 count: %d timer3 count: %d\n",
            timer1_count, timer2_count, timer3_count);
}
