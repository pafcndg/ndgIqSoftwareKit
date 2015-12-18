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

#ifndef _IPC_UART_NS16550_H_
#define _IPC_UART_NS16550_H_

#include "infra/device.h"

/**
 * @defgroup ipc_uart_ns16550 IPC UART NS16550
 * NS16550 IPC UART driver.
 *
 * This driver allows communication between BLE Core and Quark through UART.
 * @ingroup common_drivers
 * @{
 */

/**
 * IPC Uart power management driver.
 */
extern struct driver ipc_uart_ns16550_driver;

/**
 * Describes the uart IPC to handle
 */
struct ipc_uart_info {
	int uart_num; /*!< UART device to use */
	uint32_t irq_vector; /*!< IRQ number */
	uint32_t irq_mask; /*!< IRQ mask */
	void (*tx_cb)(bool wake_state, void*); /*!< callback to be called to set wake state when TX is starting or ending */
	void* tx_cb_param; /*!< tx_cb function parameter */
	struct pm_wakelock rx_wl; /*!< ipc uart RX wakelock */
	struct pm_wakelock tx_wl; /*!< ipc uart TX wakelock */
};

enum IPC_UART_RESULT_CODES {
	IPC_UART_ERROR_OK = 0,
	IPC_UART_ERROR_DATA_TO_BIG,
	IPC_UART_TX_BUSY /**< A transmission is already ongoing, message is NOT sent */
};


void ipc_uart_isr();
void ipc_uart_ns16550_disable(struct device* dev);

/**
 * This function triggers the sending of PDU buffer over UART.
 *
 * This constructs an IPC message header and triggers the sending of it and message buffer. If a transmission
 * is already ongoing, it will fail. In this case upper layer needs to queue the message buffer.
 *
 * @param dev structure of the opened device
 * @param handle structure of opened IPC uart channel
 * @param len length of message to send
 * @param p_data message buffer to send
 *
 * @return IPC_UART_ERROR_OK TX has been initiated, IPC_UART_TX_BUSY a transmission is already going, message
 *  needs to be queued
 *
 * @note This function needs to be executed with (UART) irq off to avoid pre-emption from uart_ipc_isr
 * causing state variable corruption. It also called from uart_ipc_isr() to send the next IPC message.
 */
int ipc_uart_ns16550_send_pdu(struct device* dev, void *handle, int len, void *p_data);

/**
 * This function registers a callback function being called on TX start/end.
 *
 * This constructs an IPC message header and triggers the sending of it and message buffer. If a transmission
 * is already ongoing, it will fail. In this case upper layer needs to queue the message buffer.
 *
 * @param dev structure of the opened device
 * @param cb callback function for OOB sleep mode handling called at tx start & end
 * @param param parameter passed to cb when being called
 *
 */
void ipc_uart_ns16550_set_tx_cb(struct device* dev, void (*cb)(bool, void*), void* param);

/** @} */

#endif /* _IPC_UART_NS16550_H_ */
