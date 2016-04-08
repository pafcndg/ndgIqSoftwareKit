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

// For MSG_ID_BLE_BASE
#include "ble_service_msg.h"
#include "ble_service.h"
#include "ble_service_gap_api.h"

// For uuid
#include "zephyr/bluetooth/uuid.h"
// For attribute
#include "zephyr/bluetooth/gatt.h"


/* for for advertisement parameter type & adv data elements, bt_eir */
#include "zephyr/bluetooth/bluetooth.h"

#ifdef CONFIG_SERVICES_BLE_ISPP
#include "ble_ispp.h"
#endif

// Forward declarations
struct _ble_service_cb;
struct ble_update_data_rsp;
struct bt_conn;

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
	MSG_ID_BLE_CONN_UPDATE_REQ,               /**< Message ID for <i>conn update</i> request */
	MSG_ID_BLE_SUBSCRIBE_REQ,                 /**< Message ID for <i>subscribe</i> request */
	MSG_ID_BLE_UNSUBSCRIBE_REQ,               /**< Message ID for <i>unsubscribe</i> request */
	MSG_ID_BLE_INIT_SVC_REQ,                  /**< Message ID for <i>init service</i> request */
	MSG_ID_BLE_START_ADV_REQ,                 /**< Message ID for <i>start advertisement</i> request */
	MSG_ID_BLE_STOP_ADV_REQ,                  /**< Message ID for <i>stop advertisement</i> request */
	MSG_ID_BLE_GET_SECURITY_REQ,              /**< Message ID for <i>get security</i> request */
	MSG_ID_BLE_SET_SECURITY_REQ,              /**< Message ID for <i>set security</i> request */
	MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ,        /**< Message ID for <i>passkey send reply</i> request */
	MSG_ID_BLE_CLEAR_BONDS_REQ,
	MSG_ID_BLE_GET_VERSION_REQ,               /**< Message ID for <i>get version</i> request */
	MSG_ID_BLE_RSSI_REQ,                      /**< Message ID for <i>RSSI</i> request */
	MSG_ID_BLE_GET_INFO_REQ,                  /**< Message ID for <i>get info</i> request */

	/* GATT Client requests */
	MSG_ID_BLE_DISCOVER_REQ,                  /**< Message ID for <i>discover attributes</i> request */
	MSG_ID_BLE_GET_REMOTE_DATA_REQ,           /**< Message ID for <i>get remote data</i> request */
	MSG_ID_BLE_SET_REMOTE_DATA_REQ,           /**< Message ID for <i>set remote data</i> request */

	/* GATT Server requests */
	MSG_ID_BLE_UPDATE_DATA_REQ,               /**< Message ID for <i>update attribute</i> request */

	/* BLE direct test mode command */
	MSG_ID_BLE_DTM_REQ,                       /**< Message ID for <i>BLE test command</i> request */

	/* BLE debug command */
	MSG_ID_BLE_DBG_REQ,                       /**< Message ID for <i>BLE debug command</i> request */

	MSG_ID_BLE_REQ_LAST,

	MSG_ID_BLE_ENABLE_RSP = MSG_ID_BLE_RSP, /**< Message ID for <i>enable</i> response, of type @ref ble_enable_rsp */
	MSG_ID_BLE_SET_NAME_RSP,                /**< Message ID for <i>set name</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_START_DISCOVER_RSP,          /**< Message ID for <i>start discover</i> response */
	MSG_ID_BLE_STOP_DISCOVER_RSP,           /**< Message ID for <i>stop discover</i> response */
	MSG_ID_BLE_CONNECT_RSP,                 /**< Message ID for <i>connect</i> response, of type @ref ble_conn_rsp */
	MSG_ID_BLE_DISCONNECT_RSP,              /**< Message ID for <i>disconnect</i> response, of type @ref ble_conn_rsp */
	MSG_ID_BLE_CONN_UPDATE_RSP,             /**< Message ID for <i>conn update</i> response, of type @ref ble_conn_rsp */
	MSG_ID_BLE_SUBSCRIBE_RSP,               /**< Message ID for <i>subscribe</i> response, of type @ref ble_subscribe_rsp */
	MSG_ID_BLE_UNSUBSCRIBE_RSP,             /**< Message ID for <i>unsubscribe</i> response, of type @ref ble_unsubscribe_rsp */
	MSG_ID_BLE_INIT_SVC_RSP,                /**< Message ID for <i>init service</i> response, of type @ref ble_init_service_rsp */
	MSG_ID_BLE_START_ADV_RSP,               /**< Message ID for <i>start advertisement</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_STOP_ADV_RSP,                /**< Message ID for <i>stop advertisement</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_GET_SECURITY_RSP,            /**< Message ID for <i>get security</i> response, of type @ref ble_get_security_rsp */
	MSG_ID_BLE_SET_SECURITY_RSP,            /**< Message ID for <i>set security</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP,      /**< Message ID for <i>passkey send reply</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_CLEAR_BONDS_RSP,             /**< Message ID for <i>clear bonding info</i> response, of type @ref ble_rsp */
	MSG_ID_BLE_GET_VERSION_RSP,             /**< Message ID for <i>get version</i> response, of type @ref ble_version_rsp */
	MSG_ID_BLE_RSSI_RSP,                    /**< Message ID for <i>RSSI</i> response, of type @ref ble_conn_rsp */
	MSG_ID_BLE_GET_INFO_RSP,                /**< Message ID for <i>get info</i> response, of type @ref ble_get_info_rsp */

	/* GATT Client responses */
	MSG_ID_BLE_DISCOVER_RSP,                /**< Message ID for <i>discover attributes</i> response, of type @ref ble_discover_rsp */
	MSG_ID_BLE_GET_REMOTE_DATA_RSP,         /**< Message ID for <i>get remote data</i> response, of type @ref ble_get_remote_data_rsp */
	MSG_ID_BLE_SET_REMOTE_DATA_RSP,         /**< Message ID for <i>set remote data</i> response, of type @ref ble_conn_rsp */

	/* GATT Server requests */
	MSG_ID_BLE_UPDATE_DATA_RSP,             /**< Message ID for <i>service server data update</i> response, of type @ref ble_update_data_rsp */

	/* BLE direct test mode command */
	MSG_ID_BLE_DTM_RSP,                     /**< Message ID for <i>DTM command</i> response, of type @ref ble_dtm_test_rsp */
	MSG_ID_BLE_DBG_RSP,                     /**< Message ID for <i>DTM command</i> response, of type @ref ble_dbg_req_rsp */

	MSG_ID_BLE_RSP_LAST,

	/* events */
	MSG_ID_BLE_DISCOVER_EVT = MSG_ID_BLE_EVT, /**< Message ID for struct @ref ble_discover_evt */
	MSG_ID_BLE_NOTIF_EVT,            /**< Notification or indication data when subscribe has been called */
	MSG_ID_BLE_CONNECT_EVT,          /**< Message ID for struct @ref ble_connect_evt */
	MSG_ID_BLE_DISCONNECT_EVT,       /**< Message ID for struct @ref ble_disconnect_evt */
	MSG_ID_BLE_SECURITY_EVT,         /**< Message ID for struct @ref ble_security_evt */
	MSG_ID_BLE_ADV_TO_EVT,           /**< Message ID for struct @ref ble_adv_to_evt */
	MSG_ID_BLE_PROTOCOL_EVT,         /**< Message ID for struct @ref ble_ispp_event_s */
	MSG_ID_BLE_RSSI_EVT,             /**< Message ID for struct @ref ble_rssi_evt */
	MSG_ID_BLE_NAME_EVT,             /**< Message ID for struct @ref ble_device_name_evt */
	MSG_ID_BLE_EVT_LAST
};

