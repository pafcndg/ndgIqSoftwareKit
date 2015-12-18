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

#ifndef __BLE_APP_SVC_PROPERTIES_H__
#define __BLE_APP_SVC_PROPERTIES_H__

#include "ble_app_internal.h"
#include "ble_protocol.h"
#include "services/properties_service/properties_service_api.h"
#include "ble_service_utils.h"
#include "lib/ble/ble_app.h"

enum {
	BLE_PROPERTY_ID_DEVICE_NAME = 0,
	BLE_PROPERTY_ID_SM_CONFIG,
};

void handle_ble_property_read(struct cfw_message *msg);

/**
 * Reads BLE property from flash.
 *
 * Sends a read request for a corresponding BLE property.
 *
 * @param p_cb BLE master control block.
 * @param hdl name callback handler
 * @param ble_property_id BLE property to be read.
 *
 * @return  BLE_STATUS_SUCCESS if read request was sent,
 *          otherwise BLE_STATUS_ERROR.
 *
 */
int ble_properties_get(cfw_service_conn_t *p_service_properties_conn,
		struct ble_app_storage_handler *hdl, uint8_t ble_property_id);

/**
 * Saves BLE property to flash.
 *
 * Sends a write request for a corresponding BLE property.
 *
 * @param ble_property property to be saved.
 * @param ble_property_size property size.
 * @param property_id id of the property.
 * @param p_cb BLE master control block.
 *
 * @return  BLE_STATUS_ERROR, BLE_STATUS_ERROR_PARAMETER or BLE_STATUS_SUCCESS.
 *
 */
int ble_properties_save(void *ble_property, uint8_t ble_property_size,
		uint8_t property_id, cfw_service_conn_t *p_service_properties_conn);
#endif
