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

#include "lib/ble/ble_app.h"
#include "ble_app_internal.h"

#include <string.h>
#include <assert.h>
#include "ble_service/ble_service_gap_api.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_app_storage.h"
#include "ble_service_utils.h"
#include "infra/log.h"
#include "ble_protocol.h"
#include "ble_core_interface.h"
#include "ble_service_dis.h"
#if defined(CONFIG_SERVICES_BLE_BAS_USE_BAT)
#include "ble_service_bas.h"
#endif
#if defined(CONFIG_SERVICES_BLE_ISPP)
#include "ble_service_ispp.h"
#endif
#include "cfw/cfw_messages.h"

#include "infra/factory_data.h"
#include "infra/version.h"
#include "curie_factory_data.h"

#include "util/misc.h"
/* boot reason */
#include "infra/boot.h"

/* Default device name */
#define BLE_DEV_NAME "Curie 1.0 ref app"

/* Default security manager configurations */
#define DEFAULT_SM_CFG_IO_CAPABILITY BLE_GAP_IO_NO_INPUT_NO_OUTPUT
#define DEFAULT_SM_CFG_OPTIONS BLE_GAP_BONDING
#define DEFAULT_SM_CFG_KEY_SIZE 16

/* Connection parameters used for Peripheral Preferred Connection Parameters (PPCP) and update request */
#define MIN_CONN_INTERVAL MSEC_TO_1_25_MS_UNITS(80)
#define MAX_CONN_INTERVAL MSEC_TO_1_25_MS_UNITS(151)
#define SLAVE_LATENCY 0
#define CONN_SUP_TIMEOUT MSEC_TO_10_MS_UNITS(6000)

/* Application flags for advertisement start options (must not overlap with core options BLE_ADV_OPTIONS) */
#define BLE_APP_NON_DISC_ADV 0x8000

#define BLE_APP_APPEARANCE BLE_GAP_APPEARANCE_TYPE_GENERIC_WATCH

enum BLE_APP_SVC_REG {
	DIS_SVC = 0,
#if defined(CONFIG_SERVICES_BLE_BAS_USE_BAT)
	BAS_SVC,
#endif
	ISPP_SVC,
	SVC_LAST, /* Keep last, triggers advertisement */
};

struct cback_param {
	int cb_index; /**< register parameter @ref BLE_APP_SVC_REG */
};

struct _ble_register_svc {
	 /**< callback function to execute on MSG_ID_xxx_RSP
	  * @param reg this buffer */
	void (*func_cback)(struct _ble_register_svc *reg);
	struct cback_param cb_param;
};

#define MANUFACTURER_NAME "IntelCorp"
#define MODEL_NUM         "Curie"
#define HARDWARE_REV       "1.0"

static struct ble_app_cb _ble_app_cb = { 0 };
static bool _ble_connection_enabled = false;
/* CFW Client handle */
static cfw_client_t * _client;

int ble_app_conn_update(const struct ble_gap_connection_params * p_params)
{
	if (_ble_connection_enabled)
		return ble_conn_update(_ble_app_cb.p_service_conn,
				_ble_app_cb.conn_handle, p_params, NULL);
	else
		return -1;
}

/** Helper function. */
static int read_string(struct bt_conn *conn,
		       const struct bt_gatt_attr *attr,
		       void *buf, uint16_t buf_len,
		       uint16_t offset,
		       const char *value)
{
	size_t str_len = strlen(value);

	return bt_gatt_attr_read(conn, attr, buf, buf_len, offset, value,
			str_len);
}

/**
 * The following functions overwrite the default functions defined in
 * ble_service_dis.c
 */
int on_dis_rd_manufacturer(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	return read_string(conn, attr, buf, len, offset, MANUFACTURER_NAME);
}

int on_dis_rd_model(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		void *buf, uint16_t len, uint16_t offset)
{
	return read_string(conn, attr, buf, len, offset, MODEL_NUM);
}

