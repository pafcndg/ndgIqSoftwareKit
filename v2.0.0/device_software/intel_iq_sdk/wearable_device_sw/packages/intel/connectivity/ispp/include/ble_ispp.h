/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material is
 * protected by worldwide copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 */

#ifndef _BLE_SERVICE_ISPP_H_
#define _BLE_SERVICE_ISPP_H_

#include <stdint.h>
#include "services/ble_service/ble_service_gatts_api.h"
#include "util/compiler.h"

/* Forward declaration */
struct _ble_service_cb;

/** ISPP Base UUID: dd97c415-fed9-4766-b18f-ba690d24a06a. */
#define BLE_BASE_UUID_ISPP {0x6A, 0xA0, 0x24, 0x0D, 0x69, 0xBA, 0x8F, 0xB1,\
                            0x66, 0x47, 0xD9, 0xFE, 0x15, 0xC4, 0x97, 0xDD}

/** Octets 12-13 of ISPP Base UUID. */
#define BLE_UUID_ISPP_SERVICE           0xC415

/** Octets 12-15 of ISPP Base UUID. */
#define BLE_UUID32_ISPP_SERVICE         0xDD97C415

/** Number of ISPP characteristics. */
#define BLE_SVC_ISPP_NUM_CHARS 2

/** Octets 12-13 of Control characteristic UUID. */
#define BLE_UUID_ISPP_CONTROL_CHAR      0xC416

/** Octets 12-13 of Data characteristic UUID. */
#define BLE_UUID_ISPP_DATA_CHAR         0xCF01

/** Maximum size of the ISPP characteristics */
#define BLE_ISPP_MAX_CHAR_LEN 20

/* ISPP control block is in ble_service_int.h */
enum ble_ispp_events {
	BLE_ISPP_OPEN,
	BLE_ISPP_CLOSE,
	BLE_ISPP_RX_COMPLETE,
	BLE_ISPP_TX_COMPLETE,
};

typedef struct {
	uint16_t mtu;
} ble_ispp_open_s;

typedef struct {
	uint16_t size;
} ble_ispp_rx_s;

typedef struct {
	void *context;
	int status;
	uint8_t *p_data;
	uint16_t len;
} ble_ispp_tx_s;

typedef struct {
	enum ble_ispp_events event;
	union {
		ble_ispp_open_s open;
		ble_ispp_rx_s rx;
		ble_ispp_tx_s tx;
	} evt;
} ble_ispp_event_s;

struct ble_ispp_rx_message
{
    /* MUST be first in structure */
    list_t list;  // pointer to next message
    uint16_t len; // length of the message
    uint8_t buf[]; // payload of the message
};

/** Handles the write events from BLE stack on ctrl channel characteristic.
 *
 *
 * @param p_buf The data of the event received.
 * @param buflen The length of the data.
 */
void ble_ispp_on_write_ctrl(const uint8_t *p_buf, uint8_t buflen);

/** Handles the write events from BLE stack on data channel characteristic.
 *
 * @param p_buf The data of the event received.
 * @param buflen The length of the data.
 */
void ble_ispp_on_write_data(const uint8_t *p_buf, uint8_t buflen);

/** Function for handling disconnection event from BLE stack.
 *
 */
void ble_ispp_on_disconnect(void);

/** Function for reading a message out from the ISPP.
 *
 * \return p_msg Message to pass to upper layer.
 */
struct ble_ispp_rx_message * ble_ispp_read(void);

/** Function for writing a message to the ISPP.
 *
 * @param context Context reference used upon TX complete event.
 * @param p_data Message to send.
 * @param data_len Length of the message.
 *
 * @return BLE_STATUS_SUCCESS on successful write, otherwise an error code.
 */
uint32_t ble_ispp_write(void *context, const uint8_t * p_data, uint16_t data_len);

/** Function for writing a message to the ISPP.
 *
 * @param context Context reference used upon TX complete event.
 * @param p_header The header of the message.
 * @param header_len The length of the header.
 * @param p_data Message to send.
 * @param data_len Length of the message.
 *
 * @return BLE_STATUS_SUCCESS on successful write, otherwise an error code.
 */
uint32_t ble_ispp_write2(void *context, const uint8_t * p_header,
			 uint16_t header_len, const uint8_t * p_data, uint16_t data_len);

/** Function for writing a message to the ISPP in three parts
 *
 * @param context Context reference used upon TX complete event.
 * @param p_data1 Part of the message.
 * @param len1 The length of the part.
 * @param p_data2 Part of the message.
 * @param len2 The length of the part.
 * @param p_data3 Part of the message.
 * @param len3 The length of the part.
 *
 * @return BLE_STATUS_SUCCESS on successful write, otherwise an error code.
 */
uint32_t ble_ispp_write3(void *context,
			 const uint8_t * p_data1, uint16_t len1,
			 const uint8_t * p_data2, uint16_t len2,
			 const uint8_t * p_data3, uint16_t len3);

/**@brief Function for handling the BLE ISPP events.
 *
 * @details This function shall be implemented by the module
 * using the BLE ISPP functionality to handle ISPP events.
 *
 * @param[in]   p_ispp_evt ISPP event.
 */
__weak void on_ble_ispp_evt(const ble_ispp_event_s *p_ispp_evt);

#endif /* _BLE_SERVICE_ISPP_H_ */
