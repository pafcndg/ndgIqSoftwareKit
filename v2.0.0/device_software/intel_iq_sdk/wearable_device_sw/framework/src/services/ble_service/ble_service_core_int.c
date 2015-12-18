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
#include <assert.h>
#include "os/os.h"
#include "cfw/cfw.h"
#include "cfw/cfw_debug.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_service.h"
#include "services/ble_service/ble_service_api.h"

#include "zephyr/bluetooth/gatt.h"
#include "gatt_int.h"

#include "services/ble_service/ble_service_gatt.h"
#include "services/ble_service/ble_service_gatts_api.h"
#include "services/ble_service/ble_service_gap_api.h"
#include "service_queue.h"
#include "ble_service_core_int.h"
#include "ble_service_int.h"
#include "ble_service_dis.h"
#include "ble_service_bas.h"
#include "infra/log.h"
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
#include "services/em_service/battery_service_api.h"
#endif
#include "ble_service_test.h"
#include "ble_service_utils.h"

static T_TIMER conn_params_timer = NULL;

void on_ble_gap_set_enable_config_rsp(const struct ble_response * p_params) {

	struct ble_enable_req_msg *msg_orig = p_params->p_priv;

	if (!msg_orig)
		return;
	struct ble_enable_rsp *resp =
	    (struct ble_enable_rsp *)cfw_alloc_rsp_msg(&msg_orig->header,
			    MSG_ID_BLE_ENABLE_RSP, sizeof(*resp));
	resp->status = p_params->status;

	if (BLE_STATUS_SUCCESS == resp->status) {
		/* TODO: add writable name support */

		resp->enable = 1;
		ble_gap_sm_config_req(&msg_orig->sm_config, resp);

	} else {
		// error case
		resp->enable = 0;
		cfw_send_message(resp);
	}
	bfree(msg_orig);

}

void on_ble_gap_generic_cmd_rsp(const struct ble_response * p_params) {
	void  *resp = p_params->p_priv;
	if (resp)
		cfw_send_message(resp);
}

void on_ble_gap_get_version_rsp(const struct ble_version_response *p_params) {
	struct ble_version_rsp *resp = p_params->p_rsp;

	if (!resp)
		return;
	resp->status = 0;
	resp->version = p_params->version;
	cfw_send_message(resp);
}

