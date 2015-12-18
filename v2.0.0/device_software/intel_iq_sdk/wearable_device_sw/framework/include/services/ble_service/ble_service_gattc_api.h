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

#ifndef __BLE_SERVICE_GATTC_H__
#define __BLE_SERVICE_GATTC_H__

#include "ble_service.h"
#include "ble_service_gap_api.h"
#include "ble_service_gatt.h"

/** 
 * @defgroup ble_core_service_gattc BLE Core Service GATTC
 * @ingroup ble_core_service
 *
 * BLE Core Service GATTC APIs used to implement GATT Clients.
 *
 * This is typically only used to add new client services to BLE service.
 *
 * It provides the following services:
 * - Discover remote \b primary services or a specific service
 * - Discover remote characteristics
 * - Discover remote descriptors
 * - read/write remote characteristics
 * - Getting notified on characteristic changes
 *
 * @{
 */

/**
 * Generic GATTC response message.
 */
struct ble_gattc_rsp {
	ble_status_t status;
	uint16_t conn_handle; /**< GAP connection handle */
};

/**
 * Handle range for a service operation.
 */
struct ble_gattc_handle_range {
	uint16_t start_handle;
	uint16_t end_handle;
};

typedef struct {
	struct ble_gattc_handle_range handle_range; /**< range of characteristic handles within a service */
	struct bt_uuid uuid; /**< service uuid */
} ble_gattc_svc_t;

struct ble_gattc_evt {
	uint16_t conn_handle;
	ble_status_t status;
};

/**
 * Included service.
 */
typedef struct {
	uint16_t incl_handle;		/**< handle of included service */
	ble_gattc_svc_t svc;		/**< included service */
} ble_gattc_incl_svc_t;

/* TODO: fix char property to support extended */
typedef struct {
	uint8_t prop;				/**< Characteristic property */
	uint16_t decl_handle;			/**< Characteristic declaration handle */
	uint16_t value_handle;			/**< Char's value handle */
	struct bt_uuid uuid;			/**< Characteristic's UUID */
} ble_gattc_characteristic_t;

/**
 * GATTC descriptor.
 */
typedef struct {
	uint16_t handle;	/**< descriptor handle */
	struct bt_uuid uuid;	/**< uuid of the descriptor */
} ble_gattc_descriptor_t;

enum BLE_GATTC_RD_CHAR_TYPES {
	BLE_GATTC_RD_CHAR_BY_UUID = 0,		/**< Read characteristic by UUID */
	BLE_GATTC_RD_CHAR,			/**< Read (Long) characteristic or (Long) descriptor. Maybe called multiple times in case of long */
	BLE_GATTC_RD_CHAR_MULTIPLE		/**< Read multiple characteristic attributes */
};

/**
 * Characteristic read by using UUID.
 */
typedef struct {
	struct ble_gattc_handle_range handle_range;	/**< characteristic or descriptor handle range */
	struct bt_uuid *p_uuid; /**< uuid of characteristic to read */
} ble_gattc_rd_char_by_uuid_t;

/**
 * Characteristic or descriptor read.
 *
 * Maybe used for long too.
 */
typedef struct {
	uint16_t handle;			/**< attribute handle for reading */
	uint16_t offset;			/**< offset into attribute data to read */
} ble_gattc_rd_char_t;

/**
 * Read multiple characteristics values.
 */
typedef struct {
	uint16_t handle_count;			/**< number of handles in this structure */
	uint16_t handle[];			/**< handles of attributes to read from */
} ble_gattc_rd_multi_char_t;

typedef struct {
	union {
		ble_gattc_rd_char_by_uuid_t char_by_uuid;
		ble_gattc_rd_char_t char_desc;	    /**< (Long) characteristic or descriptor to read */
		ble_gattc_rd_multi_char_t multi_char;
						    /**< read multiple characteristics */
	};
} ble_gattc_rd_characteristic_t;

typedef struct {
	uint16_t char_handle;		/**< handle of characteristic */
	uint16_t len;			/**< if len is bigger then ATT MTU size, the controller fragment buffer itself */
	uint8_t *p_value;		/**< characteristic value to write */
	uint8_t wr_type;		/**< type of write operation @ref BLE_GATT_WR_OP_TYPES */
} ble_gattc_wr_characteristic_t;

/**
 * Read characteristic response indication.
 */
struct ble_gattc_rd_char_evt {
	uint16_t conn_handle;
	ble_status_t status;
	uint16_t handle;		/**< handle of characteristic attribute read */
	uint16_t offset;		/**< offset of data returned */
};

/**
 * Characteristic write response indication.
 */
struct ble_gattc_wr_char_evt {
	uint16_t conn_handle;
	ble_status_t status;
	uint16_t char_handle;
	uint16_t len;
};

/**
 * Handle value indication or notification indication/event.
 */
struct ble_gattc_value_evt {
	uint16_t conn_handle;
	ble_status_t status;
	uint16_t handle;	/**< handle of characteristic being notified/indicated */
	uint8_t type;		/**< notification versus indication, @ref BLE_GATT_IND_TYPES */
};

/**
 * GATT timeout reason.
 */
struct ble_gattc_to_evt {
	uint16_t conn_handle;
	uint16_t reason;	/**< GATT timeout reason */
};

/**
 * Function invoked by BLE service when primary service is discovered
 *
 * @param p_evt Pointer to the event structure
 * @param data  Pointer to the data byte stream
 * @param data_len Length of the data byte stream
 */
