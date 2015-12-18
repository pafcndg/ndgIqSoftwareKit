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
#include "board.h"
#include "machine/soc/intel/quark_se/pm_pupdr.h"

#ifdef CONFIG_DEEPSLEEP
#include "drivers/intel_qrk_aonpt.h"
#include "infra/time.h"
#include "infra/log.h"
#include "scss_registers.h"
#include "os/os.h"
#ifdef CONFIG_TCMD
#include "infra/tcmd/handler.h"
#include <stdio.h>
#include <stdlib.h>
#endif

#define DEEP_SLEEP_MIN_DURATION 10
#define ESTIMATED_WAKEUP_DELAY  3

static uint32_t cycle_count = 0;
static uint32_t cycle_idle = 0;
static volatile uint32_t start = 0;

#ifdef CONFIG_TCMD
static bool enable_deep_sleep = true;
#endif
#endif /* CONFIG_DEEPSLEEP */

int32_t _tickless_idle_hook(int32_t ticks)
{
	if (pm_is_shutdown_allowed()) {
		// handle shutdown
		pm_shutdown();
	}
#ifdef CONFIG_DEEPSLEEP
	if (
#ifdef CONFIG_TCMD
		 !enable_deep_sleep ||
#endif
		 ticks < DEEP_SLEEP_MIN_DURATION ||
		 !pm_is_deepsleep_allowed()){
		return 0; /* Deepsleep failed */
	}

	start = get_uptime_32k();
	pr_debug(LOG_MODULE_OS, "Tickless hook: %d curr %d", ticks, get_time_ms());
	qrk_aonpt_configure(
			(int64_t)((ticks - ESTIMATED_WAKEUP_DELAY) * 32768) / 1000,
			NULL, true);
	qrk_aonpt_start();

	if ((pm_core_deepsleep())) {
		start = 0;
		qrk_aonpt_stop();
		return 0; /* Deepsleep failed */
	}

	/* Force interrupt enabling as it will not
	 * be done in os idle func */
	__asm__ __volatile__("sti;");

	uint32_t flags = interrupt_lock();
	/* Check if the wakeup source is on ARC core
	   or is the aon periodic timer */
	if (start) {
		__asm__ __volatile__("pushf;"
				"push %cs;"
				"call _aonptIntStub;");
	}
	interrupt_unlock(flags);
	return 1; /* Deepsleep occurred */
#else
    return 0; /* Deepsleep not available */
#endif
}

int32_t _tickless_idle_hook_exit()
{
#ifdef CONFIG_DEEPSLEEP
	if (!start) {
		return 0;
	}

	/* Compute deepsleep ticks */
	int32_t ret = (((uint64_t)(get_uptime_32k() - start))*1000 + 16383) / 32768;
	start = 0;

	cycle_count++;
	cycle_idle += ret;

	if (cycle_count % 10 == 0) {
		pr_debug(LOG_MODULE_OS, "idle %d count %d",
				cycle_idle, cycle_count);
	}

	return ret;
#else
    return 0; /* Deepsleep not available */
#endif
}

#ifdef CONFIG_DEEPSLEEP
uint32_t get_deepsleep_count()
{
	return cycle_count;
}
#endif

#if defined(CONFIG_DEEPSLEEP) && defined(CONFIG_TCMD)
void pm_stat_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	char buffer[40];
	snprintf(buffer, 40, "time: %d count: %d", cycle_idle, cycle_count);
	TCMD_RSP_FINAL(ctx, buffer);
}
DECLARE_TEST_COMMAND_ENG(system, slpstat, pm_stat_tcmd);

void pm_idle_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    if (argc == 3) {
        TCMD_RSP_FINAL(ctx, NULL);
        enable_deep_sleep = (atoi(argv[2]) == 1 ? true : false);
    } else {
        TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_INV_ARG);
    }
}

DECLARE_TEST_COMMAND_ENG(system, idle, pm_idle_tcmd);
#endif
