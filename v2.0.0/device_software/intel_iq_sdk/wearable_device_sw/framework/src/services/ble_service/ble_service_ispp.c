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

#include "ble_service_ispp.h"
#include "gatt_internal.h"

#include <string.h>

#include "cfw/cfw_service.h"
#include "ble_service_int.h"
#include "ble_service_utils.h"
#include "infra/log.h"
#include "util/misc.h"

// for bt_conn
#include "zephyr/bluetooth/bluetooth.h"
#include "conn_internal.h"

/* Dummy descriptor */
#define CONFIG_BLE_ADD_TEST_DESCRIPTOR 1

/* forward declarations */
struct _ble_service_cb;

/** ISPP control block. */
struct ble_ispp_cb ispp_cb = { 0, };

static const struct bt_uuid ispp_svc_uuid = {
	.type = BT_UUID_128,
	.u128 = { 0x6A, 0xA0, 0x24, 0x0D, 0x69, 0xBA, 0x8F, 0xB1,
		  0x66, 0x47, 0xD9, 0xFE, 0x15, 0xC4, 0x97, 0xDD },
};

/* 0xC416 */
static const struct bt_uuid ispp_ctrl_uuid = {
	.type = BT_UUID_128,
	.u128 = { 0x6A, 0xA0, 0x24, 0x0D, 0x69, 0xBA, 0x8F, 0xB1,
		  0x66, 0x47, 0xD9, 0xFE, 0x16, 0xC4, 0x97, 0xDD },
};

/* 0xCF01 */
static const struct bt_uuid ispp_data_uuid = {
	.type = BT_UUID_128,
	.u128 = { 0x6A, 0xA0, 0x24, 0x0D, 0x69, 0xBA, 0x8F, 0xB1,
		  0x66, 0x47, 0xD9, 0xFE, 0x01, 0xCF, 0x97, 0xDD },
};

/* ISPP control characteristic */
#define ctrl_ccc_cfg NULL

#define ctrl_ccc_cfg_changed NULL

static int on_ctrl_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	/* Char is only writtable, indicate the maximum length */
	return BLE_ISPP_MAX_CHAR_LEN;
}

static int on_ctrl_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset)
{
	ble_ispp_on_write_ctrl(buf, len);
	return len;
}

/* ISPP data characteristic */
#define data_ccc_cfg NULL
#define data_ccc_cfg_changed NULL

static int on_data_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	/* Char is only writable, indicate the maximum length */
	return BLE_ISPP_MAX_CHAR_LEN;
}

static int on_data_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset)
{
	ble_ispp_on_write_data(buf, len);
	return len;
}

#ifdef CONFIG_BLE_ADD_TEST_DESCRIPTOR
static const struct val_range {
	uint8_t min;
	uint8_t max;
} range = {
	.min = 0,
	.max = 100,
};

static int on_range_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const struct val_range *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
			sizeof(*value));
}
#endif

#define BLE_ISPP_CTRL_HANDLE_IDX 2
#ifdef CONFIG_BLE_ADD_TEST_DESCRIPTOR
#define BLE_ISPP_DATA_HANDLE_IDX 7
#else
#define BLE_ISPP_DATA_HANDLE_IDX 6
#endif

static const uint8_t dummy_data[BLE_ISPP_MAX_CHAR_LEN];

/* ISPP Service Declaration */
static const struct bt_gatt_attr ispp_attrs[] = {
	BT_GATT_PRIMARY_SERVICE((void*)&ispp_svc_uuid),
	BT_GATT_CHARACTERISTIC(&ispp_ctrl_uuid, BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP
			| BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(&ispp_ctrl_uuid, BT_GATT_PERM_WRITE | BT_GATT_PERM_WRITE_ENCRYPT,
			on_ctrl_read, on_ctrl_write, (void*)dummy_data),
	BT_GATT_CCC(ctrl_ccc_cfg, ctrl_ccc_cfg_changed),
	BT_GATT_CUD("control", BT_GATT_PERM_READ),
#ifdef CONFIG_BLE_ADD_TEST_DESCRIPTOR
	BT_GATT_DESCRIPTOR(BT_UUID_DECLARE_16(0x2906), BT_GATT_PERM_READ, on_range_read,
			   NULL, (void *)&range),
#endif
	BT_GATT_CHARACTERISTIC(&ispp_data_uuid, BT_GATT_CHRC_WRITE_WITHOUT_RESP
			| BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(&ispp_data_uuid, BT_GATT_PERM_WRITE | BT_GATT_PERM_WRITE_ENCRYPT,
			on_data_read, on_data_write, NULL),
	BT_GATT_CCC(data_ccc_cfg, data_ccc_cfg_changed),
	BT_GATT_CUD("data", BT_GATT_PERM_READ),
};

static void on_connected(struct bt_conn *conn, uint8_t err)
{
	ispp_cb.conn = conn;
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	ispp_cb.conn = NULL;
	ble_ispp_on_disconnect();
}

static struct bt_conn_cb conn_callbacks = {
	.connected = on_connected,
	.disconnected = on_disconnected,
};

/**
 * Initializes BLE ISPP and adds the service.
 */
static int handle_ble_init_service_ispp(struct ble_init_svc_req * msg,
				struct _ble_service_cb * p_cb)
{
	struct ble_init_service_rsp *resp;
	int status;

	status = bt_gatt_register((struct bt_gatt_attr *)ispp_attrs,
				ARRAY_SIZE(ispp_attrs));

	resp = ble_alloc_init_service_rsp(msg);
	if (status) {
		resp->status = BLE_STATUS_ERROR;
	} else {
		bt_conn_cb_register(&conn_callbacks);
	}

	return cfw_send_message(resp);
}

int ble_init_service_ispp(cfw_service_conn_t *p_service_conn, void *priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_init_svc_req, msg, p_service_conn,
			  MSG_ID_BLE_INIT_SVC_REQ, 0, priv);

	msg->init_svc = handle_ble_init_service_ispp;

	return cfw_send_message(msg);
}

uint16_t ble_ispp_ctrl_handle(void)
{
	return ble_attr_idx_to_handle(ispp_attrs, BLE_ISPP_CTRL_HANDLE_IDX);
}

uint16_t ble_ispp_data_handle(void)
{
	return ble_attr_idx_to_handle(ispp_attrs, BLE_ISPP_DATA_HANDLE_IDX);
}
