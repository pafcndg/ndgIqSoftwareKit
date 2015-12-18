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

#include "os/os_types.h"
#include "services/service_queue.h"

/* IQs */
#ifdef CONFIG_NOTIFICATIONS_IQ
#include "iq/notif_iq.h"
#endif
#ifdef CONFIG_DEVICES_IQ
#include "iq/device_iq.h"
#endif
#ifdef CONFIG_FOTA_IQ
#include "iq/firmwareupdate_iq.h"
#endif
#ifdef CONFIG_SYSTEM_EVENTS_IQ
#include "iq/system_events_iq.h"
#endif
#ifdef CONFIG_BODY_IQ
#include "iq/body_iq.h"
#endif
#ifdef CONFIG_USER_EVENTS_IQ
#include "iq/user_events_iq.h"
#endif

/* Infra */
#include "infra/log.h"
#include "infra/boot.h"

static void iq_initialisation_complete(T_QUEUE queue)
{
	pr_info(LOG_MODULE_IQ, "all iq initialization completed");
}

static void (*init_funcs[7])(T_QUEUE queue);

static uint8_t init_index = 0;
static T_QUEUE init_queue;

void init_iqs(T_QUEUE queue)
{
	int index = 0;

	if(get_boot_target() != TARGET_RECOVERY) {
#ifdef CONFIG_USER_EVENTS_IQ
		init_funcs[index++] = user_events_iq_init;
#endif
#ifdef CONFIG_DEVICES_IQ
		init_funcs[index++] = device_iq_init;
#endif
	}
#ifdef CONFIG_SYSTEM_EVENTS_IQ
	init_funcs[index++] = system_events_iq_init;
#endif
#ifdef CONFIG_FOTA_IQ
	init_funcs[index++] = firmwareupdate_iq_init;
#endif
	if(get_boot_target() != TARGET_RECOVERY) {
#ifdef CONFIG_BODY_IQ
		init_funcs[index++] = body_iq_init;
#endif
#ifdef CONFIG_NOTIFICATIONS_IQ
		init_funcs[index++] = notif_iq_init;
#endif
	}
	init_funcs[index] = iq_initialisation_complete;
	init_queue = queue;
	init_funcs[init_index++](queue);
}

void init_done(void)
{
	init_funcs[init_index++](init_queue);
}
