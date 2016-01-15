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

#ifndef BLE_APP_INTERNAL_H_
#define BLE_APP_INTERNAL_H_

#include <stdint.h>

/* For BLE_MAX_DEVICE_NAME */
#include "ble_protocol.h"
/* For bt_addr_le_t, ble_gap_sm_config_params */
#include "services/ble_service/ble_service_api.h"
/* For cfw_service_conn_t */
#include "cfw/cfw.h"
/* Forward declarations */
struct cfw_message;

struct ble_app_cb {
	cfw_service_conn_t *p_service_conn;
	cfw_service_conn_t *p_service_properties_conn;
	struct bt_conn *conn; /* Current connection reference */
	bt_addr_le_t my_bd_addr;
	/* the name must be stored in FULL because the property interface is asynchronous */
	uint8_t device_name[BLE_MAX_DEVICE_NAME + 1];
};


struct ble_app_storage_handler {
	/** callback on property reading, typically for enable/start
	 * advertisement.
	 * @param p_buf this buffer
	 * @param p_name pointer to the name, maybe NULL */
	void (*cback)(struct ble_app_storage_handler *p_buf,
			const void *p_storage_value);
	uint32_t param; /**< callback specific private data */
	struct ble_gap_sm_config_params sm_config;
	uint8_t data[]; /**< Callback private data */
};

#ifdef CONFIG_BLE_SM_IO_CAP_TEST
int ble_app_store_sm_cfg(struct ble_gap_sm_config_params *sm_params);
#endif

#endif /* BLE_APP_INTERNAL_H_ */
