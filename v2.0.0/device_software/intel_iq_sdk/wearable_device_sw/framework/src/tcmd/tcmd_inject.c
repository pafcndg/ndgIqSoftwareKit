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
#include "os/os.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw.h"
#include "infra/tcmd/handler.h"
#include "infra/log.h"

static cfw_client_t * client;

static T_QUEUE queue = NULL;

void tcmd_inject_init()
{
	queue = queue_create(10, NULL);
	/* Special client, no need for a callback as messages are polled with
	 * cfw poll test command
	 */
	client = cfw_client_init(queue, NULL, NULL);

	pr_debug(LOG_MODULE_MAIN, "TCMD inject initialized");
}

static unsigned char hex2nib(char src)
{
	if (src <= '9')
		return src - '0';
	else if (src <= 'F')
		return src - 'A' + 10;
	else
		return src - 'a' + 10;
}

void hex2bin(char * src, unsigned char * dst, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		dst[i] = hex2nib(src[i*2]) << 4 | hex2nib(src[i*2+1]);
	}
}

char nib2hex(int val)
{
	if (val < 10) {
		return val + '0';
	} else {
		return val - 10 + 'a';
	}
}

void bin2hex(unsigned char * src, char * dst, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		dst[i*2] = nib2hex((src[i]>>4)&0xf);
		dst[i*2+1] = nib2hex(src[i] & 0xf);
	}
}

int check_hex(char * str)
{
	int len = strlen(str);
	int i;
	if (len % 2) return -1;

	for (i=0; i<len; i++) {
		if (str[i] >= '0' && str[i] <= '9')
			continue;
		if (str[i] >= 'a' && str[i] <= 'f')
			continue;
		if (str[i] >= 'A' && str[i] <= 'F')
			continue;
		return -1;
	}
	return len;
}

void cfw_inject_message(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	OS_ERR_TYPE err;
	if (queue == NULL)
		tcmd_inject_init();
	if (argc > 2) {
		int len = check_hex(argv[2]);
		pr_debug(LOG_MODULE_MAIN, "message len: %d", len);

		struct cfw_message * msg =
			(struct cfw_message * ) message_alloc(len, &err);
		if (msg != NULL) {
			hex2bin(argv[2], (unsigned char *)msg, len);
			/* Override */
			CFW_MESSAGE_SRC(msg) = ((_cfw_client_t *)client)->client_port_id;
			err = port_send_message(&msg->m);
			if (err == E_OS_OK) {
				TCMD_RSP_FINAL(ctx, NULL);
			} else {
				TCMD_RSP_ERROR(ctx, "cannot inject message");
				message_free(&msg->m);
			}
		} else {
			TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_NO_MEMORY);
		}
	} else {
		TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_WRONG_ARGC);
	}
}
DECLARE_TEST_COMMAND_ENG(cfw, inject, cfw_inject_message);

#define MSG_SPLIT_SIZE 32
void cfw_poll_message(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct cfw_message *msg;
	OS_ERR_TYPE err;
	T_QUEUE_MESSAGE m;
	static char buf[MSG_SPLIT_SIZE*2 + 9];

	if (queue == NULL)
		tcmd_inject_init();

	queue_get_message(queue, &m, OS_NO_WAIT, &err);
	if (err == E_OS_OK) {
		msg = (struct cfw_message *)m;
		int i;
		for (i = 0; (CFW_MESSAGE_LEN(msg)/MSG_SPLIT_SIZE &&
				i < CFW_MESSAGE_LEN(msg)/MSG_SPLIT_SIZE); i++) {
			snprintf(buf, MSG_SPLIT_SIZE*2, "%03d/%03d ",
				 i*MSG_SPLIT_SIZE, CFW_MESSAGE_LEN(msg));
			bin2hex(((unsigned char*)msg) + i * MSG_SPLIT_SIZE,
				buf+8, MSG_SPLIT_SIZE);
			buf[8+2*MSG_SPLIT_SIZE] = '\0';
			TCMD_RSP_PROVISIONAL(ctx, buf);
		}
		if (CFW_MESSAGE_LEN(msg) % MSG_SPLIT_SIZE) {
			snprintf(buf, MSG_SPLIT_SIZE*2, "%03d/%03d ",
				 i*MSG_SPLIT_SIZE, CFW_MESSAGE_LEN(msg));
			bin2hex(((unsigned char*)msg) + i * MSG_SPLIT_SIZE, buf+8,
				CFW_MESSAGE_LEN(msg)%MSG_SPLIT_SIZE);
			buf[8+2*(CFW_MESSAGE_LEN(msg)%MSG_SPLIT_SIZE)] = '\0';
			TCMD_RSP_FINAL(ctx, buf);
		} else {
			TCMD_RSP_FINAL(ctx, NULL);
		}
		message_free(&msg->m);
	} else {
		TCMD_RSP_FINAL(ctx, NULL);
	}

}
DECLARE_TEST_COMMAND_ENG(cfw, poll, cfw_poll_message);
