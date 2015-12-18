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

#include <string.h>
#include <stdio.h>
#include "infra/console_backend_usb_acm.h"
#include "infra/console_backend_uart.h"
#include "infra/console_manager.h"
#include "infra/tcmd/handler.h"
#include "infra/log.h"
#include "util/misc.h"

#define TCMD_RSP_LENGTH 50

static char *current_backend_name = CONFIG_CONSOLE_MANAGER_DEFAULT_BACKEND;

static void console_backend_set_none(void)
{
	struct log_backend log_backend_none = {NULL, NULL};
	log_set_backend(log_backend_none);
}

static const console_backend_t console_backend_none = {
	.name = "none",
	.init = NULL,
	.set_as_log_backend = console_backend_set_none,
	.unset_as_log_backend = NULL
};

static const console_backend_t *console_backend[] = {
	&console_backend_none,
#ifdef CONFIG_CONSOLE_BACKEND_UART
	&console_backend_uart,
#endif
#ifdef CONFIG_CONSOLE_BACKEND_USB_ACM
	&console_backend_usb_acm
#endif
};

static const uint8_t no_of_backends = ARRAY_SIZE(console_backend);

int console_manager_set_log_backend(const char *console_name)
{
	uint8_t i,j;
	for (i = 0; i < no_of_backends; i++) {
		if (!strcmp(console_name, console_backend[i]->name)) {
			console_backend[i]->set_as_log_backend();
			for (j = 0; j < no_of_backends; j++) {
				if (!strcmp(current_backend_name, console_backend[j]->name)) {
					if (console_backend[j]->unset_as_log_backend) {
						console_backend[j]->unset_as_log_backend();
					}
				}
			}
			current_backend_name = (char *)console_backend[i]->name;
			return 0;
		}
	}
	return -1;
}

void console_manager_init(void)
{
	uint8_t i;
	for (i = 0; i < no_of_backends; i++) {
		if (console_backend[i]->init)
			console_backend[i]->init();
	}
	console_manager_set_log_backend(CONFIG_CONSOLE_MANAGER_DEFAULT_BACKEND);
}

#ifdef CONFIG_TCMD

static void setbackend_usage(struct tcmd_handler_ctx *ctx)
{
	char rsp_str[TCMD_RSP_LENGTH];
	char *rsp_ptr = rsp_str;
	uint8_t i;
	rsp_ptr += snprintf(rsp_ptr, TCMD_RSP_LENGTH, "usage: log setbackend [");
	for (i = 0; i < no_of_backends; i++) {
		if (!strcmp(current_backend_name, console_backend[i]->name)) {
			rsp_ptr += snprintf(rsp_ptr, TCMD_RSP_LENGTH, "%s*|",console_backend[i]->name);
		} else {
			rsp_ptr += snprintf(rsp_ptr, TCMD_RSP_LENGTH, "%s|",console_backend[i]->name);
		}
	}
	snprintf(rsp_ptr-1, TCMD_RSP_LENGTH, "]");
	TCMD_RSP_ERROR(ctx, rsp_str);
}

void setbackend_handler(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc > 3)
		goto usage;
	if (argc == 2) {
		TCMD_RSP_FINAL(ctx, current_backend_name);
		return;
	}
	if (console_manager_set_log_backend(argv[2]) != 0)
		goto usage;
	TCMD_RSP_FINAL(ctx, NULL);
	return;
usage:
	setbackend_usage(ctx);
}

DECLARE_TEST_COMMAND(log, setbackend, setbackend_handler);

#endif
