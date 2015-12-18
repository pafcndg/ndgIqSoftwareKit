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
#include "infra/log.h"
#include "infra/port.h"

#ifdef CONFIG_TCMD_SLAVE
#include "infra/ipc.h"
#endif
#include "infra/tcmd/engine.h"
#include "messages.h"
#include "infra/message.h"

#ifdef CONFIG_TCMD_MASTER
#include "master.h"
#endif
#include "async.h"

/** Private symbols */

/**
 * An extension of the message type representing a local Test Command request
 */
struct tcmd_request {
	struct message msg;
	void * priv;
	char *command;
	tcmd_rsp_cb_t callback;
	void *data;
};

/**
 * Test commands messages handler
 *
 * @param msg the message
 * @param data a module-wide opaque data (unused)
 *
 */
static void handle_tcmd_messages(struct message * msg, void *data)
{
	if (MESSAGE_SRC(msg) == MESSAGE_DST(msg)) {
		/* Unused argument */
		(void)data;
		switch(MESSAGE_ID(msg)) {
			case INFRA_MSG_TCMD_ASYNC:
			{
				struct tcmd_request *request = (struct tcmd_request *)msg;
				/* Call the Test Command engine synchronous API */
				tcmd_send(request->command, request->callback, request->data);
				break;
			}
			default:
			{
				pr_error(LOG_MODULE_MAIN, "Unhandled message type %x",
						 MESSAGE_ID(msg));
				break;
			}
		}
		message_free(msg);
	} else {
#if defined(CONFIG_TCMD_MASTER) || defined(CONFIG_TCMD_SLAVE)
		handle_tcmd_remote_messages(msg, data);
#else
		pr_error(LOG_MODULE_MAIN,"We don't support remote messages (type=%d)",
				MESSAGE_ID(msg));
		message_free(msg);
#endif
	}
}

/* Public API */

/* Test command engine communication Port */
uint16_t tcmd_port_id = 0;

void tcmd_async_init(void *queue
#ifdef CONFIG_TCMD_SLAVE
		, const char *name
#endif
)
{
	tcmd_port_id = port_alloc(queue);
	if (tcmd_port_id) {
		port_set_handler(tcmd_port_id, handle_tcmd_messages, NULL);
	}
#ifdef CONFIG_TCMD_SLAVE
	/* Register ourself as a slave Test Command engine */
	ipc_request_sync_int(IPC_REQUEST_REG_TCMD_ENGINE, tcmd_port_id, 0,
			(char *)name);
#endif
}

void tcmd_request_local(char *command, tcmd_rsp_cb_t callback, void *data)
{
	if (tcmd_port_id) {
		int size = sizeof(struct tcmd_request);
		struct tcmd_request *request = (struct tcmd_request*) message_alloc(size, NULL);
		MESSAGE_ID(&request->msg) = INFRA_MSG_TCMD_ASYNC;
		MESSAGE_LEN(&request->msg) = size;
		MESSAGE_SRC(&request->msg) = tcmd_port_id;
		MESSAGE_DST(&request->msg) = tcmd_port_id;
		MESSAGE_TYPE(&request->msg) = TYPE_INT;
		/* Pass Test Command parameters in the message */
		request->command = command;
		request->callback = callback;
		request->data = data;
		port_send_message(&request->msg);
	}
}

void tcmd_send_async(char *command, tcmd_rsp_cb_t callback, void *data)
{
#ifdef CONFIG_TCMD_MASTER
	tcmd_dispatch(command, callback, data);
#else
	tcmd_request_local(command, callback, data);
#endif
}
