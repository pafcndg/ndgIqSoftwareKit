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

#ifndef __BT_GATT_INT_H
#define __BT_GATT_INT_H

#include <stdint.h>
#include "zephyr/bluetooth/gatt.h"

/* cfw ble service status codes */
#include "services/ble_service/ble_service.h"

/* Max number of service supported, if changed update BLE core needs to be
 * updated too! */
#define BLE_GATTS_MAX_SERVICES (CONFIG_BT_GATT_BLE_MAX_SERVICES)

struct ble_init_svc_req_msg;
/*
 * Internal APIs used between host and BLE controller
 * Typically they are required if gatt.h APIs can not be mapped 1:1 onto controller API
 */

/** GATT Register structure for one service */
struct ble_gatt_register {
	struct ble_init_svc_req_msg *req; /* initial request */
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

enum ble_gatts_notif_ind_msg_type {
	MSG_ID_BLE_GATTS_SEND_NOTIF_RSP,    /**< Notification type */
	MSG_ID_BLE_GATTS_SEND_IND_RSP,      /**< Indication type */
};

struct ble_gatt_notif_ind_rsp_msg {
	bt_gatt_notify_func_t cback;       /**< Callback function to call on reception of this message */
	ble_status_t status;               /**< Status of the operation. */
	uint16_t conn_handle;              /**< Connection handle, can be 0xFFFF if value change should be broadcast */
	uint16_t handle;                   /**< Characteristic value handle */
	uint8_t msg_type;                  /**< MSG_ID_BLE_GATTS_SEND_NOTIF_RSP for notification or MSG_ID_BLE_GATTS_SEND_IND_RSP for indication */
};

/** Register a BLE GATT Service.
 *
 * @param p_param Parameters of attribute data base
 * @param p_attrs Serialised attribute buffer
 * @param attr_len length of buffer
 */
void ble_gatt_register_req(const struct ble_gatt_register * p_param,
		       uint8_t *p_buf,
		       uint16_t len);

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
 * Retrieves handle based of service attrs and index of attribute
 *
 * @param attrs Service attribute array
 * @param index index of attribute
 *
 * @return Handle of attribute or BT_GATT_INV_HANDLE if not found
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
 * Response to @ref ble_gatts_send_ind_req and ble_gatts_send_notif_req
 *
 * @param p_params Response
 */
void on_ble_gatts_send_notif_ind_rsp(const struct ble_gatt_notif_ind_rsp_msg *p_params);

/**
 *  @def BT_UUID_16_SZ
 * Packed UUID 16 bit size.
 */
#define BT_UUID_16_SZ(u) (sizeof((u).type) + sizeof((u).u16))
 /**
  *  @def BT_UUID_128_SZ
  * Packed UUID 128 bit size.
  */
#define BT_UUID_128_SZ(u) (sizeof((u).type) + sizeof((u).u128))

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
 * @param p_attrs Serialised attribute buffer
 * @param attr_len length of buffer
 */
void on_ble_gatt_register_rsp(const struct ble_gatt_register *p_param,
			  uint8_t *p_result, uint8_t len);

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
	/** Attribute flags (permissions, etc) */
	uint16_t		flags;
};
#endif /* __BT_GATT_INT_H */
