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

#include<stdio.h>
#include "cfw/cfw_service.h"

#include "services/sensor_service/sensor_svc.h"
#include "sensor_svc_list.h"
#include "sensor_data_format.h"
#include "sensor_svc_platform.h"

static void host_svc_start_scanning_req_handler(
                        ss_start_sensor_scanning_req_t *p_req,
                        void *param)
{
     /* Process BLE sensor scan request */
    if (p_req->sensor_type_bit_map & BLE_TYPE_MASK) {
        #if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
        if (get_ss_ble_status() == BLE_CLOSE) {
            SS_PRINT_LOG("ble start request");
            ble_svc_start(service_mgr_queue_get());
        } else {
            ble_start_discover(p_ble_svc_handle,
                               BLE_SERVICE_ALL_SUPPORTED_SERVICES,
                               NULL);
        }
        #endif
    }
    /* Process BOARD sensor scanning request */
    if (p_req->sensor_type_bit_map & BOARD_SENSOR_MASK) {
        svc_send_scan_cmd_to_core(p_req->sensor_type_bit_map & BOARD_SENSOR_MASK);
    }
    /* Process ANT sensor scanning request */
    if (p_req->sensor_type_bit_map & ANT_TYPE_MASK) {
        /* TODO */
    }
}

static void host_svc_stop_scanning_req_handler(
                    ss_stop_sensor_scanning_req_t *p_req,
                    void *param)
{
    #if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
    ble_stop_discover(p_ble_svc_handle, NULL);
    #else
    send_rsp_message_to_client(((conn_handle_t *)(p_req->header.conn))->client_handle,
                               MSG_ID_SS_STOP_SENSOR_CANNING_RSP,
                               SS_STATUS_SUCCESS);
    #endif
}

static void host_svc_sensor_start_req_handler(
                        ss_sensor_start_req_t *p_req,
                        void *param)
{
    uint8_t sensor_type, sensor_id;

    sensor_type = ((uint32_t)p_req->sensor >> 8) & 0xFF;
    sensor_id = (uint32_t)p_req->sensor & 0xFF;

    if ((1 << sensor_type) & BLE_TYPE_MASK) {
            #if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
            ble_dev_list_t *p_ble_dev = get_ble_list(sensor_id);
            ble_connect(p_ble_svc_handle, &p_ble_dev->addr, 0X640, 0, NULL);
            #endif
        } else if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
            svc_send_start_cmd_to_core(sensor_type, sensor_id);
        }
}

static svc_platform_handler_t host_svc_handler = {
    .start_scanning = &host_svc_start_scanning_req_handler,
    .stop_scanning  = &host_svc_stop_scanning_req_handler,
    .sensor_start   = &host_svc_sensor_start_req_handler,
};

svc_platform_handler_t* get_host_svc_handler(void)
{
    SS_PRINT_LOG("get host service handler");
    return &host_svc_handler;
}

static void host_svc_client_connected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%s: %d", __func__, p_instance->client_port);
}

static void host_svc_client_disconnected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%s: %d", __func__, p_instance->client_port);
}

static service_t host_service =
{
    .service_id = HOST_SVC_ID,
    .client_connected = host_svc_client_connected,
    .client_disconnected = host_svc_client_disconnected
};

service_t *get_host_svc(void)
{
    SS_PRINT_LOG("get host service");
    return &host_service;
}
