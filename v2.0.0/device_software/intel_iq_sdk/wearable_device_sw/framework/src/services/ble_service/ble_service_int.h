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

#ifndef __BLE_SERVICE_INT_H__
#define __BLE_SERVICE_INT_H__

#include "cfw/cfw.h"
#include "services/ble_service/ble_service_gatt.h"
#include "services/ble_service/ble_service_gatts_api.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_protocol.h"

/**
 * Max number of BLE services supported.
 */
#define BLE_MAX_SERVICE 10

/**
 * Max number of connection status callback handlers supported. */
#define BLE_MAX_CONN_STATUS 4

/* default name use when not set by service user */
#define BLE_DEFAULT_NAME "CURIE-1.0"

/* forward declarations */
struct _ble_service_cb;

enum BLE_STATE {
	BLE_ST_NOT_READY = 0,
	BLE_ST_DISABLED,
	BLE_ST_ENABLED,
	BLE_ST_DTM
};

/* only for testing first BLE advertising. To be removed after*/
void test_ble_enable_request(void);

struct ble_enable_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint32_t options;
	uint16_t appearance; /**< see BLE spec */
	struct ble_gap_connection_params peripheral_conn_params;
	struct ble_gap_connection_params central_conn_params;
	struct ble_gap_sm_config_params sm_config;
	uint8_t enable;
	uint8_t bda_len;
	uint8_t name_len;
	uint8_t data[];  /* bda first if both are present */
};

/**< Common init service message header */
struct ble_init_svc_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	int (*init_svc)(struct ble_init_svc_req_msg *msg, struct _ble_service_cb *p_cb);
	void (*init_svc_complete)(struct ble_init_svc_req_msg *req);
};

struct ble_start_advertisement_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint32_t options;
	uint8_t type; /**< see @ref GAP_ADV_TYPES */
	uint8_t bd_len;
	uint8_t sd_len;
	uint8_t ad_len;
	uint8_t data[];
};

struct ble_update_service_data_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	update_data_t update; /**< Service specific udpate handler */
	uint16_t conn_handle; /**< Connection */
	uint16_t char_handle; /**< Handle of characteristic to update */
	uint16_t len; /**< Length of data, maybe 0 */
	uint8_t data[]; /**< Characteristic value payload */
};

struct ble_protocol_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	/* Protocol specific request function
	 * @param conn_handle connection object
	 * @param len of payload data in this message
	 * @param offset into data buffer (p_data[offset])
	 * @param p_data points to data[]
	 * @param p_priv application private data */
	void (*protocol_req)(uint16_t conn_handle, uint16_t len, uint16_t offset, const uint8_t *p_data, void *p_priv);
	uint16_t conn_handle; /**< Connection */
	uint16_t len; /**< Length of data, maybe 0 */
	uint16_t offset; /**< offset into p_data. protocol dependent */
	uint8_t data[]; /**< Characteristic value payload */
};

/**
 * Helper function returning the on_write function for given char/value handle.
 * @param handle to check
 * @return Pointer to found structure or NULL if not found
 */
struct ble_on_write_range * ble_get_on_write_func(uint16_t handle);

/**
 * Helper function to allocate a new on_write function handler for a given range
 * @param p_wr pointer to range and callback entry
 *
 * @return of entry or negative if failed.
 */
int ble_alloc_on_write_range(const struct ble_on_write_range * p_wr);

struct _ble_service_uuid16_cb {
	uint16_t type;
	uint16_t uuid;
};

enum BT_GAP_CONN_EVT {
	BT_GAP_CONNECT_EVT = 0,
	BT_GAP_DISCONNECT_EVT,
};

/**
 * Connection status callback function type.
 *
 * @param status_evt connection status event @ref BT_GAP_CONN_EVT
 * @param conn_handle Connection handle
 *
 */
typedef void (*connection_status_t)(uint8_t status_evt, uint16_t conn_handle);

