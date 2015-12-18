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
#include "infra/log.h"
#include <stdlib.h>
#include "util/list.h"

/**
 * @defgroup os_linux Linux OS Abstraction Layer
 * Implements the linux OS abstraction layer.
 * @ingroup os
 * @{
 */

#define reset_err_ptr(error_ptr) \
    do { if (error_ptr != NULL) *error_ptr = E_OS_OK; } while(0)

#define set_error_panic(err_ptr, errno) \
    do { if (err_ptr == NULL) pr_error(LOG_MODULE_OS, "panic!"); else *err_ptr = errno; } while(0)


/*************************    MEMORY   *************************/

#ifdef TRACK_ALLOCS
int alloc_count = 0;
#endif

void * balloc(uint32_t size, OS_ERR_TYPE *err) {
    void * ptr;
    unsigned int flags = interrupt_lock();
    ptr = malloc(size+sizeof(void*));
    if (ptr) {
        (*(int*) ptr) = size;
        #ifdef TRACK_ALLOCS
            alloc_count++;
        #endif
    }
    interrupt_unlock(flags);
    return ptr;
}

OS_ERR_TYPE bfree(void *ptr) {
    int flags = interrupt_lock();
#ifdef TRACK_ALLOCS
    alloc_count--;
#endif
    free(ptr);
    interrupt_unlock(flags);
    return E_OS_OK;
}


/*************************    QUEUES   *************************/

typedef struct queue_ {
    list_head_t lh;
    int count;
    int used;
} q_t;

q_t q_pool[10]= {{0},};

void queue_put(void *queue, void *msg) {
    q_t * q = (q_t*) queue;
    list_add(&q->lh, (list_t *)msg);
#ifdef DEBUG_OS
    pr_debug(LOG_MODULE_OS, "queue_put: %p <- %p", queue, msg);
#endif
}

void * queue_wait(void *queue) {
    q_t * q = (q_t*) queue;
    void * elem = (void *)list_get(&q->lh);
#ifdef DEBUG_OS
    pr_debug(LOG_MODULE_OS, "queue_wait: %p -> %p", queue, elem);
#endif
    return elem;
}

void queue_get_message (T_QUEUE queue, T_QUEUE_MESSAGE* message, int timeout, OS_ERR_TYPE* err) {
    *message = queue_wait(queue);
}

void queue_send_message (T_QUEUE queue, T_QUEUE_MESSAGE message, OS_ERR_TYPE* err) {
    queue_put(queue, message);
}

T_QUEUE queue_create(uint32_t  max_size, OS_ERR_TYPE*err) {
    int i, found=0;
    q_t * q;
    for (i=0;i<10; i++) {
        q = &q_pool[i];
        if (q->used == 0) {
            q->used = 1;
            found = 1;
            break;
        }
    }
    if (!found) return (T_QUEUE)NULL;
    list_init(&q->lh);
    q->count = 0;
    return (T_QUEUE) q;
}

void queue_delete(T_QUEUE queue, OS_ERR_TYPE* err) {
    void * element = NULL;
    q_t * q = (q_t*) queue;
    while((element = list_get(&q->lh)) != NULL)
        list_remove(&q->lh, element);
    bfree(q);
}


/*************************    TIMERS   *************************/

//timer HAL
/**
 * this function is called by the os abstraction in order to initialize
 * the hardware timer.
 *
 * \param cb the callback function that hw timer implementation should call
 *           when it expires.
 * \param param the parameter passed to the callback function.
 */
void timer_hal_init(void (*cb)(void *param), void *param);
/**
 * this function should return the current time in ms of the hardware timer.
 *
 * \return the current hardware timer time in milliseconds
 */
int timer_hal_get_ms();

/**
 * This function request the harware timer callback to be called after the
 * specified delay.
 *
 * \param delay the delay after which the timer callback is requested
 */
void timer_hal_trigger(int delay);

/**
 * Timer internal structure.
 * This structure is allocated by timer_create()
 */
struct timer {
    /** timer list queueing element. */
    list_t list;
    /** timer callback function. called when the timer expires */
    T_ENTRY_POINT callback;
    /** argument for the callback function */
    void * arg;
    /** flag to indicate that timer is periodic */
    uint32_t repeat      :1;
    /** flag to indicate that the timer is active */
    uint32_t pending     :1;
    /** flag to indicate that the timer was retriggered in the callback */
    uint32_t retriggered :1;
    /** flag to indicate that we are in the context of the timer callback.
     * This is used by the timer_start(), timer_stop() and timer_delete()
     * functions.
     */
    uint32_t in_callback :1;
    /** flag to indicate that the timer was deleted in the context of
     * the callback function.
     */
    uint32_t deleted     :1;
    /** timer expiration time */
    uint32_t expires;
    /** period for periodic timer */
    uint32_t period;
};

list_head_t timer_list = {NULL, NULL};

/**
 * This structure is used to pass the current time to the timer list
 * traversal callback as well as returning the next expiration date
 * for the hardware timer at end of traversal.
 */
