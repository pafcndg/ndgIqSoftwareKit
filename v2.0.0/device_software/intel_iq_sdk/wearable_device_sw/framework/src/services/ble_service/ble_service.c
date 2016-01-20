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

#include "services/ble_service/ble_service.h"

#include <assert.h>
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "ble_protocol.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_service_int.h"
#include "ble_service_gap.h"
#include "ble_service_bas.h"
#include "ble_service_dis.h"
#include "ble_service_utils.h"
#include "ble_service_test.h"
#include "infra/log.h"
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
#include "services/em_service/battery_service_api.h"
#endif

// for bt_conn
#include "conn_internal.h"

#include "zephyr/bluetooth/bluetooth.h"
#include "zephyr/bluetooth/gatt.h"
#include "gap_internal.h"
#include "gatt_internal.h"

#include "nble_driver.h"

#include "rpc.h"

#include "util/misc.h"

#define BLE_DEFAULT_NAME "CURIE-1.0"

/* Subscription magic number */
#define BLE_SUBSCRIPTION_MAGIC 0xACBD1324

struct _ble_service_cb _ble_cb = { 0 };

static void ble_bt_rdy(int err);
static void ble_client_connected(conn_handle_t *instance);
static void ble_client_disconnected(conn_handle_t * instance);

static service_t ble_service = {
	.service_id = BLE_SERVICE_ID,
	.client_connected = ble_client_connected,
	.client_disconnected = ble_client_disconnected,
};

static T_TIMER conn_params_timer = NULL;

#ifdef CONFIG_SERVICES_BLE_GATTC
struct ble_discover_int_params {
	struct bt_gatt_discover_params params; /**< Discover parameters */
	struct ble_discover_rsp *rsp;          /**< Discover response */
	struct bt_uuid uuid;                   /**< Discover UUID request */
};

struct ble_subscribe_int_params {
	struct bt_gatt_subscribe_params params; /**< Subscription parameters */
	struct cfw_message *rsp;                /**< Subscribe or unsubscribe req */
	uint32_t magic;                         /**< Magic number */
};

struct ble_get_remote_data_int_params {
	struct bt_gatt_read_params params;       /**< Read parameter */
	struct ble_get_remote_data_req *req;     /**< Read request */
	struct ble_get_remote_data_rsp *rsp;     /**< Read response */
};
#endif

static struct bt_conn_cb conn_callbacks;

static void ble_is_not_enabled_rsp(struct cfw_message *msg, ble_status_t status)
{
	struct ble_enable_rsp *resp =
	    (struct ble_enable_rsp *)cfw_alloc_rsp_msg(msg,
			    /* translate msg from req to rsp */
						(CFW_MESSAGE_ID(msg) ^ MSG_ID_BLE_BASE)
						| MSG_ID_BLE_RSP,
						sizeof(*resp));
	resp->status = status;
	cfw_send_message(resp);
}

void handle_msg_id_ble_set_name(struct cfw_message *msg)
{
	struct ble_set_name_req *req = container_of(msg, struct ble_set_name_req, header);
	struct ble_gap_service_write_params params = {0};
	int len;

	if (req->name_len > 0) {
		len = (req->name_len > BLE_MAX_DEVICE_NAME) ? BLE_MAX_DEVICE_NAME : req->name_len;
		memcpy(params.name.name_array, req->data, len);
	} else {
		len = strlen(BLE_DEFAULT_NAME);
		memcpy(params.name.name_array, BLE_DEFAULT_NAME, len);
	}

	params.attr_type = GAP_SVC_ATTR_NAME;
	params.name.authorization = 0;
	params.name.len = len;

	params.name.sec_mode = BLE_DEVICE_NAME_WRITE_PERM;

	struct ble_device_name_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_NAME_EVT,
			sizeof(*evt) + len + 1);

	memcpy(evt->device_name, params.name.name_array, len + 1);
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */

	ble_gap_service_write_req(&params, msg);
}

void on_ble_gap_service_write_rsp(const struct ble_service_write_response * p_params)
{
	struct cfw_message *msg_orig = p_params->user_data;

	if (!msg_orig)
		return;
	struct ble_rsp *resp = (void *)cfw_alloc_rsp_msg(msg_orig,
	                  MSG_ID_BLE_SET_NAME_RSP, sizeof(*resp));
	resp->status = p_params->status;
	bfree(msg_orig);
	cfw_send_message(resp);
}

static void handle_msg_id_ble_start_discover(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_stop_discover(struct cfw_message *msg)
{
}

#if defined(CONFIG_SERVICES_BLE_CENTRAL)
static void handle_msg_id_ble_connect(struct cfw_message *msg)
{
	struct ble_connect_req *req =
			container_of(msg, struct ble_connect_req, header);
	struct ble_conn_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
			MSG_ID_BLE_CONNECT_RSP, sizeof(*resp));

	resp->conn = bt_conn_create_le(&req->bd_addr, &req->conn_params);

	if (resp->conn) {
		resp->status = BLE_STATUS_SUCCESS;
	} else {
		resp->status = BLE_STATUS_ERROR;
	}
	cfw_send_message(resp);
}
#endif

static void handle_msg_id_ble_disconnect(struct cfw_message *msg)
{
	int ret = BLE_STATUS_ERROR;
	struct ble_disconnect_req *req =
			container_of(msg, struct ble_disconnect_req, header);
	struct ble_conn_rsp *resp =
			(void *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_DISCONNECT_RSP,
					sizeof(*resp));

	ret = bt_conn_disconnect(req->conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	resp->status = (ret == 0 ? BLE_STATUS_SUCCESS : BLE_STATUS_ERROR);
	resp->conn = req->conn;

	cfw_send_message(resp);
}

static void handle_msg_id_ble_conn_update(struct cfw_message *msg)
{
	struct ble_gap_connect_update_params ble_gap_connect_update;
	struct ble_conn_update_req * req =
			container_of(msg, struct ble_conn_update_req, header);
	struct ble_conn_rsp * resp =
			(void *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_CONN_UPDATE_RSP,
					sizeof(*resp));

	resp->conn = req->conn;

	ble_core_delete_conn_params_timer();

	ble_gap_connect_update.conn_handle = req->conn->handle;
	BUILD_BUG_ON(sizeof(struct ble_gap_connection_params) != sizeof(struct ble_core_gap_connection_params));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_min) != offsetof(struct ble_core_gap_connection_params, interval_min));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_max) != offsetof(struct ble_core_gap_connection_params, interval_max));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, slave_latency) != offsetof(struct ble_core_gap_connection_params, slave_latency));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, link_sup_to) != offsetof(struct ble_core_gap_connection_params, link_sup_to));
	memcpy(&ble_gap_connect_update.params, &req->conn_params, sizeof(req->conn_params));
	ble_gap_conn_update_req(&ble_gap_connect_update, resp);
}

void on_ble_gap_conn_update_rsp(const struct ble_core_response *p_params) {
	struct ble_conn_rsp *resp = p_params->user_data;

	if (!resp)
		return;

	resp->status = p_params->status;
	cfw_send_message(resp);
}

#ifdef CONFIG_SERVICES_BLE_GATTC
static void send_gatt_notification_evt(struct bt_conn *conn, uint16_t val_hdl,
		const uint8_t *data, uint8_t data_len)
{
	struct ble_notification_data_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_NOTIF_EVT,
					sizeof(*evt) + data_len);

	evt->conn = conn;
	evt->len = data_len;
	memcpy(evt->data, data, data_len);

	cfw_send_event(&evt->header);
	bfree(evt);
}