int on_dis_rd_serial(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		 void *buf, uint16_t len, uint16_t offset)
{
	uint8_t sn[32];
	uint8_t sn_len;

	/* Check if the factory data contains a serial number otherwise return BD Address */
	if (!memcmp(global_factory_data->oem_data.magic, FACTORY_DATA_MAGIC, 4)) {
		struct oem_data *p_oem = (struct oem_data *)&global_factory_data->oem_data;
		uint8buf_to_ascii(sn, p_oem->uuid, sizeof(p_oem->uuid));
		sn_len = sizeof(sn);
	}
	else {
		uint8buf_to_ascii(sn, _ble_app_cb.my_bd_addr.addr, BLE_ADDR_LEN);
		sn_len = BLE_ADDR_LEN * 2;
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, sn, sn_len);
}

int on_dis_rd_hw_rev(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		 void *buf, uint16_t len, uint16_t offset)
{
	return read_string(conn, attr, buf, len, offset, HARDWARE_REV);
}

int on_dis_rd_fw_rev(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		 void *buf, uint16_t len, uint16_t offset)
{
	char value[21];
	snprintf(value, 21, "%.20s", version_header.version_string);

	return read_string(conn, attr, buf, len, offset, value);
}

int on_dis_rd_sw_rev(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		 void *buf, uint16_t len, uint16_t offset)
{
	char value[21];

	snprintf(value, 21,
			"%d.%d.%d-%02x%02x%02x%02x", version_header.major,
			version_header.minor, version_header.patch,
			version_header.hash[0], version_header.hash[1],
			version_header.hash[2], version_header.hash[3]);
	value[20] = '\0';

	return read_string(conn, attr, buf, len, offset, value);
}

#ifdef CONFIG_BLE_SM_IO_CAP_TEST
int ble_app_store_sm_cfg(struct ble_gap_sm_config_params *sm_params)
{
	ble_status_t status = E_OS_OK;

	pr_info(LOG_MODULE_BLE, "sm cfg cap:%d opt:%d size:%d",
			sm_params->io_caps, sm_params->options,
			sm_params->key_size);
	if (E_OS_OK != ble_properties_save(sm_params,
			sizeof(struct ble_gap_sm_config_params),
			BLE_PROPERTY_ID_SM_CONFIG, _ble_app_cb.p_service_properties_conn))
		status = E_OS_ERR;

	return status;
}
#endif

static void ble_app_enable(struct ble_app_storage_handler *p_hdl, const void *p_name)
{
	struct ble_enable_config en_config = { 0, };
	const struct ble_gap_connection_params conn_params =
			{MIN_CONN_INTERVAL, MAX_CONN_INTERVAL, SLAVE_LATENCY, CONN_SUP_TIMEOUT};
	ble_addr_t bda;
	uint8_t dev_name[BLE_MAX_DEVICE_NAME + 1];

	en_config.appearance = BLE_APP_APPEARANCE;
	en_config.central_conn_params = en_config.peripheral_conn_params = conn_params;

	if (!memcmp(global_factory_data->oem_data.magic, FACTORY_DATA_MAGIC, 4)) {
		struct curie_oem_data *p_oem =
				(struct curie_oem_data *) &global_factory_data->oem_data.project_data;
		if (p_oem->bt_mac_address_type < 2) {
			bda.type = p_oem->bt_mac_address_type;
			memcpy(bda.addr, &p_oem->bt_address[0], sizeof(bda.addr));
			en_config.p_bda = &bda;
		}
		memcpy(dev_name, p_oem->ble_name, BLE_MAX_DEVICE_NAME);
	}
	else {
		memcpy(dev_name, p_name, BLE_MAX_DEVICE_NAME);
	}
	/* Add a terminating 0 to the name */
	dev_name[BLE_MAX_DEVICE_NAME] = 0;

	en_config.p_name = dev_name;
	en_config.sm_config = p_hdl->sm_config;
#ifdef CONFIG_BLE_SM_IO_CAP_TEST
	pr_info(LOG_MODULE_BLE, "sm config io_caps:%d opt:%d size:%d", en_config.sm_config.io_caps,
				en_config.sm_config.options, en_config.sm_config.key_size);
#endif

	if (E_OS_OK != ble_enable(_ble_app_cb.p_service_conn, 1, &en_config,
			&_ble_app_cb)) {
		pr_error(LOG_MODULE_MAIN, "BLE device name too long! BLE ENABLE FAILED");
		return;
	}
	_ble_connection_enabled = true;
}