/**
 * Register a callback for connection events.
 *
 * Typically protocol type of service (e.g. serial port emulation) may want to
 * to get informed on connect/disconnect.
 *
 * @param cback connection status callback handler
 *
 * @return of entry or negative if failed.
 */
int ble_service_register_for_conn_st(connection_status_t cback);

/**
 * Broadcast connection status to registered protocols.
 *
 * @param status_event type connection status change @ref BT_GAP_CONN_EVT
 * @param conn_handle connection on which the status changed
 *
 */
void ble_service_broadcast_conn_st(uint8_t status_evt, uint16_t conn_handle);

/** Helper function for RPC internal messages
 * Do not call before BLE_SERVICE is registered!
 * @return port id of ble service. */
uint16_t ble_service_get_port_id(void);

struct ble_rpc_callin {
	struct message msg; /**< Message header, MUST be first element of structure */
	uint8_t *p_data; /**< RPC buffer, must be freed after deserializing */
	uint16_t len; /**< length of above buffer */
};

struct _ble_service_cb {
	cfw_client_t * client;	/* cfw client for BLE core */
	T_QUEUE queue; /* Queue for the messages */
	cfw_service_conn_t *p_bat_service_conn;	/* service connection of opened battery service */
	connection_status_t connection_st_cb[BLE_MAX_CONN_STATUS]; /**< protocol connection status callbacks */
	struct ble_init_svc_req_msg *svc_init_msg; /* Use for temporary storage of application init service request */
	uint16_t gap_conn_handle;
	uint16_t rpc_port_id;
	uint8_t role;
	struct ble_gap_connection_values conn_values;
	struct ble_gap_connection_params peripheral_conn_params;
	uint8_t ble_state;
	uint16_t security_state; /* bonding state, connection state */
};

struct ble_set_name_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t name_len;
	uint8_t data[];
};

struct ble_conn_update_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint16_t conn_handle;	/**< Connection handle */
	struct ble_gap_connection_params conn_params;
};

struct ble_disconnect_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint16_t conn_handle;	/**< Connection handle */
	uint8_t reason;		/**< Reason of the disconnect*/
};

struct ble_get_security_status_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t op_code;
	uint8_t len;
	uint8_t data[];
};

struct ble_set_security_status_req_msg {
	struct cfw_message header; /**< Component framework message header (@ref cfw), MUST be first element of structure */
	uint8_t op_code;
	union ble_set_sec_params params;
};

/* The advertising timeout in units of seconds. */
#define BLE_GAP_ADV_TYPE_ADV_IND          0x00	 /**< Connectable undirected. */
#define BLE_GAP_ADV_FP_ANY                0x00	 /**< Allow scan requests and connect requests from any device. */

/* Advertising parameters */
/** options see \ref BLE_ADV_OPTIONS */
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

/* Expects that req msg is NOT freed */
void handle_ble_enable(struct ble_enable_req_msg *req, struct _ble_service_cb *p_cb);
void handle_ble_disable(struct ble_enable_req_msg *req, struct _ble_service_cb *p_cb);
/* Expects msg to NOT be freed */
void handle_ble_start_adv(struct cfw_message *msg, struct _ble_service_cb *p_cb);
void handle_ble_stop_adv(struct cfw_message *msg, struct _ble_service_cb *p_cb);
void handle_ble_set_name(struct cfw_message *msg, struct _ble_service_cb *p_cb);
void handle_ble_rssi_req(struct cfw_message *msg, struct _ble_service_cb *p_cb);

/** Helper function to allocate response depending on number characteristics.
 */
struct ble_init_service_rsp * ble_alloc_init_service_rsp(
		struct ble_init_svc_req_msg *p_req);
/** Helper function to handle service registration failures */
void handle_ble_add_service_failure(struct ble_init_svc_req_msg *req,
		uint16_t status);

#ifdef CONFIG_BLE_CORE_TEST
void test_ble_service_init(void);
T_QUEUE get_service_queue(void);
#endif
void ble_core_client_handle_message(struct cfw_message *msg, void *param);
void ble_handle_message(struct cfw_message *msg, void *param);

#endif