static uint8_t ble_subscribe_cb(struct bt_conn *conn, int err, void *user_data,
			const void *data, uint16_t length)
{
	struct ble_subscribe_int_params *p_params =
			container_of(user_data, struct ble_subscribe_int_params, params);

	if (NULL == data) {
		/* this is the acknowledgment of the write request */
		struct ble_subscribe_rsp *resp =
				container_of(p_params->rsp, struct ble_subscribe_rsp, header);

		resp->p_subscription = p_params;
		resp->status = err;

		cfw_send_message(resp);
	} else {
		/* notification or indication received */
		send_gatt_notification_evt(conn,
				p_params->params.value_handle,
				data, length);
	}
	return 0;
}

static void ble_unsubscribe_rsp(struct ble_unsubscribe_rsp *resp,
			ble_status_t status)
{
	resp->status = status;

	cfw_send_message(resp);
}

static void ble_subscribe_destroy(void *user_data)
{
	struct ble_subscribe_int_params *p_params = container_of(user_data,
			struct ble_subscribe_int_params, params);

	if (p_params->rsp) {
		if (p_params->rsp->m.id == MSG_ID_BLE_UNSUBSCRIBE_RSP) {
			struct ble_unsubscribe_rsp *resp =
					container_of(p_params->rsp, struct ble_unsubscribe_rsp, header);
			ble_unsubscribe_rsp(resp, BLE_STATUS_SUCCESS);
		} else {
			ble_subscribe_cb(NULL, BLE_STATUS_NOT_SUPPORTED,
					user_data, NULL, 0);
		}
	}
	bfree(p_params);
}

static void handle_msg_id_ble_subscribe(struct cfw_message *msg)
{
	int ret;
	struct ble_subscribe_int_params *p_params = balloc(sizeof(*p_params), NULL);
	struct ble_subscribe_req *req =
			container_of(msg, struct ble_subscribe_req, header);

	struct ble_subscribe_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
					MSG_ID_BLE_SUBSCRIBE_RSP, sizeof(*resp));

	p_params->params.ccc_handle = req->params.ccc_handle;
	p_params->params.value = req->params.value;
	p_params->params.value_handle = req->params.value_handle;

	p_params->params.func = ble_subscribe_cb;
	p_params->params.destroy = ble_subscribe_destroy;

	p_params->rsp = &resp->header;
	p_params->magic = BLE_SUBSCRIPTION_MAGIC;

	ret = bt_gatt_subscribe(req->params.conn, &p_params->params);

	if (ret != 0)
		ble_subscribe_destroy(&p_params->params);
}

static void handle_msg_id_ble_unsubscribe(struct cfw_message *msg)
{
	int ret;
	struct ble_unsubscribe_req *req =
			container_of(msg, struct ble_unsubscribe_req, header);
	struct ble_subscribe_int_params *p_params = req->params.p_subscription;

	struct ble_unsubscribe_rsp *resp =
					(void *)cfw_alloc_rsp_msg(msg,
					MSG_ID_BLE_UNSUBSCRIBE_RSP, sizeof(*resp));

	if (p_params && p_params->magic == BLE_SUBSCRIPTION_MAGIC) {
		p_params->rsp = &resp->header;

		ret = bt_gatt_unsubscribe(req->params.conn, &p_params->params);

		if (ret != 0)
			ble_unsubscribe_rsp(resp, ret);
	} else
		ble_unsubscribe_rsp(resp, BLE_STATUS_ERROR_PARAMETER);
}

static uint8_t ble_get_remote_data_cb(struct bt_conn *conn, int err, void *user_data,
		    const void *data, uint16_t length)
{
	struct ble_get_remote_data_int_params *p_params =
			container_of(user_data, struct ble_get_remote_data_int_params, params);
	uint16_t data_length;
	uint8_t *p_data;

	if (p_params->rsp)
		data_length = length + p_params->rsp->data_length;
	else
		data_length = length;

	struct ble_get_remote_data_rsp *resp =
	    (void *)cfw_alloc_rsp_msg(
			    &p_params->req->header,
			    MSG_ID_BLE_GET_REMOTE_DATA_RSP,
			    sizeof(*resp) + data_length);

	resp->status = err;
	resp->conn = conn;
	resp->data_length = data_length;

	p_data = resp->data;
	if (p_params->rsp) {
		memcpy(p_data, p_params->rsp->data, p_params->rsp->data_length);
		p_data += p_params->rsp->data_length;
		cfw_msg_free(&p_params->rsp->header);
	}
	memcpy(p_data, data, length);

	p_params->rsp = resp;

	return BT_GATT_ITER_CONTINUE;
}

static void ble_get_remote_data_destroy(void *user_data) {

	struct ble_get_remote_data_int_params *p_params =
			container_of(user_data, struct ble_get_remote_data_int_params, params);

	if (!p_params->rsp) {
		struct ble_get_remote_data_rsp *resp =
		    (void *)cfw_alloc_rsp_msg(
				    &p_params->req->header,
				    MSG_ID_BLE_GET_REMOTE_DATA_RSP,
				    sizeof(*resp));

		resp->status = BLE_STATUS_NOT_SUPPORTED;
		resp->conn = p_params->req->params.conn;

		p_params->rsp = resp;

	} else {
		p_params->rsp->status = BLE_STATUS_SUCCESS;
	}

	cfw_msg_free(&p_params->req->header);

	cfw_send_message(p_params->rsp);

	bfree(p_params);
}

static void handle_msg_id_ble_get_remote_data(struct cfw_message *msg)
{
	int ret;
	struct ble_get_remote_data_int_params *p_params = balloc(sizeof(*p_params), NULL);
	struct ble_get_remote_data_req *req =
			container_of(msg, struct ble_get_remote_data_req, header);

	p_params->params.offset = req->params.offset;
	p_params->params.handle = req->params.char_handle;
	p_params->params.func = ble_get_remote_data_cb;
	p_params->params.destroy = ble_get_remote_data_destroy;

	p_params->req = req;

	p_params->rsp = NULL;

	ret = bt_gatt_read(req->params.conn, &p_params->params);

	if (ret != BLE_STATUS_SUCCESS) {
		ble_get_remote_data_destroy(&p_params->params);
	}
}

static void ble_set_remote_data_cb(struct bt_conn *conn, uint8_t err, const void *data)
{
	struct ble_set_remote_data_req *req =
			container_of(data, struct ble_set_remote_data_req, data);
	struct ble_conn_rsp *resp =
	    (void *)cfw_alloc_rsp_msg(
			    &req->header,
			    MSG_ID_BLE_SET_REMOTE_DATA_RSP,
			    sizeof(*resp));

	resp->status = err;
	resp->conn = conn;

	cfw_msg_free(&req->header);
	cfw_send_message(resp);

}

static void handle_msg_id_ble_set_remote_data(struct cfw_message *msg)
{
	struct ble_set_remote_data_req * req =
			container_of(msg, struct ble_set_remote_data_req, header);
	int ret;

	if (req->params.with_resp) {
		ret = bt_gatt_write(req->params.conn, req->params.char_handle, req->params.offset,
				req->data, req->data_length, ble_set_remote_data_cb);
		if (ret != BLE_STATUS_SUCCESS)
			ble_set_remote_data_cb(req->params.conn, ret, req->data);
	} else {
		ret = bt_gatt_write_without_response(req->params.conn, req->params.char_handle,
				req->data, req->data_length, false);
		/* since write without response does not have callback,
		 * call directly
		 */
		ble_set_remote_data_cb(req->params.conn, ret, req->data);
	}
}