void ble_app_get_sm_params(struct ble_app_storage_handler *p_hdl,
		const void *p_storage_value)
{
	struct ble_app_storage_handler *p_enable_ble = balloc(sizeof(*p_enable_ble), NULL);
	const struct ble_gap_sm_config_params *p_params = p_storage_value;

	p_enable_ble->cback = ble_app_enable;
	p_enable_ble->param = 0;
	p_enable_ble->sm_config = *p_params;

	/* read device name, will be used at ble enable */
	ble_properties_get(_ble_app_cb.p_service_properties_conn, p_enable_ble, BLE_PROPERTY_ID_DEVICE_NAME);
}

void ble_start_app_handler(cfw_service_conn_t * p_service_conn, void * param)
{
	struct ble_gap_sm_config_params sm_config;
#ifdef CONFIG_BLE_SM_IO_CAP_TEST
	struct ble_app_storage_handler *p_sm_config;
#endif

	if (NULL != _ble_app_cb.p_service_conn) {
		return;
	}
	_ble_app_cb.p_service_conn = p_service_conn;
	_ble_app_cb.conn_handle = BLE_SVC_GAP_HANDLE_INVALID;

	sm_config.io_caps = DEFAULT_SM_CFG_IO_CAPABILITY;
	sm_config.options = DEFAULT_SM_CFG_OPTIONS;
	sm_config.key_size = DEFAULT_SM_CFG_KEY_SIZE;

#ifndef CONFIG_BLE_SM_IO_CAP_TEST
	ble_app_get_sm_params(NULL, &sm_config);
#else
	p_sm_config = balloc(sizeof(*p_sm_config), NULL);
	p_sm_config->cback = ble_app_get_sm_params;
	/* read security manager configuration, will be used at ble enable */
	ble_properties_get(_ble_app_cb.p_service_properties_conn, p_sm_config, BLE_PROPERTY_ID_SM_CONFIG);
#endif

	/* List of events to receive */
	int client_events[] = { MSG_ID_BLE_CONNECT_EVT,
			MSG_ID_BLE_DISCONNECT_EVT,
			MSG_ID_BLE_SECURITY_EVT,
			MSG_ID_BLE_ADV_TO_EVT,
			MSG_ID_BLE_PROTOCOL_EVT,
			MSG_ID_BLE_NAME_EVT
	};
	cfw_register_events(_ble_app_cb.p_service_conn, client_events,
			sizeof(client_events) / sizeof(int), &_ble_app_cb);
}

void ble_app_properties_handler(cfw_service_conn_t * p_service_conn, void * param)
{
	uint8_t *dev_name = NULL;

	if (NULL != _ble_app_cb.p_service_properties_conn) {
		pr_warning(LOG_MODULE_MAIN, "ble_start_app() "
				"failed with: ret: %d", E_OS_ERR);
		return;
	}
	_ble_app_cb.p_service_properties_conn = p_service_conn;

	if (!strncmp((char*)global_factory_data->oem_data.magic, FACTORY_DATA_MAGIC, 4)) {
		struct curie_oem_data *p_oem =
				(struct curie_oem_data *) &global_factory_data->oem_data.project_data;
		dev_name = p_oem->ble_name;
	} else {
		dev_name = (uint8_t *)BLE_DEV_NAME;
	}

	/* Always try to add the name, if the property exists it will fail */
	properties_service_add(p_service_conn,
				BLE_SERVICE_ID, BLE_PROPERTY_ID_DEVICE_NAME,
				false, dev_name,
				BLE_MAX_DEVICE_NAME + 1, NULL);

#ifdef CONFIG_BLE_SM_IO_CAP_TEST
	struct ble_gap_sm_config_params default_sm_config;

	default_sm_config.io_caps = DEFAULT_SM_CFG_IO_CAPABILITY;
	default_sm_config.options = DEFAULT_SM_CFG_OPTIONS;
	default_sm_config.key_size = DEFAULT_SM_CFG_KEY_SIZE;

	/* Always try to add sm configuration, if the property exists it will fail */
	properties_service_add(p_svc_handle,
				BLE_SERVICE_ID, BLE_PROPERTY_ID_SM_CONFIG,
				false, &default_sm_config,
				sizeof(struct ble_gap_sm_config_params), NULL);
#endif

	/* start BLE app when BLE_SERVICE is available */
	cfw_open_service_helper(param, BLE_SERVICE_ID,
			ble_start_app_handler, NULL);
}

