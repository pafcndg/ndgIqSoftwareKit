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
#include "infra/tcmd/handler.h"
#include "infra/pm.h"

void pm_power_off_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc == 2) {
		TCMD_RSP_FINAL(ctx, NULL);
		power_off(SHUTDOWN);
	} else {
		TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_INV_ARG);
	}
}

DECLARE_TEST_COMMAND(system, power_off, pm_power_off_tcmd);

void pm_shutdown_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    if (argc == 3) {
        TCMD_RSP_FINAL(ctx, NULL);
        shutdown();
    } else {
        TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_INV_ARG);
    }
}

DECLARE_TEST_COMMAND(system, shutdown, pm_shutdown_tcmd);

void pm_reset_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc == 3) {
		/* We want to respond before rebooting to give a chance to
		 * print out response. */
		TCMD_RSP_FINAL(ctx, NULL);
		reboot(atoi(argv[2]));
	} else {
		TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_INV_ARG);
	}
}

DECLARE_TEST_COMMAND(system, reset, pm_reset_tcmd);