/**
 * GATT Success code and error codes.
 */
enum BLE_SVC_GATT_STATUS_CODES {
	BLE_SVC_GATT_STATUS_SUCCESS = BLE_STATUS_SUCCESS, /**< GATT success @ref BLE_STATUS_SUCCESS */
	BLE_SVC_GATT_STATUS_ENCRYPTED_MITM = BLE_SVC_GATT_STATUS_SUCCESS,
	BLE_SVC_GATT_STATUS_INVALID_HANDLE = BLE_STATUS_GATT_BASE + 0x01,/**< 0x01 see BT Spec Vol 3: Part F (ATT), chapter 3.4.1.1 */
	BLE_SVC_GATT_STATUS_READ_NOT_PERMIT,
	BLE_SVC_GATT_STATUS_WRITE_NOT_PERMIT,
	BLE_SVC_GATT_STATUS_INVALID_PDU,
	BLE_SVC_GATT_STATUS_INSUF_AUTHENTICATION,
	BLE_SVC_GATT_STATUS_REQ_NOT_SUPPORTED,
	BLE_SVC_GATT_STATUS_INVALID_OFFSET,
	BLE_SVC_GATT_STATUS_INSUF_AUTHORIZATION,
	BLE_SVC_GATT_STATUS_PREPARE_Q_FULL,
	BLE_SVC_GATT_STATUS_NOT_FOUND,
	BLE_SVC_GATT_STATUS_NOT_LONG,
	BLE_SVC_GATT_STATUS_INSUF_KEY_SIZE,
	BLE_SVC_GATT_STATUS_INVALID_ATTR_LEN,
	BLE_SVC_GATT_STATUS_ERR_UNLIKELY,
	BLE_SVC_GATT_STATUS_INSUF_ENCRYPTION,
	BLE_SVC_GATT_STATUS_UNSUPPORT_GRP_TYPE,
	BLE_SVC_GATT_STATUS_INSUF_RESOURCE,