static void _ble_start_advertisement(struct ble_app_storage_handler *p_adv,
		const void *p_name)
{
	pr_info(LOG_MODULE_MAIN, "_ble_start_advertisement: adv_type:0x%x",
			p_adv->param);
	struct ble_adv_data_params params = { 0 };
	uint8_t ad[BLE_MAX_ADV_SIZE];
	uint8_t *p = ad;

	/* Sanity check : application options do not overlap with advertisement */
	BUILD_BUG_ON(BLE_APP_NON_DISC_ADV & BLE_ADV_OPTIONS_MASK);

	params.adv_type = BLE_GAP_ADV_TYPE_ADV_IND;
	params.p_ad = ad;

	uint8_t flag = (p_adv->param & BLE_APP_NON_DISC_ADV) ?
			BLE_SVC_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED :
			BLE_SVC_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	p += ble_enc_adv_flags(p, flag);

	p += ble_enc_adv_appearance(p, BLE_APP_APPEARANCE);

	p += ble_enc_adv_manuf(p, INTEL_MANUFACTURER, NULL, 0);

	params.ad_len = p - params.p_ad;

	if (p_name) {
		p += ble_enc_adv_name(p, params.ad_len, p_name,
			strlen((char *)p_name));
	}

	params.ad_len = p - params.p_ad;

	ble_start_advertisement(_ble_app_cb.p_service_conn,
			p_adv->param & BLE_ADV_OPTIONS_MASK, &params, NULL);
}

static void _ble_register_services(struct _ble_register_svc *p_reg)
{
	uint8_t reg_param;

	reg_param = p_reg->cb_param.cb_index++;

	switch (reg_param) {
	case DIS_SVC:
		ble_init_service_dis(_ble_app_cb.p_service_conn, p_reg);
		pr_info(LOG_MODULE_MAIN, "ble registering DIS");
		break;
#if defined(CONFIG_SERVICES_BLE_BAS_USE_BAT)
	case BAS_SVC:
		ble_init_service_bas(_ble_app_cb.p_service_conn, p_reg);
		pr_info(LOG_MODULE_MAIN, "ble registering BAS");
		break;
#endif
#if defined(CONFIG_SERVICES_BLE_ISPP)
	case ISPP_SVC:
		ble_init_service_ispp(_ble_app_cb.p_service_conn, p_reg);
		pr_info(LOG_MODULE_MAIN, "ble registering ISPP");
		break;
#endif
	case SVC_LAST:{
		enum boot_targets boot_event = get_boot_target();
		struct ble_app_storage_handler *p_adv;

		bfree(p_reg);
		p_adv = balloc(sizeof(*p_adv), NULL);

		p_adv->cback = _ble_start_advertisement;
		/* at boot always use ultra fast advertisement to speed up
		 * connection */
		p_adv->param = BLE_ULTRA_FAST_ADV;

		/* in case of reboot in firmware update, no timeout, otherwise
		 * default timeout */
		if (TARGET_RECOVERY == boot_event)
			p_adv->param |= BLE_NO_ADV_TO;

		ble_get_security_status(_ble_app_cb.p_service_conn,
				BLE_SEC_BONDING_DB_STATE, NULL, p_adv);
		break;
		}
	default:
		/* to allow undefined compile time services in the case switch.
		 */
		_ble_register_services(p_reg);
		break;
	}
}