void on_ble_gap_dtm_init_rsp(struct ble_enable_rsp *p_rsp)
{
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

void on_ble_gap_wr_adv_data_rsp(const struct ble_response * p_params) {

	struct ble_gap_adv_params gap_adv_params = { 0 };
	uint8_t timeout_flag;
	uint8_t interval_flag;

	struct ble_start_advertisement_req_msg *msg_orig = p_params->p_priv;

	if(!msg_orig)
		return;

	gap_adv_params.type = msg_orig->type;
	if (msg_orig->bd_len) {
		memcpy(&gap_adv_params.p_peer_bda, msg_orig->data, msg_orig->bd_len);
	}
	gap_adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;

	timeout_flag = msg_orig->options & (BLE_SHORT_ADV_TO | BLE_NO_ADV_TO);
	interval_flag = msg_orig->options &
			(BLE_SLOW_ADV | BLE_ULTRA_FAST_ADV);

	switch (timeout_flag) {
		case BLE_SHORT_ADV_TO:
			gap_adv_params.timeout = APP_NON_DISC_ADV_FAST_TIMEOUT_IN_SECONDS;
			break;
		case BLE_NO_ADV_TO:
			gap_adv_params.timeout = APP_ADV_SLOW_TIMEOUT_IN_SECONDS;
			break;
		default:
			gap_adv_params.timeout = APP_DEFAULT_ADV_TIMEOUT_IN_SECONDS;
			break;
	}

	switch (interval_flag) {
		case BLE_SLOW_ADV:
			gap_adv_params.interval_max = APP_ADV_SLOW_INTERVAL;
			gap_adv_params.interval_min = APP_ADV_SLOW_INTERVAL;
			break;
		case BLE_ULTRA_FAST_ADV:
			gap_adv_params.interval_max = APP_ULTRA_FAST_ADV_INTERVAL;
			gap_adv_params.interval_min = APP_ULTRA_FAST_ADV_INTERVAL;
			break;
		default:
			gap_adv_params.interval_max = APP_DEFAULT_ADV_INTERVAL;
			gap_adv_params.interval_min = APP_DEFAULT_ADV_INTERVAL;
			break;
		}
	ble_gap_start_advertise_req(&gap_adv_params, msg_orig);

}

void on_ble_gap_conn_update_rsp(const struct ble_response * p_params) {
	struct ble_conn_update_rsp * resp = p_params->p_priv;

	if (!resp)
		return;

	resp->status = p_params->status;

	cfw_send_message(resp);

}

void on_ble_gap_disconnect_rsp(const struct ble_response * p_params) {
	if (p_params->p_priv) {
		struct ble_rsp *resp = p_params->p_priv;

		resp->status = p_params->status;
		cfw_send_message(resp);
	}
}

void on_ble_gap_start_advertise_rsp(const struct ble_response * p_params) {
	struct cfw_message *msg_orig = p_params->p_priv;
	struct ble_rsp *resp =
	    (struct ble_rsp *)cfw_alloc_rsp_msg(msg_orig,
			    MSG_ID_BLE_START_ADV_RSP, sizeof(*resp));
	resp->status = p_params->status;
	bfree(msg_orig);
	if (resp->status != BLE_STATUS_SUCCESS) {
		pr_warning(LOG_MODULE_BLE, "Adv start failed! status : %d", resp->status);
	}
	cfw_send_message(resp);

}

void on_ble_gap_service_write_rsp(const struct ble_service_write_response * p_params)
{
	struct cfw_message *msg_orig = p_params->p_priv;

	if (!msg_orig)
		return;
	struct ble_rsp *resp =
	    (struct ble_rsp *)cfw_alloc_rsp_msg(msg_orig,
	                  MSG_ID_BLE_SET_NAME_RSP, sizeof(*resp));
	resp->status = p_params->status;
	bfree(msg_orig);
	cfw_send_message(resp);
}

static void ble_gap_advertisement_timeout(struct _ble_service_cb * p_cb)
{
	struct ble_adv_to_evt *evt = (void *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_ADV_TO_EVT,
					sizeof(*evt));
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_stop_advertise_rsp(const struct ble_response * p_params)
{
	struct ble_rsp *resp = p_params->p_priv;

	if (resp) {
		/* In case of Higher layer API call return the status.
		 * in case of ble disable, ignore result */
		if (resp->header.m.id == MSG_ID_BLE_STOP_ADV_RSP)
			resp->status = p_params->status;
		cfw_send_message(resp);
	}

	cfw_send_message(resp);
}

void on_ble_gap_sm_config_rsp(struct ble_gap_sm_config_rsp *p_params)
{
	if (!p_params)
		return;

	if (p_params->status == BLE_STATUS_SUCCESS) {
		if (p_params->state == BLE_GAP_SM_ST_BONDING_COMPLETE)
			_ble_cb.security_state = BLE_SEC_ST_BONDED_DEVICES_AVAIL;
		else
			_ble_cb.security_state = BLE_SEC_ST_NO_BONDED_DEVICES;

		p_params->state = _ble_cb.security_state;
		ble_gap_read_bda_req(p_params->p_rsp);
	} else {
		pr_error(LOG_MODULE_BLE, "sm_config failed");
		cfw_send_message(p_params->p_rsp);
	}
}

void on_ble_gap_sm_clear_bonds_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

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
	if (p_cb->role == BLE_ROLE_PERIPHERAL &&
	    (p_cb->conn_values.latency != p_cb->peripheral_conn_params.slave_latency ||
	     p_cb->conn_values.supervision_to != p_cb->peripheral_conn_params.link_sup_to ||
	     p_cb->conn_values.interval < p_cb->peripheral_conn_params.interval_min ||
	     p_cb->conn_values.interval > p_cb->peripheral_conn_params.interval_max)) {

		/* Start the 60 seconds timer to retry updating the connection */
		timer_start(conn_params_timer, 60000, &os_err);
		ble_gap_connect_update.conn_handle = p_cb->gap_conn_handle;
		ble_gap_connect_update.params = p_cb->peripheral_conn_params;

		/* Send request to update the connection */
		ble_gap_conn_update_req(&ble_gap_connect_update, NULL);
	}
	else
		ble_core_delete_conn_params_timer();
}

void on_ble_gap_connect_evt(const struct ble_gap_connect_evt * p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;
	OS_ERR_TYPE os_err;

	p_cb->gap_conn_handle = p_evt->conn_handle;
	p_cb->conn_values = p_evt->conn_values;
	p_cb->role = p_evt->role;

	struct ble_connect_evt *evt = (struct ble_connect_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_CONNECT_EVT,
			sizeof(*evt));
	evt->status = BLE_STATUS_SUCCESS;
	evt->conn_handle = p_evt->conn_handle;
	evt->role = p_evt->role;
	evt->ble_addr = p_evt->peer_bda;
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */

	/* If peripheral and connection values are not compliant with the PPCP */
	if (p_cb->role == BLE_ROLE_PERIPHERAL &&
	    (p_cb->conn_values.latency != p_cb->peripheral_conn_params.slave_latency ||
	     p_cb->conn_values.supervision_to != p_cb->peripheral_conn_params.link_sup_to ||
	     p_cb->conn_values.interval < p_cb->peripheral_conn_params.interval_min ||
	     p_cb->conn_values.interval > p_cb->peripheral_conn_params.interval_max)) {
		/* Start a timer to configure the parameters */
		conn_params_timer = timer_create(conn_params_timer_handler, p_cb, 5000,
				false, true, &os_err);
	}

	/* Broadcast connection up event to registered clients */
	ble_service_broadcast_conn_st(BT_GAP_CONNECT_EVT, p_evt->conn_handle);
}

void on_ble_gap_disconnect_evt(const struct ble_gap_disconnect_evt * p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;

	if (p_cb->gap_conn_handle == p_evt->conn_handle) {
		p_cb->gap_conn_handle = BLE_SVC_GAP_HANDLE_INVALID;

		ble_core_delete_conn_params_timer();

		/* Broadcast connection down event to registered clients */
		ble_service_broadcast_conn_st(BT_GAP_DISCONNECT_EVT,
				p_evt->conn_handle);

		/* Do not send event if already disabled */
		if (p_cb->ble_state == BLE_ST_DISABLED) {
			pr_warning(LOG_MODULE_BLE, "ble_gap_disc_evt: BLE"
					" already disabled, hci_reason 0x%x",
					p_evt->hci_reason);
			return;
		}
		struct ble_disconnect_evt *evt = (struct ble_disconnect_evt *)
				cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_DISCONNECT_EVT,
						sizeof(*evt));
		evt->status = BLE_STATUS_SUCCESS;
		evt->conn_handle = p_evt->conn_handle;
		evt->reason = p_evt->hci_reason;
		cfw_send_event(&evt->header);
		bfree(evt); /* message has been cloned by cfw_send_event */
	} else
		pr_warning(LOG_MODULE_BLE, "ble_gap_disc_evt: conn"
				" hdl:%d not found", p_evt->conn_handle);
}

