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

#ifndef BLE_APP_H_
#define BLE_APP_H_

#include <stdint.h>

/* For T_QUEUE */
#include "os/os_types.h"

// Forward declarations
struct ble_gap_connection_params;

/**
 * Set the device name
 *
 * @param p_device_name Pointer to the NULL terminated string
 */
void ble_app_set_device_name(const uint8_t *p_device_name);

/**
 * Start BLE application.
 *
 * Register and enable BLE and starts advertising depending on BLE service
 * internal state. Messages will be treated internally
 *
 * @param queue queue on which CFW messages will be forwarded.
 *
 */
void ble_start_app(T_QUEUE queue);

/**
 * Update BLE application handle parameters.
 *
 * This function should be called before after the BLE app connection is
 * established, so you can update it with specific parameters. It can be used in
 * very specific scenarios, e.g.: if you need specific tuning for the BLE
 * connection.
 *
 * @param p_params ble specific parameters
 * @return connection update return value, -1 if the connection is not ready.
 */
int ble_app_conn_update(const struct ble_gap_connection_params * p_params);

/*
 * Start advertisement based on security status
 *
 * This function starts the advertisement with the given options. The start may
 * return an error event if an advertisement is already ongoing.
 *
 * @param param advertisement options @ref BLE_ADV_OPTIONS
 */
void ble_app_start_advertisement(uint32_t param);

/*
 * Stop advertisement.
 *
 * This function stops the advertisement. If no advertisement is going, the app
 * receives an error status which can be ignored.
 *
 */
void ble_app_stop_advertisement(void);

/**
 * Clear all BLE bonding information (linkkeys etc).
 */
void ble_app_clear_bonds(void);

#endif /* BLE_APP_H_ */