/** Handles BLE enable message */
static void handle_msg_id_ble_enable_rsp(struct cfw_message *msg)
{
	struct ble_enable_rsp *rsp = (struct ble_enable_rsp *)msg;

	if (BLE_STATUS_SUCCESS == rsp->status) {
		struct _ble_register_svc *p_reg = balloc(sizeof(*p_reg), NULL);

		p_reg->func_cback = _ble_register_services;
		p_reg->cb_param.cb_index = DIS_SVC;
		_ble_app_cb.my_bd_addr = rsp->bd_addr;

		/* start registering of the first BLE service (DIS) */
		_ble_register_services(p_reg);

		pr_info(LOG_MODULE_MAIN, "ble_enable_rsp: addr type: %d, "
				"BLE addr: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
				rsp->bd_addr.type,
				rsp->bd_addr.addr[5], rsp->bd_addr.addr[4],
				rsp->bd_addr.addr[3], rsp->bd_addr.addr[2],
				rsp->bd_addr.addr[1], rsp->bd_addr.addr[0]);
	} else {
		pr_error(LOG_MODULE_MAIN, "enable_rsp err %d", rsp->status);
	}
}

/** Handles BLE init service message */
static void handle_msg_id_ble_init_svc_rsp(struct cfw_message *msg)
{
	struct _ble_register_svc *p_reg = CFW_MESSAGE_PRIV(msg);
	struct ble_init_service_rsp * rsp = (struct ble_init_service_rsp *)msg;
	if (BLE_STATUS_SUCCESS == rsp->status)
		p_reg->func_cback(p_reg);
	else
		pr_error(LOG_MODULE_MAIN, "init_svc err %d",
				rsp->status);
}

/** Handles BLE start advertisement message */
static void handle_msg_id_ble_start_adv_rsp(struct cfw_message *msg)
{
	struct ble_rsp * rsp = (struct ble_rsp *)msg;
	if (BLE_STATUS_SUCCESS != rsp->status)
		pr_error(LOG_MODULE_MAIN, "start_adv err %d",
				rsp->status);
}

/** Handles BLE disconnect message */
static void handle_msg_id_ble_connect_evt(struct cfw_message *msg)
{
	/* nothing to do. Store BD Addr */
	struct ble_connect_evt *evt = (struct ble_connect_evt *)msg;
	union ble_set_sec_params params;

	params.dev_status = 0; /* disable pairing state in case it was on */
	ble_set_security_status(_ble_app_cb.p_service_conn, BLE_SEC_DEVICE_STATUS,
			&params, NULL);
	_ble_app_cb.conn_handle = evt->conn_handle;
	pr_info(LOG_MODULE_MAIN, "BLE connected(conn_h: %d, role: %d)",
			evt->conn_handle, evt->role);
}

void ble_app_start_advertisement(uint32_t param)
{
	struct ble_app_storage_handler *p_adv;

	/* only start advertisement if disconnected */
	if (_ble_app_cb.conn_handle == BLE_SVC_GAP_HANDLE_INVALID) {
		p_adv = balloc(sizeof(*p_adv), NULL);
		p_adv->cback = _ble_start_advertisement;
		p_adv->param = param;

		/* check if discoverable advertisement or not (== bonded) */
		ble_get_security_status(_ble_app_cb.p_service_conn,
				BLE_SEC_BONDING_DB_STATE, NULL, p_adv);
	}
}

void ble_app_stop_advertisement(void)
{
	ble_stop_advertisement(_ble_app_cb.p_service_conn, NULL);
}

/** Handles BLE disconnect message */
static void handle_msg_id_ble_disconnect_evt(struct cfw_message *msg)
{
	_ble_app_cb.conn_handle = BLE_SVC_GAP_HANDLE_INVALID;
	ble_app_start_advertisement(BLE_NO_ADV_OPT);
	pr_info(LOG_MODULE_MAIN, "BLE disconnected(conn_h: %d, hci_reason: 0x%x)",
			((struct ble_disconnect_evt *)msg)->conn_handle,
			((struct ble_disconnect_evt *)msg)->reason);
}

