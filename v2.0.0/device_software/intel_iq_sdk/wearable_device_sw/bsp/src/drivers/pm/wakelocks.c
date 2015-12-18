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
#include "infra/pm.h"
#include "infra/device.h"
#include "infra/log.h"
#include <stdbool.h>
#include <errno.h>
#define WAKELOCK_MAX_TIMEOUT 5000
/*! Wakelock management structure */
struct pm_wakelock_mgr {
    list_head_t wl_list;      /*!< List that contains locked wakelocks, ordered by timeout */
    T_TIMER     wl_timer;     /*!< Timer to wait for next wakelock to expire */
    uint8_t     is_init;      /*!< Init state of wakelock structure */
    void        (*cb)(void*); /*!< Callback function to call when wakelock list is empty */
    void*       cb_priv;      /*!< Argument to pass with the callback function */
};

static volatile struct pm_wakelock_mgr pm_wakelock_inst = {
                            .is_init = 0,
                            .wl_list = {.head=NULL, .tail=NULL},
                            .cb = NULL,
                            .cb_priv = NULL};
int pm_wakelock_init_mgr()
{
    // Init ok
    pm_wakelock_inst.is_init = 1; // Wakelocks init ok
    return 0;
}

// ***********************************
// *        Wakelock user API        *
// ***********************************

void pm_wakelock_init(struct pm_wakelock *wli, int id)
{
    wli->start_time = 0;
    wli->lock = 0;
    wli->id = id;
    wli->timeout = 0;
    wli->list.next = NULL;
}

int pm_wakelock_acquire(struct pm_wakelock *wl)
{
    list_t *l;
    int ret = 0;
    // Acquire wakelock
    uint32_t saved = interrupt_lock();
    if (wl->lock) {
        ret = -EINVAL;
        goto exit;
    }
    pm_wakelock_set_any_wakelock_taken_on_cpu(true);

    uint32_t current_time = (uint32_t)get_time_ms();
    wl->lock = 1;

    wl->start_time = current_time;

    if (!(l = pm_wakelock_inst.wl_list.head)) {
        // List empty, add first item in the list
        pm_wakelock_inst.wl_list.head = pm_wakelock_inst.wl_list.tail = (list_t*)wl;
        wl->list.next = NULL;
    }
    else {
        // Insert item  at the end of the list
        for (; l->next; (l = l->next));
        pm_wakelock_inst.wl_list.tail = (list_t*)wl;
        wl->list.next = l->next;
        l->next = (list_t*)wl;
    }
exit:
    interrupt_unlock(saved);
    return ret;
}

int pm_wakelock_release(struct pm_wakelock *wl)
{
    list_t *l;
    int ret = 0;

    // Lock IRQs
    uint32_t saved = interrupt_lock();
    // Check if wakelock is already released
    if (!wl->lock) {
        ret = -EINVAL;
        goto exit;
    }
    // Release wakelock
    wl->lock = 0;
    l = pm_wakelock_inst.wl_list.head;

    if (l == (list_t*)wl) {
        // Check if there is only one item in the wakelock list
        if (pm_wakelock_inst.wl_list.tail == (list_t*)wl) {
            pm_wakelock_inst.wl_list.head = pm_wakelock_inst.wl_list.tail = NULL;
            pm_wakelock_set_any_wakelock_taken_on_cpu(false);
            // Call callback function to notify that all wakelocks are free
            if (pm_wakelock_inst.cb != NULL) {
                pm_wakelock_inst.cb(pm_wakelock_inst.cb_priv);
            }
        } else {
                // release first item in the wakelock list
                pm_wakelock_inst.wl_list.head = wl->list.next;
        }
    }
    else {
        // Find wakelock in the list
        for (; (l->next) && (l->next != (list_t*)wl); (l = l->next));

        if (l->next != NULL) {
            // Item before wl found, remove wl item from the list
            l->next = wl->list.next;

            // Check if wl item is the last one in the list
            if (pm_wakelock_inst.wl_list.tail == (list_t*)wl) {
                pm_wakelock_inst.wl_list.tail = l;
            }
        } else {
            //wl list clear
            ret = -ENOENT;
        }
    }
exit:
    interrupt_unlock(saved);
    return ret;
}

bool pm_wakelock_is_list_empty()
{
    return (pm_wakelock_inst.wl_list.head == NULL ? true : false);
}

void pm_wakelock_set_list_empty_cb(void (*cb)(void*), void* priv)
{
    // Lock IRQs
    uint32_t saved = interrupt_lock();

    // Set the new callback function
    pm_wakelock_inst.cb = cb;
    pm_wakelock_inst.cb_priv = priv;

    interrupt_unlock(saved);
}

bool pm_check_suspend_blockers(PM_POWERSTATE state)
{
	int i = 0;

	for (; i < pm_blocker_size; i++) {
		if (!pm_blockers[i].cb(pm_blockers[i].dev, state)) {
			return false;
		}
	}
	return true;
}
