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

#ifndef __BLE_SERVICE_API_H__
#define __BLE_SERVICE_API_H__

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "ble_service.h"
#include "ble_service_gap_api.h"
#include "ble_service_gatt.h"
#include "ble_protocol.h"

#ifdef CONFIG_SERVICES_BLE_ISPP
#include "ble_ispp.h"
#endif

// Forward declarations
struct ble_service_update_data_rsp;
struct _ble_service_cb;

/**
 * @defgroup ble_service_api BLE Service API
 *
 * Define the interface for BLE service.
 *
 * @ingroup ble_service
 * @{
 */

/* Uncomment to exercise the BLE test service */
/* #define BLE_SVC_TEST 1 */

#ifdef BLE_SVC_TEST
//#define BLE_PERF_STUDY 1
extern int ble_svc_test(void *p_queue);
#endif

/** BLE High level Message IDs used for request, response, events. */
enum BLE_MSG_ID {
	MSG_ID_BLE_ENABLE_REQ = MSG_ID_BLE_BASE,  /**< Message ID for <i>enable</i> request */
	MSG_ID_BLE_SET_NAME_REQ,                  /**< Message ID for <i>set name</i> request */
	MSG_ID_BLE_START_DISCOVER_REQ,            /**< Message ID for <i>start discover</i> request */
	MSG_ID_BLE_STOP_DISCOVER_REQ,             /**< Message ID for <i>stop discover</i> request */
	MSG_ID_BLE_CONNECT_REQ,                   /**< Message ID for <i>connect</i> request */
	MSG_ID_BLE_DISCONNECT_REQ,                /**< Message ID for <i>disconnect</i> request */
	MSG_ID_BLE_CONN_UPDATE_REQ,               /**< Message ID for <i>subscribe</i> request */
	MSG_ID_BLE_SUBSCRIBE_REQ,                 /**< Message ID for <i>conn update</i> request */
	MSG_ID_BLE_UNSUBSCRIBE_REQ,               /**< Message ID for <i>unsubscribe</i> request */
	MSG_ID_BLE_INIT_SVC_REQ,                  /**< Message ID for <i>init service</i> request */
	MSG_ID_BLE_START_ADV_REQ,                 /**< Message ID for <i>start advertisement</i> request */
	MSG_ID_BLE_STOP_ADV_REQ,                  /**< Message ID for <i>stop advertisement</i> request */
	MSG_ID_BLE_DISABLE_SVC_REQ,               /**< Message ID for <i>disable service</i> request */
	MSG_ID_BLE_GET_SECURITY_REQ,              /**< Message ID for <i>get security</i> request */
	MSG_ID_BLE_SET_SECURITY_REQ,              /**< Message ID for <i>set security</i> request */
	MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ,        /**< Message ID for <i>passkey send reply</i> request */
	MSG_ID_BLE_CLEAR_BONDS_REQ,
	MSG_ID_BLE_GET_VERSION_REQ,               /**< Message ID for <i>get version</i> request */
	MSG_ID_BLE_RSSI_REQ,                      /**< Message ID for <i>RSSI</i> request */
	MSG_ID_BLE_GET_INFO_REQ,                  /**< Message ID for <i>get info</i> request */

	/* client data requests */
	MSG_ID_BLE_DISCOVER_SVC_REQ, /**< discover characteristics of a service */
	MSG_ID_BLE_GET_REMOTE_DATA_REQ,
	MSG_ID_BLE_SET_REMOTE_DATA_REQ,

	/* server data requests */
	MSG_ID_BLE_UPDATE_DATA_REQ,

	/* Proprietary Protocol */
	MSG_ID_BLE_PROTOCOL_REQ,

	/* BLE direct test mode command */
	MSG_ID_BLE_DTM_REQ,
	MSG_ID_BLE_DBG_REQ,
	MSG_ID_BLE_REQ_LAST,

