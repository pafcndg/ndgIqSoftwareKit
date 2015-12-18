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

#ifndef __BLE_SERVICE_GATT_H__
#define __BLE_SERVICE_GATT_H__

#include <stdint.h>

#include "zephyr/bluetooth/uuid.h"

#include "ble_service.h"
#include "ble_service_gap_api.h"

/** GATT common definitions.
 *
 * @ingroup ble_core_service
 *
 * @addtogroup ble_core_service_gatt BLE core service common GATT definitions
 * @{
 */

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

#define BLE_GATT_INVALID_HANDLE              0x0000  /**< reserved invalid attribute handle */
#define BLE_GATT_MAX_HANDLE                  0xffff  /**< maximum handle in a BLE server */
#define BLE_GATT_START_HANDLE_DISCOVER       0x0001  /**< Value of start handle during discovery. */

/**
 * Format of the value of a characteristic, enumeration type.
 */
enum BLE_GATT_FORMATS {
	BLE_GATT_FORMAT_RES,	/* rfu */
	BLE_GATT_FORMAT_BOOL,	/* 0x01 boolean */
	BLE_GATT_FORMAT_2BITS,	/* 0x02 2 bit */
	BLE_GATT_FORMAT_NIBBLE,	/* 0x03 nibble */
	BLE_GATT_FORMAT_UINT8,	/* 0x04 uint8 */
	BLE_GATT_FORMAT_UINT12,	/* 0x05 uint12 */
	BLE_GATT_FORMAT_UINT16,	/* 0x06 uint16 */
	BLE_GATT_FORMAT_UINT24,	/* 0x07 uint24 */
	BLE_GATT_FORMAT_UINT32,	/* 0x08 uint32 */
	BLE_GATT_FORMAT_UINT48,	/* 0x09 uint48 */
	BLE_GATT_FORMAT_UINT64,	/* 0x0a uint64 */
	BLE_GATT_FORMAT_UINT128,/* 0x0B uint128 */
	BLE_GATT_FORMAT_SINT8,	/* 0x0C signed 8 bit integer */
	BLE_GATT_FORMAT_SINT12,	/* 0x0D signed 12 bit integer */
	BLE_GATT_FORMAT_SINT16,	/* 0x0E signed 16 bit integer */
	BLE_GATT_FORMAT_SINT24,	/* 0x0F signed 24 bit integer */
	BLE_GATT_FORMAT_SINT32,	/* 0x10 signed 32 bit integer */
	BLE_GATT_FORMAT_SINT48,	/* 0x11 signed 48 bit integer */
	BLE_GATT_FORMAT_SINT64,	/* 0x12 signed 64 bit integer */
	BLE_GATT_FORMAT_SINT128,/* 0x13 signed 128 bit integer */
	BLE_GATT_FORMAT_FLOAT32,/* 0x14 float 32 */
	BLE_GATT_FORMAT_FLOAT64,/* 0x15 float 64 */
	BLE_GATT_FORMAT_SFLOAT,	/* 0x16 IEEE-11073 16 bit SFLOAT */
	BLE_GATT_FORMAT_FLOAT,	/* 0x17 IEEE-11073 32 bit SFLOAT */
	BLE_GATT_FORMAT_DUINT16,/* 0x18 IEEE-20601 format */
	BLE_GATT_FORMAT_UTF8S,	/* 0x19 UTF-8 string */
	BLE_GATT_FORMAT_UTF16S,	/* 0x1a UTF-16 string */
	BLE_GATT_FORMAT_STRUCT,	/* 0x1b Opaque structure */
	BLE_GATT_FORMAT_MAX	/* 0x1c or above reserved */
};

/**
 * GATT indication types.
 */
enum BLE_GATT_IND_TYPES {
	BLE_GATT_IND_TYPE_NONE = 0,
	BLE_GATT_IND_TYPE_NOTIFICATION,
	BLE_GATT_IND_TYPES_INDICATION,
};

/**
 * GATT Write operation types
 *
 * (BT spec Vol 3, Part G, chapter. 4.9)
 * @note long char write, Prepare & Exe request are handled internally to the controller stack
 */
enum BLE_GATT_WR_OP_TYPES {
	BLE_GATT_WR_OP_NOP = 0,	    /**< normally not used except to cancel BLE_GATT_WR_OP_REQ long char write procedure */
	BLE_GATT_WR_OP_CMD,	    /**< Write Command, (no response) */
	BLE_GATT_WR_OP_REQ,	    /**< Write Request, Write response is received , if length is longer then ATT MTU, Prepare write procedure */
	BLE_GATT_WR_OP_SIGNED_CMD,  /**< Signed Write Command */
};

/** @} */

#endif
