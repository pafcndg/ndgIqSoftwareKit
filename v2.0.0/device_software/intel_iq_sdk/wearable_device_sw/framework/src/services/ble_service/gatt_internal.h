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

#ifndef GATT_INTERNAL_H_
#define GATT_INTERNAL_H_

#include <stdint.h>
#include "zephyr/bluetooth/gatt.h"

/* Forward declarations */
struct ble_core_response;

/* Max number of service supported, if changed update BLE core needs to be
 * updated too! */
#define BLE_GATTS_MAX_SERVICES (CONFIG_BT_GATT_BLE_MAX_SERVICES)

/*
 * Internal APIs used between host and BLE controller
 * Typically they are required if gatt.h APIs can not be mapped 1:1 onto controller API
 */

/**
 * GATT indication types.
 */
enum BLE_GATT_IND_TYPES {
	BLE_GATT_IND_TYPE_NONE = 0,
	BLE_GATT_IND_TYPE_NOTIFICATION,
	BLE_GATT_IND_TYPE_INDICATION,
};

/** GATT Register structure for one service */
struct ble_gatt_register {
	uint8_t service_idx; /**< Index of service data base, used in response to match request */
	uint8_t attr_count; /**< Number of of attributes in this service */
};

/** Service index and Attribute index mapping structure.
 *
 * Mapping index into attribute tables as registered with bt_gatt_register/
 * ble_gatt_register.
 */
struct ble_gatt_attr_handle_mapping {
	uint8_t svc_idx; /**< Service index */
	uint8_t attr_idx; /**< Attribute index into service attribute table */
};

enum BLE_GATTS_WR_OPS {
	BLE_GATTS_OP_NONE = 0,
	BLE_GATTS_OP_WR,		    /**< 3.4.5.1 Write Request (Attribute), expects write response */
	BLE_GATTS_OP_WR_CMD,		    /**< 3.4.5.3 Write Command (Attribute) NO response sent */
	BLE_GATTS_OP_WR_CMD_SIGNED,	    /**< 3.4.5.4 Write Command Signed (Attribute), NO response sent */
	BLE_GATTS_OP_WR_PREP_REQ,	    /**< 3.4.6.1 Write Prepare Request, expects a prepare write request response */
	BLE_GATTS_OP_WR_EXE_REQ_CANCEL,	    /**< 3.4.6.3 Cancel Executed Write Request, cancel and clear queue (flags = 0) */
	BLE_GATTS_OP_WR_EXE_REQ_IMM	    /**< 3.4.6.3 Immediately Execute Write Request */
};

/**
 * Write event context data structure.
 */
struct ble_gatt_wr_evt {
	struct ble_gatt_attr_handle_mapping attr;    /**< Attribute mapping indexes */
	uint16_t conn_handle;			    /**< Connection handle */
	uint16_t attr_handle;			    /**< handle of attribute to write */
	uint16_t offset;			    /**< offset in attribute buffer */
	uint8_t op;				    /**< @ref BLE_GATTS_WR_OPS */
};

/**
 * Notification/Indication parameters
 */
struct ble_gatt_notif_ind_params {
	uint16_t val_handle;
	uint16_t offset;
};

/**
 * Indication or notification.
 */

struct ble_gatt_send_notif_ind_params {
	bt_gatt_notify_func_t cback;        /**< Callback function to be returned in response message */
	uint16_t conn_handle;
	struct ble_gatt_notif_ind_params params; /**< Parameters for notification/indication */
};

enum ble_gatts_notif_ind_type {
	MSG_ID_BLE_GATTS_SEND_NOTIF_RSP,    /**< Notification type */
	MSG_ID_BLE_GATTS_SEND_IND_RSP,      /**< Indication type */
};

struct ble_gatt_notif_ind_rsp {
	bt_gatt_notify_func_t cback;  /**< Callback function to call on reception of this message */
	int status;                   /**< Status of the operation. */
	uint16_t conn_handle;         /**< Connection handle, can be 0xFFFF if value change should be broadcast */
	uint16_t handle;              /**< Characteristic value handle */
	uint8_t msg_type;             /**< MSG_ID_BLE_GATTS_SEND_NOTIF_RSP for notification or MSG_ID_BLE_GATTS_SEND_IND_RSP for indication */
};