	MSG_ID_BLE_ENABLE_RSP = MSG_ID_BLE_RSP, /**< Message ID for <i>enable</i> response */
	MSG_ID_BLE_SET_NAME_RSP,                /**< Message ID for <i>set name</i> response */
	MSG_ID_BLE_START_DISCOVER_RSP,          /**< Message ID for <i>start discover</i> response */
	MSG_ID_BLE_STOP_DISCOVER_RSP,           /**< Message ID for <i>stop discover</i> response */
	MSG_ID_BLE_CONNECT_RSP,                 /**< Message ID for <i>connect</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_DISCONNECT_RSP,              /**< Message ID for <i>disconnect</i> response, of type @ref ble_disconnect_rsp */
	MSG_ID_BLE_SUBSCRIBE_RSP,               /**< Message ID for <i>subscribe</i> response, of type @ref ble_service_rsp */
	MSG_ID_BLE_CONN_UPDATE_RSP,             /**< Message ID for <i>conn update</i> response, of type @ref ble_conn_update_rsp */
	MSG_ID_BLE_UNSUBSCRIBE_RSP,             /**< Message ID for <i>unsubscribe</i> response, of type @ref ble_service_rsp */
	MSG_ID_BLE_INIT_SVC_RSP,                /**< Message ID for <i>init service</i> response, of type @ref ble_init_service_rsp*/
	MSG_ID_BLE_START_ADV_RSP,               /**< Message ID for <i>start advertisement</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_STOP_ADV_RSP,                /**< Message ID for <i>stop advertisement</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_DISABLE_SVC_RSP,             /**< Message ID for <i>disable service</i> response, of type @ref ble_disable_service_rsp */
	MSG_ID_BLE_GET_SECURITY_RSP,            /**< Message ID for <i>get security</i> response, of type @ref ble_get_security_rsp */
	MSG_ID_BLE_SET_SECURITY_RSP,            /**< Message ID for <i>set security</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP,      /**< Message ID for <i>passkey send reply</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_CLEAR_BONDS_RSP,
	MSG_ID_BLE_GET_VERSION_RSP,             /**< Message ID for <i>get version</i> response, of type @ref ble_version_rsp */
	MSG_ID_BLE_RSSI_RSP,                    /**< Message ID for <i>RSSI</i> response, of type @ref ble_version_rsp */
	MSG_ID_BLE_GET_INFO_RSP,                /**< Message ID for <i>get info</i> response, of type @ref ble_get_info_rsp */

	/* client data requests */
	MSG_ID_BLE_DISCOVER_SVC_RSP,            /**< discover characteristics of a service @ref ble_service_rsp */
	MSG_ID_BLE_GET_REMOTE_DATA_RSP,         /**< @ref ble_service_rsp */
	MSG_ID_BLE_SET_REMOTE_DATA_RSP,         /**< @ref ble_service_rsp */

	/* server data requests */
	MSG_ID_BLE_UPDATE_DATA_RSP,             /**< service server data updated */

	/* Proprietary Protocol */
	MSG_ID_BLE_PROTOCOL_RSP,

	/* BLE direct test mode command */
	MSG_ID_BLE_DTM_RSP,                     /**< @ref ble_dtm_result_msg */
	MSG_ID_BLE_DBG_RSP,
	MSG_ID_BLE_RSP_LAST,

	/* events */
	MSG_ID_BLE_DISCOVER_EVT = MSG_ID_BLE_EVT,
	MSG_ID_BLE_NOTIF_EVT,            /**< notification or indication data when subscribe has been called */
	MSG_ID_BLE_GET_REMOTE_DATA_EVT,  /**< Message ID for struct @ref ble_get_remote_data_evt  */
	MSG_ID_BLE_SVC_DATA_CHANGED_EVT, /**< service data has changed, incoming data
	                                      (struct @ref ble_service_data_changed_evt). */
	MSG_ID_BLE_CONNECT_EVT,          /**< Message ID for struct @ref ble_connect_evt */
	MSG_ID_BLE_DISCONNECT_EVT,       /**< Message ID for struct @ref ble_disconnect_evt */
	MSG_ID_BLE_SUBSCRIBE_EVT,        /**< Message ID for struct @ref ble_subscribe_evt */
	MSG_ID_BLE_UNSUBSCRIBE_EVT,      /**< Message ID for struct @ref ble_subscribe_evt */
	MSG_ID_BLE_DISCOVER_SVC_EVT,     /**< Message ID for struct @ref ble_discover_service */
	MSG_ID_BLE_SECURITY_EVT,         /**< Message ID for struct @ref ble_security_evt */
	MSG_ID_BLE_ADV_TO_EVT,           /**< Message ID for struct @ref ble_adv_to_evt */
	MSG_ID_BLE_PROTOCOL_EVT,         /**< Message ID for struct @ref ble_ispp_event_s */
	MSG_ID_BLE_RSSI_EVT,             /**< Message ID for struct @ref ble_rssi_evt */
	MSG_ID_BLE_NAME_EVT,             /**< Message ID for struct @ref ble_device_name_evt */
	MSG_ID_BLE_EVT_LAST
};

