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

#include "os/os.h"
#include "util/compiler.h"
#include "util/list.h"
#include "infra/ipc.h"
#include "infra/log.h"
#include "infra/port.h"
#include "infra/tcmd/engine.h"
#include "infra/tcmd/handler.h"
#include "async.h"
#include "messages.h"
#include "master.h"

/* Private symbols */

#define SLAVE_PREFIX_SEPARATOR '.'

/**
 * A Test Command slave engine
 */
struct tcmd_slave {
	struct tcmd_slave *next;
	uint16_t port_id;
	const char *name;
};

/**
 * A Test Command slave invocation context
 */
struct tcmd_slave_ctx {
	struct tcmd_slave *slave;
	tcmd_rsp_cb_t callback;
	void *data;
};

/** Linked list of slave Test Command engines */
static list_head_t slaves = { NULL, NULL };


static struct tcmd_slave *find_slave(const char * name)
{
	struct tcmd_slave* result = NULL;
	list_t *item = slaves.head;
	int len = strlen(name);
	while (item && !result) {
		struct tcmd_slave *slave = (struct tcmd_slave *)item;
		if (strncmp(name, slave->name, len) == 0) {
			result = slave;
		}
		item = item->next;
	}
	return result;
}

static char *skip_spaces(char *buffer)
{
	char *pc = buffer;
	/* Skip spaces */
	while (*pc == ' ') {
		pc++;
	}
	return pc;
}

static char *find_separator(char *buffer)
{
	char *result = NULL;
	char *pc = buffer;
	/* Look for separator */
	while (*pc != SLAVE_PREFIX_SEPARATOR && *pc != ' ' && *pc != '\0') {
		pc++;
		if (*pc == SLAVE_PREFIX_SEPARATOR) {
			result = pc;
		}
	}
	return result;
}

void tcmd_request_slave(char *command, tcmd_rsp_cb_t callback, void *data,
		struct tcmd_slave *slave)
{
	/*
	 * Post a remote request to process the test command
	 * The message must be big enough to contain the command buffer
	 * Memory error leads to panic
	 */
	int size = sizeof(struct tcmd_slave_request) + strlen(command) + 1;
	struct tcmd_slave_request *request =
		(struct tcmd_slave_request *) message_alloc(size, NULL);
	MESSAGE_ID(&request->msg) = INFRA_MSG_TCMD_SLAVE_REQ;
	MESSAGE_LEN(&request->msg) = size;
	MESSAGE_SRC(&request->msg) = tcmd_port_id;
	MESSAGE_DST(&request->msg) = slave->port_id;
	MESSAGE_TYPE(&request->msg) = TYPE_REQ;
	/* Allocate a call context so that we can process responses */
	struct tcmd_slave_ctx *ctx = balloc(sizeof(*ctx), NULL);
	ctx->slave = slave;
	ctx->callback = callback;
	ctx->data = data;
	request->ctx = ctx;
	/* The unprefixed command buffer must be copied */
	strcpy(request->command, command);
	port_send_message(&request->msg);
}

/* Test Commands */

static void slave_name_response(void *item, void *data)
{
	struct tcmd_handler_ctx *ctx = (struct tcmd_handler_ctx *)data;
	TCMD_RSP_PROVISIONAL(ctx, (char *) ((struct tcmd_slave *)item)->name);
}

/** List registered slaves */
static void __maybe_unused list_slaves(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	(void)argc;
	(void)argv;
	if (!list_empty(&slaves)) {
		list_foreach(&slaves, slave_name_response, ctx);
		TCMD_RSP_FINAL(ctx, NULL);
	} else {
		TCMD_RSP_ERROR(ctx, "No slave engines registered");
	}
}
DECLARE_TEST_COMMAND_ENG(tcmd, slaves, list_slaves);

/* Public API */

