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
#include <stdint.h>

#include <board.h>
#include "os/os.h"
#include "infra/tcmd/console.h"

#define BACKSPACE 127
#define CONLINE_SIZE 128
static char conline[CONLINE_SIZE];
static int conchars = 0;
static unsigned int uart_port;

/**
 * Output a null-terminated string to the specified
 * UART port.
 *
 * @param uart_port the UART port to write to
 * @param buffer the null-terminated buffer to output
 */
int uart_putc(int c)
{
	UART_POLL_OUT(uart_port, c);
	return c;
}

/** Public API */

void set_tcmd_uart_port(unsigned int port)
{
	uart_port = port;
}

void uart_console_input()
{
	unsigned char c;
	/* handle console input. */
	while (UART_POLL_IN(uart_port, &c) != -1) {
		if (c == '\r')
			c = '\n';

		if (c == BACKSPACE) {
			/* print a backspace and a space to erase last char */
			UART_POLL_OUT(uart_port, '\b');
			UART_POLL_OUT(uart_port, ' ');
			if (conchars > 0)
				conline[--conchars] = '\0';
		}
		if (c == '\n') {
			if (conchars > 0) {
				tcmd_console_read(conline, conchars, uart_putc);
				conchars = 0;
			}
		} else {
			if (conchars < CONLINE_SIZE-1) {
				if (c != BACKSPACE) {
					conline[conchars++] = c;
				}
				unsigned int flags = interrupt_lock();
				UART_POLL_OUT(uart_port, c);
				interrupt_unlock(flags);
			}
		}
	}
}
