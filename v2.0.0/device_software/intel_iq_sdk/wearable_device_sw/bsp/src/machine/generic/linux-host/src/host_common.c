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

#include <stdarg.h>
#include <assert.h>
#include "os/os.h"
#include "cfw/cfw.h"
#include "machine.h"
#include "infra/ipc.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_debug.h"
#include "host_common.h"
#include "infra/ipc_requests.h"
#include "infra/ipc_uart.h"
#include "ipc_uart_host.h"

static void * cfw_channel;

static T_QUEUE _service_queue;

void set_service_queue(T_QUEUE queue)
{
	_service_queue = queue;
}

T_QUEUE get_service_queue(void)
{
	return _service_queue;
}

extern void uart_ipc_send(void * handle, int len, void * p_data);

uint32_t interrupt_lock()
{
    return 0;
}

void interrupt_unlock(uint32_t flags)
{
}

void printk(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void panic(int err)
{
	printk("\npanic(%d)\n", err);
	assert(0);
	while(1);
}

void linux_ipc_set_channel(void * ipc_channel)
{
    cfw_channel = ipc_channel;
}

int linux_ipc_send_message(struct message *message)
{
    cfw_dump_message((struct cfw_message*)message);

    /* Only 32 bit pointers currently supported! */
    uart_ipc_send(cfw_channel, MESSAGE_LEN(message), message);
    bfree(message);
    return 0;
}

void linux_ipc_free(struct message * message)
{
    bfree(message);
}
