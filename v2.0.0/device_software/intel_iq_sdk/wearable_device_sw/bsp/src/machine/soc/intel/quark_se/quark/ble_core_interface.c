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

#include <assert.h>
#include <string.h>
#include "os/os.h"
#include "cfw/cfw.h"
#include "cfw/cfw_debug.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_service.h"
#include "infra/ipc_uart.h"
#include "infra/ipc_requests.h"
#include "infra/log.h"
#include "infra/message.h"
#include "util/misc.h"
#include "drivers/soc_gpio.h"
#include "machine.h"
#include "drivers/ipc_uart_ns16550.h"
#include "ticks.h"

#include "rpc.h"

#include "services/ble_service/ble_service_api.h"
#include "ble_service_test.h"
#include "ble_service_int.h"

/* To implement RPC functions */
#include "services/ble_service/ble_service_gap_api.h"

/* Macro definition for reset pin */
#ifdef CONFIG_BOARD_CURIE_APP
	/* WARNING: both option below are NOT working by default! */
	/* PCA10028 board - 2 options:
	 * - Use the RST pin which needs to be connected to SWDIO pin by connecting solder bridge SB17
	 * on the PCA10028 (near P18/Debug IN connector).
	 * - Use the SWDIO pin by connecting with wire the SWDIO from BLE interface board to the
	 * SWDIO on the PCA10028 (pin 2 of P18/Debug IN connector or TP9). */
	#define BTLE_RST_PIN    2
	#define BTLE_SWDIO_PIN  1
	#define BTLE_SWCLK_PIN  0

	/* Use either RST or SWDIO macro defined above */
	#define RESET_PIN       BTLE_RST_PIN
#else
	/* Curie and other board - Everything should be working out of the box */
	#define BLE_SW_CLK_PIN  27
	#define BLE_SWDIO_PIN   6
	#define RESET_PIN       BLE_SWDIO_PIN
	#define QRK_BLE_INT     5
#endif

static struct device* uart_ble_core_dev = NULL;
static struct device* gpio_dev = NULL;
static list_head_t m_rpc_tx_q;

/* When we set val to 1 it will wake up the remote (BLE Core), setting it to 0
 * will allow remote to sleep. */
static int ble_core_wake_assert(bool val)
{
	uint8_t ret = soc_gpio_write(gpio_dev, QRK_BLE_INT, val);
	if (ret != DRV_RC_OK) {
		pr_debug(LOG_MODULE_IPC, "Error setting QRK_BLE_INT %d", val);
	}
	return ret;
}

/**
 * Function registered to be invoked when the IPC starts or ends
 * a transmission session.
 *
 * This sets the wake state of ble core to either keep it awake at start of a tx
 * or to allow sleep at the end of a TX.
 *
 * @param wake_state false if remote may sleep (end of transmission). true, wake remote to start a transmission session.
 * @param ignored not used
 */
static void ble_core_ipc_tx_wake_cb(bool wake_state, void* ignored)
{
	ble_core_wake_assert(wake_state);
}

static int ble_core_interface_init(void)
{
	DRIVER_API_RC ret = DRV_RC_OK;
	/* setup IPC UART reference */
	uart_ble_core_dev = &pf_device_uart_ns16550;
	ipc_uart_ns16550_set_tx_cb(uart_ble_core_dev, ble_core_ipc_tx_wake_cb, NULL);

	/* Configure GPIO as output and high by default */
	gpio_dev = &pf_device_soc_gpio_32;

	gpio_cfg_data_t config;
	config.gpio_type = GPIO_OUTPUT;
	ret = soc_gpio_set_config(gpio_dev, QRK_BLE_INT, &config);
	if (ret != DRV_RC_OK)
		return -1;

	list_init(&m_rpc_tx_q);

	ret = ble_core_wake_assert(1);
	return ret;
}

static struct device* ble_core_interface_get(void)
{
	/* If already set up, wake ble_core and return reference */
	if (uart_ble_core_dev)
		return uart_ble_core_dev;
	/* Otherwise try to init it */
	if (DRV_RC_OK != ble_core_interface_init())
		panic(-1);
	return uart_ble_core_dev;
}

void uart_ipc_disable(void)
{
	if (!uart_ble_core_dev)
		return;
	ipc_uart_ns16550_disable(uart_ble_core_dev);
}

static void * m_rpc_channel;

struct rpc_tx_elt {
	list_t l;
	uint16_t length;
	uint8_t data[0];
};

/**
 * Try to send a element of the RPC waiting list
 *
 * @param l Pointer to the list element to try to send on the UART
 * @return
 */
static int uart_rpc_try_tx(list_t * l)
{
	struct rpc_tx_elt * p_elt;

	/* Retrieve the RPC TX element from the list pointer */
	p_elt = container_of(l, struct rpc_tx_elt, l);

	/* Try to send the element payload */
	return ipc_uart_ns16550_send_pdu(ble_core_interface_get(),
			m_rpc_channel, p_elt->length, p_elt->data);
}

