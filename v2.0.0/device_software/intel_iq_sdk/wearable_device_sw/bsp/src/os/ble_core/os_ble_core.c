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

#include <stdlib.h>
#include "os/os.h"
#include "util/list.h"

/* #include "boards.h" TODO: include correct BLE Core Quark SE configuration */

/**\brief BLE Core supports only 1 queue by default. */
#ifndef MAX_NUM_QUEUES
#define MAX_NUM_QUEUES 1
#endif

/**\brief BLE Core has limited memory, limit the number of list entries */
#ifndef MAX_LIST_ENTRIES
#define MAX_LIST_ENTRIES    0x0ffu
#endif

#ifdef TRACK_ALLOCS
int alloc_count = 0;
#endif

uint32_t interrupt_lock()
{
    /* TODO: map on BLE Core interrupt disable API if required */
    return 0;
}

void interrupt_unlock(uint32_t flags)
{
    /* TODO: map on BLE Core interrupt disable API if required */
}

void * balloc(uint32_t size, OS_ERR_TYPE * err)
{
    void * ptr;
    unsigned int flags = interrupt_lock();
    ptr = malloc(size);
    if (!ptr) {
        if (err)
            *err = E_OS_ERR_NO_MEMORY;
        else
            panic(E_OS_ERR_NO_MEMORY);
    }
#ifdef TRACK_ALLOCS
    alloc_count++;
#endif
    interrupt_unlock(flags);
    return ptr;
}

OS_ERR_TYPE bfree(void * ptr)
{
    OS_ERR_TYPE err = E_OS_OK;
    int flags = interrupt_lock();
#ifdef TRACK_ALLOCS
    alloc_count--;
#endif
    free(ptr);
    interrupt_unlock(flags);
    return err;
}

struct queue_ {
    list_head_t lh;
    uint8_t     count;
    uint8_t     max_size;
    uint8_t     max_cnt;
    uint8_t     used;
};

/* currently only one queue supported */
static struct queue_ q_pool[MAX_NUM_QUEUES];

static void queue_put(void *queue, void *msg)
{
    struct queue_ * q = (struct queue_ *) queue;

    if (q->count < q->max_size) {
        list_add(&q->lh, (list_t *)msg);
#ifdef DEBUG_OS
        pr_debug(LOG_MODULE_OS, "queue_put: %p <- %p", queue, msg);
#endif
        q->count++;
        if (q->count > q->max_cnt)
            q->max_cnt = q->count;
    } else {
        panic(E_OS_ERR_NO_MEMORY);
    }
}

static void * queue_wait(void *queue)
{
    struct queue_ * q = (struct queue_*) queue;

    void * elem = (void *)list_get(&q->lh);
#ifdef DEBUG_OS
    pr_debug(LOG_MODULE_OS, "queue_wait: %p -> %p", queue, elem);
#endif
    if (q->count)
        q->count--;
    return elem;
}

void queue_get_message(T_QUEUE queue, T_QUEUE_MESSAGE* message, int timeout, OS_ERR_TYPE* err)
{
    *message = queue_wait(queue);
}

void queue_send_message(T_QUEUE queue, T_QUEUE_MESSAGE message, OS_ERR_TYPE* err)
{
    queue_put(queue, message);
}

T_QUEUE queue_create(uint32_t max_size, OS_ERR_TYPE * err)
{
    int i, found=0;
    struct queue_ * q;
    for (i = 0; i < MAX_NUM_QUEUES; i++) {
        q = &q_pool[i];
        if (q->used == 0) {
            q->used = 1;
            found = 1;
        }
    }
    if (!found)
        return (T_QUEUE)NULL;
    list_init(&q->lh);
    q->count = 0;
    q->max_cnt = 0;
    q->max_size = (max_size < MAX_LIST_ENTRIES) ? (uint8_t)max_size : MAX_LIST_ENTRIES;
    return (T_QUEUE) q;
}

void queue_delete(T_QUEUE queue, OS_ERR_TYPE* err)
{
    void * element = NULL;
    struct queue_ * q = (struct queue_*) queue;

    while ((element = list_get(&q->lh)) != NULL) {
        list_remove(&q->lh, element);
        if (q->count)
            q->count--;
        bfree(element);
    }
}
