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
#include "services/sensor_service/sensor_svc_list.h"
#include "services/sensor_service/sensor_data_format.h"
#include "services/sensor_service/sensor_svc_platform.h"
#include "services/sensor_service/sensor_svc_utils.h"

#include "ipc_comm.h"


extern int svc_send_scan_cmd_to_core(uint32_t);

static void arc_svc_start_scanning_req_handler(
                        ss_start_sensor_scanning_req_t *p_req,
                        void *param)
{
    svc_send_scan_cmd_to_core(p_req->sensor_type_bit_map & BOARD_SENSOR_MASK);
}

static void arc_svc_stop_scanning_req_handler(
                    ss_stop_sensor_scanning_req_t *p_req,
                    void *param)
{
    #if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
    ble_stop_discover(p_ble_svc_handle, NULL);
    #else
        ss_send_rsp_msg_to_client(((conn_handle_t *)(p_req->header.conn))->client_handle,
               MSG_ID_SS_STOP_SENSOR_SCANNING_RSP, SS_STATUS_SUCCESS, NULL);

    #endif
}


static void arc_svc_sensor_subscribe_req_handler(
                ss_sensor_subscribe_data_req_t *p_req,
                void *param)
{
    uint8_t sensor_type, sensor_id;

    sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    sensor_id   = GET_SENSOR_ID(p_req->sensor);

    if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
        svc_send_subscribe_data_cmd_to_core(sensor_type, sensor_id,
                                            p_req->sampling_interval,
                                            p_req->reporting_interval);
    }
}

static void arc_svc_sensor_unsubscribe_req_handler(
                ss_sensor_unsubscribe_data_req_t *p_req,
                void *param)
{
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint8_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
        svc_send_unsubscribe_data_cmd_to_core(sensor_type, sensor_id, p_req->data_type[1]);
    }
}

static void arc_svc_sensor_set_property_req_handler(
                ss_sensor_set_property_req_t *p_req,
                void *param)
{
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint8_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
        send_request_cmd_to_core( sensor_type,sensor_id,0,
                                         p_req->data_length,
                                         p_req->value,
                                         CMD_SET_PROPERTY);
    }
}

static void arc_svc_sensor_get_property_req_handler(
                ss_sensor_get_property_req_t *p_req,
                void *param)
{
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint8_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
        send_request_cmd_to_core( sensor_type,sensor_id,0,
                                         0,0,
                                         CMD_GET_PROPERTY);
    }
}

static void arc_svc_sensor_calibration_req_handle(ss_sensor_calibration_req_t *p_req,void *param)
{
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);

    if ((1 << sensor_type) & BOARD_SENSOR_MASK) {
            svc_send_calibration_cmd_to_core((void*)p_req);
    }
}

static svc_platform_handler_t arc_svc_handler = {
    .start_scanning = &arc_svc_start_scanning_req_handler,
    .stop_scanning  = &arc_svc_stop_scanning_req_handler,
    .sensor_subscribe = &arc_svc_sensor_subscribe_req_handler,
    .sensor_unsubscribe = &arc_svc_sensor_unsubscribe_req_handler,
    .sensor_set_property= &arc_svc_sensor_set_property_req_handler,
    .sensor_get_property= &arc_svc_sensor_get_property_req_handler,
    .sensor_calibration= &arc_svc_sensor_calibration_req_handle,
};

svc_platform_handler_t* get_arc_svc_handler(void)
{
    return &arc_svc_handler;
}

static void arc_svc_client_connected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%d", p_instance->client_port);
}

static void arc_svc_client_disconnected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%d", p_instance->client_port);
}

static service_t arc_service =
{
    .service_id = SS_SVC_ID,
    .client_connected = arc_svc_client_connected,
    .client_disconnected = arc_svc_client_disconnected
};

service_t *get_arc_svc(void)
{
    return &arc_service;
}
