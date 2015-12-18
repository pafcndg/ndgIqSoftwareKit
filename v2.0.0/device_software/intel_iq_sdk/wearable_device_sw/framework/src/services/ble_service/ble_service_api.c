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

#include <string.h>

#include "ble_service_core_int.h"
#include "ble_service_int.h"
#include "ble_service_dis.h"
#include "ble_service_bas.h"
#include "ble_service_utils.h"
#include "infra/log.h"

int ble_enable(cfw_service_conn_t * p_service_conn, uint8_t enable,
		const struct ble_enable_config * p_config, void *p_priv)
{
	struct ble_enable_req_msg * msg;
	int total_len = sizeof(*msg);
	int str_len = 0;

	if (p_config->p_bda)
		total_len += sizeof(*(p_config->p_bda));
	if (p_config->p_name) {
		str_len = strlen((char *)p_config->p_name);
		if (str_len > BLE_MAX_DEVICE_NAME)
			return E_OS_ERR_NO_MEMORY;
		/* Add 1 for the terminating NULL */
		total_len += str_len + 1;
	}
	msg = (struct ble_enable_req_msg *)
		    cfw_alloc_message_for_service(p_service_conn,
				    MSG_ID_BLE_ENABLE_REQ,
				    total_len, p_priv);
	msg->options = p_config->options;
	msg->peripheral_conn_params = p_config->peripheral_conn_params;
	msg->central_conn_params = p_config->central_conn_params;
	msg->enable = enable;
	msg->name_len = str_len;
	msg->appearance = p_config->appearance;
	msg->sm_config = p_config->sm_config;

	if (p_config->p_bda) {
		msg->bda_len = sizeof(ble_addr_t);
		memcpy(msg->data, p_config->p_bda, msg->bda_len);
	} else
		msg->bda_len = 0;

	if (str_len)
		strcpy((char *)&msg->data[msg->bda_len], (char *)p_config->p_name);

	return cfw_send_message(msg);
}

int ble_set_name(cfw_service_conn_t * p_service_conn, const uint8_t * p_name,
		 void *p_priv)
{
	struct ble_set_name_req_msg *msg;
	int str_len = 0;

	if (p_name) {
		str_len = strlen((char *)p_name);
		if (str_len > BLE_MAX_DEVICE_NAME)
			str_len = BLE_MAX_DEVICE_NAME;
	}
	msg = (struct ble_set_name_req_msg *)
		    cfw_alloc_message_for_service(p_service_conn,
                        MSG_ID_BLE_SET_NAME_REQ,
				    sizeof(*msg) + str_len + 1, p_priv);
	msg->name_len = str_len;
	if (str_len)
		memcpy((char *)msg->data, (char *)p_name, str_len);

	return cfw_send_message(msg);
}

#ifdef CONFIG_BLE_CENTRAL
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
#endif

int ble_connect(cfw_service_conn_t * p_service_conn, const ble_addr_t * p_addr,
		uint32_t interval, void *p_priv)
{
	struct cfw_message *msg =
	    cfw_alloc_message_for_service(p_service_conn, MSG_ID_BLE_CONNECT_REQ,
					  sizeof(*msg), p_priv);
	return cfw_send_message(msg);
}

