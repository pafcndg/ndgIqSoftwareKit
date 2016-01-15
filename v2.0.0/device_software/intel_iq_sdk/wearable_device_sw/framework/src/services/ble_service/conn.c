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

#include "infra/log.h"

#include "zephyr/bluetooth/hci.h"
#include "zephyr/bluetooth/bluetooth.h"
#include "zephyr/bluetooth/gatt.h"
#include <misc/byteorder.h>

#include "hci_core.h"
#include "conn_internal.h"
#include "gap_internal.h"

#include "util/misc.h"

static struct bt_conn conns[3];
static struct bt_conn_cb *callback_list;

static void notify_connected(struct bt_conn *conn)
{
	struct bt_conn_cb *cb;

	for (cb = callback_list; cb; cb = cb->_next) {
		if (cb->connected) {
			cb->connected(conn, conn->err);
		}
	}
}

static void notify_disconnected(struct bt_conn *conn)
{
	struct bt_conn_cb *cb;

	for (cb = callback_list; cb; cb = cb->_next) {
		if (cb->disconnected) {
			cb->disconnected(conn, conn->err);
		}
	}
}

void bt_conn_cb_register(struct bt_conn_cb *cb)
{
	cb->_next = callback_list;
	callback_list = cb;
}

static struct bt_conn *conn_new(void)
{
	struct bt_conn *conn = NULL;
	int i;

	for (i = 0; i < ARRAY_SIZE(conns); i++) {
		if (!conns[i].ref) {
			conn = &conns[i];
			break;
		}
	}

	if (!conn) {
		return NULL;
	}

	memset(conn, 0, sizeof(*conn));

	conn->ref = 1;

	return conn;
}

struct bt_conn *bt_conn_add_le(const bt_addr_le_t *peer)
{
	struct bt_conn *conn = conn_new();

	if (!conn) {
		return NULL;
	}

	bt_addr_le_copy(&conn->le.dst, peer);
#if defined(CONFIG_BLUETOOTH_SMP)
	conn->sec_level = BT_SECURITY_LOW;
	conn->required_sec_level = BT_SECURITY_LOW;
#endif /* CONFIG_BLUETOOTH_SMP */
	conn->type = BT_CONN_TYPE_LE;
	conn->le.interval_min = BT_GAP_INIT_CONN_INT_MIN;
	conn->le.interval_max = BT_GAP_INIT_CONN_INT_MAX;

	return conn;
}

#if defined(CONFIG_BLUETOOTH_BREDR)
struct bt_conn *bt_conn_lookup_addr_br(const bt_addr_t *peer)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(conns); i++) {
		if (!atomic_get(&conns[i].ref)) {
			continue;
		}

		if (conns[i].type != BT_CONN_TYPE_BR) {
			continue;
		}

		if (!bt_addr_cmp(peer, &conns[i].br.dst)) {
			return bt_conn_ref(&conns[i]);
		}
	}

	return NULL;
}

struct bt_conn *bt_conn_add_br(const bt_addr_t *peer)
{
	struct bt_conn *conn = conn_new();

	if (!conn) {
		return NULL;
	}

	bt_addr_copy(&conn->br.dst, peer);
	conn->type = BT_CONN_TYPE_BR;

	return conn;
}
#endif

void bt_conn_set_state(struct bt_conn *conn, bt_conn_state_t state)
{
	bt_conn_state_t old_state;

	pr_debug(LOG_MODULE_BLE, "conn state %d -> %d", conn->state, state);

	if (conn->state == state) {
		pr_debug(LOG_MODULE_BLE,"no transition");
		return;
	}

	old_state = conn->state;
	conn->state = state;

	/* Actions needed for exiting the old state */
	switch (old_state) {
	case BT_CONN_DISCONNECTED:
		/* Take a reference for the first state transition after
		 * bt_conn_add_le() and keep it until reaching DISCONNECTED
		 * again.
		 */
		bt_conn_ref(conn);
		break;
	case BT_CONN_CONNECT:
#if 0
		if (conn->timeout) {
			fiber_delayed_start_cancel(conn->timeout);
			conn->timeout = NULL;

			/* Drop the reference taken by timeout fiber */
			bt_conn_unref(conn);
		}
#endif
		break;
	default:
		break;
	}

	/* Actions needed for entering the new state */
	switch (conn->state){
	case BT_CONN_CONNECTED:
		notify_connected(conn);
		break;
	case BT_CONN_DISCONNECTED:
		/* Notify disconnection and queue a dummy buffer to wake
		 * up and stop the tx fiber for states where it was
		 * running.
		 */
		if (old_state == BT_CONN_CONNECTED ||
		    old_state == BT_CONN_DISCONNECT) {
			notify_disconnected(conn);
		} else if (old_state == BT_CONN_CONNECT) {
			/* conn->err will be set in this case */
			notify_connected(conn);
		}

		/* Release the reference we took for the very first
		 * state transition.
		 */
		bt_conn_unref(conn);

		break;
	case BT_CONN_CONNECT_SCAN:
		break;
	case BT_CONN_CONNECT:
		break;
	case BT_CONN_DISCONNECT:
		break;
	default:

		break;
	}
}