	/**< TODO: maybe be not needed, to be covered by generic GAP status */
	BLE_SVC_GATT_STATUS_NO_RESOURCES = BLE_STATUS_GATT_BASE | 0x80,
	BLE_SVC_GATT_STATUS_INTERNAL_ERROR,
	BLE_SVC_GATT_STATUS_WRONG_STATE,
	BLE_SVC_GATT_STATUS_DB_FULL,
	BLE_SVC_GATT_STATUS_BUSY,
	BLE_SVC_GATT_STATUS_ERROR,
	BLE_SVC_GATT_STATUS_CMD_STARTED,
	BLE_SVC_GATT_STATUS_ILLEGAL_PARAMETER,
	BLE_SVC_GATT_STATUS_PENDING,
	BLE_SVC_GATT_STATUS_AUTH_FAIL,
	BLE_SVC_GATT_STATUS_MORE,
	BLE_SVC_GATT_STATUS_INVALID_CFG,
	BLE_SVC_GATT_STATUS_SERVICE_STARTED,
	BLE_SVC_GATT_STATUS_ENCRYPTED_NO_MITM,
	BLE_SVC_GATT_STATUS_NOT_ENCRYPTED,
	BLE_SVC_GATT_STATUS_CONGESTED,
};

/**
 * GATT Write operation types
 *
 * (BT spec Vol 3, Part G, chapter. 4.9)
 * @note Characteristics long write, Prepare & Execute request are handled internally
 */
enum BLE_GATT_WR_OP_TYPES {
	BLE_GATT_WR_OP_NOP = 0,	    /**< normally not used except to cancel BLE_GATT_WR_OP_REQ long char write procedure */
	BLE_GATT_WR_OP_CMD,	    /**< Write Command, (no response) */
	BLE_GATT_WR_OP_REQ,	    /**< Write Request, Write response is received , if length is longer then ATT MTU, Prepare write procedure */
	BLE_GATT_WR_OP_SIGNED_CMD,  /**< Signed Write Command */
};

/**
 * Attribute handle range definition.
 */
struct ble_gatt_handle_range {
	uint16_t start_handle;
	uint16_t end_handle;
};

/**
 * GATT Discover types.
 */
enum BLE_GATT_DISC_TYPES {
	BLE_GATT_DISC_PRIMARY,
	BLE_GATT_DISC_SECONDARY,
	BLE_GATT_DISC_INCLUDE,
	BLE_GATT_DISC_CHARACTERISTIC,
	BLE_GATT_DISC_DESCRIPTOR
};


/** Generic BLE status response message. */
struct ble_rsp {
	struct cfw_message header;	/**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;		/**< Response status @ref BLE_STATUS */
};

/** Generic BLE response with connection reference and status. */
struct ble_conn_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	ble_status_t status;       /**< @ref BLE_STATUS */
};

/** BLE Enable options. */
enum BLE_OPTIONS {
	BLE_OPTION_NONE = 0,
	BLE_OPTION_TEST_MODE = 0x000000001, /**< Enable BLE Service in Test mode */
};

/** BLE Enable configuration options. */
struct ble_enable_config {
	bt_addr_le_t * p_bda;  /**< Optional BT device address. If NULL, internal unique static random will be used */
	uint32_t options;    /**< Enable options @ref BLE_OPTIONS */
	struct ble_gap_connection_params central_conn_params;    /**< Central supported range */
	struct ble_gap_sm_config_params sm_config;
};

/** Parameters of MSG_ID_BLE_ENABLE_REQ. */
struct ble_enable_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint32_t options;
	struct ble_gap_connection_params central_conn_params;
	struct ble_gap_sm_config_params sm_config;
	uint8_t enable;
	uint8_t bda_present;
	bt_addr_le_t bda;
};

