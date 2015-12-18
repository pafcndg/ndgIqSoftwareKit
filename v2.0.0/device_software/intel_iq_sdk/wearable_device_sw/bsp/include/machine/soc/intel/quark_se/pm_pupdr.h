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

#ifndef __PM_PUPDR_H__
#define __PM_PUPDR_H__

#include "infra/pm.h"

enum pupdr_request {
	PUPDR_NO_REQ = 0,
	PUPDR_SHUTDOWN,
	PUPDR_REBOOT,
	PUPDR_POWER_OFF
};

/**
 * Submit a pupdr_request to the pupdr framework.
 * This will notify the cores that they need to stop as soon as possible.
 * In case a core cannot stop, a 5 second timeout will force shutdown.
 *
 * @param req pupdr_request requested
 * @param param request parameter (boot_target or shutdown_type)
 */
void pm_shutdown_request(enum pupdr_request req, int param);

/**
 * PUPDR low level shutdown function.
 * This will force a reboot/shutdown poweroff,
 * according to the parameters passed to pm_shutdown_request.
 */
void pm_shutdown(void);

/**
 * Return whether shutdown is currently allowed or not.
 * This function is core specific.
 *
 * @return true if shutdown allowed else false
 */
bool pm_is_shutdown_allowed();

/**
 * Hook to allow a user to add tests to allow shutdown.
 * This function is core specific.
 *
 * @return true if shutdown allowed else false
 */
__attribute__((weak)) bool pm_is_core_shutdown_allowed();

/**
 * Return whether deepsleep is currently allowed or not.
 * This function is core specific.
 *
 * @return true if deepsleep allowed else false
 */
bool pm_is_deepsleep_allowed();

/**
 * Hook to allow a user to add tests to allow deepsleep.
 * This function is core specific.
 *
 * @return true if deepsleep allowed else false
 */
__attribute__((weak)) bool pm_is_core_deepsleep_allowed();

/**
 * Core low level shutdown procedure, and called by pupdr functions.
 * This function must not return and is core specific.
 *
 * @param req pupdr_request requested
 * @param param request parameter (boot_target or shutdown_type)
 */
void pm_core_shutdown(enum pupdr_request req, int param);

/**
 * Core low level deepsleep procedure.
 * This function is core specific.
 *
 * For ARC core: suspends drivers, halt CPU and resume.
 * For Quark core: trigger arc deepsleep procedure, suspends drivers,
 * halt CPU and resume.
 *
 * @return 0 if success else POSIX return code error
 */
int pm_core_deepsleep();

#define PM_SUSPEND_REQUEST   0x51EEb000
#define PM_RESUME_REQUEST    0xCAFFE000

#define PM_POWERSTATE_MASK   0x000000FF
#define PM_REQUEST_MASK      0xFFFFF000

#define PM_ACK_TIMEOUT       0
#define PM_ACK_OK            (1 << 8)
#define PM_ACK_ERROR         (1 << 9)

#define PM_GET_REQUEST(req)  ((req) & PM_REQUEST_MASK)
#define PM_GET_STATE(req)    ((req) & PM_POWERSTATE_MASK)

#define PM_IS_ACK_OK(req)    ((req) & PM_ACK_OK)
#define PM_IS_ACK_ERROR(req) ((req) & PM_ACK_ERROR)

#define PM_ACK_SET_OK(req)    (req) |= PM_ACK_OK
#define PM_ACK_SET_ERROR(req) (req) |= PM_ACK_ERROR

#define PM_INIT_REQUEST(req, type, state) ((req) = \
		(type & PM_REQUEST_MASK) | \
		(state & PM_POWERSTATE_MASK))

/** @} */

#endif /* __PM_PUPDR_H_ */