struct bt_conn *bt_conn_lookup_handle(uint16_t handle)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(conns); i++) {
		if (!conns[i].ref) {
			continue;
		}
		/* We only care about connections with a valid handle */
		if (conns[i].state != BT_CONN_CONNECTED &&
		    conns[i].state != BT_CONN_DISCONNECT) {
			continue;
		}

		if (conns[i].handle == handle) {
			return bt_conn_ref(&conns[i]);
		}
	}

	return NULL;
}

struct bt_conn *bt_conn_lookup_addr_le(const bt_addr_le_t *peer)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(conns); i++) {
		if (!conns[i].ref) {
			continue;
		}

		if (conns[i].type != BT_CONN_TYPE_LE) {
			continue;
		}

		if (!bt_addr_le_cmp(peer, &conns[i].le.dst)) {
			return bt_conn_ref(&conns[i]);
		}
	}

	return NULL;
}

struct bt_conn *bt_conn_lookup_state_le(const bt_addr_le_t *peer,
					const bt_conn_state_t state)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(conns); i++) {
		if (!conns[i].ref) {
			continue;
		}

		if (conns[i].type != BT_CONN_TYPE_LE) {
			continue;
		}

		if (bt_addr_le_cmp(peer, BT_ADDR_LE_ANY) &&
		    bt_addr_le_cmp(peer, &conns[i].le.dst)) {
			continue;
		}

		if (conns[i].state == state) {
			return bt_conn_ref(&conns[i]);
		}
	}

	return NULL;
}

struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
	conn->ref++;

	return conn;
}

void bt_conn_unref(struct bt_conn *conn)
{
	conn->ref--;
}

const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn)
{
	return &conn->le.dst;
}

int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info)
{
	if (conn->state != BT_CONN_CONNECTED) {
		return -ENOTCONN;
	}

	info->type = conn->type;

	switch (conn->type) {
	case BT_CONN_TYPE_LE:
		if (conn->role == BT_HCI_ROLE_MASTER) {
#if 0
			info->le.src = &conn->le.init_addr;
			info->le.dst = &conn->le.resp_addr;
#else
			info->le.dst = &conn->le.dst;
#endif
		} else {
#if 0
			info->le.src = &conn->le.resp_addr;
			info->le.dst = &conn->le.init_addr;
#else
			info->le.src = &conn->le.dst;
#endif
		}
		return 0;
#if defined(CONFIG_BLUETOOTH_BREDR)
	case BT_CONN_TYPE_BR:
		info->br.dst = &conn->br.dst;
		return 0;
#endif
	}

	return -EINVAL;
}

static int bt_hci_disconnect(struct bt_conn *conn, uint8_t reason)
{
	struct ble_gap_disconnect_req_params ble_gap_disconnect;

	ble_gap_disconnect.conn_handle = conn->handle;
	ble_gap_disconnect.reason = reason;
	ble_gap_disconnect_req(&ble_gap_disconnect, NULL);

	bt_conn_set_state(conn, BT_CONN_DISCONNECT);
	return 0;
}

static int bt_hci_connect_le_cancel(struct bt_conn *conn)
{
	ble_gap_cancel_connect_req(conn);
	return 0;
}

void on_ble_gap_cancel_connect_rsp(const struct ble_core_response *p_params) {
	struct bt_conn *conn = p_params->user_data;

	conn->err = BT_HCI_ERR_INSUFFICIENT_RESOURCES;
	bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
}