/**
*  @return BT_GATT_ITER_CONTINUE if should continue to the next attribute
*  or BT_GATT_ITER_STOP to stop.
*/
static uint8_t ble_discover_cb(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr,
			       struct bt_gatt_discover_params *params) {
	struct ble_discover_int_params *disc_params =
			container_of(params, struct ble_discover_int_params, params);
	struct ble_discover_rsp *old_resp = disc_params->rsp;
	struct ble_discover_rsp *new_resp;

	/* Allocate an new response with an extra attribute in the array */
	new_resp = (void *)cfw_alloc_message(sizeof(*new_resp) + ((old_resp->attr_cnt + 1) * sizeof(struct bt_gatt_attr)), NULL);

	/* Copy the previous response response (including the attributes) */
	memcpy(new_resp, old_resp, sizeof(*new_resp) + (old_resp->attr_cnt * sizeof(struct bt_gatt_attr)));
	new_resp->attrs[old_resp->attr_cnt] = *attr;
	new_resp->attr_cnt = old_resp->attr_cnt + 1;

	/* Change the new response */
	disc_params->rsp = new_resp;

	/* Free the previous response */
	cfw_msg_free(&old_resp->header);

	return BT_GATT_ITER_CONTINUE;
}

static void ble_discover_destroy(void *user_data) {

	struct ble_discover_int_params *disc_params =
			container_of(user_data, struct ble_discover_int_params, params);

	cfw_send_message(disc_params->rsp);
	bfree(disc_params);
}

static void handle_msg_id_ble_discover(struct cfw_message *msg)
{
	int ret;
	struct ble_discover_int_params *disc_params = balloc(sizeof(*disc_params), NULL);
	struct ble_discover_req *req = container_of(msg, struct ble_discover_req, header);
	struct ble_discover_rsp *resp;

	disc_params->uuid = req->params.uuid;
	disc_params->params.uuid = &disc_params->uuid;
	disc_params->params.func = ble_discover_cb;
	disc_params->params.destroy = ble_discover_destroy;
	disc_params->params.start_handle = req->params.handle_range.start_handle;
	disc_params->params.end_handle = req->params.handle_range.end_handle;
	BUILD_BUG_ON((int)BLE_GATT_DISC_PRIMARY        != (int)BT_GATT_DISCOVER_PRIMARY);
	BUILD_BUG_ON((int)BLE_GATT_DISC_SECONDARY      != (int)BT_GATT_DISCOVER_SECONDARY);
	BUILD_BUG_ON((int)BLE_GATT_DISC_INCLUDE        != (int)BT_GATT_DISCOVER_INCLUDE);
	BUILD_BUG_ON((int)BLE_GATT_DISC_CHARACTERISTIC != (int)BT_GATT_DISCOVER_CHARACTERISTIC);
	BUILD_BUG_ON((int)BLE_GATT_DISC_DESCRIPTOR     != (int)BT_GATT_DISCOVER_DESCRIPTOR);
	disc_params->params.type = req->params.type;

	resp = (void *)cfw_alloc_rsp_msg(
			    &req->header,
			    MSG_ID_BLE_DISCOVER_RSP,
			    sizeof(*resp));
	disc_params->rsp = resp;

	ret = bt_gatt_discover(req->params.conn, &disc_params->params);

	if (ret < 0) {
		resp->status = BLE_STATUS_ERROR;
		ble_discover_destroy(&disc_params->params);
	}
}
#endif

struct ble_init_service_rsp *ble_alloc_init_service_rsp(
		struct ble_init_svc_req *p_req)
{
	struct ble_init_service_rsp *resp =
			(void *)cfw_alloc_rsp_msg(&p_req->header,
				MSG_ID_BLE_INIT_SVC_RSP, sizeof(*resp));

	resp->status = p_req->status;

	return resp;
}

void handle_ble_add_service_failure(struct ble_init_svc_req *req,
		uint16_t status)
{
	struct ble_init_service_rsp *resp =
			ble_alloc_init_service_rsp(req);
	resp->status = (BLE_STATUS_SUCCESS != status) ? status :
			BLE_STATUS_ERROR;
	cfw_send_message(resp);
}


static void handle_msg_id_ble_init_svc(struct cfw_message *msg, struct _ble_service_cb * p_cb)
{
	struct ble_init_svc_req *req = container_of(msg, struct ble_init_svc_req, header);

	int ret = E_OS_ERR_UNKNOWN; /* use negative values for errors */

	if (!req->init_svc)
		panic(-1);

	ret = req->init_svc(req, p_cb);

	if (0 > ret)
		handle_ble_add_service_failure(req,
				(ret == E_OS_ERR_NOT_ALLOWED) ?
				BLE_STATUS_ALREADY_REGISTERED :
				BLE_STATUS_WRONG_STATE);
}

static void handle_msg_id_ble_stop_adv(struct cfw_message *msg)
{
	struct ble_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
				MSG_ID_BLE_STOP_ADV_RSP, sizeof(*resp));

	resp->status = bt_le_adv_stop();
	cfw_send_message(resp);
}

static void handle_msg_id_ble_get_security(struct cfw_message *msg)
{
	struct ble_get_security_status_req *req =
			container_of(msg, struct ble_get_security_status_req, header);

	struct ble_get_security_rsp *resp = (void *)cfw_alloc_rsp_msg(
				&req->header,
				MSG_ID_BLE_GET_SECURITY_RSP,
				sizeof(*resp));
	resp->op_code = req->op_code;
	switch (req->op_code) {
	case BLE_SEC_DEVICE_STATUS:
		/* currently we make the assumption that security_state (bits 0
		 * -6) map onto BLE_SEC_DEVICE_STATUS */
		resp->dev_status = (_ble_cb.security_state & 0x7f);
		break;
	case BLE_SEC_BONDING_DB_STATE:
		/* define internal security state masks? */
		if (_ble_cb.security_state  & BLE_SEC_ST_BONDED_DEVICES_AVAIL) {
			resp->dev_status = BLE_SEC_ST_BONDED_DEVICES_AVAIL;
		} else {
			resp->dev_status = BLE_SEC_ST_NO_BONDED_DEVICES;
		}
		break;
	default:
		resp->status = BLE_STATUS_NOT_SUPPORTED;
		break;
	}
	cfw_send_message(resp);
}

static void handle_msg_id_ble_set_security(struct cfw_message *msg)
{
	struct ble_set_security_status_req *req =
			container_of(msg, struct ble_set_security_status_req, header);

	struct ble_set_security_rsp *resp =
			(void *)cfw_alloc_rsp_msg(
				(struct cfw_message *)req,
				MSG_ID_BLE_SET_SECURITY_RSP,
				sizeof(*resp));
	resp->op_code = req->op_code;
	switch (req->op_code) {
	case BLE_SEC_DEVICE_STATUS:
		if (req->params.dev_status & BLE_SEC_ST_PAIRABLE) {
			_ble_cb.security_state |= (BLE_SEC_ST_PAIRABLE & 0xff);
			resp->dev_status = BLE_SEC_ST_PAIRABLE;
		} else if (req->params.dev_status & BLE_SEC_ST_NO_BONDED_DEVICES) {
			_ble_cb.security_state =
					(BLE_SEC_ST_NO_BONDED_DEVICES | BLE_SEC_ST_PAIRABLE);
			resp->dev_status = BLE_SEC_ST_PAIRABLE;
		} else {
			/* not safe but assume otherwise disable pairing */
			_ble_cb.security_state &= (~BLE_SEC_ST_PAIRABLE & 0xff);
			/* dev_status is 0 as cfw_alloc_rsp_msg() does set all to 0*/
		}
		break;
	default:
		resp->status = BLE_STATUS_NOT_SUPPORTED;
		break;
	}
	cfw_send_message(resp);
}

