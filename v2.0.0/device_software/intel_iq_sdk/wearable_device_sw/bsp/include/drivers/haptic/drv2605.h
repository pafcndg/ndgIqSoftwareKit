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

#ifndef DRV2605_H_
#define DRV2605_H_

#include "drivers/haptic/haptic.h"
#include "drivers/serial_bus_access.h"
#include "os/os.h"

/**
 * @defgroup haptic_drv2605 DRV2605 Haptic Driver
 * @{
 * @ingroup common_drivers_haptic
 */

/**
 * internal variable used by driver
 */
typedef struct drv2605_info {
	sba_request_t *tx_request;             /*!< SBA request pointer */
	sba_request_t *trx_request;            /*!< SBA request pointer */
	vibration_type type;                   /*!< Vibration Type */
	vibration_u *pattern;                  /*!< Vibration pattern */
	uint8_t current_count;                 /*!< repetition current counter */
	uint8_t pattern_step;                  /*!< current pattern counter */
	T_TIMER t_step;
	T_SEMAPHORE sema_wait_i2c;
	void (*vibr_callback_func_ptr) (int8_t);
	struct pm_wakelock wakelock;
} haptic_info_t;

/**
 *  drv2605 driver.
 */
extern struct driver drv2605_driver;

/** @} */

#endif /* DRV2605_H_ */
