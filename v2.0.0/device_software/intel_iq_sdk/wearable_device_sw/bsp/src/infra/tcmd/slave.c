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
#include "infra/tcmd/engine.h"
#include "async.h"
#include "messages.h"

/* Private symbols */

static void send_response_msg(const struct tcmd_response *local_resp)
{
	struct tcmd_slave_response *response;
	int size;
	/* Try first to allocate a message big enough to hold a copy of the local
	 * response buffer */
	size = sizeof(struct tcmd_slave_response) + strlen(local_resp->buffer) + 1;
	OS_ERR_TYPE err;
	if ((response = (struct tcmd_slave_response*)message_alloc(size, &err))) {
		/* copy the buffer in the last field of our response */
		strcpy(response->buffer, local_resp->buffer);
	} else {
		/* Try to allocate a smaller message to return an error */
		size = sizeof(struct tcmd_slave_response) +
		       strlen(TCMD_ERROR_MSG_NO_MEMORY) + 1;
		/* Failing to allocate this minimal response will panic */
		response = (struct tcmd_slave_response*)message_alloc(size, NULL);
		strcpy(response->buffer, TCMD_ERROR_MSG_NO_MEMORY);
	}
	/*  We passed the original request message as an opaque data */
	struct message *request = (struct message *)local_resp->data;
	MESSAGE_ID(&response->msg) = INFRA_MSG_TCMD_SLAVE_RSP;
	MESSAGE_LEN(&response->msg) = size;
	MESSAGE_SRC(&response->msg) = tcmd_port_id;
	/* Respond to the originator */
	MESSAGE_DST(&response->msg) = MESSAGE_SRC(request);
	MESSAGE_TYPE(&response->msg) = TYPE_RSP;
	/* Restore the private field that was passed in the request */
	response->ctx = ((struct tcmd_slave_request*)request)->ctx;
	/* Copy the local response relevant fields */
	memcpy(response->group, local_resp->group, TCMD_GROUP_MAX_LENGTH);
	/* Just in case the string gets truncated, force a null termination */
	response->group[TCMD_GROUP_MAX_LENGTH] = '\0';
	memcpy(response->name, local_resp->name, TCMD_NAME_MAX_LENGTH);
	response->name[TCMD_NAME_MAX_LENGTH] = '\0';
	response->cii = local_resp->cii;
	response->type = local_resp->type;
	port_send_message(&response->msg);
	if (local_resp->type != TCMD_RSP_TYPE_PROVISIONAL) {
		/* This was the last response, we can free the original message */
		message_free(request);
	}
}

/* Public API */

void handle_tcmd_remote_messages(struct message * msg, void *data)
{
	/* Unused argument */
	(void)data;
	switch(MESSAGE_ID(msg)) {
		case INFRA_MSG_TCMD_SLAVE_REQ:
		{
			char * command = ((struct tcmd_slave_request *)msg)->command;
			/*
			 * We need to capture the responses and forward them to the master
			 * engine. We pass the original message as an opaque data that will
			 * be passed back in the responses.
			 */
			tcmd_send(command, send_response_msg, msg);
			/* The response cb is now responsible for freeing the message */
			break;
		}
		default:
		{
			pr_error(LOG_MODULE_MAIN,
				"Unhandled remote Test Command message in slave: %d",
				MESSAGE_ID(msg));
			/* We are responsible for freeing the message */
			message_free(msg);
			break;
		}
	}
}