struct timer_cb_arg {
    uint32_t current_time;
    uint32_t next_time;
} timer_cb_arg;

/**
 * Callback function for timer expiration callback timer list traversal.
 */
int timer_foreach_cb(void * elem, void *param)
{
    struct timer_cb_arg * arg = (struct timer_cb_arg*)param;
    struct timer * t = (struct timer *) elem;
    if (t->deleted) {
        free(t);
        return 1;
    }
    if (t->expires <= arg->current_time && t->pending) {
        if (t->repeat) {
            t->expires = arg->current_time + t->period;
            if (t->expires < arg->next_time) {
                arg->next_time = t->expires;
            }
            t->in_callback = 1;
            t->callback(t->arg);
            t->in_callback = 0;
        } else {
            t->retriggered = 0;
            t->in_callback = 1;
            t->callback(t->arg);
            t->in_callback = 0;
            if (!t->retriggered) {
                t->pending = 0;
                return 1;
            }
        }
    } else if (t->pending && (t->expires < arg->next_time)) {
        arg->next_time = t->expires;
    }
    return 0;
}

/**
 * This function will fire timer callback for each timer that has expired.
 * It is called by the hardware timer expiration callback.
 */
void timer_callback(void *param)
{
    timer_cb_arg.current_time = timer_hal_get_ms();
    timer_cb_arg.next_time = 0xffffffff;
    list_foreach_del(&timer_list, timer_foreach_cb, (void*)&timer_cb_arg);
    if (timer_cb_arg.next_time != 0xffffffff) {
        timer_hal_trigger(timer_cb_arg.next_time - timer_cb_arg.current_time);
    }
}

/**
 * Callback function associated with timer list traversal of
 * timer_set_next_expiration()
 */
void timer_set_next_expiration_cb(void * elem, void *param)
{
    struct timer_cb_arg * arg = (struct timer_cb_arg*)param;
    struct timer * t = (struct timer *) elem;
    if (t->pending && (t->expires < arg->next_time)) {
        arg->next_time = t->expires;
    }
}

/**
 * This function will call timer_hal in order to set the next requested
 * timer expiration time.
 * This function is called each time a timer expires or a timer is started.
 */
void timer_set_next_expiration()
{
    timer_cb_arg.current_time = timer_hal_get_ms();
    timer_cb_arg.next_time = 0xffffffff;
    list_foreach(&timer_list, timer_set_next_expiration_cb, (void*)&timer_cb_arg);
    if (timer_cb_arg.next_time != 0xffffffff) {
        timer_hal_trigger(timer_cb_arg.next_time - timer_cb_arg.current_time);
    }
}

T_TIMER timer_create(T_ENTRY_POINT callback, void *privData, uint32_t delay, bool repeat, bool startup, OS_ERR_TYPE *err)
{
    struct timer * t = (struct timer*) malloc(sizeof(*t));
    if (!t)
        return t;
    t->callback = callback;
    t->arg = privData;
    t->period = delay;
    t->repeat = repeat;
    t->deleted = 0;
    if (startup) {
        int flags = interrupt_lock();
        list_add(&timer_list, &t->list);
        t->expires = timer_hal_get_ms() + delay;
        t->pending = 1;
        timer_set_next_expiration();
        interrupt_unlock(flags);
    } else {
        t->pending = 0;
    }
    return t;
}

void timer_start(T_TIMER tmr, uint32_t timeout, OS_ERR_TYPE* err)
{
    struct timer * t = (struct timer *) tmr;
    int flags = interrupt_lock();
    t->period = timeout;
    t->expires = timer_hal_get_ms() + timeout;
    if (!t->in_callback && !t->pending) {
        list_add(&timer_list, &t->list);
        t->pending = 1;
    } else {
        t->pending = 1;
        t->retriggered = 1;
    }
    timer_set_next_expiration();
    interrupt_unlock(flags);
}


void timer_stop(T_TIMER tmr, OS_ERR_TYPE* err)
{
    struct timer * t = (struct timer *) tmr;
    int flags = interrupt_lock();
    if (!t->in_callback) {
        list_remove(&timer_list, &t->list);
    }
    t->pending = 0;
    interrupt_unlock(flags);
}

void timer_delete(T_TIMER tmr, OS_ERR_TYPE* err)
{
    struct timer * t = (struct timer *) tmr;
    int flags = interrupt_lock();
    t->deleted = 1;
    if (t->in_callback) {
        interrupt_unlock(flags);
        return;
    }
    if (t->pending) {
        list_remove(&timer_list, &t->list);
    }
    interrupt_unlock(flags);
    free(t);
}

/*************************    SEMAPHORES   *************************/
typedef struct {
    uint32_t available;
    uint32_t waiting;
} sem_t;

T_SEMAPHORE semaphore_create(uint32_t initialCount, OS_ERR_TYPE* err)
{
    reset_err_ptr(err);

    sem_t * sem = (sem_t *) balloc(sizeof(sem_t), NULL);

    if (sem == NULL) {
        set_error_panic(err, E_OS_ERR);
    } else {
        sem->available = initialCount;
        sem->waiting = 0;
    }

    return (T_SEMAPHORE) sem;
}

