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

#include "os/os.h"
#include "infra/log.h"
#include "infra/console_manager.h"
#include "util/cunit_test.h"

static void log_send(uint8_t level, int nb, const char* str)
{
	int i;
	for (i = 0; i < nb; i++) {
		log_printk(level, LOG_MODULE_LOG, str);
	}
	log_flush();
}

static void fill_str(char* str, uint32_t len) {
	char c = 'A';
	int i;
	for (i=0;i<len-1;++i,++c)
	{
		str[i]=c;
		if (c=='z')
			c = 'A'-1;
	}
	str[len-1]='\0';
}

void console_manager_test(void)
{
	int ret;
	cu_print("##############################################################\n");
	cu_print("# Purpose of the console manager test :                      #\n");
	cu_print("#     Test that switching of console backends                #\n");
	cu_print("#     Test that buffer is flushed on selected backend        #\n");
	cu_print("#     Test error cases                                       #\n");
	cu_print("##############################################################\n");

	char str[50];

	/* Free the log buffer before the tests */
	log_flush();

	fill_str(str, sizeof(str));

	/* Check by sending an Invalid console backend name */
	ret = console_manager_set_log_backend("invalid");
	CU_ASSERT("Invalid console backend", ret == -1);

	log_send(LOG_LEVEL_INFO, 1, "Sending Logs on USB ACM console backend");
	ret = console_manager_set_log_backend("usb");
	CU_ASSERT("Set USB console backend Failed", ret == 0);
	/* These logs will appear on UART console as no ACM backend is connected*/
	log_send(LOG_LEVEL_INFO, 5, str);

	ret = console_manager_set_log_backend("uart");
	CU_ASSERT("Set UART console backend Failed", ret == 0);
	log_send(LOG_LEVEL_INFO, 1, "Sending Logs on UART console backend");
	log_send(LOG_LEVEL_INFO, 5, str);

	log_send(LOG_LEVEL_INFO, 1, "Storing Logs on Cbuffer for none backend");
	ret = console_manager_set_log_backend("none");
	CU_ASSERT("Set None console backend Failed", ret == 0);
	/* This creates log saturation as backend is none */
	log_send(LOG_LEVEL_INFO, 100, str);

	log_send(LOG_LEVEL_INFO, 1, "Sending Logs Back on UART console backend");
	ret = console_manager_set_log_backend("uart");
	CU_ASSERT("Set UART console backend Failed", ret == 0);
	log_send(LOG_LEVEL_INFO, 2, str);
}