/** Parameters of MSG_ID_BLE_ENABLE_RSP. */
struct ble_enable_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< Response status @ref BLE_STATUS */
	uint8_t enable;            /**< Enable state: 0:Disabled, 1:Enabled */
	bt_addr_le_t bd_addr;
};

/** Parameters of MSG_ID_BLE_SET_NAME_REQ. */
struct ble_set_name_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t name_len;
	uint8_t data[];
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

/** BD and Name response data. */
struct ble_bda_name_info {
	bt_addr_le_t bda; /**< Current BD address */
	struct ble_bonded_devices bonded_devs; /**< Current bonded devices */
	uint8_t name_len;
	uint8_t name[]; /**< Current BLE GAP name */
};

/** Parameters for @ref MSG_ID_BLE_GET_INFO_REQ. */
struct ble_gap_get_info_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t info_type;
};

/** BLE Info response parameters. */
union ble_info_rsp_params {
	struct ble_bda_name_info bda_name_params; /**< BLE address and name */
};

/** Parameters for @ref MSG_ID_BLE_GET_INFO_RSP. */
struct ble_get_info_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status; /**< Response status @ref BLE_STATUS */
	uint8_t info_type; /**< Requested information type @ref BLE_INFO_REQ_TYPES */
	union ble_info_rsp_params info_params;
};

/** Discover parameters. */
struct ble_discover_params {
	struct bt_uuid uuid;                        /**< Attribute UUID */
	struct ble_gatt_handle_range handle_range;  /**< Discover range */
	struct bt_conn *conn;                       /**< Connection reference */
	uint8_t type;                               /**< Discover type @ref BLE_GATT_DISC_TYPES */
};

/** Parameters for @ref MSG_ID_BLE_DISCOVER_REQ. */
struct ble_discover_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_discover_params params; /**< Discover parameters */
};

struct ble_discover_attr {
	struct bt_uuid uuid;
	uint16_t handle;
	uint16_t start_handle; /* only for included services */
	uint16_t end_handle;   /* only for services and included services */
	uint8_t properties;    /* only for characteristics */
};

/** Parameters for @ref MSG_ID_BLE_DISCOVER_RSP. */
struct ble_discover_rsp {
	struct cfw_message header;           /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;                 /**< Status of operation */
	struct bt_conn *conn;                /**< Connection reference */
	uint8_t type;                        /**< Discover type @ref BLE_GATT_DISC_TYPES */
	uint8_t attr_cnt;                    /**< Number of attributes in this response */
	struct ble_discover_attr attrs[];    /**< Returned discovered attributes */
};

/** Subscribe options. */
enum BLE_SUBSCRIBE_TYPES {
	BLE_UNSUBSCRIBE = 0x00,     /**< Unsubscribe from notifications or indications */
	BLE_NOTIFICATION = 0x01,   /**< Subscribe to notifications */
	BLE_INDICATION = 0x02      /**< Subscribe to indications */
};

/** Subscribe parameters. */
struct ble_subscribe_params {
	struct bt_conn *conn;     /**< Connection reference */
	uint16_t ccc_handle;      /**< CCC handle */
	uint16_t value;           /**< Subscription type @ref BLE_SUBSCRIBE_TYPES */
	uint16_t value_handle;    /**< Subscribed value handle */
};

/** Parameters for @ref MSG_ID_BLE_SUBSCRIBE_REQ. */
struct ble_subscribe_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_subscribe_params params; /**< Subscribe request parameters */
};

/** Parameters for @ref MSG_ID_BLE_SUBSCRIBE_RSP. */
struct ble_subscribe_rsp {
	struct cfw_message header;      /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;             /**< Status of operation */
	void *p_subscription;           /**< Subscription element (for unsubscription) */
};

/** Unsubscribe parameters. */
struct ble_unsubscribe_params {
	struct bt_conn *conn;     /**< Connection reference */
	void *p_subscription;     /**< Subscription object */
};

/** Parameters for @ref MSG_ID_BLE_UNSUBSCRIBE_REQ. */
struct ble_unsubscribe_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_unsubscribe_params params; /**< Unsubscribe parameters */
};

/** Parameters for @ref MSG_ID_BLE_UNSUBSCRIBE_RSP. */
struct ble_unsubscribe_rsp {
	struct cfw_message header;      /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;             /**< Status of operation */
};