#ifdef CONFIG_BLE_CENTRAL
int ble_subscribe(cfw_service_conn_t * p_service_conn,
		  uint16_t conn_handle, uint16_t svc_handle,
		  const uint16_t * p_char_handle,
		  const struct ble_notification_config *p_config, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_SUBSCRIBE_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_unsubscribe(cfw_service_conn_t * p_service_conn,
		    uint16_t svc_handle, const uint16_t * p_char_handle,
		    void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_UNSUBSCRIBE_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}
#endif

int ble_conn_update(cfw_service_conn_t * p_service_conn,
		    uint16_t conn_handle,
		    const struct ble_gap_connection_params * p_params,
		    void *p_priv)
{
	struct ble_conn_update_req_msg *msg = (struct ble_conn_update_req_msg *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_CONN_UPDATE_REQ,
						sizeof(*msg),
						p_priv);
	msg->conn_handle = conn_handle;
	msg->conn_params = *p_params;

	return cfw_send_message(msg);
}

int ble_disconnect(cfw_service_conn_t * p_service_conn,
		   uint16_t conn_handle, void *p_priv)
{
	struct ble_disconnect_req_msg *msg;

	msg = (struct ble_disconnect_req_msg *)cfw_alloc_message_for_service(p_service_conn,
						MSG_ID_BLE_DISCONNECT_REQ,
						sizeof(*msg),
						p_priv);
	msg->conn_handle = conn_handle;

	return cfw_send_message(msg);
}

int ble_test(cfw_service_conn_t * p_service_conn,
	     const struct ble_test_cmd *p_cmd, void *p_priv)
{
	struct ble_dtm_test_req_msg *msg = (struct ble_dtm_test_req_msg *)
	    cfw_alloc_message_for_service(p_service_conn, MSG_ID_BLE_DTM_REQ,
					  sizeof(*msg), p_priv);
	msg->params = *p_cmd;
	return cfw_send_message(msg);
}

#ifdef CONFIG_BLE_CENTRAL
int ble_discover_service(cfw_service_conn_t * p_service_conn,
			 uint16_t conn_handle, uint16_t svc_handle,
			 void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_DISCOVER_SVC_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_get_remote_data(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle, uint16_t char_handle,
			void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_GET_REMOTE_DATA_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_set_remote_data(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle, uint16_t val_handle,
			const uint8_t * p_value, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_SET_REMOTE_DATA_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}
#endif

int ble_start_advertisement(cfw_service_conn_t *p_service_conn,
		uint32_t options,
		const struct ble_adv_data_params *p_adv_params,
		void *p_priv)
{
	struct ble_start_advertisement_req_msg *msg;
	uint8_t *p;
	int total_len = sizeof(*msg);

	if (p_adv_params->p_le_addr)
		total_len += sizeof(ble_addr_t);

	total_len += p_adv_params->sd_len;
	total_len += p_adv_params->ad_len;

	msg = (struct ble_start_advertisement_req_msg *)
	    cfw_alloc_message_for_service(p_service_conn,
					  MSG_ID_BLE_START_ADV_REQ,
					  total_len,
					  p_priv);

	msg->options = options;
	msg->type = p_adv_params->adv_type;
	if (p_adv_params->p_le_addr)
		msg->bd_len = sizeof(ble_addr_t);
	else
		msg->bd_len = 0;
	msg->sd_len = p_adv_params->sd_len;
	msg->ad_len = p_adv_params->ad_len;
	p = msg->data;

	if (msg->bd_len) {
		memcpy(p, p_adv_params->p_le_addr, msg->bd_len);
		p += msg->bd_len;
	}
	if (msg->sd_len) {
		memcpy(p, p_adv_params->p_sd, p_adv_params->sd_len);
		p += msg->sd_len;
	}
	if (msg->ad_len) {
		memcpy(p, p_adv_params->p_ad, p_adv_params->ad_len);
	}

	return cfw_send_message(msg);
}

int ble_stop_advertisement(cfw_service_conn_t * p_service_conn, void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_STOP_ADV_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_disable_service(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle, uint16_t svc_handle,
			void *p_priv)
{
	struct cfw_message *msg = cfw_alloc_message_for_service(p_service_conn,
								MSG_ID_BLE_DISABLE_SVC_REQ,
								sizeof(*msg),
								p_priv);
	return cfw_send_message(msg);
}

int ble_get_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_get_security_params *p_params,
			    void *p_priv)
{
	struct ble_get_security_status_req_msg *msg;
	int param_len = 0;
	msg = (struct ble_get_security_status_req_msg *)
			cfw_alloc_message_for_service(p_service_conn,
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
	struct ble_set_security_status_req_msg * msg;
	msg = (struct ble_set_security_status_req_msg *)
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
		     uint16_t conn_handle,
		     const struct ble_gap_sm_passkey *p_params,
		     void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_sm_key_reply_req_msg, msg,
			p_service_conn, MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ, 0, p_priv);

	msg->params = *p_params;
	msg->conn_handle = conn_handle;

	return cfw_send_message(msg);
}

int ble_update_service_data(cfw_service_conn_t * p_service_conn,
			    uint16_t conn_handle,
			    const struct ble_char_data *p_params, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_update_service_data_msg, msg, p_service_conn,
					MSG_ID_BLE_UPDATE_DATA_REQ,
					p_params->len,
					p_priv);

	msg->update = p_params->update;
	msg->conn_handle = conn_handle;
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
		const struct rssi_report_params *params, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_set_rssi_report_req_msg, msg, p_service_conn, MSG_ID_BLE_RSSI_REQ, 0, p_priv);

	msg->params = *params;
	return cfw_send_message(msg);
}

int ble_service_get_info(cfw_service_conn_t *p_service_conn,
		uint8_t info_type, void *p_priv)
{
	CFW_ALLOC_FOR_SVC(struct ble_gap_get_info_req_msg, msg, p_service_conn,
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
