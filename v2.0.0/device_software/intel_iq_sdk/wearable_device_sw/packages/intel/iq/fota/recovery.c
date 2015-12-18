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

#include <stdint.h>
/* We need this in order to start the storage task. */
#include <zephyr.h>
#include "os/os.h"
#include "infra/boot.h"
#include "infra/bsp.h"
#include "infra/pm.h"
#include "infra/time.h"

#include "cfw/cfw_service.h"
#include "lib/ble/ble_app.h"
#include "iq/firmwareupdate_iq.h"
#ifdef CONFIG_SYSTEM_EVENTS_IQ
#include "iq/system_events_iq.h"
#endif

#ifdef CONFIG_COMMON_IQ
#include "iq/init_iq.h"
#endif

#define RECOVERY_WAKELOCK 0xa71be /* alive */
#define RECOVERY_TIMEOUT  (60000 * 15) /* 15 minutes */

/* we do not care about anything here */
static void message_handler(struct cfw_message * msg, void * param)
{
}

static void services_setup(T_QUEUE queue)
{
	/* Task storage can be used by BLE for properties */
	task_start(TASK_STORAGE);
	/* OTA will take place via BLE */
	ble_start_app(queue);
}

/* Recovery App entry point */
void recovery(void *param)
{
	uint32_t startup_time = get_uptime_ms();
	/* We have a wakelock to prevent sleeping */
	struct pm_wakelock recovery_wakelock;
	T_QUEUE queue;
	/* Initialize OS abstraction */
	os_init();
	/* Setup bsp, system is up and running */
	queue = bsp_setup();

	/* take infinite wakelock to prevent deep sleep. it will never be
	 * released, we will reboot anyway. */
	pm_wakelock_init(&recovery_wakelock, RECOVERY_WAKELOCK);
	pm_wakelock_acquire(&recovery_wakelock);
	/* Main component framework handle */
	cfw_app_setup(queue, message_handler, NULL);
	services_setup(queue);

	init_iqs(queue);

	/* Loop to handle messages... */
	while (1) {
		OS_ERR_TYPE err = E_OS_OK;
		/* Process message with a given timeout */
		queue_process_message_wait(queue, 5000, &err);

		/* We have a timeout in case we take too much time to perform
		 * the recovery. */
		if ((get_uptime_ms() - startup_time) > RECOVERY_TIMEOUT)
		{
			emergency_reboot(TARGET_MAIN);
		}
	}
}
