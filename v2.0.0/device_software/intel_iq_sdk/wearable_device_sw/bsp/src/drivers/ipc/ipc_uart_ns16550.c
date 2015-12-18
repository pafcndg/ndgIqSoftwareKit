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

#include "drivers/ipc_uart_ns16550.h"

#include "board.h"
#include "machine.h"

#include "infra/ipc_uart.h"

#include "infra/log.h"
#include "infra/ipc_requests.h"
#include "panic_quark_se.h"

#define BOTH_EMPTY (0x40|0x20) /* TEMT & THRE empty */


enum {
	STATUS_TX_IDLE = 0,
	STATUS_TX_BUSY,
	STATUS_TX_DONE,
};

enum {
	STATUS_RX_IDLE = 0,
	STATUS_RX_HDR,
	STATUS_RX_DATA
};

struct ipc_uart {
	uint8_t * tx_data;
	uint8_t * rx_ptr;
	struct ipc_uart_channels channels[IPC_UART_MAX_CHANNEL];
	struct ipc_uart_header tx_hdr;
	struct ipc_uart_header rx_hdr;
	uint16_t send_counter;
	uint16_t rx_size;
	uint8_t tx_state;
	uint8_t rx_state;
	uint8_t uart_enabled;
	/* protect against multiple wakelock and wake assert calls */
	uint8_t tx_wakelock_acquired;
	/* TODO: remove once IRQ will take a parameter */
	struct device* device;
};

static struct ipc_uart ipc = {};

void ipc_uart_close_channel(int channel_id)
{
	ipc.channels[channel_id].state = IPC_CHANNEL_STATE_CLOSED;
	ipc.channels[channel_id].cb = NULL;
	ipc.channels[channel_id].index = channel_id;

	ipc.uart_enabled = 0;
	ipc.tx_wakelock_acquired = 0;
}

void ipc_uart_ns16550_disable(struct device* dev)
{
	if (!dev) {
		pr_error(LOG_MODULE_IPC, "bad device");
		return;
	}
	struct ipc_uart_info* info = dev->priv;
	if (!info) {
		pr_error(LOG_MODULE_IPC, "bad info");
		return;
	}
	int i;
	for (i = 0; i < IPC_UART_MAX_CHANNEL; i++)
		ipc_uart_close_channel(i);
	if (info->tx_cb)
		info->tx_cb(0, info->tx_cb_param);
	UART_IRQ_TX_DISABLE(info->uart_num);
	UART_IRQ_RX_DISABLE(info->uart_num);
}

static int ipc_uart_ns16550_suspend(struct device* dev, PM_POWERSTATE state)
{
	if (state == PM_SHUTDOWN) {
		/* Disable message reception before reboot */
		ipc_uart_ns16550_disable(dev);
	}
	return 0;
}

static int ipc_uart_ns16550_init(struct device* dev)
{
	int i;
	struct ipc_uart_info* info = dev->priv;

	/* Fail init if no info defined */
	if (!info) {
		pr_error(LOG_MODULE_IPC, "ipc_uart %d", dev->id);
		return -1;
	}
	/* Init ipc uart wakelock */
	pm_wakelock_init(&(info->rx_wl), info->rx_wl.id);
	pm_wakelock_init(&(info->tx_wl), info->tx_wl.id);

	for (i = 0; i < IPC_UART_MAX_CHANNEL; i++)
		ipc_uart_close_channel(i);

	/* TODO: replace ipc_uart_isr with board_config settings. */
	SET_INTERRUPT_HANDLER(info->irq_vector, ipc_uart_isr);

	/* Set dev used in irq handler */
	ipc.device = dev;

	ipc.uart_enabled = 0;
	ipc.tx_wakelock_acquired = 0;

	/* Initialize the reception pointer */
	ipc.rx_size = sizeof(ipc.rx_hdr);
	ipc.rx_ptr = (uint8_t *)&ipc.rx_hdr;
	ipc.rx_state = STATUS_RX_IDLE;

	return 0;
}

