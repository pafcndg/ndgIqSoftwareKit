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
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "ble_protocol.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_service_int.h"
#include "ble_service_dis.h"
#include "ble_service_bas.h"
#include "ble_service_utils.h"
#include "services/ble_service/ble_service.h"
#include "ble_service_test.h"
#include "infra/log.h"

#include "rpc.h"

/* GAP Service device name is hardcoded by Nordic stack */
#define NAME_REQUEST_VALUE 3

struct _ble_service_cb _ble_cb = { 0 };

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

int ble_service_register_for_conn_st(connection_status_t cback)
{
	int idx;

	for (idx = 0; idx < BLE_MAX_CONN_STATUS &&
				_ble_cb.connection_st_cb[idx]; idx++)
		;
	/* no space in the table anymore */
	assert(idx < BLE_MAX_CONN_STATUS);

	_ble_cb.connection_st_cb[idx] = cback;

	return idx;
}

void ble_service_broadcast_conn_st(uint8_t status_evt, uint16_t conn_handle)
{
	int idx;

	for (idx = 0; idx < BLE_MAX_CONN_STATUS; idx++)
		if (_ble_cb.connection_st_cb[idx])
			_ble_cb.connection_st_cb[idx](status_evt,
					conn_handle);
}

static void handle_msg_id_ble_set_name(struct cfw_message *msg)
{
	handle_ble_set_name(msg, &_ble_cb);
}

