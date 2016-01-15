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

#include <errno.h>
#include <stddef.h>
#include <assert.h>

#include "zephyr/bluetooth/gatt.h"
#include "zephyr/bluetooth/bluetooth.h"
#include "gap_internal.h"
#include "conn_internal.h"

#include "hci_core.h"

/* nble driver  */
#include "nble_driver.h"

#include "infra/log.h"

static bt_ready_cb_t bt_ready_cb = NULL;




void on_ble_gap_disconnect_evt(const struct ble_gap_disconnect_evt * p_evt) {
	struct bt_conn *conn;

#if 0
	/* Nordic has no disconnection error */
	if (evt->status) {
		return;
	}
#endif

	conn = bt_conn_lookup_handle(p_evt->conn_handle);
	if (!conn) {
		pr_debug(LOG_MODULE_BLE,"Unable to look up conn with handle %u", p_evt->conn_handle);
		return;
	}
#if 0
	/* Check stacks usage (no-ops if not enabled) */
	stack_analyze("rx stack", rx_fiber_stack, sizeof(rx_fiber_stack));
	stack_analyze("cmd rx stack", rx_prio_fiber_stack,
		      sizeof(rx_prio_fiber_stack));
	stack_analyze("cmd tx stack", cmd_tx_fiber_stack,
		      sizeof(cmd_tx_fiber_stack));
	stack_analyze("conn tx stack", conn->stack, sizeof(conn->stack));

#endif

	conn->err = p_evt->hci_reason;
	bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
	conn->handle = 0;

#if 0
	/* Only LE supported */
	if (conn->type != BT_CONN_TYPE_LE) {
		bt_conn_unref(conn);
		return;
	}
	/* TODO enabled when autoconn is supported */
	if (atomic_test_bit(conn->flags, BT_CONN_AUTO_CONNECT)) {
		bt_conn_set_state(conn, BT_CONN_CONNECT_SCAN);
		bt_le_scan_update(false);
	}
#endif

	bt_conn_unref(conn);

#if 0
	/* TODO */
	if (atomic_test_bit(bt_dev.flags, BT_DEV_ADVERTISING)) {
		struct net_buf *buf;
		uint8_t adv_enable = 0x01;

		buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_ENABLE, 1);
		if (buf) {
			memcpy(net_buf_add(buf, 1), &adv_enable, 1);
			bt_hci_cmd_send(BT_HCI_OP_LE_SET_ADV_ENABLE, buf);
		}
	}
#endif
}

void on_ble_gap_connect_evt(const struct ble_gap_connect_evt *evt) {

	struct bt_conn *conn;

	/* Make lookup to check if there's a connection object in CONNECT state
	 * associated with passed peer LE address.
	 */
	conn = bt_conn_lookup_state_le(&evt->peer_bda, BT_CONN_CONNECT);

#if 0
	/* Nordic has no connection error */
	if (evt->status) {
		if (!conn) {
			return;
		}

		conn->err = BT_HCI_ERR_UNACCEPT_CONN_PARAMS;
		bt_conn_set_state(conn, BT_CONN_DISCONNECTED);

		/* Drop the reference got by lookup call in CONNECT state.
		 * We are now in DISCONNECTED state since no successful LE
		 * link been made.
		 */
		bt_conn_unref(conn);

		return;
	}
#endif
	if (!conn) {
		conn = bt_conn_add_le(&evt->peer_bda);
	}

	if (!conn) {
		pr_debug(LOG_MODULE_BLE, "Unable to add new conn for handle %u", evt->conn_handle);
		return;
	}

	conn->handle = evt->conn_handle;
	bt_addr_le_copy(&conn->le.dst, &evt->peer_bda);
	conn->le.interval = evt->conn_values.interval;
	conn->le.latency = evt->conn_values.latency;
	conn->le.timeout = evt->conn_values.supervision_to;
	if (evt->role_slave)
		conn->role = BT_HCI_ROLE_SLAVE;
	else
		conn->role = BT_HCI_ROLE_MASTER;

#if 0
	src.type = BT_ADDR_LE_PUBLIC;
	memcpy(src.val, bt_dev.bdaddr.val, sizeof(bt_dev.bdaddr.val));

	/* use connection address (instead of identity address) as initiator
	 * or responder address
	 */
	if (conn->role == BT_HCI_ROLE_MASTER) {
		bt_addr_le_copy(&conn->le.init_addr, &src);
		bt_addr_le_copy(&conn->le.resp_addr, &evt->peer_addr);
	} else {
		bt_addr_le_copy(&conn->le.init_addr, &evt->peer_addr);
		bt_addr_le_copy(&conn->le.resp_addr, &src);
	}
#endif
	bt_conn_set_state(conn, BT_CONN_CONNECTED);

	/*
	 * it is possible that connection was disconnected directly from
	 * connected callback so we must check state before doing connection
	 * parameters update
	 */
	if (conn->state != BT_CONN_CONNECTED) {
		goto done;
	}

#if 0
	if ((evt->role == BT_HCI_ROLE_MASTER) ||
	    (bt_dev.le.features[0] & BT_HCI_LE_SLAVE_FEATURES)) {
		err = hci_le_read_remote_features(conn);
		if (!err) {
			goto done;
		}
	}

	update_conn_params(conn);
#endif

done:
	bt_conn_unref(conn);
#if 0
	bt_le_scan_update(false);
#endif

}