/**
 * BLE Enable options.
 */
enum BLE_OPTIONS {
	BLE_OPTION_NONE = 0,
	BLE_OPTION_TEST_MODE = 0x000000001, /**< Enable BLE Service in Test mode */
};

/**
 * BLE Enable configuration options.
 */
struct ble_enable_config {
	uint8_t * p_name;    /**< Optional null terminated BLE GAP Service device name. */
	ble_addr_t * p_bda;  /**< Optional BT device address. If NULL, static random will be used */
	uint32_t options;    /**< Enable options @ref BLE_OPTIONS */
	struct ble_gap_connection_params peripheral_conn_params; /**< Peripheral preferred */
	struct ble_gap_connection_params central_conn_params;    /**< Central supported range */
	uint16_t appearance; /**< see BLE spec */
	struct ble_gap_sm_config_params sm_config;
};

/**
 * Advertisement options.
 */
enum BLE_ADV_OPTIONS {
	BLE_NO_ADV_OPT = 0,        /**< Default: Fast advertisement interval, 180sec timeout */
	BLE_SLOW_ADV = 0X01,       /**< Slow advertisement interval */
	BLE_ULTRA_FAST_ADV = 0X02, /**< Ultra fast advertisement interval */
	BLE_SHORT_ADV_TO = 0x04,   /**< Short advertisement timeout */
	BLE_NO_ADV_TO = 0x08,      /**< NO advertisement timeout*/
	BLE_NON_DISC_ADV = 0x10,   /**< Non-discoverable advertisement, minimum advertisement data */
	BLE_ADV_OPTIONS_MASK = 0xFF/**< Mask of the advertisement options */
};

/**
 * BLE notification configuration (batching).
 * TODO: define notification types
 */
struct ble_notification_config {
	uint32_t mode;
};

/** Parameters of MSG_ID_BLE_ENABLE_RSP. */
struct ble_enable_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< Response status @ref BLE_STATUS */
	uint8_t enable;            /**< enable state: 0:Disabled, 1:Enabled */
	ble_addr_t bd_addr;
};

/** Parameters of MSG_ID_BLE_GET_VERSION_RSP. */
struct ble_version_rsp {
	struct cfw_message header;     /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;           /**< Response status @ref BLE_STATUS */
	struct version_header version; /**< BLE Core version */
};

enum BLE_INFO_REQ_TYPES {
  BLE_INFO_BDA_NAME_REQ = 0, /**< Get BDA and current device name info */
};

/**
 * Info response data.
 */
struct ble_bda_name_info {
	ble_addr_t bda; /**< Current BD address */
	uint8_t name_len;
	uint8_t name[]; /**< Current BLE GAP name */
};

/**
 * BLE Info response parameters.
 */
union ble_info_rsp_params {
	struct ble_bda_name_info bda_name_params; /**< BLE address and name */
};

/** Parameters of MSG_ID_BLE_GET_INFO_RSP. */
struct ble_get_info_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status; /**< Response status @ref BLE_STATUS */
	uint8_t info_type;
	union ble_info_rsp_params info_params;
};

/**
 * Data structure of MSG_ID_BLE_DISCOVER_EVT.
 */
struct ble_discover_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_addr_t ble_addr;       /**< Address of the peer device */
	uint16_t svc_count;
	struct bt_uuid *service;   /**< List of primary services. If no browse is true, this is null */
	uint8_t *device_name;      /**< Device name */
	uint16_t appearance;       /**< Appearance */
	int8_t rssi;               /**< rssi */
	uint8_t tx_power;          /**< tx_power */
	uint16_t conn_int;         /**< connection interval */
	/* allocate here space for services discovered */
};

/**
 * Data structure for @ref MSG_ID_BLE_CONNECT_EVT.
 */
struct ble_connect_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint16_t conn_handle;      /**< Connection handle */
	uint8_t role;              /**< GAP Roles */
	ble_addr_t ble_addr;       /**< Address of the peer device */
};

struct ble_rssi_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	int8_t rssi;               /**< rssi */
};

struct ble_device_name_evt {
	struct cfw_message header;
	uint8_t device_name[];
};

/**
 * Generic connection related response or event.
 */