/** Handles BLE get security message */
static void handle_msg_id_ble_get_security_rsp(struct cfw_message *msg)
{
	struct ble_get_security_rsp *rsp = (__typeof__(rsp))msg;
	struct ble_app_storage_handler *p_adv = CFW_MESSAGE_PRIV(msg);

	if (BLE_SEC_BONDING_DB_STATE == rsp->op_code) {
		if (BLE_SEC_ST_BONDED_DEVICES_AVAIL == (rsp->dev_status &
				BLE_SEC_ST_BONDED_DEVICES_AVAIL)) {
			/* already bonded */
			p_adv->param |= BLE_APP_NON_DISC_ADV;
			ble_properties_get(_ble_app_cb.p_service_properties_conn, p_adv,
						BLE_PROPERTY_ID_DEVICE_NAME);
			pr_info(LOG_MODULE_MAIN, "Device bonded");
		} else if (BLE_SEC_ST_NO_BONDED_DEVICES == (rsp->dev_status &
				BLE_SEC_ST_NO_BONDED_DEVICES)) {
			union ble_set_sec_params params;
			params.dev_status = BLE_SEC_ST_PAIRABLE;
			ble_set_security_status(_ble_app_cb.p_service_conn,
					BLE_SEC_DEVICE_STATUS, &params,
					p_adv);
			pr_info(LOG_MODULE_MAIN, "Device not bonded");
		}
	} else {
		/* TODO: handle other op_code's */
		pr_error(LOG_MODULE_MAIN, "Wrong BLE security op: %d, state %d",
				rsp->dev_status);
	}
}

/** Handles BLE set security message */
static void handle_msg_id_ble_set_security_rsp(struct cfw_message *msg)
{
	struct ble_set_security_rsp * rsp = (struct ble_set_security_rsp *) msg;
	struct ble_app_storage_handler *p_adv = CFW_MESSAGE_PRIV(msg);

	if (BLE_SEC_DEVICE_STATUS == rsp->op_code) {
		if (BLE_SEC_ST_PAIRABLE ==
				(rsp->dev_status & BLE_SEC_ST_PAIRABLE)) {
			/* advertise will be started after dev_name prop is read */
			ble_properties_get(_ble_app_cb.p_service_properties_conn, p_adv,
					BLE_PROPERTY_ID_DEVICE_NAME);
			/* TODO: call UI function */
		} else {
			/* TODO: call UI function, eg. LED pattern */
		}
	} else {
		/* TODO: handle other op_code's */
		pr_error(LOG_MODULE_MAIN, "Wrong BLE security op: %d, state %d",
				rsp->op_code, rsp->dev_status);
	}
}

/** Handles BLE security event */
static void handle_msg_id_ble_security_evt(struct cfw_message *msg)
{
	struct ble_security_evt * evt_msg = (struct ble_security_evt *) msg;

	switch (evt_msg->sm_status) {
	case BLE_SM_AUTH_DISP_PASSKEY:
		pr_info(LOG_MODULE_MAIN, "VERIFY PIN (conn_h:%d) >> %.6s",
				evt_msg->conn_handle, evt_msg->passkey);
		break;
	case BLE_SM_AUTH_PASSKEY_REQ:
		pr_info(LOG_MODULE_MAIN, "ENTER PIN (conn_h:%d) >>",
				evt_msg->conn_handle);
		break;
	case BLE_SM_PAIRING_START:
		pr_debug(LOG_MODULE_MAIN, ">> PAIRING START (conn_h:%d)",
				evt_msg->conn_handle);
		break;
	case BLE_SM_BONDING_COMPLETE:
		pr_debug(LOG_MODULE_MAIN, "BONDING COMPLETE (conn_h:%d): gap_status: 0x%x",
				evt_msg->conn_handle, evt_msg->gap_status);
		if (evt_msg->gap_status != BLE_SVC_GAP_STATUS_SUCCESS)
			pr_error(LOG_MODULE_BLE, "BONDING FAILED (conn_h:%d): gap_status: 0x%x",
				evt_msg->conn_handle, evt_msg->gap_status);
		break;
	case BLE_SM_LINK_ENCRYPTED:
		pr_debug(LOG_MODULE_MAIN, "ENCRYPTION ON (conn_h:%d)",
				evt_msg->conn_handle);
		break;
	}
}

