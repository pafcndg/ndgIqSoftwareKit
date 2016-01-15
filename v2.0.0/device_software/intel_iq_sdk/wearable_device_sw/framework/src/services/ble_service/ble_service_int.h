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

#ifndef BLE_SERVICE_INT_H_
#define BLE_SERVICE_INT_H_

#include "cfw/cfw.h"
#include "services/ble_service/ble_service_api.h"
#include "gap_internal.h"
#include "ble_protocol.h"

/* forward declarations */
struct _ble_service_cb;

enum BLE_STATE {
	BLE_ST_NOT_READY = 0,
	BLE_ST_DISABLED,
	BLE_ST_ENABLED,
	BLE_ST_DTM
};

struct _ble_service_cb {
	cfw_client_t * client;	/* cfw client for BLE core */
	T_QUEUE queue; /* Queue for the messages */
	cfw_service_conn_t *p_bat_service_conn;	/* service connection of opened battery service */
	struct bt_conn *conn;
	uint8_t role;
	struct ble_gap_connection_values conn_values;
	struct ble_gap_connection_params peripheral_conn_params;
	uint8_t ble_state;
	uint16_t security_state; /* bonding state, connection state */
};

extern struct _ble_service_cb _ble_cb;

/** Send advertisment timeout event to application */
void ble_gap_advertisement_timeout(void);

/** Stop and delete advertisment timer */
void ble_delete_adv_timer(void);

/** Helper function to allocate response depending on number characteristics.
 */
struct ble_init_service_rsp * ble_alloc_init_service_rsp(
		struct ble_init_svc_req *p_req);
/** Helper function to handle service registration failures */
void handle_ble_add_service_failure(struct ble_init_svc_req *req,
		uint16_t status);

#ifdef CONFIG_BLE_CORE_TEST
void test_ble_service_init(void);
T_QUEUE get_service_queue(void);
#endif

#endif /* BLE_SERVICE_INT_H_ */