struct ble_generic_conn_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< @ref BLE_STATUS */
	uint16_t conn_handle;      /**< Connection handle */
};

/**
 * Generic service related response or event.
 */
struct ble_service_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< @ref BLE_STATUS */
	uint16_t conn_handle;      /**< Connection handle */
	uint16_t svc_handle;       /**< Service Handle */
};

/**
 * Message Structure for @ref MSG_ID_BLE_DISCONNECT_EVT.
 */
struct ble_disconnect_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint16_t conn_handle;      /**< Connection handle */
	uint16_t reason;           /**< Disconnection reason */
};

/**
 * Message Data Structure for @ref MSG_ID_BLE_DISCONNECT_RSP.
 */
struct ble_disconnect_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint16_t conn_handle;      /**< Connection handle */
};

/**
 * Message Data Structure for @ref MSG_ID_BLE_CONN_UPDATE_RSP.
 */
struct ble_conn_update_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint16_t conn_handle;      /**< Connection handle */
};

/**
 * Data structure of @ref MSG_ID_BLE_SUBSCRIBE_EVT.
 * This is triggered by remote server confirmation
 */
struct ble_subscribe_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint16_t conn_handle;      /**< Connection handle */
	uint16_t svc_handle;       /**< GAP, etc. */
	uint32_t svc_uuid;         /**< 16 or 32 UUID of service. */
	uint8_t uuid_type;         /**< UUID type. */
};

/**
 * Message Structure for MSG_ID_BLE_GET_REMOTE_DATA_EVT.
 */
struct ble_get_remote_data_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< @ref BLE_STATUS. */
	uint16_t conn_handle;      /**< Connection handle */
	uint16_t char_handle;
	uint16_t len;
	uint8_t data[];
};

/**
 * BLE service parameter for initialization.
 */
struct ble_svc_params {
	struct bt_uuid svc_uuid; /**< UUID of service */
	void *p_data;            /**< service specific init data */
};

/**
 * Event Data Structure for @ref MSG_ID_BLE_DISCOVER_SVC_EVT.
 */
struct ble_discover_service {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint16_t conn_handle;
	uint16_t svc_handle;
	uint16_t count;            /**< number of service characteristics */
	uint16_t chars[];          /**< handles of the characteristics of the service */
};

/**
 * Event Data Structure for @ref MSG_ID_BLE_INIT_SVC_RSP.
 */
struct ble_init_service_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	struct bt_uuid svc_uuid;   /**< 16 or 128 bit Service UUID */
};

/**
 * Generic service related response or event.
 */
struct ble_disable_service_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint16_t svc_handle;       /**< Service Handle */
};

/** Security related op codes. */
enum BLE_SEC_OPCODES {
	BLE_SEC_DEVICE_STATUS = 0,    /**< get security device status, e.g pairable/bondable @ref BLE_SEC_ST_DEVICE_STATUS,
	                                   takes no parameters */
	BLE_SEC_CONN_STATE,           /**< current security state of connection, connection handle as parameter */
	BLE_SEC_AUTHENTICATION,       /**< only valid for event or set operation */
	BLE_SEC_BONDING_DB_STATE,     /**< return return state of bonded device database @ref BLE_SEC_ST_DEVICE_STATUS,
	                                   takes no parameter */
	BLE_SEC_BONDED_DEVICES,       /**< Get (@ref ble_get_security_status) bonded devices (stored in flash),
	                                   takes no parameters */
	BLE_SEC_BONDED_DEVINFO,       /**< Get (@ref ble_get_security_status) device info of one device retrieved via
	                                   @ref BLE_SEC_BONDED_DEVICES, takes bd address */
};

/** Security manager related op codes. */
enum BLE_SM_STATUS_CODES {
	BLE_SM_AUTH_NONE = 0,      /**< No security */
	BLE_SM_AUTH_DISP_PASSKEY,  /**< Display/verify passkey */
	BLE_SM_AUTH_PASSKEY_REQ,   /**< Enter required Passkey */
	BLE_SM_PAIRING_START,      /**< Start pairing */
	BLE_SM_BONDING_COMPLETE,   /**< Bonding complete */
	BLE_SM_LINK_ENCRYPTED,     /**< Link encrypted */
};

/**
 * Security relates state and status definitions.
 */