static void handle_msg_id_ble_passkey_send(struct cfw_message *msg,
		struct _ble_service_cb *p_cb)
{
	struct ble_gap_sm_key_reply_req *req = container_of(msg, struct ble_gap_sm_key_reply_req, header);
	struct ble_gap_sm_key_reply_req_params ble_gap_sm_key_reply;

	BUILD_BUG_ON((int)BLE_GAP_SM_PK_NONE    != BLE_CORE_GAP_SM_PK_NONE);
	BUILD_BUG_ON((int)BLE_GAP_SM_PK_PASSKEY != BLE_CORE_GAP_SM_PK_PASSKEY);
	BUILD_BUG_ON((int)BLE_GAP_SM_PK_OOB     != BLE_CORE_GAP_SM_PK_OOB);

	BUILD_BUG_ON(sizeof(struct ble_gap_sm_passkey) != sizeof(struct ble_core_gap_sm_passkey));
	BUILD_BUG_ON(offsetof(struct ble_gap_sm_passkey, type) != offsetof(struct ble_core_gap_sm_passkey, type));
	BUILD_BUG_ON(offsetof(struct ble_gap_sm_passkey, passkey) != offsetof(struct ble_core_gap_sm_passkey, passkey));
	BUILD_BUG_ON(offsetof(struct ble_gap_sm_passkey, oob) != offsetof(struct ble_core_gap_sm_passkey, oob));
	memcpy(&ble_gap_sm_key_reply.params, &req->params, sizeof(req->params));
	ble_gap_sm_key_reply.conn_handle = req->conn->handle;

	ble_gap_sm_passkey_reply_req(&ble_gap_sm_key_reply, req);
}

static void handle_msg_id_ble_clear_bonds(struct cfw_message *msg)
{
	struct ble_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
			MSG_ID_BLE_CLEAR_BONDS_RSP, sizeof(*resp));
	ble_gap_sm_clear_bonds_req(resp);
}

static void handle_msg_id_ble_get_version(struct cfw_message *msg)
{
	struct ble_version_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
					MSG_ID_BLE_GET_VERSION_RSP, sizeof(*resp));

	ble_get_version_req(resp);
}

void on_ble_get_version_rsp(const struct ble_version_response *p_params) {
	struct ble_version_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = 0;
	resp->version = p_params->version;
	cfw_send_message(resp);
}

static void handle_msg_id_ble_get_info_req(struct cfw_message *msg)
{
	struct ble_gatts_get_attribute_params params;
	struct ble_gap_get_info_req * req =
			container_of(msg, struct ble_gap_get_info_req, header);
	if (req->info_type == BLE_INFO_BDA_NAME_REQ) {
#ifdef LINUX_HOST_RUNTIME
		params.value_handle = 0;
#else
		params.value_handle = ble_service_gap_device_name_handle();
#endif
	}
	ble_gatts_get_attribute_value_req(&params, msg);
}

void on_ble_gatts_get_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params,
		uint8_t * p_value, uint8_t length)
{
	struct cfw_message *req = p_params->p_priv;

	if (req) {
		if (req->m.id == MSG_ID_BLE_GET_INFO_REQ) {
			struct ble_get_info_rsp *resp = (void *)cfw_alloc_rsp_msg(
					req,
					MSG_ID_BLE_GET_INFO_RSP,
					sizeof(*resp) + length + 1);

			bfree(req);
			resp->status = p_params->status;
			memcpy(resp->info_params.bda_name_params.name, p_value, length);
			resp->info_params.bda_name_params.name_len = length;
			resp->info_params.bda_name_params.name[length] = 0;

			if (BLE_STATUS_SUCCESS == p_params->status) {
				/* Don't send response yet we need BD address */
				ble_gap_read_bda_req(resp);
			} else {
				/* error case */
				cfw_send_message(resp);
			}
		}
	}
}

static void handle_msg_id_ble_update_data(struct cfw_message *msg,
		struct _ble_service_cb * p_cb)
{
	int ret = BLE_STATUS_NOT_SUPPORTED;
	struct ble_update_data_req *req =
			container_of(msg, struct ble_update_data_req, header);
	struct ble_update_data_rsp *resp = (void *)
			cfw_alloc_rsp_msg(msg, MSG_ID_BLE_UPDATE_DATA_RSP,
					sizeof(*resp));
	resp->status = ret;
	resp->conn = req->conn;
	resp->char_handle = req->char_handle;

	assert(req->update);

	ret = req->update(req->conn, req->char_handle, req->len, req->data, resp);

	if (ret != BLE_STATUS_SUCCESS) {
		resp->status = ret;
		cfw_send_message(resp);
	}
}

void on_ble_gatts_set_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params) {

	struct ble_update_data_rsp *resp = p_params->p_priv;

	/* Check if the request message required to forward a response message */
	if (!resp)
		return;

	resp->status = p_params->status;
	if (resp->char_handle != p_params->value_handle)
		pr_warning(LOG_MODULE_BLE, "gatts_set_val_rsp: hdl err (%d,%d)",
				resp->char_handle,
				p_params->value_handle);
	cfw_send_message(resp);
}

static void handle_msg_id_ble_dtm(struct cfw_message *msg)
{
	handle_ble_dtm_cmd(msg);
}

static void handle_msg_id_ble_set_rssi_report_req(struct cfw_message *msg)
{
	struct ble_gap_set_rssi_report_req *req =
			container_of(msg, struct ble_gap_set_rssi_report_req, header);
	struct ble_core_rssi_report_params core_params;

	struct ble_conn_rsp *resp = (void *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_RSSI_RSP,
					sizeof(*resp));

	BUILD_BUG_ON(BLE_GAP_RSSI_DISABLE_REPORT ^ BLE_CORE_GAP_RSSI_DISABLE_REPORT);
	BUILD_BUG_ON(BLE_GAP_RSSI_ENABLE_REPORT ^ BLE_CORE_GAP_RSSI_ENABLE_REPORT);
	resp->status = BLE_STATUS_SUCCESS;
	resp->conn = req->conn;

	core_params.conn_handle = resp->conn->handle;
	core_params.op          = req->params.op;
	core_params.delta_dBm   = req->params.delta_dBm;      /**< minimum RSSI dBm change to report a new RSSI value */
	core_params.min_count   = req->params.min_count;      /**< number of delta_dBm changes before sending a new RSSI report */
	ble_gap_set_rssi_report_req(&core_params, resp);
}

void on_ble_gap_set_rssi_report_rsp(const struct ble_core_response *p_params)
{
	struct ble_conn_rsp *resp = p_params->user_data;

	resp->status = p_params->status;
	cfw_send_message(resp);
}