static void handle_msg_id_ble_start_discover(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_stop_discover(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_connect(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_disconnect(struct cfw_message *msg)
{
	int ret = BLE_STATUS_ERROR;
	struct ble_gap_disconnect_req_params ble_gap_disconnect;
	struct ble_disconnect_req_msg * req = (struct ble_disconnect_req_msg *)msg;
	struct ble_disconnect_rsp * resp =
			(struct ble_disconnect_rsp *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_DISCONNECT_RSP,
					sizeof(*resp));

	resp->status = ret;
	resp->conn_handle = req->conn_handle;
	ble_gap_disconnect.conn_handle = req->conn_handle;
	ble_gap_disconnect.reason = HCI_REMOTE_USER_TERMINATED_CONNECTION;
	ble_gap_disconnect_req(&ble_gap_disconnect, resp);


	if (BLE_STATUS_SUCCESS != ret)
		cfw_send_message(resp);
}

static void handle_msg_id_ble_conn_update(struct cfw_message *msg)
{
	int ret = BLE_STATUS_ERROR;
	struct ble_gap_connect_update_params ble_gap_connect_update;
	struct ble_conn_update_req_msg * req = (struct ble_conn_update_req_msg *)msg;
	struct ble_conn_update_rsp * resp =
			(struct ble_conn_update_rsp *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_CONN_UPDATE_RSP,
					sizeof(*resp));

	resp->status = ret;
	resp->conn_handle = req->conn_handle;

	ble_core_delete_conn_params_timer();

	ble_gap_connect_update.conn_handle = req->conn_handle;
	ble_gap_connect_update.params = req->conn_params;
	ble_gap_conn_update_req(&ble_gap_connect_update, resp);
}

static void handle_msg_id_ble_subscribe(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_unsubscribe(struct cfw_message *msg)
{
}

struct ble_init_service_rsp * ble_alloc_init_service_rsp(
		struct ble_init_svc_req_msg *p_req)
{
	struct ble_init_service_rsp *resp =
			(struct ble_init_service_rsp *)cfw_alloc_rsp_msg(
				&p_req->header,
				MSG_ID_BLE_INIT_SVC_RSP,
				sizeof(*resp));

	return resp;
}

void handle_ble_add_service_failure(struct ble_init_svc_req_msg *req,
		uint16_t status)
{
	struct ble_init_service_rsp *resp =
			ble_alloc_init_service_rsp(req);
	resp->status = (BLE_STATUS_SUCCESS != status) ? status :
			BLE_STATUS_ERROR;
	cfw_msg_free(&req->header);
	cfw_send_message(resp);
}


static void handle_msg_id_ble_init_svc(struct cfw_message *msg, struct _ble_service_cb * p_cb)
{
	struct ble_init_svc_req_msg *req = (__typeof__(req))msg;

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

static void handle_msg_id_ble_start_adv(struct cfw_message *msg)
{
	handle_ble_start_adv(msg, &_ble_cb);
}

static void handle_msg_id_ble_stop_adv(struct cfw_message *msg)
{
	handle_ble_stop_adv(msg, &_ble_cb);
}

static void handle_msg_id_ble_disable_svc(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_get_security(struct cfw_message *msg)
{
	struct ble_get_security_status_req_msg *req =
			(struct ble_get_security_status_req_msg *) msg;

	struct ble_get_security_rsp *resp =
			(struct ble_get_security_rsp *)cfw_alloc_rsp_msg(
				(struct cfw_message *)req,
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
	struct ble_set_security_status_req_msg *req =
			(struct ble_set_security_status_req_msg *) msg;

	struct ble_set_security_rsp *resp =
			(struct ble_set_security_rsp *)cfw_alloc_rsp_msg(
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
	struct ble_gap_sm_key_reply_req_msg *req = (__typeof__(req)) msg;
	struct ble_gap_sm_key_reply_req_params ble_gap_sm_key_reply;

	ble_gap_sm_key_reply.params = req->params;
	ble_gap_sm_key_reply.conn_handle = req->conn_handle;

	ble_gap_sm_passkey_reply_req(&ble_gap_sm_key_reply, req);
}

static void handle_msg_id_ble_clear_bonds(struct cfw_message *msg)
{
	struct ble_rsp *resp = (struct ble_rsp *) cfw_alloc_rsp_msg(msg,
			MSG_ID_BLE_CLEAR_BONDS_RSP, sizeof(*resp));
	ble_gap_sm_clear_bonds_req(resp);
}

static void handle_msg_id_ble_get_version(struct cfw_message *msg)
{
	struct ble_version_rsp *resp =
			(struct ble_version_rsp *)cfw_alloc_rsp_msg(msg,
					MSG_ID_BLE_GET_VERSION_RSP, sizeof(*resp));

	ble_gap_get_version_req(resp);
}

static void handle_msg_id_ble_get_info_req(struct cfw_message *msg)
{
	struct ble_gatts_get_attribute_params params;
	struct ble_gap_get_info_req_msg * req =
				(struct ble_gap_get_info_req_msg *)msg;
	if (req->info_type == BLE_INFO_BDA_NAME_REQ)
		params.value_handle = NAME_REQUEST_VALUE;
		ble_gatts_get_attribute_value_req(&params, msg);
}

static void handle_msg_id_ble_get_remote_data(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_set_remote_data(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_discover_svc(struct cfw_message *msg)
{
}

static void handle_msg_id_ble_update_data(struct cfw_message *msg,
		struct _ble_service_cb * p_cb)
{
	int ret = BLE_STATUS_NOT_SUPPORTED;
	struct ble_update_service_data_msg * req = (__typeof__(req))msg;
	struct ble_service_update_data_rsp *resp = (__typeof__(resp))
			cfw_alloc_rsp_msg(msg, MSG_ID_BLE_UPDATE_DATA_RSP,
					sizeof(*resp));
	resp->status = ret;
	resp->conn_h = req->conn_handle;
	resp->char_handle = req->char_handle;

	assert(req->update);

	ret = req->update(req->conn_handle, req->char_handle, req->len,
			req->data, resp);

	if (ret != BLE_STATUS_SUCCESS) {
		resp->status = ret;
		cfw_send_message(resp);
	}
}

static void handle_msg_id_ble_protocol(struct cfw_message *msg,
		struct _ble_service_cb * p_cb)
{
	struct ble_protocol_req_msg *req = (__typeof__(req))msg;


	if (req->protocol_req) {
		req->protocol_req(req->conn_handle, req->len, req->offset,
				req->data, msg->priv);
	} else {
		struct ble_protocol_rsp *rsp = (__typeof__(rsp))
				cfw_alloc_rsp_msg(msg, MSG_ID_BLE_PROTOCOL_RSP,
						sizeof(rsp));

		rsp->status = BLE_STATUS_NOT_SUPPORTED;
		cfw_send_message(rsp);
	}
}

static void handle_msg_id_ble_dtm(struct cfw_message *msg)
{
	handle_ble_dtm_cmd(msg);
}

static void handle_msg_id_ble_set_rssi_report_req(struct cfw_message *msg)
{
	struct ble_gap_set_rssi_report_req_msg *req =
			(struct ble_gap_set_rssi_report_req_msg*)msg;

	struct ble_generic_conn_rsp * resp =
			(struct ble_generic_conn_rsp *)cfw_alloc_rsp_msg(
					msg, MSG_ID_BLE_RSSI_RSP,
					sizeof(*resp));

	resp->status = BLE_STATUS_SUCCESS;
	resp->conn_handle = req->params.conn_hdl;
	handle_ble_rssi_req(msg, &_ble_cb);

	cfw_send_message(resp);
}

#ifdef CONFIG_TCMD_BLE_DEBUG
static void handle_msg_id_ble_dbg(struct cfw_message *msg)
{
	struct debug_params params;
	struct ble_dbg_msg *resp = (struct ble_dbg_msg*)
		cfw_alloc_rsp_msg(msg, MSG_ID_BLE_DBG_RSP, sizeof(*resp));
	struct ble_dbg_msg* req = (struct ble_dbg_msg*) msg;

	params.u0 = req->u0;
	params.u1 = req->u1;

	ble_gap_dbg_req(&params, resp);
}
#endif // CONFIG_TCMD_BLE_DEBUG

static void handle_msg_id_ble_rpc_callin(struct message*msg, void*priv)
{
	struct ble_rpc_callin *rpc = (void*)msg;
	/* handle incoming message */
	rpc_deserialize(rpc->p_data, rpc->len);
	bfree(rpc->p_data);
	message_free(msg);
}

void ble_handle_message(struct cfw_message *msg, void *param)
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
		struct ble_enable_req_msg *req = (struct ble_enable_req_msg *)msg;
		if (p_cb->ble_state) {
			if (req->enable) {
				handle_ble_enable(req, p_cb);
				free_msg = false;
			} else
				handle_ble_disable(req, p_cb);
		} else {
			pr_debug(LOG_MODULE_BLE, "ble_hdl_msg: core service"
					" not opened!");
			/* core service is not yet up */
			struct ble_enable_rsp *resp =
			    (struct ble_enable_rsp *)cfw_alloc_rsp_msg(msg,
								MSG_ID_BLE_ENABLE_RSP,
								sizeof(*resp));
			resp->status = BLE_STATUS_PENDING;
			resp->enable = 0;
			cfw_send_message(resp);
		}
	}
		break;
	case MSG_ID_BLE_SET_NAME_REQ:
		handle_msg_id_ble_set_name(msg);
		break;
	case MSG_ID_BLE_START_DISCOVER_REQ:
		handle_msg_id_ble_start_discover(msg);
		break;
	case MSG_ID_BLE_STOP_DISCOVER_REQ:
		handle_msg_id_ble_stop_discover(msg);
		break;
	case MSG_ID_BLE_CONNECT_REQ:
		handle_msg_id_ble_connect(msg);
		break;
	case MSG_ID_BLE_DISCONNECT_REQ:
		handle_msg_id_ble_disconnect(msg);
		break;
	case MSG_ID_BLE_CONN_UPDATE_REQ:
		handle_msg_id_ble_conn_update(msg);
		break;
	case MSG_ID_BLE_SUBSCRIBE_REQ:
		handle_msg_id_ble_subscribe(msg);
		break;
	case MSG_ID_BLE_UNSUBSCRIBE_REQ:
		handle_msg_id_ble_unsubscribe(msg);
		break;
	case MSG_ID_BLE_INIT_SVC_REQ:
		handle_msg_id_ble_init_svc(msg, p_cb);
		free_msg = false;
		break;
	case MSG_ID_BLE_START_ADV_REQ:
		handle_msg_id_ble_start_adv(msg);
		free_msg = false;
		break;
	case MSG_ID_BLE_STOP_ADV_REQ:
		handle_msg_id_ble_stop_adv(msg);
		break;
	case MSG_ID_BLE_DISABLE_SVC_REQ:
		handle_msg_id_ble_disable_svc(msg);
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
	case MSG_ID_BLE_GET_REMOTE_DATA_REQ:
		handle_msg_id_ble_get_remote_data(msg);
		break;
	case MSG_ID_BLE_SET_REMOTE_DATA_REQ:
		handle_msg_id_ble_set_remote_data(msg);
		break;
	case MSG_ID_BLE_DISCOVER_SVC_REQ:
		handle_msg_id_ble_discover_svc(msg);
		break;
	case MSG_ID_BLE_UPDATE_DATA_REQ:
		handle_msg_id_ble_update_data(msg, p_cb);
		break;
	case MSG_ID_BLE_PROTOCOL_REQ:
		handle_msg_id_ble_protocol(msg, p_cb);
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

static void ble_client_connected(conn_handle_t * instance)
{
	if (_ble_cb.ble_state == BLE_ST_NOT_READY)
		pr_warning(LOG_MODULE_BLE, "BLE_CORE service is not registered");
}

static void ble_client_disconnected(conn_handle_t * instance)
{
}

service_t ble_service = {
	.service_id = BLE_SERVICE_ID,
	.client_connected = ble_client_connected,
	.client_disconnected = ble_client_disconnected,
};

uint16_t ble_service_get_port_id(void)
{
	return ble_service.port_id;
}

void ble_cfw_service_init(T_QUEUE queue)
{
	_ble_cb.queue = queue;
	_ble_cb.client = cfw_client_init(queue, ble_core_client_handle_message,
			&_ble_cb);
	_ble_cb.ble_state = BLE_ST_NOT_READY;

	_ble_cb.rpc_port_id = port_alloc(queue);
	assert(_ble_cb.rpc_port_id != 0);
	port_set_handler(_ble_cb.rpc_port_id, handle_msg_id_ble_rpc_callin, NULL);

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
	ble_service_bas_set_battery_updates(true);
#endif
}

void on_ble_core_up(void) {
	_ble_cb.ble_state = BLE_ST_DISABLED;

	/* register BLE service */
	if (cfw_register_service(_ble_cb.queue, &ble_service,
			ble_handle_message, &_ble_cb) == -1) {
		pr_error(LOG_MODULE_BLE, "BLE register err %d", ble_service.service_id);
	}
}
