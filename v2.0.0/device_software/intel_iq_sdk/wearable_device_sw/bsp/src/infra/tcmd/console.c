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
#include <stdlib.h>
#include <string.h>

#include "infra/tcmd/engine.h"
#include "infra/tcmd/console.h"

/** Private types and variables **/

/** A test command call context **/
struct tst_cmd_ctx {
	char  *input;
	int  (*console_putc) (int);
	void (*completion_cb) (void);
	struct tst_cmd_ctx *previous;
	struct tst_cmd_ctx *next;
};

/** A linked-list of pending call contexts **/
static struct tst_cmd_ctx *pending_ctxs = NULL;

/** Private functions **/

/**
 * Store a test command call context
 *
 * @param ctx the call context to store
 *
 */
static void store_ctx(struct tst_cmd_ctx *ctx)
{
	int flags = interrupt_lock();
	if (pending_ctxs) {
		struct tst_cmd_ctx *last_ctx = pending_ctxs;
		while (last_ctx->next)
			last_ctx = last_ctx->next;
		ctx->previous = last_ctx;
		last_ctx->next = ctx;
		ctx->next = NULL;
	} else {
		ctx->previous = NULL;
		ctx->next = NULL;
		pending_ctxs = ctx;
	}
	interrupt_unlock(flags);
}

/**
 * Release a test command call context
 *
 * @param id the call context
 *
 */
static void release_ctx(struct tst_cmd_ctx *target_ctx)
{
	int flags = interrupt_lock();
	struct tst_cmd_ctx *ctx = pending_ctxs;
	while (ctx && ctx != target_ctx)
		ctx = ctx->next;
	if (ctx) {
		if (ctx->previous) {
			ctx->previous->next = ctx->next;
		}
		if (ctx->next) {
			ctx->next->previous = ctx->previous;
		}
		if (ctx == pending_ctxs) {
			pending_ctxs = ctx->next;
		}
		interrupt_unlock(flags);
		bfree(ctx->input);
		bfree(ctx);
	} else {
		interrupt_unlock(flags);
	}
}

/**
 * Write a null-terminated string to the console output
 *
 * @param buffer the buffer to print
 * @param console_putc the console output callback to use to write data
 *
 */
void console_puts(const char *buffer, int (*console_putc) (int))
{
	const char *pc = buffer;
	while (*pc != '\0') {
		console_putc(*pc);
		pc++;
	}
}

/**
 * Write an int to the console
 *
 * This version uses sprintf to format the int.
 *
 * @param the int to write
 * @param console_putc the console output callback to use to write data
 *
 */
void console_putint(int input, int (*console_putc) (int))
{
	/* Use a buffer for the conversion (length of MAXINT + \0) */
	char buffer[10 + 1];
	sprintf(buffer, "%d", input);
	console_puts(buffer, console_putc);
}

/**
 * The test command emgine response callback.
 *
 * This function will be called repeatedly until a test command has been fully
 * processed.
 * The internal call context will be freed once the last response for a given
 * CII has been received.
 *
 * @param group the command group
 * @param name the command name
 * @param cii the command invocation id
 * @param buffer the response buffer
 * @param type the response type
 * @param data an opaque data used to convey a pointer to the console output cb
 */
static void handle_tcmd_response(const struct tcmd_response *response)
{
	/* Retrieve the call context */
	struct tst_cmd_ctx *ctx = response->data;
	/* Output the result */
	console_puts(response->group, ctx->console_putc);
	ctx->console_putc(' ');
	console_puts(response->name, ctx->console_putc);
	ctx->console_putc(' ');
	console_putint(response->cii, ctx->console_putc);
	ctx->console_putc(' ');
	console_puts(response->buffer, ctx->console_putc);
	ctx->console_putc('\r');
	ctx->console_putc('\n');
	/* If this was the last response, free our internal context */
	if (response->type != TCMD_RSP_TYPE_PROVISIONAL) {
		if (ctx->completion_cb != NULL) {
			ctx->completion_cb();
		}
		release_ctx(ctx);
	}
}

/** Public API **/

void tcmd_console_read_completion_notify(char *input, unsigned int size,
		       int (*console_putc) (int),void (*completion_cb) (void))
{
	/* Allocate an internal context for this test command invocation */
	struct tst_cmd_ctx *ctx = balloc(sizeof(struct tst_cmd_ctx), NULL);
	if (ctx) {
		/* Make a copy of the console input */
		ctx->input = balloc(size + 1, NULL);
		if (ctx) {
			memcpy(ctx->input, input, size);
			/* Null terminate the copied input buffer */
			ctx->input[size] = '\0';
			ctx->console_putc = console_putc;
			ctx->completion_cb = completion_cb;
			/* Store the call context */
			store_ctx(ctx);
			/* Call the test_command engine asynchronous API */
			tcmd_send_async(ctx->input, handle_tcmd_response, ctx);
		} else {
			console_puts(TCMD_ERROR_MSG_NO_MEMORY, console_putc);
		}
	} else {
		console_puts(TCMD_ERROR_MSG_NO_MEMORY, console_putc);
	}
}

void tcmd_console_read(char *input, unsigned int size,
		       int (*console_putc) (int))
{
	tcmd_console_read_completion_notify(input,size,console_putc,NULL);
}