void handle_tcmd_remote_messages(struct message * msg, void *data)
{
	/* Unused argument */
	(void)data;
	switch(msg->id) {
		case INFRA_MSG_TCMD_SLAVE_REG:
		{
			struct tcmd_slave_request *req_msg =
					(struct tcmd_slave_request *) msg;
			/* Store the slave engine in our static list */
			list_add_head(&slaves, (list_t *)req_msg->slave);
			break;
		}
		case INFRA_MSG_TCMD_SLAVE_RSP:
		{
			struct tcmd_slave_response *resp_msg =
					(struct tcmd_slave_response *) msg;
			/* The calling context is in the private field of the response */
			struct tcmd_slave_ctx *ctx = (struct tcmd_slave_ctx*) resp_msg->ctx;
			/* Prepare a local response */
			char pref_grp[TCMD_GROUP_MAX_LENGTH + TCMD_SLAVE_MAX_LENGTH + 2];
			snprintf(pref_grp, sizeof(pref_grp), "%s%c%s", ctx->slave->name,
					SLAVE_PREFIX_SEPARATOR, resp_msg->group);
			struct tcmd_response response = {
				pref_grp,
				resp_msg->name,
				resp_msg->cii,
				resp_msg->buffer,
				resp_msg->type,
				ctx->data /* Use original data from caller */
			};
			/* Invoke the originator callback */
			ctx->callback(&response);
			/* We can free the context if it was the last response */
			if (resp_msg->type != TCMD_RSP_TYPE_PROVISIONAL) {
				bfree(ctx);
			}
			break;
		}
		default:
		{
			pr_error(LOG_MODULE_MAIN,
				"Unhandled remote Test Command message in master: %d", msg->id);
			break;
		}
	}
	/* We are responsible for freeing the message once we're done with it */
	message_free(msg);
}

void tcmd_dispatch(char *command, tcmd_rsp_cb_t callback, void *data)
{
	struct tcmd_slave *slave = NULL;
	char *cmd_start = skip_spaces(command);
	char *separator = find_separator(cmd_start);
	if (separator) {
		/* Get a substring from the prefix */
		*separator = '\0';
		slave = find_slave(cmd_start);
		if (slave) {
			char * unprefixed_command = separator + 1;
			tcmd_request_slave(unprefixed_command, callback, data, slave);
		} else {
			struct tcmd_response error = {
				cmd_start,
				separator + 1,
				0,
				"No such slave",
				TCMD_RSP_TYPE_ERROR,
				data
			};
			callback(&error);
		}
	} else {
		/* Dispatch locally */
		tcmd_request_local(command, callback, data);
	}
}

int tcmd_ipc_sync_callback(int cpu_id, int request, int param1, int param2
		, void *ptr)
{
	/* These parameters are for future use */
	(void)cpu_id;
	(void)param2;

	int result = 0;
	switch (request) {
		case IPC_REQUEST_REG_TCMD_ENGINE:
		{
			/*
			 * Convert the IPC request to a message posted to ourself, as if it
			 * had been sent from the remote port. Failing to allocate memory
			 * at this stage will lead to a panic
			 */
			struct tcmd_slave *slave =
				(struct tcmd_slave *) message_alloc(sizeof(*slave), NULL);
			struct tcmd_slave_request *msg =
				(struct tcmd_slave_request *)message_alloc(sizeof(*msg), NULL);
			slave->port_id = param1;
			slave->name = (char *)ptr;
			MESSAGE_ID(&msg->msg) = INFRA_MSG_TCMD_SLAVE_REG;
			MESSAGE_LEN(&msg->msg) = sizeof(*msg);
			MESSAGE_SRC(&msg->msg) = slave->port_id;
			MESSAGE_DST(&msg->msg) = tcmd_port_id;
			MESSAGE_TYPE(&msg->msg) = TYPE_INT;
			msg->slave = slave;
			port_send_message(&msg->msg);
			break;
		}
		default:
		{
			pr_debug(LOG_MODULE_MAIN, "Unexpected ipc_sync_request: %d",
					request);
			result = -1;
			break;
		}
    }
    return result;
}
