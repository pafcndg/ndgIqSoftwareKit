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

#ifndef _INFRA_TCMD_ENGINE_H
#define _INFRA_TCMD_ENGINE_H

#include "infra/tcmd/defines.h"
#include "os/os.h"

/**
 * @defgroup infra_tcmd Test Commands
 * ASCII-encoded messages to perform specific tasks
 * @ingroup infra
 */

/**
 * @defgroup infra_tcmd_engine Test Commands Engine
 * Parses Test commands and selects the relevant handler.
 * @ingroup infra_tcmd
 * @{
 */

/** A Test Command response */
struct tcmd_response {
	const char *group;
	const char *name;
	/** Command Invocation Id */
	unsigned int cii;
	/** a null-terminated response buffer */
	char *buffer;
	/** the response type (provisional, final, error) */
	int type;
	/** the opaque call context specified on command invocation */
	void *data;
};

/** The response callback provided by the caller to the test command engine **/
typedef void (*tcmd_rsp_cb_t) (const struct tcmd_response*);

/**
 * Send a test command synchronously
 *
 * This involves several steps:
 * - parse the incoming command buffer to select a command handler
 * - acknowledge the command by returning an invocation id
 * - invoke the relevant command handler
 * All responses will be provided through the response callback.
 *
 * @param command a null-terminated buffer containing the command
 * @param callback the function to call repeatedly until command is complete
 * @param data: opaque data to be passed back to the caller on each response
 *
 */
void tcmd_send(char *command, tcmd_rsp_cb_t callback, void *data);

#ifdef CONFIG_TCMD_ASYNC

/**
 * Initialize the test command engine for aynchronous processing
 *
 * When issueing test commands asynchronously, we need a queue to be able to
 * post test command invocation requests.
 * Any low-priority queue available in the system will do (logs, ...)
 *
 * @param queue the queue hosting the test command asynchronous invocations
 * @param name a friendly name (only for slave engines)
 *
 */
void tcmd_async_init(T_QUEUE queue
#ifdef CONFIG_TCMD_SLAVE
		, const char *name
#endif /* CONFIG_TCMD_SLAVE */
);

/**
 * Send a test command asynchronously
 *
 * Post a message in the queue provided at initialization to invoke tcmd_send
 * asynchronously.
 *
 * @param command a null-terminated buffer containing the command
 * @param callback the function to call repeatedly until command is complete
 * @param data opaque data to be passed back to the caller on each response
 *
 */
void tcmd_send_async(char *command, tcmd_rsp_cb_t callback, void *data);

#endif /* CONFIG_TCMD_ASYNC */

/** @} */

#endif /* _INFRA_TCMD_ENGINE_H */