void on_ble_gap_conn_update_evt(const struct ble_gap_conn_update_evt * p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;

	/* Check that the connection is the correct one */
	if (p_cb->gap_conn_handle == p_evt->conn_handle)
		p_cb->conn_values = p_evt->conn_values;
}

void on_ble_gap_sm_status_evt(const struct ble_gap_sm_status_evt * p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;

	struct ble_security_evt *evt = (struct ble_security_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	evt->conn_handle = p_evt->conn_handle;

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
	bfree(evt);
}

void on_ble_gap_sm_passkey_display_evt(const struct ble_gap_sm_passkey_disp_evt * p_evt)
{
	struct ble_security_evt *evt = (struct ble_security_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	evt->conn_handle = p_evt->conn_handle;
	evt->sm_status = BLE_SM_AUTH_DISP_PASSKEY;
	memcpy(evt->passkey, p_evt->passkey, BLE_PASSKEY_LEN);

	cfw_send_event(&evt->header);
	bfree(evt);
}

void on_ble_gap_sm_passkey_req_evt(const struct ble_gap_sm_passkey_req_evt * p_evt)
{
	struct ble_security_evt *evt = (struct ble_security_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_SECURITY_EVT,
					sizeof(*evt));

	evt->conn_handle = p_evt->conn_handle;
	evt->sm_status = BLE_SM_AUTH_PASSKEY_REQ;
	evt->type = p_evt->key_type;

	cfw_send_event(&evt->header);
	bfree(evt);
}

void on_ble_gap_sm_passkey_reply_rsp(const struct ble_response * p_params)
{
	struct ble_rsp *rsp = (struct ble_rsp *)cfw_alloc_rsp_msg(p_params->p_priv,
			MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP, sizeof(*rsp));
	rsp->status = p_params->status;
	bfree(p_params->p_priv);

	cfw_send_message(rsp);
}

void on_ble_gap_to_evt(const struct ble_gap_timout_evt *p_evt)
{
	struct _ble_service_cb * p_cb = &_ble_cb;
	pr_debug(LOG_MODULE_BLE, "ble_gap_to_evt_msg: to reason: %d",
			p_evt->reason);
	switch (p_evt->reason) {
	case BLE_SVC_GAP_TO_ADV:
		ble_gap_advertisement_timeout(p_cb);
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

void on_ble_gatts_set_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params) {

	struct ble_service_update_data_rsp *resp = p_params->p_priv;

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

void on_ble_gatts_get_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params,
		uint8_t * p_value, uint8_t length)
{
	struct cfw_message *req = p_params->p_priv;

	if (req) {
		if (req->m.id == MSG_ID_BLE_GET_INFO_REQ) {
			struct ble_get_info_rsp *resp =
			    (struct ble_get_info_rsp *)cfw_alloc_rsp_msg(
					    req,
					    MSG_ID_BLE_GET_INFO_RSP,
					    sizeof(*resp) + length + 1);

			bfree(req);
			resp->status = p_params->status;
			memcpy(resp->info_params.bda_name_params.name,
					p_value, length);
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

void on_ble_gap_service_read_rsp(const struct ble_response *p_params) {

	struct ble_rsp *resp = p_params->p_priv;

	resp->status = p_params->status;

	cfw_send_message(resp);
}

void on_ble_gap_read_bda_rsp(const struct ble_service_read_bda_response * p_params) {

	struct cfw_message *rsp = p_params->p_priv;
	if (NULL != rsp) {
		// check if it is part of the enable procedure
		if (rsp->m.id == MSG_ID_BLE_ENABLE_RSP) {

			struct ble_enable_rsp *r = (void *)rsp;
			r->bd_addr = p_params->bd;

		} else if (rsp->m.id == MSG_ID_BLE_GET_INFO_RSP) {
			struct ble_get_info_rsp *r = (void *)rsp;
			r->info_params.bda_name_params.bda = p_params->bd;
		}
		cfw_send_message(rsp);
	}
}

void on_ble_gap_rssi_evt(const struct ble_gap_rssi_evt * p_evt)
{
	struct ble_rssi_evt *evt = (struct ble_rssi_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_RSSI_EVT,
					sizeof(*evt));
	evt->rssi = p_evt->rssi_lvl;
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

void on_ble_gap_set_rssi_report_rsp(const struct ble_response *p_params)
{
	struct ble_enable_req_msg *msg_orig = p_params->p_priv;

	if (!msg_orig)
		return;
	struct ble_rsp *resp =
			(struct ble_rsp *)cfw_alloc_rsp_msg(&msg_orig->header,
					MSG_ID_BLE_RSSI_RSP, sizeof(*resp));

	resp->status = p_params->status;
	if (resp->status != BLE_STATUS_SUCCESS)
		pr_warning(LOG_MODULE_BLE, "rssi_report_rsp:rssi report request"
				" failed with status: 0x%x", resp->status);
	cfw_send_message(resp);
	bfree(msg_orig);
}

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
void handle_msg_id_batt_svc_level_updated_evt_msg(struct cfw_message *msg,
		struct _ble_service_cb * p_cb)
{
	uint8_t bat_level = ((bs_listen_evt_msg_t*)msg)->bs_evt_content.bat_soc;

	handle_ble_update_service_bas(p_cb->gap_conn_handle, bat_level);
}
#endif

void on_ble_gap_dbg_rsp(const struct debug_response * p_params)
{
	struct ble_dbg_msg *resp = p_params->p_priv;
	if (!resp)
		return;
	resp->u0 = p_params->u0;
	resp->u1 = p_params->u1;
	cfw_send_message(resp);
}


void on_ble_gattc_discover_primary_service_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_discover_included_services_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_discover_descriptor_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_rd_chars_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_wr_op_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_discover_char_rsp(const struct ble_gattc_rsp *rsp, void *p_priv)
{

}

void on_ble_gattc_disc_prim_svc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len)
{

}

void on_ble_gattc_disc_incl_svc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len)
{

}

void on_ble_gattc_desc_disc_evt(const struct ble_gattc_evt *p_evt,
		ble_gattc_descriptor_t *p_desc, uint8_t desc_len)
{

}

void on_ble_gattc_wr_char_evt(const struct ble_gattc_wr_char_evt *p_evt)
{

}

void on_ble_gattc_char_disc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len)
{

}

void on_ble_gattc_rd_char_evt(const struct ble_gattc_rd_char_evt *p_evt,
		uint8_t *data, uint8_t data_len)
{

}

void on_ble_gattc_value_evt(const struct ble_gattc_value_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen)
{

}

void on_ble_gattc_to_evt(const struct ble_gattc_to_evt *p_evt)
{

}

void ble_core_client_handle_message(struct cfw_message *msg, void *param)
{
	struct _ble_service_cb *p_cb = param;
	uint16_t msg_id = CFW_MESSAGE_ID(msg);

	switch (msg_id) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		if ((void *)BATTERY_SERVICE_ID == msg->priv) {
			p_cb->p_bat_service_conn=
					(cfw_service_conn_t *) ((cfw_open_conn_rsp_msg_t *)
					msg)->service_conn;
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
		cfw_svc_available_evt_msg_t * evt =
				(cfw_svc_available_evt_msg_t *)msg;
		if (evt->service_id == BATTERY_SERVICE_ID)
			cfw_open_service_conn(p_cb->client, BATTERY_SERVICE_ID,
					(void *)BATTERY_SERVICE_ID);
		break;
		}
#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT
	case MSG_ID_BATT_SVC_LEVEL_UPDATED_EVT:
		handle_msg_id_batt_svc_level_updated_evt_msg(msg, p_cb);
		break;
	case MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP: {
		uint8_t bat_level = ((bs_get_soc_rsp_msg_t *) msg)->bat_soc;
		if (E_OS_OK != handle_ble_update_service_bas(p_cb->gap_conn_handle,
				bat_level))
			pr_error(LOG_MODULE_BLE, "ble_srvc_bas_lvl_upd: failed,"
					" IPC error");
		break;
		}
#endif
	default:
		cfw_print_default_handle_error_msg(LOG_MODULE_BLE, msg_id);
		break;
	}
	cfw_msg_free(msg);
}

void ble_core_resume_enable(struct ble_enable_req_msg *req, struct _ble_service_cb * p_cb, uint8_t * p_name)
{
	struct ble_wr_config_params config;

	memset(&config,0,sizeof (config));

	strncpy(&config.device_name[0], p_name, sizeof(config.device_name));
	config.name_present = 1;
	config.appearance = req->appearance;
	config.tx_power = 127; /* TODO: define INVALID value */
	config.peripheral_conn_params = req->peripheral_conn_params;
	config.central_conn_params = req->central_conn_params;
	config.sm_config = req->sm_config;

	if (req->bda_len > 0) {
		memcpy(&config.bda, req->data, sizeof(ble_addr_t));
		config.bda_present = 1;
	}

	/* Save the peripheral connection parameters in the control block */
	p_cb->peripheral_conn_params = req->peripheral_conn_params;

	ble_gap_set_enable_config_req(&config, req);
}

void handle_ble_enable(struct ble_enable_req_msg *req, struct _ble_service_cb *p_cb)
{
	pr_info(LOG_MODULE_BLE, "ble_enable: state %d, options:"
			"0x%x", p_cb->ble_state, req->options);

	switch (req->options) {
	case BLE_OPTION_NONE:
		if (p_cb->ble_state == BLE_ST_DISABLED)
			p_cb->gap_conn_handle =
			BLE_SVC_GAP_HANDLE_INVALID;
		p_cb->ble_state = BLE_ST_ENABLED;
		break;
	case BLE_OPTION_TEST_MODE:
		handle_ble_dtm_init(req);
		p_cb->ble_state = BLE_ST_DTM;
		return;
		break;
	}

	uint8_t *p_name = (req->name_len > 0) ? &req->data[req->bda_len] :
			(uint8_t *)BLE_DEFAULT_NAME;

	ble_core_resume_enable(req, p_cb, p_name);
}

void handle_ble_disable(struct ble_enable_req_msg *req, struct _ble_service_cb *p_cb)
{
	pr_debug(LOG_MODULE_BLE, "ble_disable");
	p_cb->ble_state = BLE_ST_DISABLED;
	struct ble_gap_disconnect_req_params ble_gap_disconnect;

	if (p_cb->gap_conn_handle != BLE_SVC_GAP_HANDLE_INVALID) {
		ble_gap_disconnect.conn_handle = p_cb->gap_conn_handle;
		ble_gap_disconnect.reason = HCI_REMOTE_USER_TERMINATED_CONNECTION;
		ble_gap_disconnect_req(&ble_gap_disconnect, NULL);
	}

	/* send MSG_ID_BLE_ENABLE_RSP on advertise disable complete in order to
	 * cproxy_disconnect */
	struct ble_enable_rsp *resp =
		(struct ble_enable_rsp *)cfw_alloc_rsp_msg(
				&req->header,
				MSG_ID_BLE_ENABLE_RSP,
				sizeof(*resp));

	ble_gap_stop_advertise_req(resp);
}

void handle_ble_start_adv(struct cfw_message *msg, struct _ble_service_cb *p_cb)
{
	struct ble_start_advertisement_req_msg *req =
			(struct ble_start_advertisement_req_msg *) msg;
	struct ble_gap_adv_rsp_data adv_data;
	uint8_t ad[BLE_MAX_ADV_SIZE];

	adv_data.len = req->ad_len;
	adv_data.p_data = ad;
	memcpy(adv_data.p_data, &req->data[req->bd_len + req->sd_len], req->ad_len);

	/* write advertisement data */
	ble_gap_wr_adv_data_req(adv_data.p_data, adv_data.len, NULL, 0, req);
}

void handle_ble_stop_adv(struct cfw_message *msg, struct _ble_service_cb *p_cb)
{
	struct ble_rsp *resp = (struct ble_rsp *)cfw_alloc_rsp_msg(msg,
				MSG_ID_BLE_STOP_ADV_RSP, sizeof(*resp));
	ble_gap_stop_advertise_req(resp);
}

void handle_ble_set_name(struct cfw_message *msg, struct _ble_service_cb *p_cb)
{
	struct cfw_message *msg_orig =
	    (struct cfw_message *)balloc(sizeof(*msg_orig), NULL);
	memcpy(msg_orig, msg, sizeof(*msg_orig));
	struct ble_set_name_req_msg *req = (struct ble_set_name_req_msg *)msg;
	struct ble_gap_service_write_params params = {0};
	int len;

	if (req->name_len > 0) {
		len = (req->name_len > BLE_MAX_DEVICE_NAME) ? BLE_MAX_DEVICE_NAME : req->name_len;
		memcpy(params.name.name_array, req->data, len);
	} else {
		len = strlen(BLE_DEFAULT_NAME);
		memcpy(params.name.name_array, BLE_DEFAULT_NAME, len);
	}
	params.name.name_array[len] = 0;

	params.attr_type = GAP_SVC_ATTR_NAME;
	params.name.authorization = 0;
	params.name.len = len;


	params.name.sec_mode = BLE_DEVICE_NAME_WRITE_PERM;

	struct ble_device_name_evt *evt = (struct ble_device_name_evt *)
			cfw_alloc_evt_msg(&ble_service, MSG_ID_BLE_NAME_EVT,
			sizeof(*evt) + len + 1);

	strcpy(evt->device_name, params.name.name_array);
	cfw_send_event(&evt->header);
	bfree(evt);

	ble_gap_service_write_req(&params, msg_orig);

}

void handle_ble_rssi_req(struct cfw_message *msg, struct _ble_service_cb *p_cb)
{
	struct ble_gap_set_rssi_report_req_msg *req =
			(struct ble_gap_set_rssi_report_req_msg*) msg;
	ble_gap_set_rssi_report_req(&req->params, msg);
}

void on_ble_gap_sm_pairing_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_clr_white_list_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_start_scan_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_cancel_connect_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_connect_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_set_option_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_stop_scan_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gap_wr_white_list_rsp(const struct ble_response * p_params) {
	struct ble_rsp *resp = p_params->p_priv;

	if (!resp)
		return;
	resp->status = p_params->status;
	cfw_send_message(resp);
}

void on_ble_gatts_write_conn_attributes_rsp(const struct ble_response *p_params)
{
	return;
}

void on_ble_gatts_read_conn_attributes_rsp(const struct ble_response *p_params)
{
	return;
}

void on_ble_gatts_start_service_rsp(const struct ble_response * p_params)
{
	struct ble_rsp *resp = p_params->p_priv;

	resp->status = p_params->status;

	cfw_send_message(resp);
}

void on_ble_gatts_send_svc_changed_rsp(const struct ble_response *p_params)
{
}

void  on_ble_gatts_remove_service_rsp(const struct ble_response *p_params)
{
}

void on_ble_gatts_add_included_svc_rsp(const struct ble_response *p_params)
{
	struct ble_gattc_rsp *resp = p_params->p_priv;

	resp->status = p_params->status;

	if (resp->status != BLE_STATUS_SUCCESS ) {
		pr_error(LOG_MODULE_BLE, "gatts_add_inc_svc err %d", resp->status);
	}

	cfw_send_message(resp);
}

void ble_log(const struct ble_log_s *p_param, char *p_buf, uint8_t buflen)
{
	pr_info(LOG_MODULE_BLE, p_buf, p_param->param0, p_param->param1, p_param->param2, p_param->param3);
}
