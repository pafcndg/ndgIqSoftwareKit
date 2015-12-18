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
#include "infra/log.h"
#include "os/os.h"
#include "drivers/usb_acm.h"

extern bool is_console_backend_usb_acm_ready(void);

#define LOG_ACM_INTERFACE (0)                                   /* index of the ACM interface to use */
#define LOG_MAX_TOTAL_MSG_LEN (30 + LOG_MAX_MSG_LEN + 2)        /* Header + maximum size of a message + "\r\n" */

static T_SEMAPHORE usb_ready = NULL;
static char message[LOG_MAX_TOTAL_MSG_LEN];
static uint16_t message_size = 0;

static void cb_xfer_done(int actual, void *data)
{
	semaphore_give(usb_ready, NULL);
}

static void usb_puts(const char *s, uint16_t len)
{
	/* Lazy initialization of usb_ready semaphore */
	if (!usb_ready)
		usb_ready = semaphore_create(0, NULL);
	/* Assembling of the message */
	if (message_size + len > LOG_MAX_TOTAL_MSG_LEN)
		len = LOG_MAX_TOTAL_MSG_LEN - message_size;
	strncpy(&message[message_size], s, len);
	message_size += len;

	if (message_size >= 2) {
		if (strncmp(&message[message_size - 2], "\r\n", 2) == 0) { /* If the message is assembled */
			acm_write(LOG_ACM_INTERFACE, (uint8_t *)message,
				  message_size, cb_xfer_done,
				  NULL);
			if (semaphore_take(usb_ready,
					   OS_WAIT_FOREVER) != E_OS_OK)
				panic(E_OS_ERR);
			message_size = 0;
		}
	}
}

void release_usb_backend_xfer(void)
{
	OS_ERR_TYPE err;
	/* Neglect err in case of multiple release from usb acm backend */
	semaphore_give(usb_ready, &err);
}

static bool is_usb_acm_backend_ready(void)
{
	return is_console_backend_usb_acm_ready();
}

struct log_backend log_backend_usb = { usb_puts, is_usb_acm_backend_ready};
