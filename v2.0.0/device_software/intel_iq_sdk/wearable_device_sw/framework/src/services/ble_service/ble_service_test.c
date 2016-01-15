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
#include "os/os.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_service.h"
#include "ble_service_int.h"
#include "ble_service_test.h"
#ifdef CONFIG_UART_NS16550
#include "board.h"
#include "machine.h"
#include "drivers/ipc_uart_ns16550.h"
#endif
#include "ble_service_utils.h"
#include "infra/log.h"
#include "infra/ipc_uart.h"
#include "ble_protocol.h"
#include "gap_internal.h"
#include "util/misc.h"

#ifdef CONFIG_UART_NS16550
static int uart_raw_ble_core_tx_rx(uint8_t * send_data, uint8_t send_no,
			       uint8_t * rcv_data, uint8_t rcv_no)
{
	struct device* dev = &pf_device_uart_ns16550;
	struct ipc_uart_info* info = dev->priv;
	int i;
	uint8_t rx_byte;
	int res;
	/* send command */
	for (i = 0; i < send_no; i++)
		uart_poll_out(info->uart_num, send_data[i]);
	/* answer */
	i = 0;
	do {
		res = uart_poll_in(info->uart_num, &rx_byte);
		if (res == 0) {
			rcv_data[i++] = rx_byte;
		}
	} while (i < rcv_no);
	return i;
}
#endif

int ble_test_reconfigure_transport()
{
#ifdef CONFIG_UART_NS16550
	uart_ipc_disable();
	return 0;
#else
	return -1;
#endif
}

void handle_ble_dtm_init(struct ble_enable_req *req)
{
	struct ble_enable_rsp *resp = (void *)cfw_alloc_rsp_msg(
					&req->header,
					MSG_ID_BLE_ENABLE_RSP, sizeof(*resp));

	ble_gap_dtm_init_req(resp);
	bfree(req);
}

void handle_ble_dtm_cmd(struct cfw_message *msg)
{
	uint8_t send_data[7];
	uint8_t rcv_data[9] = {};
	int send_no;
	int rcv_no;

	struct ble_dtm_test_req *req = container_of(msg, struct ble_dtm_test_req, header);
	struct ble_dtm_test_rsp *resp =
			(void *)cfw_alloc_rsp_msg(msg, MSG_ID_BLE_DTM_RSP, sizeof(*resp));

	resp->status = BLE_STATUS_NOT_SUPPORTED;
	send_data[0] = DTM_HCI_CMD;
	send_data[1] = req->params.mode;
	send_data[2] = DTM_HCI_OPCODE2;

	BUILD_BUG_ON(BLE_TEST_START_DTM_RX ^ BLE_CORE_TEST_START_DTM_RX);
	BUILD_BUG_ON(BLE_TEST_START_DTM_TX ^ BLE_CORE_TEST_START_DTM_TX);
	BUILD_BUG_ON(BLE_TEST_SET_TXPOWER ^ BLE_CORE_TEST_SET_TXPOWER);
	BUILD_BUG_ON(BLE_TEST_START_TX_CARRIER ^ BLE_CORE_TEST_START_TX_CARRIER);
	BUILD_BUG_ON(BLE_TEST_END_DTM ^ BLE_CORE_TEST_END_DTM);

	switch (req->params.mode) {
	case BLE_TEST_START_DTM_RX:
		send_data[3] = 1;	/* length */
		send_data[4] = req->params.rx.freq;
		send_no = 5;
		rcv_no = 7;
		break;
	case BLE_TEST_START_DTM_TX:
		send_data[3] = 3;	/* length */
		send_data[4] = req->params.tx.freq;
		send_data[5] = req->params.tx.len;
		send_data[6] = req->params.tx.pattern;
		send_no = 7;
		rcv_no = 7;
		break;
	case BLE_TEST_SET_TXPOWER:
		send_data[3] = 1;	/* length */
		send_data[4] = req->params.tx_pwr.dbm;
		send_no = 5;
		rcv_no = 7;
		break;
	case BLE_TEST_START_TX_CARRIER:
		send_data[3] = 1;	/* length */
		send_data[4] = req->params.tx.freq;
		send_no = 5;
		rcv_no = 7;
		break;
	case BLE_TEST_END_DTM:
		send_data[3] = 0;	/* length */
		send_no = 4;
		rcv_no = 9;
		break;
	default:
		pr_debug(LOG_MODULE_BLE, "hdl_ble_dtm_cmd(): Wrong "
				"dtm cmd: %d", req->params.mode);
		resp->status = BLE_STATUS_NOT_SUPPORTED;
		cfw_send_message(resp);
		return;
	}

#ifdef CONFIG_UART_NS16550
	uart_raw_ble_core_tx_rx(send_data, send_no, rcv_data, rcv_no);
	resp->status = rcv_data[DTM_HCI_STATUS_IDX];
#endif
	uint8_t *p;
	switch (req->params.mode) {
	case BLE_TEST_END_DTM:
		p = &rcv_data[DTM_HCI_LE_END_IDX];
		LESTREAM_TO_UINT16(p, resp->result.nb);
		break;
	}

	cfw_send_message(resp);
}