static void ipc_uart_push_frame(uint16_t len, uint8_t * p_data)
{
	pr_debug(LOG_MODULE_IPC, "push_frame: received:frame len: %d, p_data: "
			"len %d, src %d, channel %d", ipc.rx_hdr.len, len,
			ipc.rx_hdr.src_cpu_id,
			ipc.rx_hdr.channel);

	if ((ipc.rx_hdr.channel < IPC_UART_MAX_CHANNEL) &&
	    (ipc.channels[ipc.rx_hdr.channel].cb != NULL)) {
		ipc.channels[ipc.rx_hdr.channel].cb(ipc.rx_hdr.channel,
				IPC_MSG_TYPE_MESSAGE,
				len,
				p_data);
	} else {
		bfree(p_data);
		pr_error(LOG_MODULE_IPC, "uart_ipc: bad channel %d",
				ipc.rx_hdr.channel);
	}
}

void ipc_uart_isr()
{
	/* TODO: remove once IRQ supports parameter */
	struct device* dev = ipc.device;
	struct ipc_uart_info* info = dev->priv;
	uint8_t *p_tx;

	while (UART_IRQ_HW_UPDATE(info->uart_num) && UART_IRQ_IS_PENDING(info->uart_num)) {
		if (UART_IRQ_ERR_DETECTED(info->uart_num)) {
			uint8_t c;
			if (UART_BREAK_CHECK(info->uart_num)){
				handle_panic_notification(BLE_CORE);
			}
			UART_POLL_IN(info->uart_num, &c);
		} else if (UART_IRQ_RX_READY(info->uart_num)) {
			int rx_cnt;

			while ((rx_cnt = UART_FIFO_READ(info->uart_num, ipc.rx_ptr, ipc.rx_size)) != 0) {
				if ((ipc.uart_enabled) && (ipc.rx_state == STATUS_RX_IDLE)) {
					/* acquire wakelock until frame is fully received */
					pm_wakelock_acquire(&info->rx_wl);
					ipc.rx_state = STATUS_RX_HDR;
				}

				/* Until UART has enabled at least one channel, data should be discarded */
				if (ipc.uart_enabled) {
					ipc.rx_size -= rx_cnt;
					ipc.rx_ptr += rx_cnt;
				}

				if (ipc.rx_size == 0) {
					if (ipc.rx_state == STATUS_RX_HDR) {
						ipc.rx_ptr = balloc(ipc.rx_hdr.len, NULL);
						ipc.rx_size = ipc.rx_hdr.len;
						ipc.rx_state = STATUS_RX_DATA;
					}
					else {
#ifdef IPC_UART_DBG_RX
						uint8_t *p_rx = ipc.rx_ptr - ipc.rx_hdr.len;
						for(int i = 0; i < ipc.ipc.rx_hdr.len; i++) {
							pr_debug(LOG_MODULE_IPC, "ipc_uart_isr: %d byte is %d", i, p_rx[i]);
						}
#endif

						ipc_uart_push_frame(ipc.rx_hdr.len, ipc.rx_ptr - ipc.rx_hdr.len);
						ipc.rx_size = sizeof(ipc.rx_hdr);
						ipc.rx_ptr = (uint8_t *)&ipc.rx_hdr;
						ipc.rx_state = STATUS_RX_IDLE;
						/* Frame received, release wakelock */
						pm_wakelock_release(&info->rx_wl);
					}
				}

			}
		} else if (UART_IRQ_TX_READY(info->uart_num)) {
			int tx_len;

			if (ipc.tx_state == STATUS_TX_DONE) {
				ipc.tx_state = STATUS_TX_IDLE;
				uint8_t lsr = UART_LINE_STATUS(info->uart_num);
				UART_IRQ_TX_DISABLE(info->uart_num);
				pr_debug(LOG_MODULE_IPC, "ipc_isr_tx: disable TXint, LSR: 0x%2x\n",
						lsr);
				/* wait for FIFO AND THR being empty! */
				while ((lsr & BOTH_EMPTY) != BOTH_EMPTY) {
					lsr = UART_LINE_STATUS(info->uart_num);
				}

				/* No more TX activity, send event and release wakelock */
				if (info->tx_cb) {
					info->tx_cb(0, info->tx_cb_param);
				}
				pm_wakelock_release(&info->tx_wl);
				ipc.tx_wakelock_acquired = 0;
				return;
			}
			if (NULL == ipc.tx_data){
				pr_warning(LOG_MODULE_IPC, "ipc_uart_isr: Bad Tx data");
				return;
			}

			if (!ipc.tx_wakelock_acquired) {
				ipc.tx_wakelock_acquired = 1;
				/* Starting TX activity, send wake assert event and acquire wakelock */
				if (info->tx_cb) {
					info->tx_cb(1, info->tx_cb_param);
				}
				pm_wakelock_acquire(&info->tx_wl);
			}
			if (ipc.send_counter < sizeof(ipc.tx_hdr)) {
				p_tx = (uint8_t *) &ipc.tx_hdr +
						ipc.send_counter;
				tx_len = sizeof(ipc.tx_hdr) - ipc.send_counter;
			} else {
				p_tx = ipc.tx_data +
						(ipc.send_counter - sizeof(ipc.tx_hdr));
				tx_len = ipc.tx_hdr.len -
						(ipc.send_counter - sizeof(ipc.tx_hdr));
			}
			ipc.send_counter += UART_FIFO_FILL(info->uart_num, p_tx, tx_len);

			if (ipc.send_counter == (ipc.tx_hdr.len + sizeof(ipc.tx_hdr))) {
				ipc.send_counter = 0;
#ifdef IPC_UART_DBG_TX
				pr_debug(LOG_MODULE_IPC, "ipc_uart_isr: sent IPC FRAME "
						"len %d", ipc.tx_hdr.len);
#endif

				p_tx = ipc.tx_data;
				ipc.tx_data = NULL;
				ipc.tx_state = STATUS_TX_DONE;

				/* free sent message and pull send next frame one in the queue */
				if (ipc.channels[ipc.tx_hdr.channel].cb) {
					ipc.channels[ipc.tx_hdr.channel].cb(
						ipc.tx_hdr.channel,
						IPC_MSG_TYPE_FREE,
						ipc.tx_hdr.len,
						p_tx);
				} else
					bfree(p_tx);
#ifdef IPC_UART_DBG_TX
				uint8_t lsr = UART_LINE_STATUS(info->uart_num);
				pr_debug(LOG_MODULE_IPC, "ipc_isr_tx: tx_idle LSR: 0x%2x\n",
						lsr);
#endif
			}
		} else {
			pr_warning(LOG_MODULE_IPC, "ipc_uart_isr: bad ISR");
		}
	}
}

