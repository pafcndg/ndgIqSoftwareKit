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

#ifndef __REBOOT_H__
#define __REBOOT_H__

#include "boot.h"

/**
 * Defines power management API.
 * @ingroup infra_pm
 * @{
 */

#define platform_panic_reboot NVIC_SystemReset

/**
 * List of all shutdown type.
 */
enum shutdown_types {
	SHUTDOWN,
	CRITICAL_SHUTDOWN,
	THERMAL_SHUTDOWN,
	BATTERY_SHUTDOWN,
	REBOOT,
	MAX_SHUTDOWN_REASONS = 0xFF
};

struct reboot_notifier {
	void (*notify)(enum shutdown_types type);
};

#define NOTIFY_REBOOT(name) \
	const struct reboot_notifier __##name \
	__attribute__((section(".reboot.notifiers." #name), used)) = { name }

/**
 * Requests a reboot of the platform.
 *
 * Triggers a platform reboot in the selected functional mode.
 *
 * @param boot_target according to boot_targets enum
 */
void reboot(enum boot_targets boot_target);

/**
 * Requests an emergency reboot of the platform.
 *
 * Platfrom will reboot without any notification.
 *
 * @param boot_target according to boot_targets enum
 */
void emergency_reboot(enum boot_targets boot_target);

/** Shutdown device
 */
void shutdown(void);

/** Shutdown device because of low battery level
 */
void low_batt_shutdown(void);

/** Shutdown device because of temperature
 */
void thermal_shutdown(void);

/**
 * Requests a power off of the platform.
 *
 * Triggers the power off procedure of the platform
 *
 * Note that the latched off mode needs specific hardware support to physically
 * disconnect the battery and re-connect it with a button press.
 *
 * @param shutdown_type  shutdown type
 */
void power_off(enum shutdown_types shutdown_type);

/** @} */

#endif
