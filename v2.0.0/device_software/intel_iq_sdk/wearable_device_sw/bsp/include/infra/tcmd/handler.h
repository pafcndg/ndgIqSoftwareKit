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

#ifndef _TCMD_INFRA_HANDLER_H
#define _TCMD_INFRA_HANDLER_H

#include "infra/tcmd/defines.h"
#include "util/compiler.h"

/**
 * @defgroup infra_tcmd Test Commands
 * ASCII-encoded messages to perform specific tasks
 * @ingroup infra
 */

 /**
 * @defgroup test_command_syntax Test Command Syntax
 * Test commands supported on Intel&reg; Curie&trade;.
 * @ingroup infra_tcmd
 */

/**
 * @addtogroup infra_tcmd_engine
 * @{
 */

/**
 * The Test command response context provided by the Test Command engine to the
 * Test Command handlers.
 *
 * This context has to be passed along a null-terminated char * buffer to the
 * associated macros to send responses.
 *
 * Example:
 *     TCMD_RSP_FINAL(ctx, NULL)
 *     TCMD_RSP_PROVISIONAL(ctx, "1.0")
 *     TCMD_RSP_ERROR(ctx, "Internal error")
 */
struct tcmd_handler_ctx {
	void (*cb) (void*, int, char *);
};

#define TCMD_RSP_FINAL(x, y)       x->cb(x, TCMD_RSP_TYPE_FINAL,y)
#define TCMD_RSP_ERROR(x, y)       x->cb(x, TCMD_RSP_TYPE_ERROR,y)
#define TCMD_RSP_PROVISIONAL(x, y) x->cb(x, TCMD_RSP_TYPE_PROVISIONAL,y)

/** A test command handler */
struct tcmd_handler {
	const char *group;
	const char *name;
	void (*run) (int, char **, struct tcmd_handler_ctx *);
	void *padding;
};

/**
 * For the test command engine to work, the test commands need to be stored
 * in a dedicated code section whose boundaries are identified by its start
 * and end addresses. This requires a specific linker script, similar to the
 * following one:
 *
 *     SECTIONS {
 *         .test_cmd_sections : {
 *           . = ALIGN(8);
 *           __test_cmds_start = .;
 *           *(SORT(.test_cmds.*))
 *           __test_cmds_end = .;
 *         }
 *     }
 *     INSERT BEFORE .rodata;
*/

/**
 * Use this macro to declare a new command handler, providing:
 * - the command group (without quotes),
 * - the command name (without quotes),
 * - the command handler (pointer to function)
 * The test handler will be automatically added to a dedicated section
 * that will itself be inserted in the global test commands section, as
 * explained above.
 */
#define DECLARE_TEST_COMMAND(group, name, handler) \
        _DECLARE_TEST_COMMAND_PRESCAN(group, name, handler)

/* We use an internal macro to force argument prescan if a macro was passed as
 * either the group or name */
#define _DECLARE_TEST_COMMAND_PRESCAN(group, name, handler) \
	const struct tcmd_handler __test_cmd_##group##_##name   \
	__section(".test_cmds." #group #name)    \
		= {#group, #name, handler, 0}

/**
 * Use this Macro to declare engineering Test Commands.
 *
 * Engineering Test Commands are removed from production builds.
 */
#ifdef NDEBUG
#define DECLARE_TEST_COMMAND_ENG(group, name, handler)
#else
#define DECLARE_TEST_COMMAND_ENG(group, name, handler) \
	DECLARE_TEST_COMMAND(group, name, handler)
#endif

/** @} */

#endif /* _INFRA_TCMD_HANDLER_H */