void on_ble_gattc_disc_prim_svc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len);

/**
 * Function invoked by BLE service when included service is discovered
 *
 * @param p_evt Pointer to the event structure
 * @param data  Pointer to the data byte stream
 * @param data_len Length of the data byte stream
 */
void on_ble_gattc_disc_incl_svc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len);

/**
 * Function invoked by BLE service when characteristic is discovered
 *
 * @param p_evt Pointer to the event structure
 * @param data  Pointer to the data byte stream
 * @param data_len Length of the data byte stream
 */
void on_ble_gattc_char_disc_evt(const struct ble_gattc_evt *p_evt,
		uint8_t *data, uint8_t data_len);

/**
 * Function invoked by BLE service when descriptor is discovered
 *
 * @param p_evt Pointer to the event structure
 * @param p_desc Pointer to the descriptor structure
 * @param desc_len Length of the descriptor array
 */
void on_ble_gattc_desc_disc_evt(const struct ble_gattc_evt *p_evt,
		ble_gattc_descriptor_t *p_desc, uint8_t desc_len);

/**
 * Function invoked by BLE service for read characteristic event
 *
 * @param p_evt Pointer to the event structure
 * @param p_buf Pointer to the data byte stream
 * @param buflen Length of the data byte stream
 */
void on_ble_gattc_rd_char_evt(const struct ble_gattc_rd_char_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen);

/**
 * Function invoked by BLE service for write characteristic event
 *
 * @param p_evt Pointer to the event structure
 */
void on_ble_gattc_wr_char_evt(const struct ble_gattc_wr_char_evt *p_evt);

/**
 * Function invoked by BLE service for value event
 *
 * @param p_evt Pointer to the event structure
 * @param p_buf Pointer to the data byte stream
 * @param buflen Length of the data byte stream
 */
void on_ble_gattc_value_evt(const struct ble_gattc_value_evt *p_evt,
		uint8_t *p_buf, uint8_t buflen);

/**
 * Function invoked by BLE service for gattc timeout protocol error
 *
 * @param p_evt Pointer to the event structure
 */
void on_ble_gattc_to_evt(const struct ble_gattc_to_evt *p_evt);

struct ble_gattc_discover_primary_service_params {
	uint16_t conn_handle;
	struct bt_uuid uuid;
};

struct ble_gattc_discover_included_service_params {
	uint16_t conn_handle;
	struct ble_gattc_handle_range handle_range;
};

struct ble_gattc_discover_characteristic_params {
	uint16_t conn_handle;
	struct ble_gattc_handle_range handle_range;
};

struct ble_gattc_discover_descriptor_params {
	uint16_t conn_handle;
	struct ble_gattc_handle_range handle_range;
};

struct ble_gattc_read_characteristic_params {
	uint16_t conn_handle; /**< Connection handle*/
	uint16_t char_handle; /**< Handle of the attribute to be read */
	uint16_t offset; /**< Offset into the attribute value to be read */
};

struct _ble_gattc_wr_characteristic {
	uint16_t char_handle; /**< Handle of characteristic */
	uint8_t wr_type; /**< Type of write operation @ref BLE_GATT_WR_OP_TYPES */
};

/**
 * Discover primary service.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_discover_primary_service_req(const struct
		ble_gattc_discover_primary_service_params *req, void *p_priv);

/**
 * Response to @ref ble_gattc_discover_primary_service_req.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_discover_primary_service_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/**
 * Discover included services on a previously discovered primary service.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_discover_included_service_req(const struct
		ble_gattc_discover_included_service_params *req, void *p_priv);

/**
 * Response to @ref ble_gattc_discover_included_service_req.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_discover_included_services_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/**
 * Discover characteristics on a service.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_discover_char_req(const struct ble_gattc_discover_characteristic_params *req,
		void *p_priv);

/**
 * Response to @ref ble_gattc_discover_char_req.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_discover_char_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/**
 * Discover descriptor on a service.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_discover_descriptor_req(const struct ble_gattc_discover_descriptor_params *req,
		void *p_priv);

/**
 * Response to @ref ble_gattc_discover_descriptor_req.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_discover_descriptor_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/**
 * Read characteristic on remote server.
 *
 * @param req    Request structure.
 * @param p_priv Pointer to private data.
 */
void ble_gattc_rd_chars_req(const struct ble_gattc_read_characteristic_params *req, void *p_priv);

/**
 * Response to @ref ble_gattc_rd_chars_req.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_rd_chars_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/**
 * Write characteristic on server.
 *
 * @param p_char_h_wr Buffer containing connection handle, handle of characteristic and type of write operation.
 * @param len         Length of p_char_h_wr.
 * @param p_buf       Characteristic value to write.
 * @param buflen      Characteristic value length. If length is bigger then ATT MTU size, the controller fragment buffer itself.
 * @param p_priv      Pointer to private data.
 */
void ble_gattc_wr_op(uint8_t *p_char_h_wr, uint8_t len,
		uint8_t *p_buf, uint8_t buflen, void *p_priv);

/**
 * Response to @ref ble_gattc_wr_op.
 *
 * @param rsp Response
 * @param p_priv Pointer to private data.
 */
void on_ble_gattc_wr_op_rsp(const struct ble_gattc_rsp *rsp, void *p_priv);

/** @} */

#endif