void bt_conn_set_param_le(struct bt_conn *conn,
			  const struct bt_le_conn_param *param)
{
	conn->le.interval_min = param->interval_min;
	conn->le.interval_max = param->interval_max;
	conn->le.latency = param->latency;
	conn->le.timeout = param->timeout;
}

/* Used to determine whether to start scan and which scan type should be used */
int bt_le_scan_update(bool fast_scan)
{
	return -EINVAL;
}

#if defined(CONFIG_BLUETOOTH_CENTRAL)
int bt_le_set_auto_conn(bt_addr_le_t *addr,
			const struct bt_le_conn_param *param)
{
	return -EINVAL;
}
#endif /* CONFIG_BLUETOOTH_CENTRAL */

static int bt_init(void)
{
#if NOT_USED_FOR_NOW
	struct bt_driver *drv = bt_dev.drv;
#endif
	int err = 0;

#if NOT_USED_FOR_NOW
	err = drv->open();
	if (err) {
		BT_ERR("HCI driver open failed (%d)", err);
		return err;
	}

	err = hci_init();
#endif

	if (!err) {
		err = bt_conn_init();
	}

#if NOT_USED_FOR_NOW
	if (!err) {
		atomic_set_bit(bt_dev.flags, BT_DEV_READY);
		bt_le_scan_update(false);
	}
#endif

	return err;
}

void on_nble_up(void)
{
	if (bt_ready_cb)
		bt_ready_cb(bt_init());
}

int bt_enable(bt_ready_cb_t cb)
{
	bt_ready_cb = cb;

#ifdef CONFIG_IPC_UART_NS16550
	nble_driver_init();
#endif

	if (!cb) {
		return bt_init();
	}

	return 0;
}


static bool valid_adv_param(const struct bt_le_adv_param *param)
{
	switch (param->type) {
	case BT_LE_ADV_IND:
	case BT_LE_ADV_SCAN_IND:
	case BT_LE_ADV_NONCONN_IND:
		break;
	default:
		return false;
	}

#if 0
	// checks done in Nordic
	switch (param->addr_type) {
	case BT_LE_ADV_ADDR_PUBLIC:
	case BT_LE_ADV_ADDR_NRPA:
		break;
	default:
		return false;
	}

	if (param->interval_min > param->interval_max ||
	    param->interval_min < 0x0020 || param->interval_max > 0x4000) {
		return false;
	}
#endif

	return true;
}

int bt_le_adv_start(const struct bt_le_adv_param *param,
		    const struct bt_eir *ad, const struct bt_eir *sd)
{
	struct ble_gap_adv_params params = { 0 };
	int i;

	if (!valid_adv_param(param)) {
		return -EINVAL;
	}

	if (!ad) {
		goto send_scan_rsp;
	}

	for (i = 0; ad[i].len; i++) {
		uint8_t *p;

		/* Check if ad fit in the remaining buffer */
		if (params.ad.len + ad[i].len + 1 > 31) {
			break;
		}

		p = &params.ad.data[params.ad.len];
		*p++ = ad[i].len;
		*p++ = ad[i].type;
		memcpy(p, ad[i].data, ad[i].len - 1);
		params.ad.len += ad[i].len + 1;
	}

send_scan_rsp:
	if (!sd) {
		goto send_set_param;
	}

	for (i = 0; sd[i].len; i++) {
		uint8_t *p;

		/* Check if sd fit in the remaining buffer */
		if (params.sd.len + sd[i].len + 1 > 31) {
			break;
		}

		p = &params.sd.data[params.sd.len];
		*p++ = sd[i].len;
		*p++ = sd[i].type;
		memcpy(p, sd[i].data, sd[i].len - 1);
		params.sd.len += sd[i].len + 1;
	}

send_set_param:
	/* Timeout is handled by application timer */
	params.timeout = 0;
	/* forced to none currently (no whitelist support) */
	params.filter_policy = 0;
	params.interval_max = param->interval_max;
	params.interval_min = param->interval_min;
	params.type = param->type;

	ble_gap_start_advertise_req(&params);

	return 0;
}

int bt_le_adv_stop(void)
{
	ble_gap_stop_advertise_req(NULL);

	return 0;
}