/** Handles BLE stop advertisement event */
static void handle_msg_id_ble_adv_to_evt(struct cfw_message *msg)
{
	/* when advertisement timedout, start slow advertising without timeout */
	/* read the device name first */
	ble_app_start_advertisement(BLE_SLOW_ADV | BLE_NO_ADV_TO);
}

/** Handles BLE set device name event */
static void handle_msg_id_ble_device_name_evt(struct cfw_message *msg)
{
	struct ble_device_name_evt *evt = (struct ble_device_name_evt *)msg;
	pr_info(LOG_MODULE_BLE,"event change device name to %s", evt->device_name);
	ble_properties_save(evt->device_name, strlen(evt->device_name) + 1,
			BLE_PROPERTY_ID_DEVICE_NAME, _ble_app_cb.p_service_properties_conn);
}

static void handle_ble_messages(struct cfw_message * msg, void * param)
{
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_BLE_ENABLE_RSP:
		handle_msg_id_ble_enable_rsp(msg);
		break;
	case MSG_ID_BLE_INIT_SVC_RSP:
		handle_msg_id_ble_init_svc_rsp(msg);
		break;
	case MSG_ID_BLE_START_ADV_RSP:
		handle_msg_id_ble_start_adv_rsp(msg);
		break;
	case MSG_ID_BLE_GET_SECURITY_RSP:
		handle_msg_id_ble_get_security_rsp(msg);
		break;
	case MSG_ID_BLE_SET_SECURITY_RSP:
		handle_msg_id_ble_set_security_rsp(msg);
		break;
	case MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP:
		/* nothing to do */
		break;
	case MSG_ID_BLE_CONNECT_EVT:
		handle_msg_id_ble_connect_evt(msg);
		break;
	case MSG_ID_BLE_DISCONNECT_EVT:
		handle_msg_id_ble_disconnect_evt(msg);
		break;
	case MSG_ID_BLE_SECURITY_EVT:
		handle_msg_id_ble_security_evt(msg);
		break;
	case MSG_ID_BLE_ADV_TO_EVT:
		handle_msg_id_ble_adv_to_evt(msg);
		break;
	case MSG_ID_PROP_SERVICE_ADD_PROP_RSP:
		break;
	case MSG_ID_PROP_SERVICE_READ_PROP_RSP:
		handle_ble_property_read(msg);
		break;
	case MSG_ID_PROP_SERVICE_WRITE_PROP_RSP:
		break;
	case MSG_ID_BLE_CLEAR_BONDS_RSP:
		pr_info(LOG_MODULE_MAIN, "Pairing cleared");
		break;
	case MSG_ID_BLE_NAME_EVT:
		handle_msg_id_ble_device_name_evt(msg);
		break;
	default:
		pr_debug(LOG_MODULE_MAIN, "Unhandled BLE message");
	}
	cfw_msg_free(msg);
}

void ble_start_app(T_QUEUE queue)
{
	/* Get a client handle */
	_client = cfw_client_init(queue, handle_ble_messages, NULL);
	assert(_client);

	/* Init BLE service */
	ble_cfw_service_init(queue);

	/* Open service */
	cfw_open_service_helper(_client, PROPERTIES_SERVICE_ID,
			ble_app_properties_handler, _client);

	/* Reset the nordic to force sync - Warning: JTAG debugger may prevent reset! */
	ble_core_reset();
}

void ble_app_clear_bonds(void)
{
	ble_clear_bonds(_ble_app_cb.p_service_conn, NULL);
}
