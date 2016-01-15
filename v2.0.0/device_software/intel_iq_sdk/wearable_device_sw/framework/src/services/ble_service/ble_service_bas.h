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

#ifndef __BLE_SERVICE_BAS_H__
#define __BLE_SERVICE_BAS_H__

#include "cfw/cfw.h"
#include "ble_protocol.h"
#include "services/ble_service/ble_service_api.h"
#include "ble_service_int.h"

/**
 * Register BAS service.
 *
 * Starts registering the BAS service with provide init values.
 *
 * @param p_service_conn client service connection (cfw service connection)
 * @param priv pointer to application private data
 *
 * @return @ref OS_ERR_TYPE, in failure case msg needs to freed by callee
 */
int ble_init_service_bas(cfw_service_conn_t * p_service_conn, void *priv);

/**
 * Updates battery level.
 *
 * This updates the battery level value. This triggers a notification if remote
 * has enable notifications. Otherwise only value is updated.
 *
 * @param p_service_conn CFW application service connection
 * @param conn Connection on which the value shall be updated.
 * @param level New battery level (0-100%)
 * @param p_priv application private data (optional
 */
int ble_service_update_bat_level(cfw_service_conn_t *p_service_conn,
		struct bt_conn *conn, uint8_t level, void *p_priv);

#ifdef CONFIG_SERVICES_BLE_BAS_USE_BAT

/**
 * Helper function for battery update.
 *
 * - If the notifications are off, the battery level is stored in the GATT,
 *   so that the attribute is correct for the next _read_.
 * - If the notifications are on, a BLE message is sent to notify the new value.
 */
int handle_ble_update_service_bas(struct bt_conn *conn, uint8_t level);
#endif

/**
 * Enables or disables BLE BAS battery level updates.
 *
 * If disabled, the function ::ble_service_bas_level_update won't notify the
 * change even if notifications are set.
 * @param bas_en  set (1) or unset (0) the battery service updates
 */
void ble_service_bas_set_battery_updates(bool bas_en);

#endif
