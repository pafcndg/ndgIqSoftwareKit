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

#include "services/ble_service/ble_service_api.h"

#include <errno.h>
#include <string.h>

#include "ble_service_int.h"
#include "cfw/cfw_service.h"

#include "zephyr/bluetooth/bluetooth.h"

static T_TIMER adv_timer = NULL;

int ble_enable(cfw_service_conn_t *p_service_conn, uint8_t enable,
		const struct ble_enable_config *p_config, void *p_priv)
{
	struct ble_enable_req * msg =
			(void *) cfw_alloc_message_for_service(p_service_conn,
				    MSG_ID_BLE_ENABLE_REQ,
				    sizeof(*msg), p_priv);
	msg->options = p_config->options;
	msg->central_conn_params = p_config->central_conn_params;
	msg->enable = enable;
	msg->sm_config = p_config->sm_config;

	if (p_config->p_bda) {
		msg->bda_present = 1;
		msg->bda = *p_config->p_bda;
	}

	return cfw_send_message(msg);
}

int ble_set_name(cfw_service_conn_t * p_service_conn, const uint8_t * p_name,
		 void *p_priv)
{
	struct ble_set_name_req *msg;
	int str_len = 0;

	if (p_name) {
		str_len = strlen((char *)p_name);
		if (str_len > BLE_MAX_DEVICE_NAME)
			str_len = BLE_MAX_DEVICE_NAME;
	}
	msg = (void *)cfw_alloc_message_for_service(p_service_conn,
				    MSG_ID_BLE_SET_NAME_REQ,
				    sizeof(*msg) + str_len + 1, p_priv);
	msg->name_len = str_len;
	if (str_len)
		memcpy((char *)msg->data, (char *)p_name, str_len);

	return cfw_send_message(msg);
}

int ble_start_discover(cfw_service_conn_t * p_service_conn, size_t cnt,
		       const uint16_t * p_services, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_START_DISCOVER_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_stop_discover(cfw_service_conn_t * p_service_conn, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_STOP_DISCOVER_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_connect(cfw_service_conn_t *p_service_conn, const bt_addr_le_t *p_addr,
		const struct bt_le_conn_param *p_conn_params, void *p_priv)
{
	struct ble_connect_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
				MSG_ID_BLE_CONNECT_REQ, sizeof(*msg), p_priv);

	msg->bd_addr = *p_addr;
	msg->conn_params = *p_conn_params;

	return cfw_send_message(msg);
}

int ble_subscribe(cfw_service_conn_t *p_service_conn,
		const struct ble_subscribe_params *p_params,
		void *p_priv)
{
	struct ble_subscribe_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_SUBSCRIBE_REQ,
						sizeof(*msg), p_priv);
	msg->params = *p_params;

	return cfw_send_message(msg);
}

int ble_unsubscribe(cfw_service_conn_t *p_service_conn,
		struct ble_unsubscribe_params *p_params, void *p_priv)
{
	struct ble_unsubscribe_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_UNSUBSCRIBE_REQ,
						sizeof(*msg), p_priv);
	msg->params = *p_params;

	return cfw_send_message(msg);
}

int ble_conn_update(cfw_service_conn_t * p_service_conn,
		    struct bt_conn *conn,
		    const struct ble_gap_connection_params *p_params,
		    void *p_priv)
{
	struct ble_conn_update_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_CONN_UPDATE_REQ,
						sizeof(*msg),
						p_priv);
	msg->conn = conn;
	msg->conn_params = *p_params;

	return cfw_send_message(msg);
}

int ble_disconnect(cfw_service_conn_t * p_service_conn,
		struct bt_conn *conn, void *p_priv)
{
	struct ble_disconnect_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_DISCONNECT_REQ,
						sizeof(*msg),
						p_priv);
	msg->conn = conn;

	return cfw_send_message(msg);
}

int ble_test(cfw_service_conn_t * p_service_conn,
	     const struct ble_test_cmd *p_cmd, void *p_priv)
{
	struct ble_dtm_test_req *msg = (void *)
	    cfw_alloc_message_for_service(p_service_conn, MSG_ID_BLE_DTM_REQ,
					  sizeof(*msg), p_priv);
	msg->params = *p_cmd;
	return cfw_send_message(msg);
}

