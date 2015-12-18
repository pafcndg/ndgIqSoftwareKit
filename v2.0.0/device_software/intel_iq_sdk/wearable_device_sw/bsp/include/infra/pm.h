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

#ifndef __PM_H__
#define __PM_H__

#include "util/list.h"
#include "os/os.h"
#include <stdint.h>
#include <stdbool.h>
#include "infra/reboot.h"

/**
 * @defgroup infra_pm Power Management
 * Defines power management functions for the infrastructure.
 * @ingroup infra
 * @{
 */

#define WAKELOCK_FOREVER     0    /*!< Infinite timeout code for wakelocks */

/**
 * List of all device power states (From shutdown to running).
 */
typedef enum {
    PM_NOT_INIT = 0, /*!< Device not initialized */
    PM_SHUTDOWN,     /*!< Device stopped */
    PM_SUSPENDED,    /*!< Device suspended */
    PM_RUNNING,      /*!< Device working properly */
    PM_COUNT
} PM_POWERSTATE;

struct device; // Declare device struct witch is used in pm_suspend_blocker
/**
 * Structure that contains driver callback to check that deep sleep is allowed.
 * Callback must return true if deep sleep is allowed.
 */
struct pm_suspend_blocker {
	struct device *dev;
	bool (*cb)(struct device *dev, PM_POWERSTATE state);
};

/**
 * Wakelock managing structure.
 *
 * - A wakelock is a lock used to prevent the platform to enter suspend/shutdown.
 * - When a device acquires a wakelock, it needs to release it (or wait for
 *   timeout) before platform can enter in suspend/resume mode.
 * - The platform is notified that all wakelocks have been released through the
 *   callback function configured using the function
 *   pm_wakelock_set_list_empty_cb.
 *
 * A component using wakelocks should allocate a struct pm_wakelock.
 */
struct pm_wakelock {
    list_t list;         /*!< Internal list management member */
    int id;              /*!< Client identifier for the hook. must be platform global */
    uint32_t timeout;    /*!< Wakelock timeout in ms (used to release the wakelock after a timeout) */
    uint32_t start_time; /*!< Start time in ms (used to compute expiration time) */
    unsigned int lock;   /*!< Lock to avoid acquiring a lock several times */
};

/**
 * Initializes wakelock management structure.
 *
 * @return 0 on success, -ENOMEM if device is out of memory
 */
int pm_wakelock_init_mgr();

/**
 * Initializes a wakelock structure.
 *
 * @param wli the wakelock structure pointer to initialize.
 * @param id the identifier of the wakelock.
 */
void pm_wakelock_init(struct pm_wakelock *wli, int id);

/**
 * Acquires a wakelock.
 *
 * Prevents the platform to sleep for timeout milliseconds. Wakelock
 * will be automatically released after timeout.
 *
 * @param wl the wakelock to acquire.
 *
 * @return 0 on success, -EINVAL if already locked or timeout is greater than WAKELOCK_MAX_TIMEOUT
 */
int pm_wakelock_acquire(struct pm_wakelock *wl);

/**
 * Releases a wakelock.
 *
 * @param wl the wakelock to release
 *
 * @return 0 on success, -EINVAL if already released
 */
int pm_wakelock_release(struct pm_wakelock *wl);

/**
 * Checks if wakelock list is empty.
 *
 * @return true (1) if at least one wakelock is acquired else false (0)
 */
bool pm_wakelock_is_list_empty();

/**
 * Checks that all deep sleep blockers are released.
 *
 * @param state state to transition to
 * @return true (1) if deep sleep mode transition is allowed else false (0)
 */
bool pm_check_suspend_blockers(PM_POWERSTATE state);

#define DECLARE_SUSPEND_BLOCKERS(...) \
const struct pm_suspend_blocker pm_blockers[] = {__VA_ARGS__}; \
const uint8_t pm_blocker_size = sizeof(pm_blockers)/sizeof(*pm_blockers);

/**
 * Suspends blockers array.
 *
 * @warning DECLARE_SUSPEND_BLOCKERS macro must be used to declare this array
 */
extern const struct pm_suspend_blocker pm_blockers[];

/**
 * Suspends blockers array size.
 *
 * @warning DECLARE_SUSPEND_BLOCKERS macro must be used to declare this array
 */
extern const uint8_t pm_blocker_size;

/**
 * Sets callback to call when wakelock list is empty.
 *
 * It will replace the previous callback.
 *
 * @param cb   the callback function
 * @param priv the private data to pass to the callback function
 */
void pm_wakelock_set_list_empty_cb(void (*cb)(void*), void* priv);

/**
 * Defines the function that pm slaves nodes needs to implement in order
 * to handle power management IPC requests.
 *
 * @param cpu_id the CPU triggering the pm request
 * @param pm_req the pm request type (PM_SHUTDOWN/REBOOT/POWER_OFF)
 * @param param the request parameter
 */
int pm_notification_cb(uint8_t cpu_id, int pm_req, int param);

/**
 * Board specific hook to provide shutdown.
 *
 * If a board requires a specific action to provide shutdown, it will need to
 * implement this function and set the CONFIG_HAS_BOARD_SHUTDOWN option. The
 * function will be called by master core's pm shutdown API.
 */
void board_shutdown_hook(void);

/**
 * Board specific hook to provide poweroff.
 *
 * If a board requires a specific action to provide poweroff, it will need to
 * implement this function and set the CONFIG_HAS_BOARD_SHUTDOWN option. The
 * function will be called by master core's pm poweroff API.
 *
 * to disconnect battery.
 */
void board_poweroff_hook(void);

/**
 * Core specific function to set wakelock state shared variable.
 *
 * Set cpu specific wakelock state shared variable.
 * For example, when all wakelocks are released and when at least
 * one is taken.
 *
 * @param wl_status: current wakelock status to set.
 */
void pm_wakelock_set_any_wakelock_taken_on_cpu(bool wl_status);

/**
 * Check slaves wakelocks state
 *
 * @return false if all wakelocks are released
 */
bool pm_wakelock_are_other_cpu_wakelocks_taken(void);

/**
 * Set the callback for shutdown events.
 *
 * @param shutdown_event_cb: callback for shutdown events.
 */
void pm_set_shutdown_event_cb(void (*shutdown_event_cb)(enum shutdown_types));

/**
 * Set the callback for reboot events.
 *
 * @param reboot_event_cb: callback for reboot events.
 */
void pm_set_reboot_event_cb(void (*reboot_event_cb)(enum boot_targets));

/** @} */

#endif /* __PM_H_ */