enum BLE_SEC_ST_DEVICE_STATUS {
	BLE_SEC_ST_IDLE = 0x0000,            /**< idle state, pairing requests will fail, @ref BLE_SEC_DEVICE_STATUS */
	BLE_SEC_ST_PAIRABLE = 0x0001,        /**< Pairing allowed application will get pin requests
	                                          if supported security options of device */
	BLE_SEC_ST_PAIRING_IN_PROG = 0x0002, /**< Pairing in progress */
	BLE_SEC_ST_BONDING_SUCCESS = 0x0004, /**< Pairing resulted in bonding success and device is stored in database */
	BLE_SEC_ST_PAIRING_SUCCESS = 0x0008, /**< Pairing succeeded but only stored temporarily (not stored in flash) */
	BLE_SEC_ST_PAIRING_FAILED = 0x0010,  /**< Pairing failed */
	BLE_SEC_ST_BONDED_DEVICES_AVAIL = 0x0080, /**< At least one bonded device in database, @ref BLE_SEC_BONDING_DB_STATE */
	BLE_SEC_ST_NO_BONDED_DEVICES = 0x0100, /**< Bonding database is empty , @ref BLE_SEC_BONDING_DB_STATE */
};

/**
 * Parameters for ble_get_security_status.
 *
 * @note Not all @ref BLE_SEC_OPCODES takes parameters, in this case pass null
 */
union ble_get_security_params {
	uint16_t conn_handle; /**< @ref BLE_SEC_CONN_STATE */
};

/**
 * Connection related security structure
 */
struct ble_conn_sec {
	uint16_t conn_handle;
	uint8_t mode;
};

/**
 * Get Security Response data structure.
 */
struct ble_get_security_rsp {
	struct cfw_message header;   /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint8_t op_code;             /**< @ref BLE_SEC_OPCODES */
	union {
		uint16_t dev_status; /**< @ref BLE_SEC_DEVICE_STATUS  */
		struct ble_conn_sec sec_mode; /**< @ref BLE_SEC_CONN_STATE */
	};
};

/**
 * Set Security State Response data structure.
 */
struct ble_set_security_rsp {
	struct cfw_message header;   /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint8_t op_code;             /**< @ref BLE_SEC_OPCODES */
	union {
		uint16_t dev_status; /**< @ref BLE_SEC_DEVICE_STATUS  */
		struct ble_conn_sec sec_mode; /**< @ref BLE_SEC_CONN_STATE */
	};
};

/**
 * Authentication data.
 */
struct ble_auth_data {
	union {
		uint8_t passkey[6];   /**< 6 digit key (000000 - 999999), only valid for @ref BLE_SM_AUTH_PASSKEY_REQ */
		uint8_t obb_data[16]; /**< 16 byte of OBB data */
	};
	uint8_t type;                 /**< @ref BLE_GAP_SM_PASSKEY_TYPE */
};

/**
 * Security event data structure type @ref BLE_SEC_AUTHENTICATION.
 */
struct ble_security_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint16_t conn_handle;      /**< Connection handle */
	uint8_t sm_status;         /**< @ref BLE_SM_STATUS_CODES */
	union {
		uint8_t passkey[6]; /**< 6 digit key (000000 - 999999), only valid for @ref BLE_SM_AUTH_DISP_PASSKEY */
		uint8_t type;       /**< key requested @ref BLE_GAP_SM_PASSKEY_TYPE, for @ref BLE_SM_AUTH_PASSKEY_REQ */
		uint8_t gap_status; /**< gap status, 0 success otherwise failure code. @ref BLE_SM_BONDING_COMPLETE */
	};
};

/**
 * Security data parameter structure.
 */
union ble_set_sec_params {
	uint16_t dev_status;       /**< @ref BLE_SEC_DEVICE_STATUS */
	struct ble_conn_sec conn;  /**< @ref BLE_SEC_CONN_STATE */
	struct ble_auth_data auth; /**< @ref BLE_SEC_AUTHENTICATION */
};

/**
 * BLE service specific update data function
 *
 * @param conn_handle Handle of connection
 * @param char_handle Value handle of characteristic to update
 * @param len of payload data in this message
 * @param p_data points to data[]
 * @param p_priv udpate response sent back to callee.
 * @param p_cb ble master control block
 * @return BLE_STATUS_SUCCESS for success, otherwise BLE failure code.
 */
typedef int (*update_data_t)(uint16_t conn_handle, uint16_t char_handle, uint16_t len, uint8_t *p_data,
		struct ble_service_update_data_rsp *p_rsp);