void * ipc_uart_channel_open(int channel_id,
			int (*cb) (int, int, int, void *))
{
	struct ipc_uart_channels *chan;

	if (channel_id > (IPC_UART_MAX_CHANNEL - 1))
		return NULL;

	chan = &ipc.channels[channel_id];

	if (chan->state != IPC_CHANNEL_STATE_CLOSED)
		return NULL;

	chan->state = IPC_CHANNEL_STATE_OPEN;
	chan->cb = cb;

	ipc.uart_enabled = 1;
	ipc.tx_wakelock_acquired = 0;

	return chan;
}

int ipc_uart_ns16550_send_pdu(struct device* dev, void *handle, int len, void *p_data)
{
	struct ipc_uart_info* info = dev->priv;
	struct ipc_uart_channels *chan = (struct ipc_uart_channels *) handle;

	if (ipc.tx_state == STATUS_TX_BUSY) {
		return IPC_UART_TX_BUSY;
	}
	/* It is eventually possible to be in DONE state (sending last bytes of previous message),
	 * so we move immediately to BUSY and configure the next frame */
	ipc.tx_state = STATUS_TX_BUSY;

	ipc.tx_hdr.len = len;
	ipc.tx_hdr.channel = chan->index;
	ipc.tx_hdr.src_cpu_id = 0;
	ipc.tx_data = p_data;

	/* Enable the interrupt (ready will expire if it was disabled) */
	UART_IRQ_TX_ENABLE(info->uart_num);

	return IPC_UART_ERROR_OK;
}

void ipc_uart_ns16550_set_tx_cb( struct device* dev, void (*cb)(bool, void*),
			void* param)
{
	struct ipc_uart_info* info = dev->priv;
	info->tx_cb = cb;
	info->tx_cb_param = param;
}

struct driver ipc_uart_ns16550_driver = {
	.init = ipc_uart_ns16550_init,
	.suspend = ipc_uart_ns16550_suspend,
	.resume = NULL,
};
