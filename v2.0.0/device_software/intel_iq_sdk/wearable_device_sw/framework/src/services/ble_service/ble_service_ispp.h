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

#ifndef _BLE_SERVICE_ISPP_PROTOCOL_H_
#define _BLE_SERVICE_ISPP_PROTOCOL_H_

#include <stdint.h>
#include "cfw/cfw.h"

#include "zephyr/bluetooth/gatt.h"

struct ble_service_ispp_param {
	uint32_t options; /**< Options for ISPP init. */
};

/** ISPP control block. */
struct ble_ispp_cb {
	struct bt_conn *conn;
};

extern struct ble_ispp_cb ispp_cb;

/**
 * Register ISPP service.
 *
 * Starts registering the ISPP service with provide init values.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param priv pointer to application private data
 *
 * @return @ref OS_ERR_TYPE, in failure case msg needs to freed by callee
 *
 * @note
 */
int ble_init_service_ispp(cfw_service_conn_t *p_service_conn, void *priv);

/**
 * Helper functions to retrieve the corresponding value handle.
 */
uint16_t ble_ispp_ctrl_handle(void);
uint16_t ble_ispp_data_handle(void);

#endif /* _BLE_SERVICE_ISPP_PROTOCOL_H_ */