/**
 * Attribute handle range definition.
 */
struct ble_core_gatt_handle_range {
	uint16_t start_handle;
	uint16_t end_handle;
};

typedef struct {
	struct ble_core_gatt_handle_range handle_range; /**< range of characteristic handles within a service */
	struct bt_uuid uuid; /**< service uuid */
} ble_gattc_svc_t;

/**
 * Primary service
 */
struct ble_gattc_prim_svc {
	struct bt_uuid uuid;			/**< Attribute's UUID */
	uint16_t handle;			/**< Attribute handle */
	struct ble_core_gatt_handle_range handle_range; /**< characteristic handle range */
};

/**
 * Generic GATTC response message.
 */
struct ble_gattc_rsp {
	int status;
	uint16_t conn_handle; /**< GAP connection handle */
};

struct ble_gattc_disc_rsp {
	int status;
	uint16_t conn_handle;
	uint8_t type;		/**< @ref BLE_GATT_DISC_TYPES */
};

/**
 * Included service.
 */
struct ble_gattc_incl_svc {
	uint16_t incl_handle;		/**< Handle of included service */
	ble_gattc_svc_t svc;		/**< Included service */
	struct ble_core_gatt_handle_range handle_range; /**< characteristic handle range */
} ;

/* GATTC characteristic */
struct ble_gattc_characteristic {
	uint8_t prop;				/**< Characteristic property */
	uint16_t decl_handle;			/**< Characteristic declaration handle */
	uint16_t value_handle;			/**< Char's value handle */
	struct bt_uuid uuid;			/**< Characteristic's UUID */
};

/**
 * GATTC descriptor.
 */
struct ble_gattc_descriptor {
	uint16_t handle;	/**< descriptor handle */
	struct bt_uuid uuid;	/**< UUID of the descriptor */
};

struct ble_gatts_set_attribute_params {
	uint16_t value_handle;      /* mandatory */
	uint16_t offset;            /* by default 0 */
};

struct ble_gatts_get_attribute_params {
	uint16_t value_handle;     /* mandatory */
};

struct ble_gatts_attribute_response {
	int status;        /**< Status of the operation. */
	uint16_t value_handle;      /* mandatory */
	void * p_priv;
};

/**
 * Set an attribute value.
 *
 * The response to this request is received through @ref on_ble_gatts_send_svc_changed_rsp
 *
 * @param p_params     attribute parameters
 * @param p_value      attribute value data to write
 * @param len          length of attribute value to write
 * @param p_priv       pointer to private data
 *
 */
void ble_gatts_set_attribute_value_req(const struct ble_gatts_set_attribute_params *p_params,
		uint8_t *p_value, uint8_t len, void *p_priv);

/**
 * Response to @ref ble_gatts_send_svc_changed_req.
 *
 * @param p_params Response
 */
void on_ble_gatts_set_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params);

/**
 * Get an attribute value.
 *
 * The response to this request is received through @ref on_ble_gatts_get_attribute_value_rsp
 *
 * @param p_params     getting attribute parameters
 * @param p_priv       pointer to private data
 *
 */
void ble_gatts_get_attribute_value_req(const struct ble_gatts_get_attribute_params *p_params,
				 void *p_priv);

/**
 * Response to @ref ble_gatts_get_attribute_value_req.
 *
 * @param p_params     response
 * @param p_value      attribute value
 * @param length       length of attribute value
 */
void on_ble_gatts_get_attribute_value_rsp(const struct ble_gatts_attribute_response *p_params,
		uint8_t * p_value, uint8_t length);

struct ble_gatts_svc_changed_params {
	uint16_t conn_handle;
	uint16_t start_handle;
	uint16_t end_handle;
};