/**
 * Try to send an RPC TX queue element during the free operation
 * of the previous message.  This is invoked under interrupt context
 * and therefore does not require protection.  It is also expected
 * that the tx operation can not fail.
 */
static void uart_rpc_try_tx_on_free(void)
{
	list_t * l;
	int ret;

	/* Get next element in tx q */
	l = list_get(&m_rpc_tx_q);
	if (l) {
		ret = uart_rpc_try_tx(l);

		/* It is not possible to fail when called on free event */
		assert(ret == IPC_UART_ERROR_OK);
	}
}

/**
 * Try to send an RPC TX queue element after enqueuing an element to the
 * RPC TX queue.  At this point the state of the UART driver is not known
 * and there could be a transmission in progress, so the procedure is to
 * protect from interruption, pick (not get) the first element and only
 * get it out of the queue if the tx operation was successful.
 */
static void uart_rpc_try_tx_on_add(void)
{
	list_t * l;
	int ret;

	int flags = interrupt_lock();

	/* Pick next element in tx q */
	l = m_rpc_tx_q.head;
	if (l) {
		ret = uart_rpc_try_tx(l);

		/* If it was sent correctly, remove it from the queue */
		if (ret == IPC_UART_ERROR_OK) {
			l = list_get(&m_rpc_tx_q);
		}
	}

	interrupt_unlock(flags);
}

/**
 * Function handling the events from the UART IPC.
 *
 * @param channel Channel on which the event applies
 * @param request IPC_MSG_TYPE_MESSAGE when a new message was received, IPC_MSG_TYPE_FREE when the previous message was sent and can be freed.
 * @param len Length of the data
 * @param p_data Pointer to the data
 * @return 0
 */
static int uart_ipc_rpc_cback(int channel, int request, int len, void * p_data)
{
	switch(request) {
	case IPC_MSG_TYPE_MESSAGE: {
#ifdef CONFIG_RPC_IN
		/* if BLE service is available, handle it in BLE service context */
		struct ble_rpc_callin *rpc = (struct ble_rpc_callin *)message_alloc(sizeof(*rpc), NULL);

		MESSAGE_ID(&rpc->msg) = 0;
		MESSAGE_LEN(&rpc->msg) = sizeof(*rpc);
		MESSAGE_SRC(&rpc->msg) = _ble_cb.rpc_port_id;
		MESSAGE_DST(&rpc->msg) = _ble_cb.rpc_port_id;
		MESSAGE_TYPE(&rpc->msg) = TYPE_INT;
		rpc->p_data = p_data;
		rpc->len = len;
		if (port_send_message(&rpc->msg) != E_OS_OK)
			panic(-1);

		/* do not free p_data as used by message handler */
#endif
	}
		break;
	case IPC_MSG_TYPE_FREE:
		/* Free the message */
		bfree(p_data);

		/* Try to send another message immediately */
		uart_rpc_try_tx_on_free();
		break;
	default:
		/* Free the message */
		bfree(p_data);
		pr_error(LOG_MODULE_MAIN, "Unsupported RPC request");
		break;
	}
	return 0;
}

uint8_t * rpc_alloc_cb(uint16_t length) {
	struct rpc_tx_elt * p_elt;

	p_elt = balloc(length + offsetof(struct rpc_tx_elt, data), NULL);
	assert(p_elt != NULL);

	/* Save the length of the buffer */
	p_elt->length = length;

	return p_elt->data;
}

/* called under NON-interrupt context */
void rpc_transmit_cb(uint8_t * p_buf, uint16_t length) {
	struct rpc_tx_elt * p_elt;

	p_elt = container_of(p_buf, struct rpc_tx_elt, data);

	list_add(&m_rpc_tx_q, &p_elt->l);

	uart_rpc_try_tx_on_add();
}

/* BLE Core reset is achieved by asserting low the SWDIO pin.
 * However, the BLE Core chip can be in SWD debug mode, and NRF_POWER->RESET = 0 due to,
 * other constraints: therefore, this reset might not work everytime, especially after
 * flashing or debugging.
 */
void ble_core_reset(void)
{
	/* RESET_PIN depends on the board and the local configuration: check top of file */
	gpio_cfg_data_t pin_cfg = { .gpio_type = GPIO_OUTPUT };
	struct device *gpio_dev = &pf_device_soc_gpio_32;

	soc_gpio_set_config(gpio_dev, RESET_PIN, &pin_cfg);

	/* Reset hold time is 0.2us (normal) or 100us (SWD debug) */
	soc_gpio_write(gpio_dev, RESET_PIN, 0);
	local_task_sleep_ms(1);

	/* Open the UART channel for RPC while Nordic is in reset */
	m_rpc_channel = ipc_uart_channel_open(RPC_CHANNEL, uart_ipc_rpc_cback);

	soc_gpio_write(gpio_dev, RESET_PIN, 1);

	/* Set back GPIO to input to avoid interfering with external debugger */
	pin_cfg.gpio_type = GPIO_INPUT;
	soc_gpio_set_config(gpio_dev, RESET_PIN, &pin_cfg);

}