int bt_conn_disconnect(struct bt_conn *conn, uint8_t reason)
{
#if defined(CONFIG_BLUETOOTH_CENTRAL)
	/* Disconnection is initiated by us, so auto connection shall
	 * be disabled. Otherwise the passive scan would be enabled
	 * and we could send LE Create Connection as soon as the remote
	 * starts advertising.
	 */
	if (conn->type == BT_CONN_TYPE_LE) {
		bt_le_set_auto_conn(&conn->le.dst, NULL);
	}
#endif

	switch (conn->state) {
	case BT_CONN_CONNECT_SCAN:
		conn->err = BT_HCI_ERR_INSUFFICIENT_RESOURCES;
		bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
		/* scan update not yet implemented */
		return 0;
	case BT_CONN_CONNECT:
		return bt_hci_connect_le_cancel(conn);
	case BT_CONN_CONNECTED:
		return bt_hci_disconnect(conn, reason);
	case BT_CONN_DISCONNECT:
		return 0;
	case BT_CONN_DISCONNECTED:
	default:
		return -ENOTCONN;
	}
}

struct bt_conn *bt_conn_create_slave_le(const bt_addr_le_t *peer,
				  const struct bt_le_adv_param *param)
{
	return NULL;
}

#if defined(CONFIG_BLUETOOTH_CENTRAL)
static int hci_le_create_conn(struct bt_conn *conn)
{
	struct ble_gap_connect_req_params conn_params;

	conn_params.bda = conn->le.dst;
	conn_params.conn_params.interval_min = conn->le.interval_min;
	conn_params.conn_params.interval_max = conn->le.interval_max;
	conn_params.conn_params.slave_latency = conn->le.latency;
	conn_params.conn_params.link_sup_to = conn->le.timeout;

	conn_params.scan_params.interval = sys_cpu_to_le16(BT_GAP_SCAN_FAST_INTERVAL);
	conn_params.scan_params.window = conn_params.scan_params.interval;
	conn_params.scan_params.selective = 0;
	conn_params.scan_params.active = 1;
	conn_params.scan_params.timeout = 0;

	ble_gap_connect_req(&conn_params, conn);

	return 0;
}

void on_ble_gap_connect_rsp(const struct ble_core_response *p_params) {
	struct bt_conn *conn = p_params->user_data;

	/* If the connection request was not issued successfully */
	if (p_params->status) {
		conn->err = BT_HCI_ERR_UNACCEPT_CONN_PARAMS;
		bt_conn_set_state(conn, BT_CONN_DISCONNECTED);
	}
}

struct bt_conn *bt_conn_create_le(const bt_addr_le_t *peer,
				const struct bt_le_conn_param *param)
{
	struct bt_conn *conn;

	if (!bt_le_conn_params_valid(param->interval_min, param->interval_max,
				param->latency, param->timeout)) {
		return NULL;
	}

	/* if (atomic_test_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN))
		return NULL; */

	conn = bt_conn_lookup_addr_le(peer);
	if (conn) {
		switch (conn->state) {
		case BT_CONN_CONNECT_SCAN:
			bt_conn_set_param_le(conn, param);
			return conn;
		case BT_CONN_CONNECT:
		case BT_CONN_CONNECTED:
			return conn;
		default:
			bt_conn_unref(conn);
			return NULL;
		}
	}

	conn = bt_conn_add_le(peer);
	if (!conn) {
		return NULL;
	}
#if 0
	bt_conn_set_state(conn, BT_CONN_CONNECT_SCAN);

	bt_le_scan_update(true);
#endif

	bt_addr_le_copy(&conn->le.dst, peer);

	bt_conn_set_param_le(conn, param);

	/* for the time being, the implementation bypassed the scan procedure */
	if (hci_le_create_conn(conn)) {
		goto done;
	}

	bt_conn_set_state(conn, BT_CONN_CONNECT);

done:
	/* bt_conn_unref(conn); - related to conn being referenced in bt_conn_add_le() */

	return conn;
}
#else

void on_ble_gap_connect_rsp(const struct ble_core_response *p_params) {
}

#endif /* CONFIG_BLUETOOTH_CENTRAL */

int bt_conn_le_conn_update(struct bt_conn *conn, uint16_t min, uint16_t max,
			   uint16_t latency, uint16_t timeout)
{
	return -ENOTCONN;
}

int bt_conn_init(void)
{
#if NOT_USED_FOR_NOW
	int err;

	net_buf_pool_init(frag_pool);
	net_buf_pool_init(dummy_pool);

	bt_att_init();

	err = bt_smp_init();
	if (err) {
		return err;
	}

	bt_l2cap_init();

	background_scan_init();
#endif
	return 0;
}