/**
 * Send a service change indication.
 *
 * The response to this request is received through @ref on_ble_gatts_send_svc_changed_rsp
 *
 * @note Not yet supported
 *
 * @param p_params service parameters
 * @param p_priv       pointer to private data
 */
void ble_gatts_send_svc_changed_req(const struct ble_gatts_svc_changed_params *p_params, void *p_priv);

/**
 * Response to @ref ble_gatts_send_svc_changed_req.
 *
 * @param p_params Response
 */
void on_ble_gatts_send_svc_changed_rsp(const struct ble_core_response *p_params);

/** Register a BLE GATT Service.
 *
 * @param p_param Parameters of attribute data base
 * @param p_attrs Serialized attribute buffer
 * @param attr_len length of buffer
 */
void ble_gatt_register_req(const struct ble_gatt_register * p_param,
		       uint8_t *p_buf,
		       uint16_t len);

/**
 * Conversion table entry ble_core to host attr index
 *
 * This is returned as a table on registering. */
struct ble_gatt_attr_idx_entry {
	uint16_t handle; /* handle from ble controller should be sufficient */
};

/** Response to registering a BLE GATT Service.
 *
 * The returned buffer contains an array (@ref ble_gatt_attr_idx_entry)with the
 * corresponding handles.
 *
 * @param p_param Parameters of attribute data base
 * @param p_attrs Returned attributes index list
 * @param attr_len length of buffer
 */
void on_ble_gatt_register_rsp(const struct ble_gatt_register *p_param,
		const struct ble_gatt_attr_idx_entry *p_attrs, uint8_t len);

/**
 * Function invoked by the BLE core when a write occurs.
 *
 * @param p_evt  Pointer to the event structure
 * @param p_buf  Pointer to data buffer
 * @param buflen Buffer length
 */
void on_ble_gatts_write_evt(const struct ble_gatt_wr_evt *p_evt,
			    const uint8_t *p_buf, uint8_t buflen);

/**
 * Retrieves handle based on attribute array and index of attribute
 *
 * @param attrs attribute array
 * @param index index of attribute
 *
 * @return Handle of attribute or 0 if not found
 */
uint16_t ble_attr_idx_to_handle(const struct bt_gatt_attr *attrs, uint8_t index);

/**
 * Send notification.
 *
 * The response to this request is received through @ref on_ble_gatts_send_notif_ind_rsp
 *
 * @param p_params     notification parameters
 * @param p_value      indication data to write
 * @param length       length of indication - may be 0, in this case already stored data is sent
 *
 */
void ble_gatt_send_notif_req(const struct ble_gatt_send_notif_ind_params *p_params,
			     uint8_t *p_value, uint16_t length);

/**
 * Send indication.
 *
 * The response to this request is received through @ref on_ble_gatts_send_notif_ind_rsp
 *
 * @param p_params     indication parameters
 * @param p_value      indication data to write
 * @param length       length of indication - may be 0, in this case already stored data is sent
 *
 */
void ble_gatt_send_ind_req(const struct ble_gatt_send_notif_ind_params *p_params,
			   uint8_t *p_value, uint8_t length);

/**
 * Response to @ref ble_gatts_send_ind_req and @ref ble_gatts_send_notif_req
 *
 * @param p_params Response
 */
void on_ble_gatts_send_notif_ind_rsp(const struct ble_gatt_notif_ind_rsp *p_params);

/** Discover parameters. */
#define DISCOVER_FLAGS_UUID_PRESENT 1

struct ble_core_discover_params {
	struct bt_uuid uuid;                        /**< Attribute UUID */
	struct ble_core_gatt_handle_range handle_range; /**< Discover range */
	uint16_t conn_handle;                       /**< Connection handle */
	uint8_t type;                               /**< Discover type @ref BLE_GATT_DISC_TYPES */
	uint8_t flags;
};

/**
 * Discover service.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_discover_req(const struct ble_core_discover_params *req, void *p_priv);

/**
 * Response to @ref ble_gattc_discover_req.
 *
 * @param p_evt     Pointer to the event structure
 * @param data      Pointer to the data
 * @param data_len  Length of the data
 */