#ifdef CONFIG_TCMD_BLE_DEBUG
static void handle_msg_id_ble_dbg(struct cfw_message *msg)
{
	struct debug_params params;
	struct ble_dbg_req_rsp *resp = (void *)
		cfw_alloc_rsp_msg(msg, MSG_ID_BLE_DBG_RSP, sizeof(*resp));
	struct ble_dbg_req_rsp* req = (struct ble_dbg_req_rsp*) msg;

	params.u0 = req->u0;
	params.u1 = req->u1;

	ble_gap_dbg_req(&params, resp);
}
#endif // CONFIG_TCMD_BLE_DEBUG

void on_ble_gap_dbg_rsp(const struct debug_response * p_params)
{
	struct ble_dbg_req_rsp *resp = p_params->user_data;
	if (!resp)
		return;
	resp->u0 = p_params->u0;
	resp->u1 = p_params->u1;
	cfw_send_message(resp);
}

static void handle_msg_id_ble_rpc_callin(struct message *msg, void *priv)
{
	struct ble_rpc_callin *rpc = container_of(msg, struct ble_rpc_callin, msg);
	/* handle incoming message */
	rpc_deserialize(rpc->p_data, rpc->len);
	bfree(rpc->p_data);
	message_free(msg);
}

static void ble_core_resume_enable(struct ble_enable_req *req, struct _ble_service_cb *p_cb)
{
	struct ble_wr_config_params config;

	config.bda_present = req->bda_present;
	memcpy(&config.bda, &req->bda, sizeof(req->bda));
	config.tx_power = 127;

	BUILD_BUG_ON(sizeof(struct ble_gap_connection_params) != sizeof(struct ble_core_gap_connection_params));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_min) != offsetof(struct ble_core_gap_connection_params, interval_min));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_max) != offsetof(struct ble_core_gap_connection_params, interval_max));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, slave_latency) != offsetof(struct ble_core_gap_connection_params, slave_latency));
	BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, link_sup_to) != offsetof(struct ble_core_gap_connection_params, link_sup_to));
	memcpy(&config.central_conn_params, &req->central_conn_params, sizeof(req->central_conn_params));

	ble_set_enable_config_req(&config, req);
}

void on_ble_set_enable_config_rsp(const struct ble_core_response *p_params) {

	struct ble_enable_req *msg_orig = p_params->user_data;

	if (!msg_orig)
		return;

	struct ble_enable_rsp *resp = (void *)cfw_alloc_rsp_msg(&msg_orig->header,
			    MSG_ID_BLE_ENABLE_RSP, sizeof(*resp));
	resp->status = p_params->status;

	if (BLE_STATUS_SUCCESS == resp->status) {
		resp->enable = 1;
		/* make sure this function is called once, at boot time */
		bt_conn_cb_register(&conn_callbacks);

		BUILD_BUG_ON(sizeof(struct ble_core_gap_sm_config_params) != sizeof(struct ble_gap_sm_config_params));
		BUILD_BUG_ON(offsetof(struct ble_core_gap_sm_config_params, options) != offsetof(struct ble_gap_sm_config_params, options));
		BUILD_BUG_ON(offsetof(struct ble_core_gap_sm_config_params, io_caps) != offsetof(struct ble_gap_sm_config_params, io_caps));
		BUILD_BUG_ON(offsetof(struct ble_core_gap_sm_config_params, key_size) != offsetof(struct ble_gap_sm_config_params, key_size));
		BUILD_BUG_ON((int)BLE_GAP_BONDING != BLE_CORE_GAP_BONDING);
		BUILD_BUG_ON((int)BLE_GAP_MITM    != BLE_CORE_GAP_MITM);
		BUILD_BUG_ON((int)BLE_GAP_OOB     != BLE_CORE_GAP_OOB);
		ble_gap_sm_config_req((const struct ble_core_gap_sm_config_params *)&msg_orig->sm_config, resp);
	} else {
		// error case
		resp->enable = 0;
		cfw_send_message(resp);
	}
	bfree(msg_orig);
}

static void handle_ble_enable(struct ble_enable_req *req,
		struct _ble_service_cb *p_cb)
{
	pr_info(LOG_MODULE_BLE, "ble_enable: state %d, options:"
			"0x%x", p_cb->ble_state, req->options);

	switch (req->options) {
	case BLE_OPTION_NONE:
		if (p_cb->ble_state == BLE_ST_DISABLED)
			p_cb->conn = NULL;
		p_cb->ble_state = BLE_ST_ENABLED;
		break;
	case BLE_OPTION_TEST_MODE:
		handle_ble_dtm_init(req);
		p_cb->ble_state = BLE_ST_DTM;
		return;
		break;
	}

	ble_core_resume_enable(req, p_cb);
}

static void handle_ble_disable(struct ble_enable_req *req, struct _ble_service_cb *p_cb)
{
	struct ble_enable_rsp *resp;

	pr_debug(LOG_MODULE_BLE, "ble_disable");
	p_cb->ble_state = BLE_ST_DISABLED;

	if (p_cb->conn) {
		struct ble_gap_disconnect_req_params ble_gap_disconnect;

		ble_gap_disconnect.conn_handle = p_cb->conn->handle;
		ble_gap_disconnect.reason = BT_HCI_ERR_REMOTE_USER_TERM_CONN;
		ble_gap_disconnect_req(&ble_gap_disconnect, NULL);
	}

	/* send MSG_ID_BLE_ENABLE_RSP on advertise disable complete in order to
	 * cproxy_disconnect */
	resp = (void *)cfw_alloc_rsp_msg(&req->header,
				MSG_ID_BLE_ENABLE_RSP,
				sizeof(*resp));

	ble_gap_stop_advertise_req(resp);
}

