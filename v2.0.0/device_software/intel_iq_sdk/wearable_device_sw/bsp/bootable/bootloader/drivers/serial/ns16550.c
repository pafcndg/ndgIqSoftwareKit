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

#include "uart.h"

#define readc(_r) (*(volatile unsigned char*)(_r))
#define writec(_r, _v) \
	do { \
		*(volatile unsigned char*)(_r) = ((unsigned char)_v);\
	} while(0)

#define RBR (uart_base_addr + 0x0) /* Rx buffer */
#define THR (uart_base_addr + 0x0) /* Tx buffer */
#define DLL (uart_base_addr + 0x0) /* Divisor latch low */
#define IER (uart_base_addr + 0x4) /* Interrupt enable */
#define DLH (uart_base_addr + 0x4) /* Divisor latch high */
#define IIR (uart_base_addr + 0x8) /* Interrupt identification */
#define FCR (uart_base_addr + 0x8) /* FIFO control */
#define LCR (uart_base_addr + 0xc) /* Line control */
#define MCR (uart_base_addr + 0x10) /* Modem control */
#define LSR (uart_base_addr + 0x14) /* Line status */
#define MSR (uart_base_addr + 0x18) /* Modem status */

/* Regs bits */
#define LCR_DLAB 0x80 /* divisor latch enable */
#define MCR_DTR 0x1 /* disable dtr */
#define MCR_RTS 0x2 /* disable rts */
#define LCR_CS8 0x03   /* 8 bits data size */
#define LCR_2_STB 0x04 /* 2 stop bits */
#define LCR_1_STB 0x00 /* 1 stop bit */
#define LCR_PEN 0x08   /* parity enable */
#define LCR_PDIS 0x00  /* parity disable */
#define FCR_TH_14 0xC0 /* 14 bytes threshold */
#define FCR_EN 0x01 /* enable FIFO */
#define FCR_CLRX 0x02 /* clear rx FIFO */
#define FCR_CLTX 0x04 /* clear tx FIFO */
#define LSR_DR 0x01 /* rx data ready */
#define LSR_TE 0x40  /* tx empty */

static uint32_t uart_base_addr = 0;

void uart_init(uint32_t base_addr, int baud_rate)
{
	uint32_t divisor;
	uart_base_addr = base_addr;

	/* calculate and set baud rate divisor */
	divisor = CONFIG_CLOCK_SPEED / (baud_rate * 16);
	writec(LCR, LCR_DLAB);
	writec(DLL, (divisor & 0xff));
	writec(DLH, ((divisor >> 8) & 0xff));

	/* 8n1 */
	writec(LCR, LCR_CS8 | LCR_1_STB | LCR_PDIS);

	/* No hw flow control, disable DTR and RTS */
	writec(MCR, MCR_RTS | MCR_DTR);

	/* Enable fifo
	 * Clear TX and RX FIFO
	 * Set 14 bits threshold
	 */
	writec(FCR,
		FCR_EN | FCR_CLRX | FCR_CLTX | FCR_TH_14);

	/* disable interrupts  */
	writec(IER, 0x00);

	/* clear the base_addr */
	readc(RBR);

}

int uart_in(unsigned char *c)
{
	/* return -1 if data not ready */
	if ((readc(LSR) & LSR_DR) == 0x00)
		return (-1);

	/* got a character */
	*c = readc(RBR);
	return 0;
}

void uart_out(unsigned char c)
{
	/* wait for transmitter empty */
	while ((readc(LSR) & LSR_TE) == 0);
	writec(THR, c);
}
