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

#ifndef _NS16550_PM_H_
#define _NS16550_PM_H_

#include <drivers/uart.h> /* defined by the OS */

/**
 * @defgroup ns16550_pm NS16550 PM
 * NS16550 UART driver wrapper that controls power management.
 *
 * This driver implements power management for NS16550 driver.
 * It does not implement a full UART driver, because it is already done in the OS.
 * @ingroup common_drivers
 * @{
 */

/**
 * uart power management driver.
 */
extern struct driver ns16550_pm_driver;

/**
 * describes UART to handle.
 */
struct ns16550_pm_device {
	int uart_num;                       /*!< UART channel to initialize */
	uint8_t vector;                     /*!< UART ISR vector */
	uint32_t uart_int_mask;             /*!< SSS Interrupt Routing Mask Register */
	void (*uart_rx_callback)(void);     /*!< UART registered callback */
	struct uart_init_info* init_info;   /*!< init info used by the OS */
};

/** @} */

#endif /* _NS16550_PM_H_ */
