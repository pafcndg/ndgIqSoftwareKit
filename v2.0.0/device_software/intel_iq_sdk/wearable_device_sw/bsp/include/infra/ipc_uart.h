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

#ifndef _IPC_UART_H_
#define _IPC_UART_H_

#include <stdint.h>

/**
 * @defgroup IPC_UART IPC UART channel definitions
 * Defines the interface used for QRK/BLE Core UART IPC
 *
 * QRK/BLE Core Inter-Processor Communication uses UART to transport messages.
 *
 * @ingroup ipc
 * @{
 */

#define RPC_CHANNEL 0

/**
 * Definitions valid for NONE sync IPC UART headers.
 */

/**
 * @note this structure must be self-aligned and self-packed
 */
struct ipc_uart_header {
	uint16_t len; /**< Length of IPC message. */
	uint8_t channel; /**< Channel number of IPC message. */
	uint8_t src_cpu_id; /**< CPU id of IPC sender. */
};

#define IPC_CHANNEL_STATE_CLOSED 0
#define IPC_CHANNEL_STATE_OPEN 1

#define IPC_UART_MAX_CHANNEL 1

struct ipc_uart_channels {
	uint16_t index;
	uint16_t state;
	int (*cb) (int chan, int request, int len, void * data);
};

/**
 * Opens a UART channel for QRK/BLE Core IPC, and defines the callback function
 * for receiving IPC messages.
 *
 * @param channel IPC channel ID to use
 * @param cb      Callback to handle messages
 *
 * @return
 *         - Pointer to channel structure if success,
 *         - NULL if opening fails.
 */
void * ipc_uart_channel_open(int channel, int (*cb)(int chan, int request, int len, void * data));

/**
 * Sends a message through UART IPC.
 *
 * @param handle Channel handler
 * @param len    Length of packet
 * @param p_data Payload of the message
 *
 * @return 0 if success; error code otherwise.
 */
int uart_ipc_send_message(void * handle, int len, void *p_data);

/**
 * Initializes the pool of UART channels.
 */
void uart_ipc_init();

/**
 * Disables (closes) all UART channels
 */
void uart_ipc_disable(void);

/** @} */

#endif /* _IPC_UART_H_ */
