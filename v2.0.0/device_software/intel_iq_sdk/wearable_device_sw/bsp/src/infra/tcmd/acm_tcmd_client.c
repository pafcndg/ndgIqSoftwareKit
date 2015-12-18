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

#include <stdio.h>
#include <string.h>
#include "drivers/usb_acm.h"
#include "infra/log.h"
#include "infra/tcmd/console.h"
#include "util/list.h"
#include "util/workqueue.h"
#include "os/os.h"

#define TCMD_BUF_SIZE 128

static char tcmd_buf[TCMD_BUF_SIZE];
static int tcmd_idx = 0;

static char tcmd_resp_buf[TCMD_BUF_SIZE];
static int tcmd_resp_idx = 0;

static void acm_tcmd_write_cb(int actual, void *data)
{
	*(int*)data = 1;
}

static void queue_buf(uint8_t *buf, int len)
{
	volatile int complete = 0;
	int timeout = 10000;

	int ret = acm_write(0, buf, len, acm_tcmd_write_cb, (void*)&complete);
	if (ret)
		pr_debug(LOG_MODULE_USB, "write ret: %d", ret);
	while (!complete && timeout--);
	if(timeout < 0) {
		pr_error(LOG_MODULE_USB, "Timeout waiting for write");
	}
	return;
}

static int tcmd_putc(int c)
{
	tcmd_resp_buf[tcmd_resp_idx] = c;
	if(c == '\n') {
		tcmd_resp_buf[tcmd_resp_idx+1] = '\0';
		queue_buf(tcmd_resp_buf, tcmd_resp_idx+1);
		tcmd_resp_idx = 0;
	} else if (tcmd_resp_idx < TCMD_BUF_SIZE-1){
		tcmd_resp_idx++;
	} else {
		tcmd_resp_buf[tcmd_resp_idx] = '\0';
		queue_buf(tcmd_resp_buf, tcmd_resp_idx);
		tcmd_resp_idx = 0;
	}
	return 0;
}

static void acm_tcmd_read_cb(int actual, void *data);

void acm_tcmd_trigger_read()
{
	int ret;
	ret = acm_read(0, tcmd_buf+tcmd_idx, TCMD_BUF_SIZE-tcmd_idx, acm_tcmd_read_cb, NULL);
	if (ret) {
		pr_debug(LOG_MODULE_USB, "read 1 ret: %d", ret);
		tcmd_idx = 0;
	}
}

static void acm_tcmd_write_cb_dummy(int actual, void *data)
{
}

static void acm_tcmd_read_work(void * data)
{
	int i;
	for (i=0; i<(int)data; i++) {
		if (tcmd_buf[tcmd_idx] == '\r')
			tcmd_buf[tcmd_idx] = '\n';
		if (tcmd_buf[tcmd_idx] == '\n' && tcmd_idx == 0) {
			tcmd_putc('\n');
			continue;
		}
		if ((tcmd_buf[tcmd_idx] == '\n' && tcmd_idx > 0) ||
				tcmd_idx == TCMD_BUF_SIZE-1) {
			tcmd_buf[tcmd_idx] = '\0';
			tcmd_putc('\r');
			tcmd_putc('\n');
			tcmd_console_read(tcmd_buf, tcmd_idx,
					  tcmd_putc);
			tcmd_idx = 0;
		} else {
			tcmd_idx++;
		}
	}
	if (tcmd_idx > 0) {
		acm_write(0, tcmd_buf+tcmd_idx-i, (int)data, acm_tcmd_write_cb_dummy, NULL);
	}
	acm_tcmd_trigger_read();
}

static void acm_tcmd_read_cb(int actual, void *data)
{
	workqueue_queue_work(acm_tcmd_read_work, (void*)actual);
}