static void ble_service_message_handler(struct cfw_message *msg, void *param)
{
	bool free_msg = true;
	struct _ble_service_cb *p_cb = param;
	uint16_t msg_id = CFW_MESSAGE_ID(msg);

	if (p_cb->ble_state < BLE_ST_ENABLED &&
	    msg_id != MSG_ID_CFW_OPEN_SERVICE_REQ &&
	    msg_id != MSG_ID_BLE_ENABLE_REQ) {
		ble_is_not_enabled_rsp(msg, BLE_STATUS_NOT_ENABLED);
		goto out;
	}

	switch (msg_id) {
	case MSG_ID_CFW_OPEN_SERVICE_REQ:
		break;
	case MSG_ID_BLE_ENABLE_REQ: {
		struct ble_enable_req *req =
			container_of(msg, struct ble_enable_req, header);
		if (p_cb->ble_state) {
			if (req->enable) {
				handle_ble_enable(req, p_cb);
				free_msg = false;
			} else
				handle_ble_disable(req, p_cb);
		} else {
			pr_debug(LOG_MODULE_BLE, "ble_hdl_msg: core service not opened!");
			/* core service is not yet up */
			struct ble_enable_rsp *resp = (void *)cfw_alloc_rsp_msg(msg,
				MSG_ID_BLE_ENABLE_RSP, sizeof(*resp));
			resp->status = BLE_STATUS_PENDING;
			resp->enable = 0;
			cfw_send_message(resp);
		}
	}
		break;
	case MSG_ID_BLE_SET_NAME_REQ:
		handle_msg_id_ble_set_name(msg);
		free_msg = false;
		break;
	case MSG_ID_BLE_START_DISCOVER_REQ:
		handle_msg_id_ble_start_discover(msg);
		break;
	case MSG_ID_BLE_STOP_DISCOVER_REQ:
		handle_msg_id_ble_stop_discover(msg);
		break;
#ifdef CONFIG_SERVICES_BLE_CENTRAL
	case MSG_ID_BLE_CONNECT_REQ:
		handle_msg_id_ble_connect(msg);
		break;
#endif
	case MSG_ID_BLE_DISCONNECT_REQ:
		handle_msg_id_ble_disconnect(msg);
		break;
	case MSG_ID_BLE_CONN_UPDATE_REQ:
		handle_msg_id_ble_conn_update(msg);
		break;
#ifdef CONFIG_SERVICES_BLE_GATTC
	case MSG_ID_BLE_SUBSCRIBE_REQ:
		handle_msg_id_ble_subscribe(msg);
		break;
	case MSG_ID_BLE_UNSUBSCRIBE_REQ:
		handle_msg_id_ble_unsubscribe(msg);
		break;
	case MSG_ID_BLE_GET_REMOTE_DATA_REQ:
		handle_msg_id_ble_get_remote_data(msg);
		free_msg = false;
		break;
	case MSG_ID_BLE_SET_REMOTE_DATA_REQ:
		handle_msg_id_ble_set_remote_data(msg);
		free_msg = false;
		break;
	case MSG_ID_BLE_DISCOVER_REQ:
		handle_msg_id_ble_discover(msg);
		break;
#endif
	case MSG_ID_BLE_INIT_SVC_REQ:
		handle_msg_id_ble_init_svc(msg, p_cb);
		break;
	case MSG_ID_BLE_STOP_ADV_REQ:
		handle_msg_id_ble_stop_adv(msg);
		break;
	case MSG_ID_BLE_GET_SECURITY_REQ:
		handle_msg_id_ble_get_security(msg);
		break;
	case MSG_ID_BLE_SET_SECURITY_REQ:
		handle_msg_id_ble_set_security(msg);
		break;
	case MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ:
		handle_msg_id_ble_passkey_send(msg, p_cb);
		break;
	case MSG_ID_BLE_CLEAR_BONDS_REQ:
		handle_msg_id_ble_clear_bonds(msg);
		break;
	case MSG_ID_BLE_GET_VERSION_REQ:
		handle_msg_id_ble_get_version(msg);
		break;
	case MSG_ID_BLE_UPDATE_DATA_REQ:
		handle_msg_id_ble_update_data(msg, p_cb);
		break;
	case MSG_ID_BLE_DTM_REQ:
		handle_msg_id_ble_dtm(msg);
		break;
	case MSG_ID_BLE_RSSI_REQ:
		handle_msg_id_ble_set_rssi_report_req(msg);
		break;
	case MSG_ID_BLE_GET_INFO_REQ:
		handle_msg_id_ble_get_info_req(msg);
		free_msg = false;
		break;
#ifdef CONFIG_TCMD_BLE_DEBUG
	case MSG_ID_BLE_DBG_REQ:
		handle_msg_id_ble_dbg(msg);
		break;
#endif
	default:
		pr_warning(LOG_MODULE_BLE, "unsupported %d", msg_id);
		break;
	}
out:
	if (free_msg)
		cfw_msg_free(msg);
}

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
static void handle_msg_id_batt_svc_level_updated(bs_listen_evt_msg_t *evt,
		struct _ble_service_cb * p_cb)
{
	uint8_t bat_level = evt->bs_evt_content.bat_soc;

	handle_ble_update_service_bas(p_cb->conn, bat_level);
}
#endif

static void ble_service_battery_evt_handler(struct cfw_message *msg, void *param)
{
	struct _ble_service_cb *p_cb = param;
	uint16_t msg_id = CFW_MESSAGE_ID(msg);

	switch (msg_id) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		cfw_open_conn_rsp_msg_t *evt = container_of(msg, cfw_open_conn_rsp_msg_t, rsp_header.header);
		if ((void *)BATTERY_SERVICE_ID == msg->priv) {
			p_cb->p_bat_service_conn = evt->service_conn;
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
			int events[] = { MSG_ID_BATT_SVC_LEVEL_UPDATED_EVT };
			cfw_register_events(p_cb->p_bat_service_conn, events,
					sizeof(events)/sizeof(int), p_cb);
			bs_get_battery_info(p_cb->p_bat_service_conn, BS_CMD_BATT_LEVEL, p_cb);
#endif
		}
		break;
		}
	case MSG_ID_CFW_REGISTER_SVC_AVAIL_EVT_RSP:
	case MSG_ID_CFW_REGISTER_EVT_RSP:
		break;
	case MSG_ID_CFW_SVC_AVAIL_EVT: {
		cfw_svc_available_evt_msg_t *evt = container_of(msg, cfw_svc_available_evt_msg_t, header);
		if (evt->service_id == BATTERY_SERVICE_ID)
			cfw_open_service_conn(p_cb->client, BATTERY_SERVICE_ID,
					(void *)BATTERY_SERVICE_ID);
		break;
		}
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
	case MSG_ID_BATT_SVC_LEVEL_UPDATED_EVT:
		handle_msg_id_batt_svc_level_updated(container_of(msg, bs_listen_evt_msg_t, header), p_cb);
		break;
	case MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP: {
		struct bs_get_status_batt_rsp_msg *evt =
				container_of(msg, struct bs_get_status_batt_rsp_msg, rsp_header.header);
		uint8_t bat_level = evt->bs_get_soc.bat_soc;
		if (E_OS_OK != handle_ble_update_service_bas(p_cb->conn, bat_level))
			pr_error(LOG_MODULE_BLE, "ble_srvc_bas_lvl_upd: failed");
		break;
		}
#endif
	default:
		cfw_print_default_handle_error_msg(LOG_MODULE_BLE, msg_id);
		break;
	}
	cfw_msg_free(msg);
}

static void ble_client_connected(conn_handle_t *instance)
{
	if (_ble_cb.ble_state == BLE_ST_NOT_READY)
		pr_warning(LOG_MODULE_BLE, "BLE_CORE service is not registered");
}

static void ble_client_disconnected(conn_handle_t * instance)
{
}

void ble_cfw_service_init(T_QUEUE queue)
{
	_ble_cb.queue = queue;
	_ble_cb.client = cfw_client_init(queue, ble_service_battery_evt_handler, &_ble_cb);
	_ble_cb.ble_state = BLE_ST_NOT_READY;

#ifdef CONFIG_IPC_UART_NS16550
	nble_driver_configure(queue, handle_msg_id_ble_rpc_callin);
#endif

	bt_enable(ble_bt_rdy);

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
	ble_service_bas_set_battery_updates(true);
#endif
}

static void ble_bt_rdy(int err)
{
	_ble_cb.ble_state = BLE_ST_DISABLED;

	/* register BLE service */
	if (cfw_register_service(_ble_cb.queue, &ble_service,
			ble_service_message_handler, &_ble_cb) == -1) {
		pr_error(LOG_MODULE_BLE, "BLE register failed");
	}
}

void on_ble_gap_sm_config_rsp(struct ble_gap_sm_config_rsp *p_params)
{
	if (!p_params)
		return;

	BUILD_BUG_ON(BLE_GAP_SM_ST_BONDING_COMPLETE ^ BLE_CORE_GAP_SM_ST_BONDING_COMPLETE);
	if (p_params->status == BLE_STATUS_SUCCESS) {
		if (p_params->state == BLE_GAP_SM_ST_BONDING_COMPLETE)
			_ble_cb.security_state = BLE_SEC_ST_BONDED_DEVICES_AVAIL;
		else
			_ble_cb.security_state = BLE_SEC_ST_NO_BONDED_DEVICES;

		p_params->state = _ble_cb.security_state;
		ble_gap_read_bda_req(p_params->user_data);
	} else {
		pr_error(LOG_MODULE_BLE, "sm_config failed");
		cfw_send_message(p_params->user_data);
	}
}

