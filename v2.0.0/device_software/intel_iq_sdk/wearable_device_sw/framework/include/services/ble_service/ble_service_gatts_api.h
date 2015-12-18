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

#ifndef __BLE_SERVICE_GATTS_H__
#define __BLE_SERVICE_GATTS_H__

#include "ble_service.h"
#include "ble_service_gap_api.h"
#include "ble_service_gatt.h"

/**  @defgroup ble_core_service_gatts BLE Core Service GATTS
 * @ingroup ble_core_service
 *
 * BLE Core GATTS Service APIs to implement GATT Servers.
 *
 * This API should only be used by BLE service to implement additional BLE profiles/services.
 *
 * Those the GATT server APIs provide the following services:
 * - Create an new (server) BLE service
 * - Add characteristics to the service
 * - Write local server characteristics
 * - Receive data when updated by client
 *
 * @note If a service is based on a 128 bit UUID (vendor service), all the characteristic
 * need to use the same 128 bit UUID base and only vary octets 12-13 of base UUID.
 *
 * @{
 */

/**
 * BLE GATTS max attribute length.
 * @note BLE controller dependent
 */
#define BLE_SVC_GATTS_FIX_ATTR_LEN_MAX 510 /**< Maximum length for fixed length Attribute Values. */
#define BLE_SVC_GATTS_VAR_ATTR_LEN_MAX 512 /**< Maximum length for variable length Attribute Values. */

/* forward declaration for callback handlers */
struct _ble_service_cb;
struct ble_gatt_notif_ind_rsp_msg;

/**
 * GATT server write ops.
 */
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
 * GATTS timeout.
 */
struct ble_gatts_timeout_evt {
	uint16_t conn_handle;    /**< Connection handle */
	uint16_t reason;         /**< Reason for timeout */
};

/**
 * Set attribute parameters.
 */
struct ble_gatts_set_attribute_params {
	uint16_t value_handle;      /* mandatory */
	uint16_t offset;            /* by default 0 */
};

struct ble_gatts_attribute_response {
	ble_status_t status;        /**< Status of the operation. */
	uint16_t value_handle;      /* mandatory */
	void * p_priv;
};

struct ble_gatts_get_attribute_params {
	uint16_t value_handle;     /* mandatory */
};

struct ble_gatts_svc_changed_params {
	uint16_t conn_handle;
	uint16_t start_handle;
	uint16_t end_handle;
};

struct conn_attributes_params {
	uint16_t conn_handle;
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
void on_ble_gatts_send_svc_changed_rsp(const struct ble_response *p_params);


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

/**
 * Write stack specific data of a previously bonded connection.
 *
 * The response to this request is received through @ref on_ble_gatts_write_conn_attributes_rsp
 *
 * @note this data is typically stored in NVRAM in relation ship to the bonded device!
 *       (e.g. CCD)
 *
 * @param p_params bounded connection parameter
 * @param p_data       data blob specific to stack to write
 * @param len          length of above byte stream (little endian)
 * @param p_priv       pointer to private data
 *
 */
void ble_gatts_write_conn_attributes_req(const struct conn_attributes_params *p_params,
				    uint8_t * p_data,
				    uint8_t len,
				    void *p_priv);

/**
 * Response to @ref ble_gatts_write_conn_attributes_req.
 *
 * @param p_params Response
 */
void on_ble_gatts_write_conn_attributes_rsp(const struct ble_response *p_params);

/**
 * Read stack specific data of the bonded connection.
 *
 * @note this data is typically stored in NVRAM in relation ship to the bonded device!
 *
 * @param p_params bounded connection parameter
 * @param p_priv       pointer to private data
 *
 */
void ble_gatts_read_conn_attributes_req(const struct conn_attributes_params *p_params,
				   void *p_priv);
/**
 * Response to @ref ble_gatts_read_conn_attributes_req.
 *
 * @param p_params Response
 */
void on_ble_gatts_read_conn_attributes_rsp(const struct ble_response *p_params);

/** @} */

#endif
