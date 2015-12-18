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

#include "ble_service_bas.h"

#include "zephyr/bluetooth/gatt.h"
#include "gatt_int.h"
#include "conn_internal.h"

#include "ble_protocol.h"
#include "services/ble_service/ble_service_gap_api.h"
#include "services/ble_service/ble_service_api.h"
#include "services/ble_service/ble_service_gatts_api.h"
#include "ble_service_core_int.h"
#include "ble_service_utils.h"

#include "infra/log.h"

enum {
	BLE_SVC_BAS_NOTIF_SUP_OFF = 0,
	BLE_SVC_BAS_NOTIF_SUP_ON = 0x8000,
};

static uint8_t battery_level = 33; /* 33 % as init value */

/* Battery Service Variables */
static const struct bt_uuid16 bas_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_BAS,
};

static const struct bt_uuid16 blvl_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_BAS_BATTERY_LEVEL,
};

#define blvl_ccc_cfg NULL

#define blvl_ccc_cfg_changed NULL

static int read_blvl(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		     void *buf, uint16_t len, uint16_t offset)
{
	const uint8_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
			sizeof(*value));
}

/* Example presentation format */
static const struct bt_gatt_pf blvl_pf = {
	.unit = 0x27AD, /* percentage */
	.descr = 0x0001, /* first */
	.format = 0x04, /* unsigned 8 bit */
	.exp = 0,
	.name_spc = BT_GATT_NAME_SPACE_SIG,
};

/* Value handle index (0 based) into below. */
#define BLVL_LEVEL_HANDLE_IDX 2

/* Value handle index (0 based) into below. */
#define BLVL_CCC_HANDLE_IDX 3

/* Battery Service Declaration */
static const struct bt_gatt_attr bas_attrs[] = {
	BT_GATT_PRIMARY_SERVICE(&bas_uuid),
	BT_GATT_CHARACTERISTIC(&blvl_uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY),
	BT_GATT_DESCRIPTOR(&blvl_uuid, BT_GATT_PERM_READ,
			   read_blvl, NULL, &battery_level),
	BT_GATT_CCC(blvl_ccc_cfg, blvl_ccc_cfg_changed),
	BT_GATT_PF(&blvl_pf),
};

static void ble_service_bas_add_service_complete(struct ble_init_svc_req_msg *req);

static int handle_ble_init_service_bas(struct ble_init_svc_req_msg *msg,
				struct _ble_service_cb *p_cb)
{
	struct _bt_gatt_ccc *ccc = bas_attrs[BLVL_CCC_HANDLE_IDX].user_data;

	/* use CCCD value for ble service specific purpose to save ram.
	 * notif & indication use the 2 LSBs. Allow battery updated by default
	 */
	ccc->value = BLE_SVC_BAS_NOTIF_SUP_ON;

	/* if register is already ongoing, return busy */
	if (_ble_cb.svc_init_msg)
		return E_OS_ERR_BUSY;
	_ble_cb.svc_init_msg = msg;

	return bt_gatt_register((struct bt_gatt_attr *)bas_attrs,
				ARRAY_SIZE(bas_attrs));
}

int ble_init_service_bas(cfw_service_conn_t * p_service_conn,
		struct _ble_register_svc *p_reg)
{
	CFW_ALLOC_FOR_SVC(struct ble_init_svc_req_msg, msg, p_service_conn,
			  MSG_ID_BLE_INIT_SVC_REQ, 0, p_reg);

	msg->init_svc = handle_ble_init_service_bas;
	msg->init_svc_complete = ble_service_bas_add_service_complete;

	return cfw_send_message(msg);
}

static void ble_service_bas_add_service_complete(struct ble_init_svc_req_msg *req)
{
	struct ble_init_service_rsp * resp = ble_alloc_init_service_rsp(req);

	/* free initial request msg */
	cfw_msg_free(&req->header);

	cfw_send_message(resp);

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
	cfw_register_svc_available(_ble_cb.client, BATTERY_SERVICE_ID, &_ble_cb);
#endif
}

static struct ble_service_update_data_rsp *cli_rsp = NULL;
static void bat_lvl_notified(struct bt_conn *conn, uint16_t handle, uint8_t err)
{
	if (cli_rsp) {
		cli_rsp->status = (err == 0) ? BLE_STATUS_SUCCESS :
				BLE_STATUS_ERROR;
		cfw_send_message(cli_rsp);
		cli_rsp = NULL;
	}
}

static int bas_update_bat_level(uint16_t conn_handle, uint16_t char_handle,
				uint16_t len, uint8_t *p_data,
				struct ble_service_update_data_rsp *p_rsp)
{
	struct _bt_gatt_ccc *ccc = bas_attrs[BLVL_CCC_HANDLE_IDX].user_data;

	/* use CCCD value for ble service specific purpose to save ram.
	 * notif & indication use the 2 LSBs. */
	if ((ccc->value & (BLE_SVC_BAS_NOTIF_SUP_ON | BT_GATT_CCC_NOTIFY)) ==
			(BLE_SVC_BAS_NOTIF_SUP_ON | BT_GATT_CCC_NOTIFY)) {
		struct bt_conn conn;

		conn.handle = conn_handle;

		cli_rsp = p_rsp;
		bt_gatt_notify(&conn, char_handle, (const void *)p_data,
				   len, bat_lvl_notified);
	} else {
		struct ble_gatts_set_attribute_params params;

		params.value_handle = char_handle;
		params.offset = 0;
		ble_gatts_set_attribute_value_req(&params, p_data, len, p_rsp);
	}
	return BLE_STATUS_SUCCESS;
}

#if !defined(CONFIG_SERVICES_BLE_BAS_USE_BAT) || !defined(NDEBUG)
int ble_service_update_bat_level(cfw_service_conn_t *p_service_conn,
		uint16_t conn_handle, uint8_t level, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_update_service_data_msg, msg, p_service_conn,
					MSG_ID_BLE_UPDATE_DATA_REQ,
					sizeof(level),
					p_priv);

	msg->update = bas_update_bat_level;
	msg->conn_handle = conn_handle;
	msg->char_handle = ble_attr_idx_to_handle(bas_attrs,
						  BLVL_LEVEL_HANDLE_IDX);
	msg->len = sizeof(level);
	msg->data[0]= level;
	return cfw_send_message(msg);
}
#endif

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
int handle_ble_update_service_bas(uint16_t conn_handle, uint8_t level)
{
	return bas_update_bat_level(conn_handle,
			ble_attr_idx_to_handle(bas_attrs,
					       BLVL_LEVEL_HANDLE_IDX),
			sizeof(level), &level, NULL);
}

void ble_service_bas_set_battery_updates(bool bas_en)
{
	struct _bt_gatt_ccc *ccc = bas_attrs[BLVL_CCC_HANDLE_IDX].user_data;

	ccc->value = (bas_en) ? (ccc->value | BLE_SVC_BAS_NOTIF_SUP_ON) :
			(ccc->value & ~BLE_SVC_BAS_NOTIF_SUP_ON);
}
#endif