/** Write parameters. */
struct ble_set_remote_data_params {
	struct bt_conn *conn;           /**< Connection reference */
	uint16_t char_handle;           /**< Characteristic handle */
	uint16_t offset;                /**< Characteristic data offset */
	bool with_resp;                 /**< Equal to true is response is needed.
	                                     Otherwise none response is provided */
};

/** Parameters for @ref MSG_ID_BLE_SET_REMOTE_DATA_REQ. */
struct ble_set_remote_data_req {
	struct cfw_message header;      /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_set_remote_data_params params; /**< Keeping API structure in which data pointer shall not be used */
	uint16_t data_length;           /**< Data Length */
	uint8_t data[0];                /**< Data */
};

/** Read parameters. */
struct ble_get_remote_data_params {
	struct bt_conn *conn;   /**< Connection reference */
	uint16_t char_handle;   /**< Attribute handle */
	uint16_t offset;        /**< Offset at which to read */
};

/** Parameters for @ref MSG_ID_BLE_GET_REMOTE_DATA_REQ. */
struct ble_get_remote_data_req {
	struct cfw_message header;                /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_get_remote_data_params params; /**< Get remote data parameters */
};

/** Parameters for @ref MSG_ID_BLE_GET_REMOTE_DATA_RSP. */
struct ble_get_remote_data_rsp {
	struct cfw_message header;      /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;           /**< Connection reference */
	ble_status_t status;            /**< Status of operation */
	uint16_t data_length;           /**< Data Length */
	uint8_t data[0];                /**< Data */
};

/** Parameters for @ref MSG_ID_BLE_DISCOVER_EVT. */
struct ble_discover_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	bt_addr_le_t ble_addr;     /**< Address of the peer device */
	uint16_t svc_count;
	struct bt_uuid *service;   /**< List of primary services. If no browse is true, this is null */
	uint8_t *device_name;      /**< Device name */
	uint16_t appearance;       /**< Appearance */
	int8_t rssi;               /**< rssi */
	uint8_t tx_power;          /**< tx_power */
	uint16_t conn_int;         /**< connection interval */
	/* allocate here space for services discovered */
};

/** Parameters for @ref MSG_ID_BLE_CONNECT_EVT. */
struct ble_connect_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint8_t role;              /**< HCI Role (BT_HCI_ROLE_MASTER/BT_HCI_ROLE_SLAVE) */
	bt_addr_le_t ble_addr;     /**< Address of the peer device */
};

/** Parameters for @ref MSG_ID_BLE_RSSI_EVT. */
struct ble_rssi_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	int8_t rssi;               /**< Received Signal Strength Indication */
};

/** Parameters for @ref MSG_ID_BLE_NAME_EVT. */
struct ble_device_name_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t device_name[];
};

/** Parameters for @ref MSG_ID_BLE_DISCONNECT_REQ. */
struct ble_disconnect_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	uint8_t reason;            /**< Reason of the disconnect*/
};

/** Parameters for @ref MSG_ID_BLE_DISCONNECT_EVT. */
struct ble_disconnect_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	ble_status_t status;       /**< BLE_SUCCESS, etc. */
	uint16_t reason;           /**< Disconnection reason */
};

/** Parameters for @ref MSG_ID_BLE_CONNECT_REQ. */
struct ble_connect_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_le_conn_param conn_params;
	bt_addr_le_t bd_addr;
};

/** Parameters for @ref MSG_ID_BLE_CONN_UPDATE_REQ. */
struct ble_conn_update_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;	/**< Connection reference */
	struct ble_gap_connection_params conn_params;
};

/** Parameters for @ref MSG_ID_BLE_INIT_SVC_REQ. */
struct ble_init_svc_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	int (*init_svc)(struct ble_init_svc_req *msg, struct _ble_service_cb *p_cb);
	void (*init_svc_complete)(struct ble_init_svc_req *req);
	uint8_t status;
};

/** Parameters for @ref MSG_ID_BLE_INIT_SVC_RSP. */
struct ble_init_service_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	struct bt_uuid svc_uuid;   /**< 16 or 128 bit Service UUID */
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

