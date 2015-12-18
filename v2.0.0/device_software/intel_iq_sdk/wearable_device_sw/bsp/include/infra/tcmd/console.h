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

#ifndef _INFRA_TCMD_CONSOLE_H
#define _INFRA_TCMD_CONSOLE_H

/**
 * @defgroup infra_tcmd_console Test Commands Console
 * Console helpers for Test Commands.
 * A thin wrapper around the engine that can easily be plugged on serial
 * interfaces such as UART to support multiple Test Commands in parallel.
 * @ingroup infra_tcmd
 * @{
 */

/**
 *
 * Generate a test command invocation from a console input.
 *
 * WARNING: this copies the console input. If you want to avoid this copy, you
 * need to write your own test command client amd invoke test_command directly.
 *
 * This function acts as a test command buffer, allowing multiple test commands
 * to be issued in parallel:
 * - copy a buffer from console input,
 * - link it to a calling context,
 * - invoke the test command engine,
 *
 * As test command responses are received, they are pushed to the console using
 * a console output callback passed by the caller.
 * When all reponses have been processed, the calling context is freed.
 *
 *  @param input the console input
 *  @param size the input size
 *  @param console_putc the console output callback
 *
 */
void tcmd_console_read(char *input, unsigned int size, int(*console_putc)(int));

/**
 *
 * Generate a test command invocation from a console input.
 *
 * WARNING: this copies the console input. If you want to avoid this copy, you
 * need to write your own test command client amd invoke test_command directly.
 *
 * This function acts as a test command buffer, allowing multiple test commands
 * to be issued in parallel:
 * - copy a buffer from console input,
 * - link it to a calling context,
 * - invoke the test command engine,
 *
 * As test command responses are received, they are pushed to the console using
 * a console output callback passed by the caller.
 * When all reponses have been processed,the completion_cb is called and the calling
 * context is freed.
 *
 *  @param input the console input
 *  @param size the input size
 *  @param console_putc the console output callback
 *  @param completion_cb the completion callback
 *
 */
void tcmd_console_read_completion_notify(char *input, unsigned int size,
               int (*console_putc) (int),void (*completion_cb) (void));

/** @} */

#endif /* _INFRA_TCMD_CONSOLE_H */
