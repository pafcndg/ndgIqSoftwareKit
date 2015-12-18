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
#include "cfw/cfw.h"
#include "cfw/cfw_service.h"

#include "zephyr/bluetooth/gatt.h"
#include "gatt_int.h"

#include "ble_protocol.h"
#include "services/ble_service/ble_service_gap_api.h"
#include "services/ble_service/ble_service_gatts_api.h"
#include "ble_service_core_int.h"
#include "ble_service_dis.h"
#include "infra/log.h"
#include "infra/version.h"
#include "util/misc.h"

/* Device Information Service Variables */
static struct bt_uuid16 dis_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS,
};

/* Manufacturer string */
static const struct bt_uuid16 manufacturer_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_MANUFACTURER_NAME_STRING,
};

/* Manufacturer string */
static struct bt_uuid16 model_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_MODEL_NUMBER_STRING,
};

/* Serial number string */
static const struct bt_uuid16 serial_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_SERIAL_NUMBER_STRING,
};

/* Hardware revision string */
static const struct bt_uuid16 hw_rev_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_HARDWARE_REVISION_STRING,
};

/* Firmware revision string */
static const struct bt_uuid16 fw_rev_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_FIRMWARE_REVISION_STRING,
};

/* Software revision string */
static const struct bt_uuid16 sw_rev_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_SOFTWARE_REVISION_STRING,
};

#ifdef BLE_DIS_SYSTEM_ID
/* System ID */
static const struct bt_uuid16 sys_id_uuid = {
	.type = BT_UUID_16,
	.u16 = BT_UUID_DIS_SYSTEM_ID,
};

const struct ble_dis_system_id __attribute__((weak)) dis_system_id = {
		.manufact_id = MANUFACTURER_ID,
		.org_unique_id = ORG_UNIQUE_ID,
};
#endif

static int read_string(struct bt_conn *conn,
					   const struct bt_gatt_attr *attr,
					   void *buf, uint16_t buf_len,
					   uint16_t offset, const char *value)
{
	size_t str_len = strlen(value);

	return bt_gatt_attr_read(conn, attr, buf, buf_len, offset, value,
			str_len);
}

/* The following functions should be overwritten by the application as needed.
 * They are used to init the DIS service.
 */
__attribute__((weak)) int on_dis_rd_manufacturer(struct bt_conn *conn,
						 const struct bt_gatt_attr *attr,
						 void *buf, uint16_t len,
						 uint16_t offset)
{
	const char *value = "Intel Corp.";

	return read_string(conn, attr, buf, len, offset, value);
}

__attribute__((weak)) int on_dis_rd_model(struct bt_conn *conn,
					  const struct bt_gatt_attr *attr,
					  void *buf, uint16_t len,
					  uint16_t offset)
{
	const char *value = "Curie-Default";

	return read_string(conn, attr, buf, len, offset, value);
}

__attribute__((weak)) int on_dis_rd_serial(struct bt_conn *conn,
					   const struct bt_gatt_attr *attr,
					   void *buf, uint16_t len,
					   uint16_t offset)
{
	const char *value = "0123456789abcdef0123";

	return read_string(conn, attr, buf, len, offset, value);
}

__attribute__((weak)) int on_dis_rd_hw_rev(struct bt_conn *conn,
					   const struct bt_gatt_attr *attr,
					   void *buf, uint16_t len,
					   uint16_t offset)
{
	const char *value = "1.0";

	return read_string(conn, attr, buf, len, offset, value);
}

__attribute__((weak)) int on_dis_rd_fw_rev(struct bt_conn *conn,
					   const struct bt_gatt_attr *attr,
					   void *buf, uint16_t len,
					   uint16_t offset)
{
	const char *value = "ATP1BOOT01-XXXXW0234";

	return read_string(conn, attr, buf, len, offset, value);
}

__attribute__((weak)) int on_dis_rd_sw_rev(struct bt_conn *conn,
					   const struct bt_gatt_attr *attr,
					   void *buf, uint16_t len,
					   uint16_t offset)
{
	const char *value = "0.0.1-xxxx";

	return read_string(conn, attr, buf, len, offset, value);
}

#ifdef BLE_DIS_SYSTEM_ID
/**Function for encoding a System ID.
 *
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 * @param[in]   p_sys_id           System ID to be encoded.
 */
int on_dis_rd_sys_id(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		     void *buf, uint16_t len, uint16_t offset)
{
	const struct ble_dis_system_id *value = attr->user_data;

	if (buf == NULL)
		return sizeof(*value);

	return bt_gatt_attr_read(conn, attr, buf, buf_len, offset, value,
			str_len);
}
#endif

static const struct bt_gatt_attr dis_attrs[] = {
	BT_GATT_PRIMARY_SERVICE(&dis_uuid),
	BT_GATT_CHARACTERISTIC(&manufacturer_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&manufacturer_uuid, BT_GATT_PERM_READ,
			   on_dis_rd_manufacturer, NULL, NULL),
	BT_GATT_CHARACTERISTIC(&model_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&model_uuid, BT_GATT_PERM_READ, on_dis_rd_model,
			   NULL, NULL),
	BT_GATT_CHARACTERISTIC(&serial_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&serial_uuid, BT_GATT_PERM_READ, on_dis_rd_serial,
			   NULL, NULL),
	BT_GATT_CHARACTERISTIC(&hw_rev_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&hw_rev_uuid, BT_GATT_PERM_READ, on_dis_rd_hw_rev,
			   NULL, NULL),
	BT_GATT_CHARACTERISTIC(&fw_rev_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&fw_rev_uuid, BT_GATT_PERM_READ, on_dis_rd_fw_rev,
			   NULL, NULL),
	BT_GATT_CHARACTERISTIC(&sw_rev_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&sw_rev_uuid, BT_GATT_PERM_READ, on_dis_rd_sw_rev,
			   NULL, NULL),
#ifdef BLE_DIS_SYSTEM_ID
	BT_GATT_CHARACTERISTIC(&sys_id_uuid, BT_GATT_CHRC_READ),
	BT_GATT_DESCRIPTOR(&sys_id_uuid, BT_GATT_PERM_READ, on_rd_sys_id, NULL,
			   &dis_system_id),
#endif
};

static void ble_dis_add_service_complete(struct ble_init_svc_req_msg *req);

static int handle_ble_init_service_dis(struct ble_init_svc_req_msg *msg,
				struct _ble_service_cb *p_cb)
{
	/* remember sender */
	/* if register is already ongoing, return busy */
	if (_ble_cb.svc_init_msg)
		return E_OS_ERR_BUSY;
	_ble_cb.svc_init_msg = msg;

	return bt_gatt_register((struct bt_gatt_attr *)dis_attrs,
				ARRAY_SIZE(dis_attrs));
}

int ble_init_service_dis(cfw_service_conn_t * p_service_conn,
		struct _ble_register_svc *p_reg)
{
	CFW_ALLOC_FOR_SVC(struct ble_init_svc_req_msg, msg, p_service_conn,
			  MSG_ID_BLE_INIT_SVC_REQ, 0, p_reg);

	msg->init_svc = handle_ble_init_service_dis;
	msg->init_svc_complete = ble_dis_add_service_complete;

	return cfw_send_message(msg);
}

static void ble_dis_add_service_complete(struct ble_init_svc_req_msg *req)
{
	struct ble_init_service_rsp * resp = ble_alloc_init_service_rsp(req);

	/* free initial request msg */
	cfw_msg_free(&req->header);

	/* trigger the sending of the resp message */
	cfw_send_message(resp);
}