int ble_discover(cfw_service_conn_t * p_service_conn,
			 const struct ble_discover_params * p_params,
			 void *p_priv)
{
	struct ble_discover_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
				MSG_ID_BLE_DISCOVER_REQ,
				sizeof(*msg),
				p_priv);
	msg->params = *p_params;
	return cfw_send_message(msg);
}

int ble_get_remote_data(cfw_service_conn_t * p_service_conn,
			const struct ble_get_remote_data_params *p_params,
			void *p_priv)
{
	struct ble_get_remote_data_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
				MSG_ID_BLE_GET_REMOTE_DATA_REQ,
				sizeof(*msg),
				p_priv);
	msg->params = *p_params;
	return cfw_send_message(msg);
}

int ble_set_remote_data(cfw_service_conn_t * p_service_conn,
			const struct ble_set_remote_data_params *p_params,
			uint16_t data_length, uint8_t *data,
			void *p_priv)
{
	struct ble_set_remote_data_req *msg =
			(void *)cfw_alloc_message_for_service(p_service_conn,
				MSG_ID_BLE_SET_REMOTE_DATA_REQ,
				sizeof(*msg) + data_length,
				p_priv);

	msg->params = *p_params;
	msg->data_length = data_length;
	memcpy(&msg->data, data, data_length);

	return cfw_send_message(msg);
}

/** Advertising timeing parameters.
 *
 *  options see @ref BLE_ADV_OPTIONS */
/* options: BLE_NO_ADV_OPT */
#define APP_DEFAULT_ADV_INTERVAL           160
#define APP_DEFAULT_ADV_TIMEOUT_IN_SECONDS 180
/* options: BLE_NON_DISC_ADV */
#define APP_NON_DISC_ADV_FAST_TIMEOUT_IN_SECONDS    30
/* options: BLE_SLOW_ADV or BLE_SLOW_ADV | BLE_NON_DISC_ADV */
#define APP_ADV_SLOW_INTERVAL              2056
#define APP_ADV_SLOW_TIMEOUT_IN_SECONDS    0
/* options: BLE_ULTRA_FAST_ADV */
#define APP_ULTRA_FAST_ADV_INTERVAL             32

void ble_delete_adv_timer(void)
{
	OS_ERR_TYPE os_err;

	/* Destroy the timer */
	timer_delete(adv_timer, &os_err);

	/* Make sure that the reference to the timer is lost */
	adv_timer = NULL;
}

static void adv_timer_handler(void * privData)
{
	/* stop & clean up timer */
	ble_delete_adv_timer();

	bt_le_adv_stop();

	/* send timeout event to application */
	ble_gap_advertisement_timeout();
}

int ble_start_advertisement(cfw_service_conn_t *p_service_conn,
		const struct ble_adv_params *p_adv_params,
		void *p_priv)
{
	struct cfw_message *msg;
	struct ble_rsp *resp;
	struct bt_le_adv_param param;
	int status = 0;
	int timeout;
	OS_ERR_TYPE os_err;
	uint8_t timeout_flag, interval_flag;

	/* To avoid complicated marshalling only send a response message. */
	msg = (void *) cfw_alloc_message_for_service(p_service_conn,
					  MSG_ID_BLE_START_ADV_REQ,
					  sizeof(*msg),
					  p_priv);
	/* return immediately a status. internal callback will show failure */
	resp = (void *)cfw_alloc_rsp_msg(msg, MSG_ID_BLE_START_ADV_RSP,
			sizeof(*resp));
	cfw_msg_free(msg);

	timeout_flag = p_adv_params->options & (BLE_SHORT_ADV_TO | BLE_NO_ADV_TO);
	interval_flag = p_adv_params->options & (BLE_SLOW_ADV | BLE_ULTRA_FAST_ADV);

	switch (timeout_flag) {
		case BLE_SHORT_ADV_TO:
			timeout = APP_NON_DISC_ADV_FAST_TIMEOUT_IN_SECONDS * 1000;
			break;
		case BLE_NO_ADV_TO:
			timeout = APP_ADV_SLOW_TIMEOUT_IN_SECONDS * 1000;
			break;
		default:
			timeout = APP_DEFAULT_ADV_TIMEOUT_IN_SECONDS * 1000;
			break;
	}

	if (timeout != 0) {
		/* Start a timer to configure the parameters */
		adv_timer = timer_create(adv_timer_handler, NULL, timeout,
				false, true, &os_err);
	}
	switch (interval_flag) {
		case BLE_SLOW_ADV:
			param.interval_max = APP_ADV_SLOW_INTERVAL;
			param.interval_min = APP_ADV_SLOW_INTERVAL;
			break;
		case BLE_ULTRA_FAST_ADV:
			param.interval_max = APP_ULTRA_FAST_ADV_INTERVAL;
			param.interval_min = APP_ULTRA_FAST_ADV_INTERVAL;
			break;
		default:
			param.interval_max = APP_DEFAULT_ADV_INTERVAL;
			param.interval_min = APP_DEFAULT_ADV_INTERVAL;
			break;
	}

	param.type = p_adv_params->adv_type;

	if (!(p_adv_params->p_le_addr)) {

		status = bt_le_adv_start(&param, p_adv_params->p_ad,
				p_adv_params->p_sd);
	} else {
		struct bt_conn *p_conn;
		p_conn = bt_conn_create_slave_le(p_adv_params->p_le_addr,
				&param);
		if (!p_conn)
			status = -ENOMEM;
	}

	if (status)
		resp->status = BLE_STATUS_ERROR_PARAMETER;

	return cfw_send_message(resp);
}