void on_ble_gattc_discover_rsp(const struct ble_gattc_disc_rsp *p_evt,
		const uint8_t *data, uint8_t data_len);

/** GATT Attribute stream structure.
 *
 * This structure is a "compressed" copy of @ref bt_gatt_attr.
 * UUID pointer and user_data pointer are used as offset into buffer itself.
 * The offset is from the beginning of the buffer. therefore a value of 0
 * means that UUID or user_data is not present. */
struct ble_gatt_attr {
	/** Attribute UUID offset */
	uint16_t		uuid_offset;
	/** Attribute user data offset */
	uint16_t		user_data_offset;
	/**< User data max length */
	uint16_t		max_len;
	/** Attribute permissions */
	uint16_t		perm;
};

struct ble_gattc_read_params {
	uint16_t conn_handle; /**< Connection handle*/
	uint16_t char_handle; /**< Handle of the attribute to be read */
	uint16_t offset; /**< Offset into the attribute value to be read */
};

struct ble_gattc_read_rsp {
	uint16_t conn_handle;
	int status;
	uint16_t handle;		/**< handle of characteristic attribute read */
	uint16_t offset;		/**< offset of data returned */
};

struct ble_gattc_write_params {
	uint16_t conn_handle; /**< Connection handle*/
	uint16_t char_handle; /**< Handle of the attribute to be read */
	uint16_t offset;      /**< Offset into the attribute value to be write */
	bool with_resp;       /**< Equal to true is response is needed */
};

struct ble_gattc_write_rsp {
	uint16_t conn_handle;
	int status;
	uint16_t char_handle;
	uint16_t len;
};


/**
 * Read characteristic on remote server.
 *
 * @param params Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_read_req(const struct ble_gattc_read_params *params, void *p_priv);

/**
 * Response to @ref ble_gattc_read_req.
 *
 * @param p_evt Pointer to the event structure
 * @param data Pointer to the data byte stream
 * @param data_len Length of the data byte stream
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_read_rsp(const struct ble_gattc_read_rsp *p_evt,
		uint8_t *data, uint8_t data_len, void *p_priv);

/**
 * Write characteristic on server.
 *
 * @param params Write parameters
 * @param p_buf Characteristic value to write.
 * @param buflen Characteristic value length. If length is bigger then ATT MTU size, the controller fragment buffer itself.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_write_req(const struct ble_gattc_write_params *params,
		const uint8_t *p_buf, uint8_t buflen, void *p_priv);

/**
 * Response to @ref ble_gattc_write_req.
 *
 * @param p_evt       Pointer to the event structure
 * @param p_priv      Pointer to private data.
 */
void on_ble_gattc_write_rsp(const struct ble_gattc_write_rsp *p_evt, void *p_priv);

#if defined(CONFIG_BLUETOOTH_GATT_CLIENT)
void bt_gatt_connected(struct bt_conn *conn);
void bt_gatt_disconnected(struct bt_conn *conn);
#endif

struct ble_gattc_value_evt {
	uint16_t conn_handle;
	int status;
	uint16_t handle;	/**< handle of characteristic being notified/indicated */
	uint8_t type;		/**< notification versus indication, @ref BLE_GATT_IND_TYPES */
};

/**
 * Function invoked by BLE service for value event
 *
 * @param p_evt Pointer to the event structure
 * @param p_buf Pointer to the data byte stream
 * @param buflen Length of the data byte stream
 */
void on_ble_gattc_value_evt(const struct ble_gattc_value_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen);

struct ble_gattc_to_evt {
	uint16_t conn_handle;
	uint16_t reason;	/**< GATT timeout reason */
};

/**
 * Function invoked by BLE service for gattc timeout protocol error
 *
 * @param p_evt Pointer to the event structure
 */
void on_ble_gattc_to_evt(const struct ble_gattc_to_evt *p_evt);


#endif /* GATT_INTERNAL_H_ */
