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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "infra/log.h"
#include "infra/tcmd/handler.h"


void log_set(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc != 3 || !isdigit((unsigned char)argv[2][0])) {
		TCMD_RSP_ERROR(ctx, "cmd: log set <lev>");
		return;
	}

	int8_t action = atoi(argv[2]);
	if (!log_set_global_level(action))
		TCMD_RSP_FINAL(ctx, NULL);
	else
		TCMD_RSP_ERROR(ctx, "0:err, 1:warn, 2:info, 3:debug");
}

DECLARE_TEST_COMMAND(log, set, log_set);

void log_list(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	#define LENGTH 80
	char answer[LENGTH];

	/* Display all module */
	uint8_t i;
	for (i = 0; i < LOG_MODULE_NUM; i++) {
		snprintf(answer, LENGTH, "%d: %s", i, log_get_module_name(i));
		TCMD_RSP_PROVISIONAL(ctx, answer);
	}
	TCMD_RSP_FINAL(ctx, NULL);
}

DECLARE_TEST_COMMAND_ENG(log, list, log_list);

void log_print(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc != 4) {
		TCMD_RSP_ERROR(ctx, "cmd: log print <n> <string_message>");
		return;
	}
	int log_count = atoi(argv[2]);
	int i;
	for (i=0; i < log_count; i++)
		pr_info(LOG_MODULE_LOG, "%s", argv[3]);
	TCMD_RSP_FINAL(ctx, NULL);
}

DECLARE_TEST_COMMAND_ENG(log, print, log_print);