void ble_log(const struct ble_log_s *p_param, char *p_buf, uint8_t buflen)
{
	pr_info(LOG_MODULE_BLE, p_buf, p_param->param0, p_param->param1, p_param->param2, p_param->param3);
}

void on_ble_get_bonded_device_list_rsp(const struct ble_get_bonded_device_list_rsp* p_params)
{
	struct cfw_message *rsp = p_params->p_priv;

	if (NULL != rsp) {
		struct ble_get_info_rsp *r = container_of(rsp, struct ble_get_info_rsp, header);

		BUILD_BUG_ON(sizeof(struct ble_bonded_devices) != sizeof(struct ble_core_bonded_devices));
#ifdef CONFIG_TCMD_BLE_DEBUG
		BUILD_BUG_ON(offsetof(struct ble_bonded_devices, addrs) != offsetof(struct ble_core_bonded_devices, addrs));
#endif
		BUILD_BUG_ON(offsetof(struct ble_bonded_devices, addr_count) != offsetof(struct ble_core_bonded_devices, addr_count));
		memcpy(&r->info_params.bda_name_params.bonded_devs, &p_params->bonded_dev, sizeof(p_params->bonded_dev));
		cfw_send_message(rsp);
	}
}

void on_ble_gap_generic_cmd_rsp(const struct ble_core_response * p_params) {
	void *resp = p_params->user_data;
	if (resp)
		cfw_send_message(resp);
}

void on_ble_gap_dtm_init_rsp(void *user_data)
{
	struct ble_enable_rsp *p_rsp = user_data;
	p_rsp->status = 0;
	if (BLE_STATUS_SUCCESS == p_rsp->status) {
		p_rsp->enable = 1;
#ifdef CONFIG_SERVICES_BLE_IMPL
		ble_test_reconfigure_transport();
#endif
	} else {
		p_rsp->enable = 0;
	}
	cfw_send_message(p_rsp);
}

void on_ble_gap_disconnect_rsp(const struct ble_core_response * p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;

	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_start_advertise_rsp(const struct ble_core_response *p_params)
{
	struct ble_rsp *resp = p_params->user_data;

	if (resp) {
		resp->status = p_params->status;
		cfw_send_message(resp);
	}
	if (p_params->status != BLE_STATUS_SUCCESS)
		pr_warning(LOG_MODULE_BLE, "start advertise failed with 0x%x",
				p_params->status);
}

void on_ble_gap_stop_advertise_rsp(const struct ble_core_response * p_params)
{
	struct ble_rsp *resp = p_params->user_data;

	if (resp) {
		/* In case of Higher layer API call return the status.
		 * in case of ble disable, ignore result */
		if (resp->header.m.id == MSG_ID_BLE_STOP_ADV_RSP)
			resp->status = p_params->status;
		cfw_send_message(resp);
	}
}

void on_ble_gap_sm_clear_bonds_rsp(const struct ble_core_response * p_params) {
	struct ble_rsp *resp = p_params->user_data;

	resp->status = p_params->status;
	cfw_send_message(resp);
}

void ble_core_delete_conn_params_timer(void)
{
	OS_ERR_TYPE os_err;

	/* Destroy the timer */
	timer_delete(conn_params_timer, &os_err);

	/* Make sure that the reference to the timer is lost */
	conn_params_timer = NULL;
}

static void conn_params_timer_handler(void * privData)
{
	OS_ERR_TYPE os_err;
	struct _ble_service_cb * p_cb = privData;
	struct ble_gap_connect_update_params ble_gap_connect_update;

	/* Check if there was an update in the connection parameters */
	if (p_cb->role != BT_HCI_ROLE_MASTER &&
	    (p_cb->conn_values.latency != p_cb->peripheral_conn_params.slave_latency ||
	     p_cb->conn_values.supervision_to != p_cb->peripheral_conn_params.link_sup_to ||
	     p_cb->conn_values.interval < p_cb->peripheral_conn_params.interval_min ||
	     p_cb->conn_values.interval > p_cb->peripheral_conn_params.interval_max)) {

		/* Start the 60 seconds timer to retry updating the connection */
		timer_start(conn_params_timer, 60000, &os_err);
		ble_gap_connect_update.conn_handle = p_cb->conn->handle;

		BUILD_BUG_ON(sizeof(struct ble_gap_connection_params) != sizeof(struct ble_core_gap_connection_params));
		BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_min) != offsetof(struct ble_core_gap_connection_params, interval_min));
		BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, interval_max) != offsetof(struct ble_core_gap_connection_params, interval_max));
		BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, slave_latency) != offsetof(struct ble_core_gap_connection_params, slave_latency));
		BUILD_BUG_ON(offsetof(struct ble_gap_connection_params, link_sup_to) != offsetof(struct ble_core_gap_connection_params, link_sup_to));
		memcpy(&ble_gap_connect_update.params, &p_cb->peripheral_conn_params, sizeof(p_cb->peripheral_conn_params));

		/* Send request to update the connection */
		ble_gap_conn_update_req(&ble_gap_connect_update, NULL);
	}
	else
		ble_core_delete_conn_params_timer();
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
	struct _ble_service_cb *p_cb = &_ble_cb;
	OS_ERR_TYPE os_err;

	p_cb->conn = conn;
	p_cb->conn_values.interval = conn->le.interval;
	p_cb->conn_values.latency = conn->le.latency;
	p_cb->conn_values.supervision_to = conn->le.timeout;
	/* BT_HCI_ROLE_MASTER/BT_HCI_ROLE_SLAVE */
	p_cb->role = conn->role;
	if (conn->role != BT_HCI_ROLE_MASTER) {
		bt_conn_ref(conn);
	}

	/* Stop timer and free timer */
	ble_delete_adv_timer();

	struct ble_connect_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_CONNECT_EVT,
			sizeof(*evt));
	if (!err) {
		evt->status = BLE_STATUS_SUCCESS;

		/* If peripheral and connection values are not compliant with the PPCP */
		if (p_cb->role != BT_HCI_ROLE_MASTER &&
		    (p_cb->conn_values.latency != p_cb->peripheral_conn_params.slave_latency ||
		     p_cb->conn_values.supervision_to != p_cb->peripheral_conn_params.link_sup_to ||
		     p_cb->conn_values.interval < p_cb->peripheral_conn_params.interval_min ||
		     p_cb->conn_values.interval > p_cb->peripheral_conn_params.interval_max)) {
			/* Start a timer to configure the parameters */
			conn_params_timer = timer_create(conn_params_timer_handler, p_cb, 5000,
					false, true, &os_err);
		}

	}
	else {
		evt->status = BLE_STATUS_ERROR;
		bt_conn_ref(conn);
	}
	evt->conn = conn;
	evt->role = conn->role;
	evt->ble_addr = conn->le.dst;
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	struct _ble_service_cb * p_cb = &_ble_cb;

	p_cb->conn = NULL;
	bt_conn_unref(conn);

	ble_core_delete_conn_params_timer();

	/* Do not send event if already disabled */
	if (p_cb->ble_state == BLE_ST_DISABLED) {
		pr_warning(LOG_MODULE_BLE, "ble_gap_disc_evt: BLE"
				" already disabled, hci_reason 0x%x",
				reason);
		return;
	}

	struct ble_disconnect_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_DISCONNECT_EVT,
					sizeof(*evt));
	evt->status = BLE_STATUS_SUCCESS;
	evt->conn = conn;
	evt->reason = reason;
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */

}