/**
 * GATT server characteristic data to update.
 */
struct ble_char_data {
	update_data_t update; /**< Update data execution function executed in ble_service context */
	uint16_t char_handle; /**< Handle of characteristic */
	uint16_t len;
	uint8_t *p_data;      /**< data depending on service */
};

/**
 * Service characteristic data update response.
 */
struct ble_service_update_data_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint16_t conn_h;           /**< Connection handle, maybe invalid if there is no connection! */
	uint16_t char_handle;      /**< handle of characteristic value to write */
};

/**
 * Service service characteristic values has been updated @ref MSG_ID_BLE_SVC_DATA_CHANGED_EVT.
 */
struct ble_service_data_changed_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint16_t svc_handle;       /**< which service affected by the change */
	uint16_t char_handle;      /**< characteristic value handle that has been updated */
	uint16_t count;            /**< number of data samples, by default 1 */
	uint16_t len;              /**< length of data written by remote device */
	uint8_t data[];
};

/**
 * Service service characteristic values has been updated @ref MSG_ID_BLE_NOTIF_EVT.
 */
struct ble_notification_data_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< @ref ble_status_t */
	uint16_t conn_handle;      /**< connection handle */
	uint16_t svc_handle;       /**< which service affected by the change */
	uint16_t char_handle;      /**< characteristic value handle that has been updated */
	uint16_t count;            /**< number of data samples, by default 1. */
	uint16_t len;              /**< length of data written by remote device */
	uint8_t data[];
};

/**
 * BLE Protocol identifier.
 */
enum BLE_PROTOCOL {
	BLE_LAST_NO_USED
};

struct ble_protocol_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< @ref ble_status_t */
	uint16_t conn_handle;      /**< Connection handle */
	int protocol_status;       /**< Protocol specific status. */
};


/**
 * Open or data event has been received via protocol service @ref MSG_ID_BLE_PROTOCOL_EVT.
 */
struct ble_protocol_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t protocol;          /**< @ref BLE_PROTOCOL */
};

/**
 * Advertisement timeout ended @ref MSG_ID_BLE_ADV_TO_EVT.
 */
struct ble_adv_to_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
};

/**
 * Advertising data parameter structure.
 * The parameters modify the advertisement data content of
 * the generated advertisement array.
 */
struct ble_adv_data_params {
	uint8_t adv_type; /**< see @ref GAP_ADV_TYPES */
	uint8_t sd_len;
	uint8_t ad_len;
	ble_addr_t *p_le_addr;
	uint8_t *p_sd;
	uint8_t *p_ad;
};

/**
 * BLE debug req and rsp message.
 */
struct ble_dbg_msg {
	struct cfw_message header;
	uint32_t u0;
	uint32_t u1;
};

/**
 * Initialize cfw BLE service framework and register with service manager.
 *
 * To be called at boot time if BLE is supported
 *
 * @param queue cfw BLE service message queue
 */
void ble_cfw_service_init(T_QUEUE queue);

void on_ble_core_up(void);


/** Enable/Disable BLE stack. To be called before any BLE service related call.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param enable 1: enable BLE stack 0: disable BLE stack
 * @param p_config configuration parameters when enabling BLE. shall be null in case of BLE disable. @ref ble_enable_config
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with @ref MSG_ID_BLE_ENABLE_RSP and type @ref ble_enable_rsp.
 */
int ble_enable(cfw_service_conn_t * p_service_conn, uint8_t enable,
		const struct ble_enable_config * p_config, void *p_priv);

/**
 * Set/Change local BLE name.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_name Local Name of the Local BLE device
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SET_NAME_RSP and type @ref ble_rsp.
 */
int ble_set_name(cfw_service_conn_t * p_service_conn,
		 const uint8_t * p_name,
		 void *p_priv);

/**
 * Scan for remote devices (Central Role) optionally specifying the profile/services UUIDs.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param cnt number of UUIDs, must match array
 * @param p_services null no specific services, otherwise array of size cnt
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_START_DISCOVER_RSP and type @ref ble_rsp.
 */
int ble_start_discover(cfw_service_conn_t * p_service_conn, size_t cnt,
		       const uint16_t * p_services, void *p_priv);

/**
 * Stop Scanning for remote devices.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_STOP_DISCOVER_RSP and type @ref ble_rsp.
 */
int ble_stop_discover(cfw_service_conn_t * p_service_conn,
		      void *p_priv);

