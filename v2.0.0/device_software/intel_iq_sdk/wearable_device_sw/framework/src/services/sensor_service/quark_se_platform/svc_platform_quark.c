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

#include <stdio.h>
#include <stdint.h>

#include "cfw/cfw_service.h"

#include "services/sensor_service/sensor_svc.h"
#include "sensor_svc_list.h"
#include "sensor_data_format.h"
#include "sensor_svc_platform.h"
#include "sensor_svc_utils.h"
#include "services/sensor_service/sensor_svc_api.h"

#include "machine.h"

#include "cfw/cfw_service.h"
#include "cfw/cfw_messages.h"

/*
 * \brief :
 * --------------------------------------------------------------------------------------
 * APP<------>SS/QRK SERVICE<------->SC/ARC SERVICE<-------->SENSOR CORE<------->SENSOR DRIVERs
 *                          |
 *                          |
 *        QRK               |               ARC
 *                          |
 * --------------------------------------------------------------------------------------
 *
 * QRK SERIVCE has 2 handle:1.service_handle 2.test handle
 * Test handle: just handle the SC_SERVICR msg/data resp.
 *
 *when the quark_ss_svc start,it will create a test client for sensor_core svc
 */
static cfw_service_conn_t *p_sc_service_conn = NULL;
static cfw_client_t * p_client = NULL;

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
static void (*core_connected_cb)(void *);
static void * core_connected_cb_data;

void sensor_core_svc_rsp_handle(struct cfw_message *p_msg, void *p_param)
{
    switch (CFW_MESSAGE_ID(p_msg))
    {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:
        {
            cfw_open_conn_rsp_msg_t * cnf = (cfw_open_conn_rsp_msg_t*) p_msg;
            p_sc_service_conn = (cfw_service_conn_t *)cnf->service_conn;
            if( NULL == p_sc_service_conn ){
                SS_PRINT_ERR("Opening Score_svc failed");
                return;
            }
            SS_PRINT_LOG("Opening Score_svc successful");
            if (core_connected_cb) {
                core_connected_cb(core_connected_cb_data);
            }
        }break;
    case MSG_ID_CFW_REGISTER_EVT_RSP:
        {
            SS_PRINT_LOG("Register evt!!");
        }break;
    case MSG_ID_CFW_REGISTER_SVC_AVAIL_EVT_RSP:
        break;
    case MSG_ID_CFW_SVC_AVAIL_EVT:
        {
            uint16_t  service_id = ((cfw_svc_available_evt_msg_t *) p_msg)->service_id;
            SS_PRINT_LOG("AVAIL_EVT: %d", service_id);
            if ( ARC_SC_SVC_ID == service_id) {
                cfw_open_service_conn(p_client, ARC_SC_SVC_ID, NULL);
            }else{
                SS_PRINT_LOG("AVAIL_EVT:service_id is error");
            }
        }break;
    case MSG_ID_SS_START_SENSOR_SCANNING_RSP:
        {
            SS_PRINT_LOG("[%s]Scanning rsp", __func__);
        }break;
    case MSG_ID_SS_START_SENSOR_SCANNING_EVT:
        {
            ss_sensor_scan_event_t *p_evt = (ss_sensor_scan_event_t *)p_msg;
            uint8_t sensor_type = (uint8_t)(((uint32_t)p_evt->handle) >> 8);
            uint8_t sensor_id = (uint8_t)(((uint32_t)p_evt->handle) & 0xff);

            send_scan_evt_message_to_clients(sensor_type,sensor_id);
            SS_PRINT_LOG("[%s]Scanning evt", __func__);

        } break;
    case MSG_ID_SS_STOP_SENSOR_SCANNING_RSP:
        {
            SS_PRINT_LOG("[%s]Stop SCANNING RSP", __func__);
        } break;
    case MSG_ID_SS_SENSOR_PAIR_RSP:
        {
            SS_PRINT_LOG("[%s]PAIR RSP", __func__);
        } break;
    case MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP:
        {
            ss_svc_message_general_rsp_t *p_rsp =(ss_svc_message_general_rsp_t *)p_msg;
            uint8_t sensor_type = (uint8_t)(((uint32_t)p_rsp->handle) >> 8);
            uint8_t sensor_id = (uint8_t)(((uint32_t)p_rsp->handle) & 0xff);
            SS_PRINT_LOG("SUBSCRIBE_DATA_RSP:type=%d,id=%d,status=%d]",\
                                                                sensor_type,sensor_id, \
                                                                p_rsp->status);
            send_sensor_subscribing_rsp_message_to_clients(sensor_type,
                    sensor_id, p_rsp->status);
        }break;
    case MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_EVT:
        {
            ss_sensor_subscribe_data_event_t *p_evt = (ss_sensor_subscribe_data_event_t *)p_msg;
            ss_sensor_data_header_t *p_data_header = &p_evt->sensor_data_header;
            uint8_t sensor_type = (uint8_t)(((uint32_t)p_evt->handle) >> 8);
            uint8_t sensor_id = (uint8_t)(((uint32_t)p_evt->handle) & 0xff);
            send_sensor_subscribing_evt_message_to_clients(sensor_type, sensor_id, 0,  \
                                                            p_data_header->timestamp, \
                                                            (void*)p_data_header->data, \
                                                            p_data_header->data_length);

        } break;
    case MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP:
        {
            SS_PRINT_LOG("[%s]UNSUBSCRIBED RSP", __func__);
            ss_sensor_unsubscribe_data_rsp_t *p_rsp = (ss_sensor_unsubscribe_data_rsp_t *)p_msg;
            uint8_t sensor_type = GET_SENSOR_TYPE(p_rsp->handle);
            uint8_t sensor_id = GET_SENSOR_ID(p_rsp->handle);
            send_sensor_unsubscribing_rsp_message_to_clients(sensor_type,sensor_id, p_rsp->status);
        } break;
    case MSG_ID_SS_SENSOR_UNPAIR_RSP:
        break;
    case MSG_ID_SS_SENSOR_UNLINK_EVT:
        break;
    default:
        SS_PRINT_ERR("[%s]unhandled msg %d", __func__, CFW_MESSAGE_ID(p_msg));
        break;
    }
    cfw_msg_free(p_msg);
}
#endif