int ble_stop_advertisement(cfw_service_conn_t * p_service_conn, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_STOP_ADV_REQ,
								sizeof(*msg),
								p_priv);
	/* stop timer beforehand to avoid potential race condition */
	ble_delete_adv_timer();

	return cfw_send_message(msg);
}

int ble_get_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_get_security_params *p_params,
			    void *p_priv)
{
	struct ble_get_security_status_req *msg;
	int param_len = 0;

	msg = (void *)cfw_alloc_message_for_service(p_service_conn,
					MSG_ID_BLE_GET_SECURITY_REQ,
					sizeof(*msg) + param_len,
					p_priv);
	msg->op_code = op;
	/* TODO: support more security operations */
	return cfw_send_message(msg);
}

int ble_set_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_set_sec_params *p_params,
			    void *p_priv)
{
	struct ble_set_security_status_req * msg = (void *)
			cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_SET_SECURITY_REQ,
						sizeof(*msg),
						p_priv);
	msg->op_code = op;
	switch (op) {
	case BLE_SEC_DEVICE_STATUS:
		msg->params.dev_status = p_params->dev_status;
		break;
	default:
		break;
	}
	return cfw_send_message(msg);
}

int ble_send_passkey(cfw_service_conn_t *p_service_conn,
		     struct bt_conn *conn,
		     const struct ble_gap_sm_passkey *p_params,
		     void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_sm_key_reply_req, msg,
			p_service_conn, MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ, 0, p_priv);

	msg->params = *p_params;
	msg->conn = conn;

	return cfw_send_message(msg);
}

int ble_update_service_data(cfw_service_conn_t *p_service_conn,
			    struct bt_conn *conn,
			    const struct ble_char_data *p_params, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_update_data_req, msg, p_service_conn,
					MSG_ID_BLE_UPDATE_DATA_REQ,
					p_params->len,
					p_priv);

	msg->update = p_params->update;
	msg->conn = conn;
	msg->char_handle = p_params->char_handle;
	msg->len = p_params->len;
	memcpy(msg->data, p_params->p_data, msg->len);
	return cfw_send_message(msg);
}

int ble_service_get_version(cfw_service_conn_t *p_service_conn, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_GET_VERSION_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_set_rssi_report(cfw_service_conn_t * p_service_conn,
		struct bt_conn *conn, const struct rssi_report_params *params, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_set_rssi_report_req, msg, p_service_conn, MSG_ID_BLE_RSSI_REQ, 0, p_priv);

	msg->conn = conn;
	msg->params = *params;
	return cfw_send_message(msg);
}

int ble_service_get_info(cfw_service_conn_t *p_service_conn,
		uint8_t info_type, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_get_info_req, msg, p_service_conn,
			MSG_ID_BLE_GET_INFO_REQ, sizeof(*msg), p_priv);

	msg->info_type = info_type;
	return cfw_send_message(msg);
}

int ble_clear_bonds(cfw_service_conn_t * p_service_conn, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_CLEAR_BONDS_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}