/** Security relates state and status definitions. */
enum BLE_SEC_ST_DEVICE_STATUS {
	BLE_SEC_ST_IDLE = 0x0000,            /**< Idle state, pairing requests will fail, @ref BLE_SEC_DEVICE_STATUS */
	BLE_SEC_ST_PAIRABLE = 0x0001,        /**< Pairing allowed application will get pin requests
	                                          if supported security options of device */
	BLE_SEC_ST_PAIRING_IN_PROG = 0x0002, /**< Pairing in progress */
	BLE_SEC_ST_BONDING_SUCCESS = 0x0004, /**< Pairing resulted in bonding success and device is stored in database */
	BLE_SEC_ST_PAIRING_SUCCESS = 0x0008, /**< Pairing succeeded but only stored temporarily (not stored in flash) */
	BLE_SEC_ST_PAIRING_FAILED = 0x0010,  /**< Pairing failed */
	BLE_SEC_ST_BONDED_DEVICES_AVAIL = 0x0080, /**< At least one bonded device in database, @ref BLE_SEC_BONDING_DB_STATE */
	BLE_SEC_ST_NO_BONDED_DEVICES = 0x0100, /**< Bonding database is empty , @ref BLE_SEC_BONDING_DB_STATE */
};

/** Parameters for ble_get_security_status. */
union ble_get_security_params {
	struct bt_conn *conn;
};

/** Parameters for @ref MSG_ID_BLE_GET_SECURITY_REQ. */
struct ble_get_security_status_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t op_code;
	uint8_t len;
	uint8_t data[];
};

/** Connection related security structure */
struct ble_conn_sec {
	struct bt_conn *conn;
	uint8_t mode;
};

/** Parameters for @ref MSG_ID_BLE_GET_SECURITY_RSP. */
struct ble_get_security_rsp {
	struct cfw_message header;   /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint8_t op_code;             /**< @ref BLE_SEC_OPCODES */
	union {
		uint16_t dev_status; /**< @ref BLE_SEC_DEVICE_STATUS  */
		struct ble_conn_sec sec_mode; /**< @ref BLE_SEC_CONN_STATE */
	};
};

/** Authentication data. */
struct ble_auth_data {
	union {
		uint8_t passkey[6];   /**< 6 digit key (000000 - 999999), only valid for @ref BLE_SM_AUTH_PASSKEY_REQ */
		uint8_t obb_data[16]; /**< 16 byte of OBB data */
	};
	uint8_t type;                 /**< @ref BLE_GAP_SM_PASSKEY_TYPE */
};

/**
 * Security data parameter structure.
 */
union ble_set_sec_params {
	uint16_t dev_status;       /**< @ref BLE_SEC_DEVICE_STATUS */
	struct ble_conn_sec conn;  /**< @ref BLE_SEC_CONN_STATE */
	struct ble_auth_data auth; /**< @ref BLE_SEC_AUTHENTICATION */
};

/** Parameters for @ref MSG_ID_BLE_SET_SECURITY_REQ. */
struct ble_set_security_status_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t op_code;
	union ble_set_sec_params params;
};

/** Parameters for @ref MSG_ID_BLE_SET_SECURITY_RSP. */
struct ble_set_security_rsp {
	struct cfw_message header;   /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;
	uint8_t op_code;             /**< @ref BLE_SEC_OPCODES */
	union {
		uint16_t dev_status; /**< @ref BLE_SEC_DEVICE_STATUS  */
		struct ble_conn_sec sec_mode; /**< @ref BLE_SEC_CONN_STATE */
	};
};

/** Parameters for @ref MSG_ID_BLE_SECURITY_EVT. */
struct ble_security_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	uint8_t sm_status;         /**< @ref BLE_SM_STATUS_CODES */
	union {
		uint8_t passkey[6]; /**< 6 digit key (000000 - 999999), only valid for @ref BLE_SM_AUTH_DISP_PASSKEY */
		uint8_t type;       /**< key requested @ref BLE_GAP_SM_PASSKEY_TYPE, for @ref BLE_SM_AUTH_PASSKEY_REQ */
		uint8_t gap_status; /**< gap status, 0 success otherwise failure code. @ref BLE_SM_BONDING_COMPLETE */
	};
};

/** Parameters for @ref MSG_ID_BLE_PASSKEY_SEND_REPLY_REQ. */
struct ble_gap_sm_key_reply_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	struct ble_gap_sm_passkey params;
};

/**
 * BLE service specific update data function
 *
 * @param conn Reference of connection (can be NULL to indicate that it shall update value globally)
 * @param char_handle Value handle of characteristic to update
 * @param len of payload data in this message
 * @param p_data points to data[]
 * @param p_rsp udpate response sent back to callee
 * @return BLE_STATUS_SUCCESS for success, otherwise BLE failure code.
 */