static struct bt_conn_cb conn_callbacks = {
		.connected = on_connected,
		.disconnected = on_disconnected,
};

void on_ble_gap_sm_passkey_display_evt(const struct ble_gap_sm_passkey_disp_evt *p_evt)
{
	struct ble_security_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	struct bt_conn *conn;

	conn = bt_conn_lookup_handle(p_evt->conn_handle);
	if (conn) {
		bt_conn_unref(conn);

		evt->conn = conn;
		evt->sm_status = BLE_SM_AUTH_DISP_PASSKEY;
		memcpy(evt->passkey, p_evt->passkey, BLE_PASSKEY_LEN);

		cfw_send_event(&evt->header);
	}
	bfree(evt); /* message has been cloned by cfw_send_event */
}


void on_ble_gap_sm_status_evt(const struct ble_gap_sm_status_evt * p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;

	struct ble_security_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	struct bt_conn *conn;

	BUILD_BUG_ON(BLE_GAP_SM_ST_START_PAIRING ^ BLE_CORE_GAP_SM_ST_START_PAIRING);
	BUILD_BUG_ON(BLE_GAP_SM_ST_BONDING_COMPLETE ^ BLE_CORE_GAP_SM_ST_BONDING_COMPLETE);
	BUILD_BUG_ON(BLE_GAP_SM_ST_LINK_ENCRYPTED ^ BLE_CORE_GAP_SM_ST_LINK_ENCRYPTED);
	BUILD_BUG_ON(BLE_GAP_SM_ST_SECURITY_UPDATE ^ BLE_CORE_GAP_SM_ST_SECURITY_UPDATE);
	conn = bt_conn_lookup_handle(p_evt->conn_handle);
	if (conn) {
		bt_conn_unref(conn);

		evt->conn = conn;

		switch (p_evt->status) {
		case BLE_GAP_SM_ST_START_PAIRING:
			/* pairing/bonding started */
			evt->sm_status = BLE_SM_PAIRING_START;
			p_cb->security_state &= ~(BLE_SEC_ST_BONDING_SUCCESS |
					BLE_SEC_ST_PAIRING_SUCCESS |
					BLE_SEC_ST_PAIRING_FAILED);
			p_cb->security_state |= BLE_SEC_ST_PAIRING_IN_PROG;
			break;
		case BLE_GAP_SM_ST_BONDING_COMPLETE:
			/* pairing/bonding complete, assuming bonding only */
			evt->sm_status = BLE_SM_BONDING_COMPLETE;
			evt->gap_status = p_evt->gap_status;
			p_cb->security_state &= ~BLE_SEC_ST_PAIRING_IN_PROG;

			/* bonding success */
			if (!p_evt->gap_status) {
				p_cb->security_state |= (BLE_SEC_ST_BONDING_SUCCESS |
						BLE_SEC_ST_BONDED_DEVICES_AVAIL);
			} else {
				p_cb->security_state |= BLE_SEC_ST_PAIRING_FAILED;
				pr_debug(LOG_MODULE_BLE, "sm_status_evt: BONDING FAILED: gap_status: 0x%x",
						p_evt->gap_status);
			}
			break;
		case BLE_GAP_SM_ST_LINK_ENCRYPTED:
			evt->sm_status = BLE_SM_LINK_ENCRYPTED;
			break;
		case BLE_GAP_SM_ST_SECURITY_UPDATE:
			evt->sm_status = BLE_SM_LINK_ENCRYPTED;
			break;
		default:
			panic(-1);
		}

		cfw_send_event(&evt->header);
	}
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_sm_passkey_req_evt(const struct ble_gap_sm_passkey_req_evt * p_evt)
{
	struct ble_security_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	struct bt_conn *conn;

	conn = bt_conn_lookup_handle(p_evt->conn_handle);
	if (conn) {
		bt_conn_unref(conn);

		evt->conn = conn;
		evt->sm_status = BLE_SM_AUTH_PASSKEY_REQ;
		evt->type = p_evt->key_type;

		cfw_send_event(&evt->header);
	}
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_sm_passkey_reply_rsp(const struct ble_core_response * p_params)
{
	struct ble_rsp *rsp = (void *)cfw_alloc_rsp_msg(p_params->user_data,
			MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP, sizeof(*rsp));
	rsp->status = p_params->status;
	bfree(p_params->user_data);

	cfw_send_message(rsp);
}

void ble_gap_advertisement_timeout(void)
{
	struct ble_adv_to_evt *evt = (void *)cfw_alloc_evt_msg(&ble_service,
			MSG_ID_BLE_ADV_TO_EVT,
			sizeof(*evt));
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_to_evt(const struct ble_gap_timout_evt *p_evt)
{
	pr_debug(LOG_MODULE_BLE, "ble_gap_to_evt_msg: to reason: %d",
			p_evt->reason);
	switch (p_evt->reason) {
	case BLE_SVC_GAP_TO_ADV:
		ble_gap_advertisement_timeout();
		break;
	case BLE_SVC_GAP_TO_SEC_REQ:
	case BLE_SVC_GAP_TO_SCAN:
	case BLE_SVC_GAP_TO_CONN:
		/* TODO: link loss */
		/* Eventually this could happen and we need the connection handle */
	default:
		pr_warning(LOG_MODULE_BLE, "ble_gap_to_evt_msg:"
				"TO reason: %d not handled",
				p_evt->reason);
		break;
	}
}

void on_ble_gap_read_bda_rsp(const struct ble_service_read_bda_response *p_params) {

	struct cfw_message *rsp = p_params->user_data;
	if (NULL != rsp) {
		// check if it is part of the enable procedure
		if (rsp->m.id == MSG_ID_BLE_ENABLE_RSP) {
			struct ble_enable_rsp *r = container_of(rsp, struct ble_enable_rsp, header);
			r->bd_addr = p_params->bd;
			cfw_send_message(rsp);

		} else if (rsp->m.id == MSG_ID_BLE_GET_INFO_RSP) {
			struct ble_get_info_rsp *r = container_of(rsp, struct ble_get_info_rsp, header);
			r->info_params.bda_name_params.bda = p_params->bd;
			/* Don't send response yet, we need the list of bonded devices */
			ble_get_bonded_device_list_req(rsp);
		}
	}
}



void on_ble_gap_service_read_rsp(const struct ble_core_response *p_params) {

	struct ble_rsp *resp = p_params->user_data;

	resp->status = p_params->status;

	cfw_send_message(resp);
}

void on_ble_gap_rssi_evt(const struct ble_gap_rssi_evt *p_evt)
{
	struct ble_rssi_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_RSSI_EVT,
					sizeof(*evt));
	evt->rssi = p_evt->rssi_lvl;
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_sm_pairing_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_clr_white_list_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_start_scan_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_set_option_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_stop_scan_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_wr_white_list_rsp(const struct ble_core_response *p_params) {
	struct ble_rsp *resp = p_params->user_data;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_conn_update_evt(const struct ble_gap_conn_update_evt *p_evt)
{
	struct _ble_service_cb *p_cb = &_ble_cb;

	/* Check that the connection is the correct one */
	if (p_cb->conn->handle == p_evt->conn_handle)
		p_cb->conn_values = p_evt->conn_values;
}