/**
 * Connect to a remote sensor/device and optionally browse remote services.
 *
 * - Connect to the remote device.
 * - GAP is connected state on receiving connect event.
 * - GATT is connected too if do_not_browse is not set.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_addr address of the peer device
 * @param interval connection time interval Range: 0x0006 to 0x0C80, (unit: 1.25 ms)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_CONNECT_RSP and type @ref ble_rsp.
 */
int ble_connect(cfw_service_conn_t * p_service_conn, const ble_addr_t * p_addr,
		uint32_t interval, void *p_priv);

/**
 * Enable notification from remote sensors.
 *
 * Subscribe to BLE notification of service. At least service handle must be used. if no
 * characteristic handle is specified, all characteristic are made notify-able.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle handle of connection
 * @param svc_handle BLE stack service handle returned on service browse or connect events
 * @param p_char_handle pointer to characteristics handle. maybe null
 * @param p_config notification configuration TBD
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SUBSCRIBE_RSP and type @ref ble_service_rsp.
 */
int ble_subscribe(cfw_service_conn_t * p_service_conn, uint16_t conn_handle,
		  uint16_t svc_handle, const uint16_t * p_char_handle,
		  const struct ble_notification_config *p_config, void *p_priv);

/**
 * Disable notification from remote sensors.
 *
 * Unsubscribe from BLE notification of service. At least service handle must be used. if no
 * characteristic handle is specified, all characteristic are made non notify-able.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param svc_handle BLE service handle returned on service browse or connect events
 * @param p_char_handle pointer to characteristics handle. maybe null
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_UNSUBSCRIBE_RSP.
 */
int ble_unsubscribe(cfw_service_conn_t * p_service_conn,
		    uint16_t svc_handle, const uint16_t * p_char_handle,
		    void *p_priv);

/**
 * Request to update the connection.
 *
 * The behavior of this function depends on the role of the device on the
 * connection:
 * - if peripheral, it will send the request based on the values in the
 *   <i>p_params</i> argument.
 * - if central, it will enforce updating the values to the values configured
 *   in <i>p_params</i> argument.
 *   The interval_min will be used as the forced value.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle The connection handle
 * @param p_params The new requested connection parameters
 * @param p_priv pointer to private structure returned in the response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_UNSUBSCRIBE_RSP.
 */
int ble_conn_update(cfw_service_conn_t * p_service_conn,
		    uint16_t conn_handle,
		    const struct ble_gap_connection_params * p_params,
		    void *p_priv);

/**
 * Disconnect remote device/sensor (peripheral & central role).
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle link to disconnect
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_DISCONNECT_RSP and type @ref ble_service_rsp.
 */
int ble_disconnect(cfw_service_conn_t * p_service_conn,
		   uint16_t conn_handle, void *p_priv);

/**
 * Enable/Disable  BLE test mode (e.g. DTM).
 *
 * This command allows send the DTM defined by BT spec. They are relayed to the BLE controller
 * which has been put into DTM mode by calling @ref ble_enable with @ref BLE_OPTION_TEST_MODE.
 * The commands DTM commands are relayed to the BLE controller.
 * To exit DTM, the chip needs to be reset.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_cmd Test command to execute
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_DTM_RSP.
 */
int ble_test(cfw_service_conn_t * p_service_conn,
	     const struct ble_test_cmd *p_cmd,
	     void *p_priv);

/**
 * Discover characteristics UUID and its handles for a specified service.
 *
 * @note client role
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle
 * @param svc_handle handle of service to discover
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_DISCOVER_SVC_RSP and type @ref ble_service_rsp.
 */
int ble_discover_service(cfw_service_conn_t * p_service_conn,
			 uint16_t conn_handle,
			 uint16_t svc_handle,
			 void *p_priv);

/**
 * Read remote data.
 *
 * @note  Client Role
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle
 * @param char_handle handle of char value to write
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_GET_REMOTE_DATA_RSP and type @ref ble_service_rsp.
 *       - Event with ID @ref MSG_ID_BLE_GET_REMOTE_DATA_EVT and type @ref ble_get_remote_data_evt.
 */
int ble_get_remote_data(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle,
			uint16_t char_handle,
			void *p_priv);

/**
 * Set remote data on a remote service.
 *
 * @note client role usage
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle
 * @param val_handle handle of handle of value to write
 * @param p_value data to set
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SET_REMOTE_DATA_RSP and type @ref ble_service_rsp.
 */