typedef int (*update_data_t)(struct bt_conn *conn, uint16_t char_handle, uint16_t len, uint8_t *p_data,
		struct ble_update_data_rsp *p_rsp);

/** GATT server characteristic data to update. */
struct ble_char_data {
	update_data_t update; /**< Update data execution function executed in ble_service context */
	uint16_t char_handle; /**< Handle of characteristic */
	uint16_t len;
	uint8_t *p_data;      /**< data depending on service */
};

/** Parameters for @ref MSG_ID_BLE_UPDATE_DATA_REQ. */
struct ble_update_data_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn; /**< Connection reference */
	update_data_t update; /**< Service specific update handler */
	uint16_t char_handle; /**< Handle of characteristic to update */
	uint16_t len; /**< Length of data, maybe 0 */
	uint8_t data[]; /**< Characteristic value payload */
};

/** Parameters for @ref MSG_ID_BLE_UPDATE_DATA_RSP. */
struct ble_update_data_rsp {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	uint16_t char_handle;      /**< Handle of characteristic value to write */
	ble_status_t status;       /**< Status of the data update operation */
};

/** Parameters for @ref MSG_ID_BLE_NOTIF_EVT. */
struct ble_notification_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;      /**< Connection reference */
	uint16_t handle;           /**< Characteristic value handle that has been updated */
	uint8_t len;               /**< Length of data in the notification */
	uint8_t data[];
};

/** Parameters for @ref MSG_ID_BLE_ADV_TO_EVT. */
struct ble_adv_to_evt {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
};

/**
 * Advertising data parameter structure.
 * The parameters modify the advertisement data content of
 * the generated advertisement array.
 */
struct ble_adv_params {
	uint8_t options;
	uint8_t adv_type; /**< see @ref GAP_ADV_TYPES */
	bt_addr_le_t *p_le_addr;
	struct bt_eir *p_ad; /**< Advertisement data, null terminated */
	struct bt_eir *p_sd; /**< Advertisement data, null terminated */
};

/**
 * BLE debug req and rsp message.
 */
struct ble_dbg_req_rsp {
	struct cfw_message header;
	uint32_t u0;
	uint32_t u1;
};

/** Parameters for @ref MSG_ID_BLE_DTM_REQ. */
struct ble_dtm_test_req {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct ble_test_cmd params;
};

/** Parameters for @ref MSG_ID_BLE_DTM_RSP. */
struct ble_dtm_test_rsp {
	struct cfw_message header;              /**< Component framework message header (@ref cfw), MUST be first element of structure */
	ble_status_t status;                    /**< Status of the operation */
	struct ble_dtm_test_result result;      /**< Result data of DTM RX test */
};

/** Parameters for @ref MSG_ID_BLE_RSSI_REQ. */
struct ble_gap_set_rssi_report_req {
	struct cfw_message header;        /**< Component framework message header (@ref cfw), MUST be first element of structure */
	struct bt_conn *conn;             /**< Connection reference */
	struct rssi_report_params params; /**< RSSI report parameters */
};

/**
 * Initialize cfw BLE service framework.
 *
 * This initializes the CFW, enables nble and registers CFW ble service.
 *
 * @param queue cfw BLE service message queue
 */
void ble_cfw_service_init(T_QUEUE queue);

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
 * @param p_conn_params connection parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_CONNECT_RSP and type @ref ble_rsp.
 */
int ble_connect(cfw_service_conn_t * p_service_conn, const bt_addr_le_t * p_addr,
		const struct bt_le_conn_param *p_conn_params, void *p_priv);

/**
 * Enable notification from remote sensors.
 *
 * Subscribe to BLE notification of service. At least service handle must be used. if no
 * characteristic handle is specified, all characteristic are made notify-able.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_params subscription parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SUBSCRIBE_RSP and type @ref ble_subscribe_rsp.
 */
int ble_subscribe(cfw_service_conn_t *p_service_conn,
		const struct ble_subscribe_params *p_params,
		void *p_priv);

/**
 * Disable notification from remote sensors.
 *
 * Unsubscribe from BLE notification of service. At least service handle must be used. if no
 * characteristic handle is specified, all characteristic are made non notify-able.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_params unsubscription parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_UNSUBSCRIBE_RSP.
 */
int ble_unsubscribe(cfw_service_conn_t *p_service_conn,
		struct ble_unsubscribe_params *p_params, void *p_priv);

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
 * @param conn The connection reference
 * @param p_params The new requested connection parameters
 * @param p_priv pointer to private structure returned in the response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_CONN_UPDATE_RSP.
 */