void semaphore_delete(T_SEMAPHORE semaphore, OS_ERR_TYPE* err)
{
    sem_t * sema = (sem_t *) semaphore;

    reset_err_ptr(err);

    /* Make sure nobody is waiting on it otherwise report a failure */
    int flags = interrupt_lock();
    if (sema->waiting != 0) {
        set_error_panic(err, E_OS_ERR_BUSY);
    }
    interrupt_unlock(flags);

    bfree((void *)semaphore);
}

void semaphore_give(T_SEMAPHORE semaphore, OS_ERR_TYPE* err)
{
    sem_t * sema = (sem_t *) semaphore;
    reset_err_ptr(err);

    int flags = interrupt_lock();
    sema->available ++;
    interrupt_unlock(flags);
}

OS_ERR_TYPE semaphore_take(T_SEMAPHORE semaphore, int timeout)
{
    sem_t * sema = (sem_t *) semaphore;
    OS_ERR_TYPE error = E_OS_ERR_TIMEOUT;
    uint32_t time = timer_hal_get_ms();

    int flags = interrupt_lock();
    if (sema->available > 0) {
        sema->available --;
        error = E_OS_OK;
    } else if (timeout != OS_NO_WAIT) {
        sema->waiting++;

        do {
            /* Let a chance to an ISR to give us the semaphore */
            interrupt_unlock(flags);
            flags = interrupt_lock();

            if (sema->available > 0) {
                sema->available --;
                sema->waiting--;
                error = E_OS_OK;
                break;
            }
        } while(((int) (timer_hal_get_ms() - time) < timeout) || (timeout == OS_WAIT_FOREVER));
    }

    interrupt_unlock(flags);
    return error;
}

int32_t semaphore_get_count(T_SEMAPHORE semaphore, OS_ERR_TYPE* err)
{
    sem_t * sema = (sem_t *) semaphore;

    reset_err_ptr(err);

    int flags = interrupt_lock();
    int32_t count = sema->available - sema->waiting;
    interrupt_unlock(flags);

    return count;
}

/*************************    MUTEXES   *************************/
// Sema HAL
int8_t is_in_isr_context()
{
	return 0; /* linux runs only non-isr context */
}

/* List containing all locked mutexes in the system */
list_head_t mutex_list;

typedef struct {
    list_t list;
    uint32_t requestor;
    uint32_t count;
} mutex_t;

static bool list_finder_cb(list_t * plist, void * data)
{
    return (plist == data);
}

T_MUTEX mutex_create(OS_ERR_TYPE* err)
{
    reset_err_ptr(err);

    mutex_t * pmutex = (mutex_t*) balloc(sizeof(mutex_t), NULL);

    if (pmutex == NULL) {
        set_error_panic(err, E_OS_ERR_NO_MEMORY);
    } else {
        pmutex->requestor = (uint32_t) __builtin_return_address(0);
        pmutex->count = 0;
    }

    return (T_MUTEX) pmutex;
}

void mutex_delete(T_MUTEX mutex, OS_ERR_TYPE* err)
{
    reset_err_ptr(err);

    int flags = interrupt_lock();

    if (mutex == NULL) {
        set_error_panic(err, E_OS_ERR);
    } else if (list_find_first(&mutex_list, list_finder_cb, mutex) != NULL) {
        set_error_panic(err, E_OS_ERR_BUSY);
    } else {
        bfree((void *)mutex);
    }

    interrupt_unlock(flags);
}

void mutex_unlock(T_MUTEX mutex, OS_ERR_TYPE* err)
{
    mutex_t * pmutex = (mutex_t *) mutex;
    reset_err_ptr(err);

    if ((is_in_isr_context() != 0) || (mutex == NULL)) {
        set_error_panic(err, E_OS_ERR_NOT_ALLOWED);

    } else {
        int flags = interrupt_lock();

        if (list_find_first(&mutex_list, list_finder_cb, mutex) != NULL) {
            pmutex->count--;

            if (pmutex->count == 0)
                list_remove(&mutex_list, &pmutex->list);
        }

        interrupt_unlock(flags);
    }
}

OS_ERR_TYPE mutex_lock(T_MUTEX mutex, int timeout)
{
    mutex_t * pmutex = (mutex_t *) mutex;
    OS_ERR_TYPE error = E_OS_ERR_TIMEOUT;

    if ((is_in_isr_context() != 0) || (mutex == NULL)) {
        error = E_OS_ERR_NOT_ALLOWED;
    } else {
        int flags = interrupt_lock();

        pmutex->count++;

        /* If mutex not already locked add it to the lock list */
        if (list_find_first(&mutex_list, list_finder_cb, mutex) == NULL) {
            list_add(&mutex_list, &pmutex->list);
        }
        error = E_OS_OK;

        interrupt_unlock(flags);
    }

    return error;
}

/*************************    INIT   *************************/
void os_init() {
    timer_hal_init(timer_callback, NULL);

    list_init(&mutex_list);
}

/** @} */
