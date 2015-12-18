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
#include "infra/tcmd/engine.h"
#include "infra/tcmd/handler.h"

/** Start address of the code section dedicated to test command handlers **/
extern const struct tcmd_handler __test_cmds_start[];
/** End address of the code section dedicated to test command handlers **/
extern const struct tcmd_handler __test_cmds_end[];

/** Private types **/

/** The test command engine version **/
#define TCMD_ENGINE_VERSION "1.1"

/** Pre-defined response buffers **/
#define TCMD_RESPONSE_ACK		"ACK"
#define TCMD_RESPONSE_OK		"OK"
#define TCMD_RESPONSE_ERROR		"ERROR"

/** Command invocation ID counter **/
static unsigned int last_cii = 0;

/** A test command request used when processing test commands responses **/
struct tcmd_context {
	/* The first field is the response context needed by the handler */
	struct tcmd_handler_ctx resp_ctx;
	/* Other fields are for internal use only */
	unsigned int id;
	tcmd_rsp_cb_t callback;
	void *data;
	const struct tcmd_handler *cmd;
};

/** Private functions **/

/**
 * Find a command handler for the specified group and command name
 *
 * We go through the ordered list of registered test commands that have
 * been put by the linker in a specific section identified by start and
 * end addresses.
 *
 * @param group the command group
 * @param name the command name
 *
 * @return a pointer to a test command handler struct or NULL
 *
 */
static const struct tcmd_handler *find_command(const char *group,
	const char *name)
{
	const struct tcmd_handler *cmd = __test_cmds_start;
	const struct tcmd_handler *result = NULL;
	int grp_len = strlen(group);
	int name_len = strlen(name);
        bool exact_match = false;
	while ((!result || !exact_match) && (cmd < __test_cmds_end)) {
		if ((strncmp(group, cmd->group, grp_len) == 0)
		    && (strncmp(name, cmd->name, name_len) == 0)) {
                        if ((grp_len == strlen(cmd->group))
                            && (name_len == strlen(cmd->name))) {
                                exact_match = true;
                                result = cmd;
                        } else {
                                /* only keep the first partial match */
                                if (!result)
                                    result = cmd;
                        }
		}
		cmd++;
	}
	return result;
}

/**
 * Parse a test command buffer
 *
 * We go through the buffer to split arguments in place by
 * inserting '\0' at words separations.
 * The arguments starting addresses are stored in a dynamically
 * allocated array that must be freed by the caller.
 * Note: the caller MUST NOT free the arguments themselves, as the
 * corresponding memory will be freed when the input buffer is.
 *
 * @param buffer the test command invocation buffer
 * @argv the split params array (the array must be freed by the caller)
 *
 * @return the number of arguments found or -1 if an error occured
 */
static int split_arguments(char *buffer, char ***argv)
{
	/* First pass: calculate argc and split args in place */
	char *pc = buffer;
	bool in_arg = false;
	int argc = 0;
	while ((*pc) != '\0') {
		if ((*pc) == ' ') {
			in_arg = false;
			*pc = '\0';
		} else {
			if (!in_arg) {
				in_arg = true;
				argc++;
			}
		}
		pc++;
	}
	if (argc) {
		/* Second pass: store args pointers in argv */
		OS_ERR_TYPE err;
		*argv = (char **)balloc(argc * sizeof(char *), &err);
		if (*argv) {
			int i = 0;
			pc = buffer;
			in_arg = false;
			while (i < argc) {
				if ((*pc) == '\0') {
					in_arg = false;
				} else {
					if (!in_arg) {
						in_arg = true;
						(*argv)[i] = pc;
						i++;
					}
				}
				pc++;
			}
		} else {
			argc = -1;
		}
	}
	return argc;
}

/**
 * Test command handlers response callback function
 *
 * This function will be called repeatedly by test command handlers as
 * they send back responses to a test command invocation.
 * Test command handlers can optionnally pass a data buffer that will be
 * passed back to the caller. Such data buffers can be freely disposed of
 * by the test command handlers once this function returns.
 * Please note that data buffers are always sent using their own provisional
 * response, so if a data buffer is provided on a non-provisional request, one
 * will implicitly be created before issueing the final response.
 * The actual invocation context for a given command is retrieved thanks
 * to an opaque data passed back by the test handlers.
 * Once all response have been processed, the invocation context is freed.
 *
 * @data an opaque data containing the invocation context
 * @param type the response type (provisional, final, error)
 * @param buffer the test command response buffer
 *
 */
static void tcmd_response_cb(void *data, int type, char *buffer)
{
	if (data) {
		struct tcmd_response response;
		/* Retrieve the invocation context passed as an opaque data */
		struct tcmd_context *ctx = (struct tcmd_context *)data;
		/* Prepare the response */
		response.group = ctx->cmd->group;
		response.name = ctx->cmd->name;
		response.cii = ctx->id;
		response.data = ctx->data;
		if (buffer) {
			/* Data buffers always use their own provisional response */
			response.buffer = buffer;
			response.type = TCMD_RSP_TYPE_PROVISIONAL;
			ctx->callback(&response);
		}
		if (type != TCMD_RSP_TYPE_PROVISIONAL) {
			/* We won't receive any more responses */
			if (type == TCMD_RSP_TYPE_ERROR) {
				response.buffer = TCMD_RESPONSE_ERROR;
			} else {
				response.buffer = TCMD_RESPONSE_OK;
			}
			/* Tell the caller we're done */
			response.type = type;
			ctx->callback(&response);
			/* Free our context */
			bfree(ctx);
		}
	}
}