int ble_set_remote_data(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle,
			uint16_t val_handle,
			const uint8_t * p_value,
			void *p_priv);

/**
 * Set advertisement data and start advertisement.
 *
 * This will set that advertisement and scan response data.
 * If there is already an ongoing advertisement, the advertisement data (and/or scan response)
 * will be simply updated with the data in the stack. If the options need to be changed,
 * the application needs to do first a ble_stop_advertisment().
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param options service specific options
 * @param p_adv_params advertising parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_START_ADV_RSP and type @ref ble_rsp.
 *       - status BLE_STATUS_WRONG_STATE: advertisement already started, only
 *         advertisement data was updated
 *       - status BLE_STATUS_ERROR_PARAMETER: incorrect advertisement parameters
 *       - status BLE_STATUS_ERROR: generic error for other possible error cases
 *       - status BLE_STATUS_SUCCESS
 */
int ble_start_advertisement(cfw_service_conn_t *p_service_conn,
		uint32_t options,
		const struct ble_adv_data_params *p_adv_params,
		void *p_priv);

/**
 * Stop advertisement of initialized services
 *
 * This will stop the advertisement.
 * Initialized services also
 * If services need to be added this maybe required via ble_init_service().
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_STOP_ADV_RSP and type @ref ble_rsp.
 */
int ble_stop_advertisement(cfw_service_conn_t * p_service_conn,
			   void *p_priv);

/**
 * Disable a BLE service.
 *
 * This will update the advertisement data too and may result in stop-start sequence
 * for the advertisement process
 * If services need to be added this maybe required via ble_init_service().
 *
 * @note Currently NOT supported
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle
 * @param svc_handle handle of service to stop
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_DISABLE_SVC_RSP and type @ref ble_service_rsp.
 */
int ble_disable_service(cfw_service_conn_t * p_service_conn,
			uint16_t conn_handle,
			uint16_t svc_handle,
			void *p_priv);

/**
 * Get Current security status
 *
 * This will return the security status or state depending on operation requested.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param op security operation @ref BLE_SEC_OPCODES
 * @param p_params parameters related to security operation
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_GET_SECURITY_RSP and type @ref ble_get_security_rsp.
 */
int ble_get_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_get_security_params * p_params,
			    void *p_priv);

/**
 * Set Current security status
 *
 * This will set the security status or state depending on operation requested.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param op security operation
 * @param p_params parameters related to security operation
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SET_SECURITY_RSP and type @ref ble_get_security_rsp.
 */
int ble_set_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_set_sec_params * p_params,
			    void *p_priv);

/**
 * Send authentification passkey.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle
 * @param p_params passkey params
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_PASSKEY_SEND_RSP (status) and type @ref ble_service_rsp.
 */
int ble_send_passkey(cfw_service_conn_t *p_service_conn,
		     uint16_t conn_handle,
		     const struct ble_gap_sm_passkey * p_params,
		     void *p_priv);

/**
 * Update GATT server characteristic value data.
 *
 * This will set the server data characteristic depending on connection handle.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn_handle connection handle; maybe invalid (0xffff)
 * @param p_params characteristic data to write
 * @param p_priv pointer to private structure returned in a response
 *
 * @note In general each BLE service will implement it own function udpate function.
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_UPDATE_DATA_RSP and type @ref ble_service_update_data_rsp.
 */
int ble_update_service_data(cfw_service_conn_t *p_service_conn,
			    uint16_t conn_handle,
			    const struct ble_char_data *p_params,
			    void *p_priv);

/**
 * Request to send RSSI report request
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param params RSSI report paramaters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_RSSI_RSP.
 */
int ble_set_rssi_report(cfw_service_conn_t * p_service_conn,
		const struct rssi_report_params *params, void *p_priv);

/**
 * Request to read ble_core version
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_GET_VERSION_RSP.
 */
int ble_service_get_version(cfw_service_conn_t *p_service_conn, void *p_priv);

/**
 * Request to read device information (current BD address and GAP name)
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param info_type type of information to be returned
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 */
int ble_service_get_info(cfw_service_conn_t *p_service_conn,
		uint8_t info_type, void *p_priv);

/**
 * Request to clear BLE bonds
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 */
int ble_clear_bonds(cfw_service_conn_t * p_service_conn, void *p_priv);
/** @} */

#endif /* __BLE_SVC_API_H__ */