bool sensor_svc_open_sensor_core_svc(void *p_queue, void (*cb)(void *),
		void * cb_param)
{
#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
	core_connected_cb = cb;
	core_connected_cb_data = cb_param;
#endif
	p_client = cfw_client_init(p_queue,sensor_core_svc_rsp_handle, "Score_tst");
    if(!p_client)
        return false;
    cfw_register_svc_available(p_client, ARC_SC_SVC_ID, NULL);
    return true;
}

static void quark_svc_start_scanning_req_handler(
                        ss_start_sensor_scanning_req_t *p_req,
                        void *param)
{
    if(!p_req){
        SS_PRINT_ERR("err p_req msg");
        force_panic();
    }
    /* Process BOARD sensor scanning request */
    if (p_req->sensor_type_bit_map & BOARD_SENSOR_MASK) {
        if (p_sc_service_conn == NULL) {
            SS_PRINT_ERR("Service not opened yet");
            return;
        }
        ss_start_sensor_scanning(p_sc_service_conn, NULL, p_req->sensor_type_bit_map);
    }
}

static void quark_svc_stop_scanning_req_handler(
                    ss_stop_sensor_scanning_req_t *p_req,
                    void *param)
{
    if(!p_req){
        SS_PRINT_ERR("err p_req msg");
        force_panic();
    }
    /* Process BOARD sensor scanning request */
    if (p_req->sensor_type_bit_map & BOARD_SENSOR_MASK) {
        ss_stop_sensor_scanning(p_sc_service_conn, NULL, p_req->sensor_type_bit_map);
    }

}

static void quark_svc_sensor_subscribe_req_handler(
                ss_sensor_subscribe_data_req_t *p_req,
                void *param)
{
    ss_sensor_subscribe_data_req_t *p_msg;
    p_msg = (ss_sensor_subscribe_data_req_t *) cfw_alloc_message_for_service(p_sc_service_conn,
            MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_REQ, sizeof(*p_msg), NULL);
    if (p_msg == NULL) {
        SS_PRINT_ERR("err p_req msg");
        force_panic();
    }
    /* Fill Request Parameter */
    p_msg->sensor = p_req->sensor;
    p_msg->data_type = p_req->data_type;
    p_msg->sampling_interval = p_req->sampling_interval;
    p_msg->reporting_interval = p_req->reporting_interval;
    /* Send the message */
    cfw_send_message(p_msg);
}

static void quark_svc_sensor_unsubscribe_req_handler(
                ss_sensor_unsubscribe_data_req_t *p_req,
                void *param)
{
    ss_sensor_unsubscribe_data_req_t *p_msg;

    /* Allocate sensor unsubscribing data request message */
    p_msg = (ss_sensor_unsubscribe_data_req_t *) cfw_alloc_message_for_service(p_sc_service_conn,
            MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_REQ, sizeof(*p_msg), NULL);
    if (p_msg == NULL) {
        SS_PRINT_ERR("err p_req msg");
        force_panic();
    }
    /* Fill Request Parameter */
    p_msg->sensor = p_req->sensor;
    p_msg->data_type = p_req->data_type;

    /* Send the message */
    cfw_send_message(p_msg);
}

static svc_platform_handler_t quark_svc_handler = {
    .start_scanning = &quark_svc_start_scanning_req_handler,
    .sensor_subscribe = &quark_svc_sensor_subscribe_req_handler,
    .sensor_unsubscribe = &quark_svc_sensor_unsubscribe_req_handler,
    .stop_scanning  = &quark_svc_stop_scanning_req_handler,
};

svc_platform_handler_t* get_quark_svc_handler(void)
{
    return &quark_svc_handler;
}

static void quark_svc_client_connected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%d",p_instance->client_port);
}

static void quark_svc_client_disconnected(conn_handle_t *p_instance)
{
    SS_PRINT_LOG("%d", p_instance->client_port);
}

static service_t quark_service =
{
    .service_id = SS_SVC_ID,
    .client_connected = quark_svc_client_connected,
    .client_disconnected = quark_svc_client_disconnected
};

service_t *get_quark_svc(void)
{
    return &quark_service;
}