/**
 * The special 'help' test command handler
 *
 * This handler is special as it is not registered like the other ones, so
 * technically, it cannot be found (instead, its invocation is hard-wired
 * to the recognition of the 'help' pattern).
 * To build the response, we go through the list of handlers stored in the
 * dedicated test command section build by the linker, whose boundaries are
 * specified by dedicated variables conmtaining its start and end addresses.
 *
 * @param argc the number of arguments (1)
 * @param argv the arguments array
 * @param ctx  the context to pass back to responses
 *
 */
static void list_commands(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Unused arguments */
	(void)argc;
	(void)argv;
	const struct tcmd_handler *first_cmd = __test_cmds_start;
	/* We output one line per group */
	while (first_cmd < __test_cmds_end) {
		/* First pass: calculate response buffer size */
		const char *cur_grp = first_cmd->group;
		unsigned int cur_grp_len = strlen(cur_grp);
		/* Format is "<group>: <commands>\0" */
		unsigned int size = cur_grp_len + 2 + strlen(first_cmd->name) + 1;
		const struct tcmd_handler *last_cmd = first_cmd;
		const struct tcmd_handler *cmd = first_cmd + 1;
		while ((cmd < __test_cmds_end)
			&& (strncmp(cur_grp, cmd->group, cur_grp_len) == 0)) {
			/* Commands are separated by a space */
			size += strlen(cmd->name) + 1;
			last_cmd = cmd;
			cmd++;
		}
		/* Second pass: format response */
		OS_ERR_TYPE err;
		char *output = (char *)balloc(size * sizeof(char), &err);
		if (output) {
			sprintf(output, "%s:", cur_grp);
			char *cursor = output + cur_grp_len + 1;
			for (cmd = first_cmd; cmd <= last_cmd; cmd++) {
				sprintf(cursor, " %s", cmd->name);
				cursor += strlen(cmd->name) + 1;
			}
			TCMD_RSP_PROVISIONAL(ctx, output);
			bfree(output);
		} else {
			TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_NO_MEMORY);
			return;
		}
		first_cmd = last_cmd + 1;
	}
	TCMD_RSP_FINAL(ctx, NULL);
}

/** The 'help' test command handler full declaration **/
static struct tcmd_handler help_cmd = {
	"help",
	"",
	list_commands,
	NULL
};

/**
 * The test engine 'version' test command handler
 *
 * This handler is merely used for unit tests
 *
 * @param argc the number of arguments (1)
 * @param argv the arguments array
 * @param ctx  the context to pass back to responses
 *
 */
static void __maybe_unused tcmd_version(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Unused arguments */
	(void)argc;
	(void)argv;
	TCMD_RSP_FINAL(ctx, TCMD_ENGINE_VERSION);
}
DECLARE_TEST_COMMAND_ENG(tcmd, version, tcmd_version);

/** Public API **/

void tcmd_send(char *command, tcmd_rsp_cb_t callback, void *data)
{
	unsigned int cii = 0;
	int argc = 0;
	char **argv = NULL;
	const struct tcmd_handler *cmd = NULL;
	/*
	 * Increment our invocation counter, skipping zero if we overflow.
	 * We are in the test command caller thread so we need to disable
	 * interrupts to protect access to the CII counter.
	 */
	uint32_t keys = interrupt_lock();
	cii = ++last_cii ? last_cii : 1;
	interrupt_unlock(keys);
	/* Parse the command buffer */
	argc = split_arguments(command, &argv);
	if (argc == -1) {
		struct tcmd_response response = {
			command,
			"",
			cii,
			TCMD_ERROR_MSG_NO_MEMORY,
			TCMD_RSP_TYPE_ERROR,
			data
		};
		callback(&response);
	} else {
		const char *group = argc ? argv[0] : "";
		const char *name = (argc > 1) ? argv[1] : "";
		if (argc < 2) {
			/* We handle the 'help' command as a special case */
			if ((argc == 1) && (strncmp(group, "help", 4) == 0)) {
				cmd = &help_cmd;
			}
		} else {
			/* Lookup the test command handlers section to find a match */
			cmd = find_command(group, name);
		}
		if (cmd) {
			/* Immediately acknowledge the command */
			struct tcmd_response response = {
				group,
				name,
				cii,
				TCMD_RESPONSE_ACK,
				TCMD_RSP_TYPE_PROVISIONAL,
				data
			};
			callback(&response);
			/* Allocate a test command context */
			OS_ERR_TYPE err;
			struct tcmd_context *ctx =
				(struct tcmd_context *)balloc(sizeof(*ctx), &err);
			if (ctx) {
				/* Populate context with the invocation parameters */
				ctx->id = cii;
				ctx->callback = callback;
				ctx->data = data;
				ctx->cmd = cmd;
				ctx->resp_ctx.cb = tcmd_response_cb;
				/* Invoke the test command handler */
				cmd->run(argc, argv, &ctx->resp_ctx);
			} else {
				struct tcmd_response response = {
					group,
					name,
					cii,
					TCMD_ERROR_MSG_NO_MEMORY,
					TCMD_RSP_TYPE_ERROR,
					data
				};
				callback(&response);
			}
		} else {
			/* We didn't find any handler for the provided group and name */
			struct tcmd_response response = {
				group,
				name,
				cii,
				TCMD_ERROR_MSG_NOT_FOUND,
				TCMD_RSP_TYPE_ERROR,
				data
			};
			callback(&response);
		}
		if (argc > 0) {
			bfree(argv);
		}
	}
}