int ble_conn_update(cfw_service_conn_t * p_service_conn,
		    struct bt_conn * conn,
		    const struct ble_gap_connection_params * p_params,
		    void *p_priv);

/**
 * Disconnect remote device/sensor (peripheral & central role).
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn reference of the connection to disconnect
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_DISCONNECT_RSP and type @ref ble_conn_rsp.
 */
int ble_disconnect(cfw_service_conn_t * p_service_conn,
		struct bt_conn *conn, void *p_priv);

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
 *       - Message with ID MSG_ID_BLE_DTM_RSP and type @ref ble_dtm_test_rsp.
 */
int ble_test(cfw_service_conn_t * p_service_conn,
	     const struct ble_test_cmd *p_cmd,
	     void *p_priv);

/**
 * Discover services, characteristics or descriptors on a peer device.
 *
 * @note client role
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_params discover parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_DISCOVER_RSP and type @ref ble_discover_rsp.
 */
int ble_discover(cfw_service_conn_t * p_service_conn,
			 const struct ble_discover_params *p_params,
			 void *p_priv);


/**
 * Read remote data.
 *
 * @note  Client Role
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_params get remote data parameters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_GET_REMOTE_DATA_RSP and type @ref ble_get_remote_data_rsp.
 */
int ble_get_remote_data(cfw_service_conn_t * p_service_conn,
			const struct ble_get_remote_data_params *p_params,
			void *p_priv);

/**
 * Set remote data on a remote service.
 *
 * @note client role usage
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param p_params set remote data parameters
 * @param data_length length of data
 * @param data data
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SET_REMOTE_DATA_RSP and type @ref ble_conn_rsp.
 */
int ble_set_remote_data(cfw_service_conn_t * p_service_conn,
			const struct ble_set_remote_data_params *p_params,
			uint16_t data_length, uint8_t *data,
			void *p_priv);

/** Advertisement options. */
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
 * Set advertisement data and start advertisement.
 *
 * This will set that advertisement and scan response data.
 * If there is already an ongoing advertisement, the advertisement data (and/or scan response)
 * will be simply updated with the data in the stack. If the options need to be changed,
 * the application needs to do first a ble_stop_advertisment().
 *
 * @param p_service_conn client service connection (cfw service connection)
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
		const struct ble_adv_params *p_adv_params,
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
 * Get Current security status
 *
 * This will return the security status or state depending on operation requested.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param op security operation code @ref BLE_SEC_OPCODES
 * @param p_params parameters related to security operation (can be NULL if not required)
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
 * @param op security operation code @ref BLE_SEC_OPCODES
 * @param p_params parameters related to security operation code (can be NULL if not required)
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_SET_SECURITY_RSP and type @ref ble_set_security_rsp.
 */
int ble_set_security_status(cfw_service_conn_t * p_service_conn,
			    uint8_t op,
			    const union ble_set_sec_params * p_params,
			    void *p_priv);

/**
 * Send authentification passkey.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn connection reference
 * @param p_params passkey params
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID MSG_ID_BLE_PASSKEY_SEND_REPLY_RSP (status) and type @ref ble_rsp.
 */
int ble_send_passkey(cfw_service_conn_t *p_service_conn,
		     struct bt_conn *conn,
		     const struct ble_gap_sm_passkey * p_params,
		     void *p_priv);

/**
 * Update GATT Server characteristic value data.
 *
 * This will set the server data characteristic depending on connection handle.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn connection reference (can be NULL to update value for all connection)
 * @param p_params characteristic data to write
 * @param p_priv pointer to private structure returned in a response
 *
 * @note In general each BLE service will implement it own function update function.
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_UPDATE_DATA_RSP and type @ref ble_update_data_rsp.
 */
int ble_update_service_data(cfw_service_conn_t *p_service_conn,
			    struct bt_conn *conn,
			    const struct ble_char_data *p_params,
			    void *p_priv);

/**
 * Request to send RSSI report request
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param conn Connection reference
 * @param params RSSI report paramaters
 * @param p_priv pointer to private structure returned in a response
 *
 * @return @ref OS_ERR_TYPE
 * @note Expected notification:
 *       - Message with ID @ref MSG_ID_BLE_RSSI_RSP, and type @ref ble_conn_rsp.
 */
int ble_set_rssi_report(cfw_service_conn_t * p_service_conn, struct bt_conn *conn,
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
