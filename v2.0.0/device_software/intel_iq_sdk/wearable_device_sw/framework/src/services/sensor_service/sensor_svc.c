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

#include "os/os.h"
#include "cfw/cfw_service.h"
#include "cfw/cfw_messages.h"
#include "infra/message.h"

#include "services/sensor_service/sensor_svc.h"
#include "services/sensor_service/sensor_svc_list.h"
#include "services/sensor_service/sensor_data_format.h"
#include "services/sensor_service/sensor_svc_platform.h"
#include "services/sensor_service/sensor_svc_utils.h"
#include "services/sensor_service/sensor_svc_calibration.h"

uint16_t ss_svc_port_id= 0;
#define SENSOR_FSM_SWITCH(cur_status, flag)                     \
do {                                                            \
    if (!flag) {                                                \
        p_list->result.conn_status = cur_status + 1;            \
        if (p_list->result.conn_status > RELEASING) {           \
            p_list->result.conn_status = RELEASING;             \
        }                                                       \
    } else {                                                    \
        p_list->result.conn_status = cur_status - 1;            \
        if (p_list->result.conn_status < READY) {       \
            p_list->result.conn_status = READY;         \
        }                                                       \
    }                                                           \
} while(0)

#define CLIENT_FSM_SWITCH(next_status, cur_status, flag)    \
do {                                                        \
    if (!flag) {                                            \
        next_status = cur_status + 1;                       \
        if (next_status > RELEASING) {                      \
            next_status = RELEASING;                        \
        }                                                   \
    } else {                                                \
            next_status = cur_status - 1;                   \
            if (next_status > RELEASING) {                  \
                next_status = UNUSED;                       \
            }                                               \
        }                                                   \
} while(0)

#define TO_CLIENT_ARBIT_LIST(p_client_handle) container_of(p_client_handle, client_arbit_info_list_t, p_handle)
#define GET_DST_PORT(client_handle) (((conn_handle_t *)(((cfw_service_conn_t *) (client_handle))->server_handle))->client_port)
#define IS_CONNECTED_STATUS(status) ((status) == PAIRED ? 1 :   \
                                      ((status) == SUBSCRIBING ? 1 :    \
                                      ((status) == SUBSCRIBED ? 1 :     \
                                      ((status) == SUBSCRIBE_EVENT ? 1 :    \
                                      ((status) == UNSUBSCRIBED ? 1 :       \
                                      ((status) == SENSOR_STOPPING ? 1 :    \
                                      ((status) == SENSOR_STOPPED ? 1 :     \
                                      ((status) == UNPAIRING ? 1 : 0))))))))

#define CLIENT_IS_SCANNING(status) ((status) == SCAN_REQ ? 1 :          \
                                    ((status) == SCANNING ? 1 :         \
                                    ((status) == SCANNED ? 1 :0)))
/**
 * @brief  Update the connection status between service and client
 * @param  p_list: Arbitration list pointer
 *                new_status: Service connection status
 * @retval  SS_STATUS_ERROR : Update failed
 *               SS_STATUS_SUCCESS: Update successfull
 */
int8_t ss_client_con_status_update(client_arbit_info_list_t *p_list,
                            svc_status_t new_status)
{
    int8_t ret = SS_STATUS_ERROR;
    switch(new_status) {
    /*  Determine whether need to update status */
    case PAIRING:
        if (p_list->arbit_info.conn_status == READY  ||
            p_list->arbit_info.conn_status == UNPAIRED       ||
            p_list->arbit_info.conn_status == UNLINKED       ||
            p_list->arbit_info.conn_status == RELEASING) {
            ret = SS_STATUS_SUCCESS;
        }
        break;
     case SUBSCRIBING:
        if (p_list->arbit_info.conn_status == PAIRED          ||
            p_list->arbit_info.conn_status == UNSUBSCRIBED    ||
            p_list->arbit_info.conn_status == SENSOR_STOPPING ||
            p_list->arbit_info.conn_status == SENSOR_STOPPED) {
            ret = SS_STATUS_SUCCESS;
        }
        break;
     case UNSUBSCRIBING:
        if (p_list->arbit_info.conn_status == SUBSCRIBED ||
            p_list->arbit_info.conn_status == SUBSCRIBE_EVENT) {
            ret = SS_STATUS_SUCCESS;
        } break;
     case SENSOR_STOPPING:
        if (p_list->arbit_info.conn_status == PAIRED          ||
            p_list->arbit_info.conn_status == SUBSCRIBING     ||
            p_list->arbit_info.conn_status == SUBSCRIBED      ||
            p_list->arbit_info.conn_status == SUBSCRIBE_EVENT ||
            p_list->arbit_info.conn_status == UNSUBSCRIBING   ||
            p_list->arbit_info.conn_status == UNSUBSCRIBED) {
            ret = SS_STATUS_SUCCESS;
        }
        break;
     case UNPAIRING:
        if (p_list->arbit_info.conn_status == PAIRED          ||
            p_list->arbit_info.conn_status == SUBSCRIBING     ||
            p_list->arbit_info.conn_status == SUBSCRIBED      ||
            p_list->arbit_info.conn_status == SUBSCRIBE_EVENT ||
            p_list->arbit_info.conn_status == UNSUBSCRIBING   ||
            p_list->arbit_info.conn_status == UNSUBSCRIBED) {
            ret = SS_STATUS_SUCCESS;
        }
        break;
    case RELEASING:
       if (p_list->arbit_info.conn_status == READY   ||
           p_list->arbit_info.conn_status == UNPAIRED        ||
           p_list->arbit_info.conn_status == UNLINKED) {
           ret = SS_STATUS_SUCCESS;
       }
       break;
    default:
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Error: Current client connection status %d",
                                        p_list->arbit_info.conn_status);
        #endif
        break;
    }
    if (ret == SS_STATUS_SUCCESS) {
        p_list->arbit_info.conn_status = new_status;
    }
    return ret;
}

int8_t ss_sensor_arbit_info_update(client_arbit_info_t *p_arbit, void *p_info)
{
    if (p_arbit == NULL) {
        return SS_STATUS_ERROR;
    }
    client_arbit_info_t *p_arbit_info = (client_arbit_info_t *)p_info;

    p_arbit->conn_status = p_arbit_info->conn_status;
    if (p_arbit_info->conn_status == SUBSCRIBING) {
        p_arbit->subscribe_data_param.reporting_interval =
            p_arbit_info->subscribe_data_param.reporting_interval;
        p_arbit->subscribe_data_param.sampling_interval =
            p_arbit_info->subscribe_data_param.sampling_interval;
    }
    return SS_STATUS_SUCCESS;
}

/**
 * @brief  Arbitrate new request connection info with previous to
 *             detemind whether update sensor current connection info.
 * @param  p_list: Sensor device list pointer
 *         p_info: The latest arbitration result info
 * @retval  SS_STATUS_ERROR : Requesting an illegal status for sensor
 *           SS_STATUS_SUCCESS: Need to update sensor  status
 *           SS_STATUS_CMD_REPEAT: Sensor has been the requesting status
 */
int8_t ss_sensor_new_status_arbit(ss_sensor_dev_list_t *p_list,
                                         uint8_t sensor_status)
{
    int8_t ret = SS_STATUS_ERROR;

    if (p_list == NULL) {
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Sensor device list is NULL");
        #endif
        return SS_STATUS_ERROR;
    }

    switch (sensor_status) {
    case PAIRING:
        if (p_list->result.conn_status == READY ||
            p_list->result.conn_status == UNPAIRED      ||
            p_list->result.conn_status == UNLINKED      ||
            p_list->result.conn_status == RELEASING) {
            ret = SS_STATUS_SUCCESS;
        } else { /* In other cases the sensor has been paired */
            ret = SS_STATUS_CMD_REPEAT;
        } break;
    case SUBSCRIBING:
        if (p_list->result.conn_status == PAIRED          ||
            p_list->result.conn_status == UNSUBSCRIBED    ||
            p_list->result.conn_status == SENSOR_STOPPING ||
            p_list->result.conn_status == SENSOR_STOPPED  ||
            p_list->result.conn_status == UNPAIRED) {
            ret = SS_STATUS_SUCCESS;
        } else if (p_list->result.conn_status == SUBSCRIBING     ||
                   p_list->result.conn_status == SUBSCRIBED      ||
                   p_list->result.conn_status == SUBSCRIBE_EVENT ||
                   p_list->result.conn_status == UNSUBSCRIBING) {
            ret = SS_STATUS_CMD_REPEAT;
        } break;
    case UNSUBSCRIBING:
        if (p_list->result.conn_status == SUBSCRIBED  ||
            p_list->result.conn_status == SUBSCRIBE_EVENT) {
            ret = SS_STATUS_SUCCESS;
        } else if (p_list->result.conn_status == UNSUBSCRIBING   ||
                   p_list->result.conn_status == UNSUBSCRIBED    ||
                   p_list->result.conn_status == SENSOR_STOPPING ||
                   p_list->result.conn_status == SENSOR_STOPPED  ||
                   p_list->result.conn_status == UNPAIRING) {
            ret = SS_STATUS_CMD_REPEAT;
        } break;
    case SENSOR_STOPPING:
        if (p_list->result.conn_status == PAIRING         ||
            p_list->result.conn_status == SUBSCRIBING     ||
            p_list->result.conn_status == SUBSCRIBED      ||
            p_list->result.conn_status == SUBSCRIBE_EVENT ||
            p_list->result.conn_status == UNSUBSCRIBING   ||
            p_list->result.conn_status == UNSUBSCRIBED) {
            ret = SS_STATUS_SUCCESS;
        } else if (p_list->result.conn_status == SENSOR_STOPPING ||
                   p_list->result.conn_status == SENSOR_STOPPED) {
            ret = SS_STATUS_CMD_REPEAT;
        } break;
    case UNPAIRING:
        if (p_list->result.conn_status == PAIRED          ||
            p_list->result.conn_status == SUBSCRIBING     ||
            p_list->result.conn_status == SUBSCRIBED      ||
            p_list->result.conn_status == SUBSCRIBE_EVENT ||
            p_list->result.conn_status == UNSUBSCRIBING   ||
            p_list->result.conn_status == UNSUBSCRIBED    ||
            p_list->result.conn_status == SENSOR_STOPPING ||
            p_list->result.conn_status == SENSOR_STOPPED) {
            ret = SS_STATUS_SUCCESS;
        } else if (p_list->result.conn_status == UNPAIRING ||
                   p_list->result.conn_status == UNPAIRED  ||
                   p_list->result.conn_status == RELEASING) {
            ret = SS_STATUS_CMD_REPEAT;
        } break;
    case RELEASING:
        ret = SS_STATUS_SUCCESS;
        break;
    default:
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Requesting illegal sensor status: %d", sensor_status);
        #endif
        break;
    }
    if (ret == SS_STATUS_ERROR) { /* Just used for debug*/
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Requesting sensor status failed. Sensor status: %d, requesting status: %d",
                    p_list->result.conn_status, sensor_status);
        #endif
    }
    return ret;
}

int ss_send_rsp_msg_to_client(void *client_handle, uint16_t msg_id,
                               ss_service_status_t status,
                               void *p_sensor_handle)
{
    ss_svc_message_general_rsp_t *p_msg = (ss_svc_message_general_rsp_t *)
                                balloc(sizeof(ss_svc_message_general_rsp_t), NULL);
    CFW_MESSAGE_TYPE(&p_msg->header)= TYPE_RSP;
    CFW_MESSAGE_ID(&p_msg->header) = msg_id;
    CFW_MESSAGE_LEN(&p_msg->header) = sizeof(ss_svc_message_general_rsp_t);
    CFW_MESSAGE_DST(&p_msg->header) = GET_DST_PORT(client_handle);
    CFW_MESSAGE_SRC(&p_msg->header) = _cfw_get_service_port (SS_SVC_ID);
    p_msg->header.priv = NULL;
    p_msg->header.conn = client_handle;
    p_msg->status = status;
    p_msg->handle = p_sensor_handle;
    return cfw_send_message(p_msg);
}

static int send_evt_msg_to_client(struct cfw_message *p_msg,
                               void *client_handle,
                               uint16_t msg_id)
{
    if (p_msg == NULL) {
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Sending event message failed");
        #endif
        return SS_STATUS_ERROR;
    }

    CFW_MESSAGE_TYPE(p_msg)= TYPE_RSP;
    CFW_MESSAGE_ID(p_msg) = msg_id;
    CFW_MESSAGE_DST(p_msg) = GET_DST_PORT(client_handle);

#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
    SS_PRINT_LOG("...client port id.%d........", CFW_MESSAGE_DST(p_msg));
#endif
    CFW_MESSAGE_SRC(p_msg) = _cfw_get_service_port (SS_SVC_ID);
    p_msg->priv = NULL;
    p_msg->conn = client_handle;
    if (cfw_send_message(p_msg) != 0) {
        return SS_STATUS_ERROR;
    } else {
        return SS_STATUS_SUCCESS;
    }
}

void ss_send_scan_rsp_msg_to_clients(uint32_t sensor_type_bit_map,
                                      ss_service_status_t status)
{
    list_head_t client_header = get_client_list_head();

    ss_client_list_t *p_list = (ss_client_list_t *)client_header.head;

    int err = -1;
    while(p_list) {
        if (ss_update_client_list_status(p_list,
                 sensor_type_bit_map, SCANNING) == SS_LIST_SUCCESS) {
            if (!(p_list->rsp_status & SCAN_RSP_FLAG)) {/* To avoid multiply response */
                /* Respone sensor scanning request */
                err = ss_send_rsp_msg_to_client(p_list->p_client_handle,
                      MSG_ID_SS_START_SENSOR_SCANNING_RSP, status, NULL);
                if (err != 0) {
                    SS_PRINT_ERR("Scanning respone message can't reach to client!!!");
                    panic(0);
                    return;
                }
                p_list->rsp_status |= SCAN_RSP_FLAG;
            } else {
                err = 0;
            }
        }
        p_list = (ss_client_list_t *)p_list->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("No client is requesting scan");
    }
}

void ss_send_stop_scan_rsp_msg_to_clients(uint32_t sensor_type_bit_map,
                                           ss_service_status_t status)
{
    list_head_t client_header = get_client_list_head();
    ss_client_list_t *p_list = (ss_client_list_t *)client_header.head;
    int err = -1;

    while(p_list) {
        if (ss_update_client_list_status(p_list,
            sensor_type_bit_map, SCAN_STOPPED) == SS_LIST_SUCCESS) {
            uint8_t unconn_status[32] = {0};
            ss_get_client_list_status_ext(p_list, BOARD_SENSOR_MASK,
                                                  unconn_status);
            uint8_t i = 0;
            for(i = 0; i < 32; i++) {
                if (unconn_status[i] == SCAN_STOP_REQ)
                    break;
            }
            if (i == 32) { /* To be sure all types of sensors have stopped scanning */
                /* Respone sensor scanning request */
                err = ss_send_rsp_msg_to_client(p_list->p_client_handle,
                      MSG_ID_SS_STOP_SENSOR_SCANNING_RSP, status, NULL);
                if (err != 0) {
                    SS_PRINT_ERR("Stopping scan respone message can't reach to client!!!");
                    panic(0);
                    return;
                }
            } else {
                err = 0;
            }
        }
        p_list = (ss_client_list_t *)p_list->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("No client is scanning");
    }
}

void ss_send_pair_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Excuting failed");
        return;
    }
    client_arbit_info_list_t *l =
        (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    SENSOR_FSM_SWITCH(PAIRING, status);  /* Update sensor status */
    while(l) {
        if (l->arbit_info.conn_status == PAIRING) {
            ss_send_rsp_msg_to_client(l->p_handle, MSG_ID_SS_SENSOR_PAIR_RSP, status, sensor_handle);
            /* Update client's connection status */
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, PAIRING, status);
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }

}

void ss_send_subscribing_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                    ss_service_status_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);

    if (p_list == NULL) {
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("The sensor (0x:%x) cann't be found", sensor_handle);
        #endif
        return;
    }
    SENSOR_FSM_SWITCH(SUBSCRIBING, status);  /* Update sensor status */
    client_arbit_info_list_t *l =
        (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    int err = -1;

    while(l) {
        if (l->arbit_info.conn_status == SUBSCRIBING) {
            err = ss_send_rsp_msg_to_client(l->p_handle,
               MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP, status, sensor_handle);
            if (err != 0) {
                SS_PRINT_ERR("Subscribing respone message can't reach to client!!!");
                panic(0);
                return;
            }
             /* Update client's connection status */
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, SUBSCRIBING, status);
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("No client ready to rsv subscribe rsp!!");
    }

}

void ss_send_subscribing_evt_msg_to_clients(ss_sensor_t sensor_handle,
                            uint8_t data_type, uint32_t timestamp, void *p_data, uint16_t len)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    uint8_t sensor_type = GET_SENSOR_TYPE(sensor_handle);

    if (p_list == NULL) {
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("The sensor (0x:%x) cann't be found", sensor_handle);
        #endif
        return;
    }
    SENSOR_FSM_SWITCH(SUBSCRIBED, SS_STATUS_SUCCESS);  /* Update sensor status */

    client_arbit_info_list_t *l = (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;

    int err = -1;

    while(l) {
        if (l->arbit_info.conn_status == SUBSCRIBED  ||
            l->arbit_info.conn_status == SUBSCRIBE_EVENT) {
            l->arbit_info.conn_status = SUBSCRIBE_EVENT;  /* Update client's connection status */
            ss_sensor_subscribe_data_event_t *p_msg = balloc(
                sizeof(ss_sensor_subscribe_data_event_t) + len, NULL);
            if (p_msg == NULL) {
                SS_PRINT_ERR("Allocing mem failed");
                return;
            }
            p_msg->handle = sensor_handle;
            CFW_MESSAGE_LEN(&p_msg->head) = sizeof(ss_sensor_subscribe_data_event_t) + len;
            p_msg->sensor_data_header.data_length = len;
            p_msg->sensor_data_header.sensor_type = sensor_type;
            p_msg->sensor_data_header.subscription_type = data_type;
            p_msg->sensor_data_header.timestamp = timestamp;
            data_cpy(p_msg->sensor_data_header.data, p_data, len);
            send_evt_msg_to_client((struct cfw_message *) p_msg, l->p_handle, MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_EVT);
            err = 0;
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("Arbitration list may be damaged");
        l = (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
        while(l) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("sensor handle:0x%x, conn_status:%d ",
                sensor_handle, l->arbit_info.conn_status);
#endif
            l = (client_arbit_info_list_t *)l->list.next;
        }
    }
}

void ss_send_unsubscribing_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                    ss_service_status_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);

    if (p_list == NULL) {
    #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("The sensor (0x:%x) cann't be found", sensor_handle);
    #endif
        return;
    }
    SENSOR_FSM_SWITCH(UNSUBSCRIBING, status);  /* Update sensor status */
    client_arbit_info_list_t *l = (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    int err = -1;

    while(l) {
        if (l->arbit_info.conn_status == UNSUBSCRIBING) {
            err = ss_send_rsp_msg_to_client(l->p_handle,
                MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP, status, sensor_handle);
            if (err != 0) {
                SS_PRINT_ERR("Subscribing rsp msg can't reach to client!!!");
                panic(0);
                return;
            }
            if (IS_ON_BOARD_SENSOR_TYPE(GET_SENSOR_TYPE(sensor_handle))) {
                ss_arbit_info_list_delete(&p_list->arbit_info_list_header, (list_t *)l);
                if (ss_arbit_info_list_length(&p_list->arbit_info_list_header) == 0) {
                    ss_sensor_node_delete((list_t *)p_list);
                }
            }
            /* Update client's connection status */
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, UNSUBSCRIBING, status);
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("Arbitration list may be damaged");
    }

}

void ss_send_set_property_rsp_to_clients(ss_sensor_t sensor_handle,uint8_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Excuting failed");
        return;
    }
    client_arbit_info_list_t *l =(client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    svc_foreach_list(l) {
        if (l->arbit_info.flag & SENSOR_SET_PROPERTY_FLAG_MASK ) {
            l->arbit_info.flag &= ~SENSOR_SET_PROPERTY_FLAG_MASK;
            uint8_t err = ss_send_rsp_msg_to_client(l->p_handle, MSG_ID_SS_SENSOR_SET_PROPERTY_RSP, status, sensor_handle);
            if (err == -1) {
                SS_PRINT_ERR("Arbitration list may be damaged!!!");
                return;
            }
       }
    }
}

void ss_send_get_property_data_to_clients(ss_sensor_t sensor_handle,uint8_t length, uint8_t *propert_data, uint8_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Excuting failed");
        return;
    }
    client_arbit_info_list_t *l =(client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    svc_foreach_list(l) {
        if (l->arbit_info.flag & SENSOR_GET_PROPERTY_FLAG_MASK ) {
            l->arbit_info.flag &= ~SENSOR_GET_PROPERTY_FLAG_MASK;
            uint8_t err = ss_send_rsp_msg_to_client(l->p_handle, MSG_ID_SS_SENSOR_GET_PROPERTY_RSP, status, sensor_handle);
            if (err == -1) {
                SS_PRINT_ERR("Arbitration list may be damaged!!!");
                return;
            }
            if(!length || !propert_data)
                return;
            ss_sensor_get_property_evt_t *p_msg = balloc(sizeof(ss_sensor_get_property_evt_t) + length, NULL);
            if (p_msg == NULL) {
                SS_PRINT_ERR("Allocing memory failed");
                return;
            }
            p_msg->handle= sensor_handle;
            CFW_MESSAGE_LEN(&p_msg->head) = sizeof(ss_sensor_get_property_evt_t) + length;
            p_msg->data_length = length;
            data_cpy(p_msg->value, propert_data, length);
            send_evt_msg_to_client((struct cfw_message *) p_msg, l->p_handle, MSG_ID_SS_SENSOR_GET_PROPERTY_EVT);
        }
    }
}

static uint8_t clb_sensor_type_flag = 0;

void ss_send_cal_rsp_and_data_to_clients(ss_sensor_t sensor_handle, uint8_t calibration_type,
                                      uint8_t length, uint8_t *cal_data, uint8_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Excuting failed");
        return;
    }
    client_arbit_info_list_t *l =(client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    svc_foreach_list(l) {
        if (l->arbit_info.flag & SENSOR_CALIBRATION_FLAG_MASK ) {
            clb_sensor_type_flag--;
            if(!clb_sensor_type_flag)
                l->arbit_info.flag &= ~SENSOR_CALIBRATION_FLAG_MASK;
            uint8_t err = ss_send_rsp_msg_to_client(l->p_handle, MSG_ID_SS_SENSOR_CALIBRATION_RSP, status, sensor_handle);
            if (err == -1) {
                SS_PRINT_ERR("Arbitration list may be damaged!!!");
                return;
            }
            if(!length || !cal_data)
                return;
            ss_sensor_get_cal_data_evt_t *p_msg = balloc(sizeof(ss_sensor_get_cal_data_evt_t) + length, NULL);
            if (p_msg == NULL) {
                SS_PRINT_ERR("Allocing memory failed");
                return;
            }
            p_msg->handle= sensor_handle;
            CFW_MESSAGE_LEN(&p_msg->head) = sizeof(ss_sensor_get_cal_data_evt_t) + length;
            p_msg->data_length = length;
            p_msg->calibration_type = calibration_type;
            data_cpy(p_msg->value, cal_data, length);
            send_evt_msg_to_client((struct cfw_message *) p_msg, l->p_handle, MSG_ID_SS_SENSOR_CALIBRATION_EVT);
        }
    }
}

void ss_send_unpair_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                    ss_service_status_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);

    if (p_list == NULL) {
    #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
       SS_PRINT_LOG("The sensor (0x:%x) cann't be found", sensor_handle);
    #endif
        return;
    }
    SENSOR_FSM_SWITCH(UNPAIRING, status);  /* Update sensor status */
    client_arbit_info_list_t *l = (client_arbit_info_list_t *)
                                p_list->arbit_info_list_header.head;
    int err = -1;

    while(l) {
        if (l->arbit_info.conn_status == UNPAIRING) {
            err = ss_send_rsp_msg_to_client(l->p_handle,
                 MSG_ID_SS_SENSOR_UNPAIR_RSP, status, sensor_handle);
            if (err != 0) {
                SS_PRINT_ERR("Unpair rsp msg can't reach to client!!!");
                panic(0);
                return;
                ss_delete_client_list(l->p_handle); /* Remove the client info from client list */
            }
            /* Update client's connection status */
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, UNPAIRING, status);
            ss_arbit_info_list_delete(&p_list->arbit_info_list_header, (list_t *)l);
            if (ss_arbit_info_list_length(&p_list->arbit_info_list_header) == 0) {
                ss_sensor_node_delete((list_t *)p_list);
            }
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }
    if (err == -1) {
        SS_PRINT_ERR("No sensors request unpairing");
    }
}

void ss_send_unlink_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                 ss_service_status_t status)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    uint8_t sensor_type = GET_SENSOR_TYPE(sensor_handle);

    if (p_list == NULL) {
        SS_PRINT_ERR("Sensor (0x:%x) cann't be found", sensor_handle);
        return;
    }
    p_list->result.conn_status = UNLINKED;      /* Update sensor status */

    client_arbit_info_list_t *l = (client_arbit_info_list_t *)
                                    p_list->arbit_info_list_header.head;
    int err = -1;

    while(l) {
        if (((0x01 << sensor_type) & BLE_TYPE_MASK) ||
            ((0x01 << sensor_type) & ANT_TYPE_MASK)) {
            if (IS_CONNECTED_STATUS(l->arbit_info.conn_status)) {
                /* Any sensor being connected will recevie the unlink event */
                l->arbit_info.conn_status = UNLINKED;
                err = ss_send_rsp_msg_to_client(l->p_handle, MSG_ID_SS_SENSOR_UNPAIR_EVT, status, sensor_handle);
                if (err != 0) {
                    SS_PRINT_ERR("Unpair rsp can't reach to client");
                    panic(0);
                    return;
                }
                ss_arbit_info_list_delete(&p_list->arbit_info_list_header, (list_t *)l);
                if (ss_arbit_info_list_length(&p_list->arbit_info_list_header) == 0) {
                    ss_sensor_node_delete((list_t *)p_list);
                }
            }
            l = (client_arbit_info_list_t *)l->list.next;
        }
    }
    if (err == -1) {
        SS_PRINT_ERR("No wireless sensor being connected");
    }
}

void ss_send_scan_data_to_clients(uint8_t sensor_type, void *p_data)
{
    list_head_t client_head = get_client_list_head();

    ss_client_list_t *p_list = (ss_client_list_t *)client_head.head;

    while(p_list) {
        /* Find the client who keeps scanning or scanned status */
        if (ss_update_client_list_status(p_list, 1 << sensor_type, SCANNED)
                                                        == SS_LIST_SUCCESS) {
            uint8_t len_ext = 0;
#if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
            if (IS_BLE_SENSOR_TYPE(sensor_type)) {
                len_ext = ((ss_ble_scan_data_t *)p_data)->device_name_len;
            }
#endif
            ss_sensor_scan_event_t *p_evt = (ss_sensor_scan_event_t *)
                balloc(sizeof(ss_sensor_scan_event_t) + len_ext, NULL);
            CFW_MESSAGE_LEN(&p_evt->head) = sizeof(ss_sensor_scan_event_t) + len_ext;
            p_evt->sensor_type = sensor_type;
            if(IS_ON_BOARD_SENSOR_TYPE(sensor_type)) {
                p_evt->on_board_data = *(ss_on_board_scan_data_t *)p_data;
            } else if (IS_ANT_SENSOR_TYPE(sensor_type)) {
#if defined(ANT_SERVICE) && (ANT_SERVICE == 1)
                p_evt->ant_data = *(ss_ant_scan_data_t *)p_data;
#endif
            } else if (IS_BLE_SENSOR_TYPE(sensor_type)) {
#if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
                p_evt->ble_data = *(ss_ble_scan_data_t *)p_data;
                if (len_ext)
                    memcpy(p_evt->ble_data.device_name,
                          ((ss_ble_scan_data_t *)p_data)->device_name,
                          len_ext);
#endif
            }

            send_evt_msg_to_client((struct cfw_message *) p_evt,
                p_list->p_client_handle, MSG_ID_SS_START_SENSOR_SCANNING_EVT);
        }
        p_list = (ss_client_list_t *)p_list->list.next;
    }
}

static void ss_svc_start_scanning_req_handler(
            ss_start_sensor_scanning_req_t *p_req, void *param)
{
    void *p_handle;
    ss_client_list_t *p_list;

    p_handle = ((conn_handle_t *)(p_req->header.conn))->client_handle;
    p_list = ss_get_client_list(p_handle);
    if (p_list == NULL) {
        /* Save client handle */
        p_list = ss_client_list_add(p_handle);
        if (p_list == NULL) {
            SS_PRINT_ERR("Saving client handle failed");
            return;
        }
    }
    /*
      * Initialized client unconnection status before send scanning request.
      */
    p_list->unconn_status1 = UNUSED;
    p_list->unconn_status2 = UNUSED;
    p_list->unconn_status3 = UNUSED;
    p_list->rsp_status = 0x00;

    /* Record current client status */
    ss_update_client_list_status(p_list, p_req->sensor_type_bit_map, SCAN_REQ);
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)param;

   p_platform_handler->start_scanning(p_req, NULL);
}

static void ss_svc_stop_scanning_req_handler(
             ss_stop_sensor_scanning_req_t *p_req, void *param)
{
    void *p_handle;
    ss_client_list_t *p_list;

    p_handle = GET_CLIENT_HANDLE(p_req);
    p_list = ss_get_client_list(p_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Client list may be damaged");
        ss_send_stop_scan_rsp_msg_to_clients(p_req->sensor_type_bit_map, SS_STATUS_ERROR);
        return;
    }
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)param;
    /* Record current client status */
    ss_update_client_list_status(p_list, p_req->sensor_type_bit_map, SCAN_STOP_REQ);
    /*
     *  When there are more than one clients, must be sure that all clients have requested stop scanning
     *  before requesting stop scanning operation to physical sensor. Or one client scanning process may
     *  be interrupted accidently by other client's stop scanning request.
     */
    p_platform_handler->stop_scanning(p_req, NULL);
}

static void ss_svc_subscribe_handler(ss_sensor_subscribe_data_req_t *p_req,
                                     void *p_param)
{
    void *p_client_handle = GET_CLIENT_HANDLE(p_req);;
    ss_sensor_dev_list_t *p_list;

    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint32_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    p_list = ss_get_sensor_dev_list(p_req->sensor);
    if (p_list == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* On-board sensor list need to built up here  */
            p_list = ss_sensor_list_add(sensor_type,sensor_id);
            if (p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
                SS_PRINT_ERR("Sensor(0x%x) can't req to subscribe data",
                        (uint32_t)p_req->sensor);
#endif
                goto EXIT;
            }
            /* On-board sensor default status is PAIRED */
            p_list->result.conn_status = PAIRED;
        }
    }
    if (p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Sensor(0x%x) cann't request to subscribe data", (uint32_t)p_req->sensor);
#endif
        goto EXIT;
    }
    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);
    if (l == NULL) {
        /* On-board sensor arbit info need be saved here */
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* Save client connection info*/
            l = ss_arbit_info_list_add(p_list, p_client_handle);
            if (l == NULL) {
                SS_PRINT_ERR("Saving sensor arbitration failed");
                return;
            }
            /* Keep On-board sensor status being same with wireless sensor */
            l->arbit_info.conn_status = PAIRED;
        } else {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("Not connect. Client can't req subscribing");
#endif
            goto EXIT;
        }
    }
    if (ss_client_con_status_update(l, SUBSCRIBING) == SS_STATUS_ERROR) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("l->arbit_info.conn_status : %d", l->arbit_info.conn_status);
        SS_PRINT_ERR("Client can't switch to SUBSCRBING status from %d",
                                                    l->arbit_info.conn_status);
#endif
        goto EXIT;
    }
    uint8_t arbitrating_is_ok = ss_sensor_new_status_arbit(p_list, SUBSCRIBING);
    switch(arbitrating_is_ok) {
            case SS_STATUS_SUCCESS:
                p_list->result.conn_status = SUBSCRIBING; /* Set sensor status */
                p_list->result.subscribe_data_param.reporting_interval =
                                                        p_req->reporting_interval;
                p_list->result.subscribe_data_param.sampling_interval =
                                                        p_req->sampling_interval;
                svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
                p_platform_handler->sensor_subscribe(p_req, NULL);
                break;
            case SS_STATUS_CMD_REPEAT:
                {
                    uint16_t sampling_interval = 0, reporting_interval = 0;

                    client_arbit_info_list_t *iterator = (client_arbit_info_list_t *)
                                        p_list->arbit_info_list_header.head;
                    while(iterator) {
                        if (iterator->arbit_info.conn_status == SUBSCRIBING     ||
                            iterator->arbit_info.conn_status == SUBSCRIBED      ||
                            iterator->arbit_info.conn_status == SUBSCRIBE_EVENT ||
                            iterator->arbit_info.conn_status == UNSUBSCRIBING) {
                            sampling_interval = common_multiple_cal(sampling_interval,
                            iterator->arbit_info.subscribe_data_param.sampling_interval);
                            sampling_interval %= 101; /* The value is 0~100*/

                            reporting_interval = common_multiple_cal(reporting_interval,
                            iterator->arbit_info.subscribe_data_param.reporting_interval);
                            reporting_interval %= 101; /* The value is 0~100*/
                        }
                        iterator = (client_arbit_info_list_t *)iterator->list.next;
                    }
                    if((sampling_interval == p_list->result.subscribe_data_param.sampling_interval)
                      &&
                      (reporting_interval == p_list->result.subscribe_data_param.reporting_interval)
                      ) {
                        ss_send_rsp_msg_to_client(p_client_handle,
                                                   MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP,
                                                   SS_STATUS_SUCCESS, p_req->sensor);
                        CLIENT_FSM_SWITCH(l->arbit_info.conn_status, SUBSCRIBING, SS_STATUS_SUCCESS);
                    } else {
                        p_req->reporting_interval = reporting_interval;
                        p_req->sampling_interval = sampling_interval;
                        /* Update sensor subscribing parameters */
                        p_list->result.subscribe_data_param.reporting_interval = reporting_interval;
                        p_list->result.subscribe_data_param.sampling_interval = sampling_interval;

                        svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
                        p_platform_handler->sensor_subscribe(p_req, NULL);
                    }

                } break;
            case SS_STATUS_ERROR:
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
                SS_PRINT_ERR("Requesting status err. cur status:%d, req status:%d",
                p_list->result.conn_status, SUBSCRIBING);
#endif
                CLIENT_FSM_SWITCH(l->arbit_info.conn_status, SUBSCRIBING, SS_STATUS_ERROR);
                goto EXIT;
                break;
            default:
                break;
        }
    return;
EXIT:
    SS_PRINT_LOG("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP,
                     SS_STATUS_ERROR, p_req->sensor);

}

static void ss_svc_unsubscribe_handler(ss_sensor_unsubscribe_data_req_t *p_req, void *p_param)
{
    void *p_client_handle;
    ss_sensor_dev_list_t *p_list;

    p_client_handle = GET_CLIENT_HANDLE(p_req);
    p_list = ss_get_sensor_dev_list(p_req->sensor);

    if (p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Client(0x%x) can't request unsubscribing", (uint32_t)p_client_handle);
#endif
        goto EXIT;
    }
    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);
    if (l == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Can't find client connection info in sensor(0x%x) list",
            (uint32_t)p_list->sensor_handle);
#endif
        goto EXIT;
    }
    if (ss_client_con_status_update(l, UNSUBSCRIBING) == SS_STATUS_ERROR) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("l->arbit_info.conn_status : %d", l->arbit_info.conn_status);
        SS_PRINT_ERR("The client can not switch to UNSUBSCRIBING status from %d",
                                                    l->arbit_info.conn_status);
#endif
        goto EXIT;
    }
    uint8_t arbitrating_is_ok = ss_sensor_new_status_arbit(p_list, UNSUBSCRIBING);
    client_arbit_info_list_t *p_client_arbit_list  = (client_arbit_info_list_t *)
            p_list->arbit_info_list_header.head;

    switch(arbitrating_is_ok) {
        case SS_STATUS_SUCCESS:
            {
                while(p_client_arbit_list) {    /* To be sure no clients waiting for data */
                    if (p_client_arbit_list->arbit_info.conn_status == SUBSCRIBED ||
                        p_client_arbit_list->arbit_info.conn_status == SUBSCRIBE_EVENT) {
                        break;
                    }
                    p_client_arbit_list = (client_arbit_info_list_t *)
                                          p_client_arbit_list->list.next;
                }
                if (p_client_arbit_list == NULL) {
                    p_list->result.conn_status = UNSUBSCRIBING;  /* Set sensor status */
                    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
                    p_platform_handler->sensor_unsubscribe(p_req, NULL);
                } else {
                    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP,
                                       SS_STATUS_SUCCESS, p_req->sensor);
                    CLIENT_FSM_SWITCH(l->arbit_info.conn_status,
                                    UNSUBSCRIBING, SS_STATUS_SUCCESS);
                }
            } break;
        case SS_STATUS_CMD_REPEAT:
            ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP,
                                       SS_STATUS_SUCCESS, p_client_handle);
            CLIENT_FSM_SWITCH(p_client_arbit_list->arbit_info.conn_status,
                UNSUBSCRIBING, SS_STATUS_SUCCESS);
            break;
        case SS_STATUS_ERROR:
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_ERR("Requesting sensor status is illegal. \
            sensor current status:%d, requesting status:%d",
            p_list->result.conn_status, UNSUBSCRIBING);
#endif
            CLIENT_FSM_SWITCH(p_client_arbit_list->arbit_info.conn_status,
                UNSUBSCRIBING, SS_STATUS_ERROR);
            goto EXIT;
            break;
        default:
        break;
    }
    return;
EXIT:
    SS_PRINT_LOG("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP,
                       SS_STATUS_ERROR, p_req->sensor);
}

static void ss_svc_get_property_handle(ss_sensor_get_property_req_t *p_req, void *p_param)
{

    void *p_client_handle = GET_CLIENT_HANDLE(p_req);
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint32_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(p_req->sensor);
    if(p_list == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* On-board sensor list may need to be built up here  */
            p_list = ss_sensor_list_add(sensor_type,sensor_id);
            /* On-board sensor default status is PAIRED */
            if(p_list != NULL)
                p_list->result.conn_status = PAIRED;
        }
    }
    if(p_list == NULL) {
        SS_PRINT_ERR("The sensor(0x%x) is not in sensor list", (uint32_t)p_req->sensor);
        goto EXIT;
    }

    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);

    if (l == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* Save client connection info*/
            l = ss_arbit_info_list_add(p_list, p_client_handle);
            if (l == NULL) {
                SS_PRINT_ERR("Saving sensor arbitration failed");
                return;
            }
            /* Keep On-board sensor status being same with wireless sensor */
            l->arbit_info.conn_status = PAIRED;
        }
    }
    if(l == NULL) {
        SS_PRINT_ERR("Client(0x%x) does not support the sensor(0x%x)",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
        goto EXIT;
    }

    if (!IS_CONNECTED_STATUS(l->arbit_info.conn_status)) {
        SS_PRINT_ERR("Client(0x%x) does not connect to sensor(0x%x). sensor status:%d",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor, l->arbit_info.conn_status);
        goto EXIT;
    }
    l->arbit_info.flag |= SENSOR_GET_PROPERTY_FLAG_MASK;

    svc_platform_handler_t * p_platform_handler =(svc_platform_handler_t *)p_param;
    p_platform_handler->sensor_get_property(p_req, NULL);
    return;
EXIT:
    SS_PRINT_ERR("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_GET_PROPERTY_RSP, SS_STATUS_ERROR, p_req->sensor);
}

static void ss_svc_set_property_handle(ss_sensor_set_property_req_t *p_req, void *p_param)
{

    void *p_client_handle = GET_CLIENT_HANDLE(p_req);
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint32_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(p_req->sensor);
    if(p_list == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* On-board sensor list may need to be built up here  */
            p_list = ss_sensor_list_add(sensor_type,sensor_id);
            /* On-board sensor default status is PAIRED */
            if(p_list != NULL)
                p_list->result.conn_status = PAIRED;
        }
    }
    if(p_list == NULL) {
        SS_PRINT_ERR("The sensor(0x%x) is not in sensor list", (uint32_t)p_req->sensor);
        goto EXIT;
    }

    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);

    if (l == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* Save client connection info*/
            l = ss_arbit_info_list_add(p_list, p_client_handle);
            if (l == NULL) {
                SS_PRINT_ERR("Saving sensor arbitration failed");
                return;
            }
            /* Keep On-board sensor status being same with wireless sensor */
            l->arbit_info.conn_status = PAIRED;
        }
    }
    if(l == NULL) {
        SS_PRINT_ERR("Client(0x%x) does not support the sensor(0x%x)",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
        goto EXIT;
    }

    if (!IS_CONNECTED_STATUS(l->arbit_info.conn_status)) {
        SS_PRINT_ERR("Client(0x%x) does not connect to sensor(0x%x). sensor status:%d",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor, l->arbit_info.conn_status);
        goto EXIT;
    }
    l->arbit_info.flag |= SENSOR_SET_PROPERTY_FLAG_MASK;

    svc_platform_handler_t * p_platform_handler =(svc_platform_handler_t *)p_param;
    p_platform_handler->sensor_set_property(p_req, NULL);
    return;
EXIT:
    SS_PRINT_ERR("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_SET_PROPERTY_RSP, SS_STATUS_ERROR, p_req->sensor);
}

static void ss_svc_calibration_handle(ss_sensor_calibration_req_t *p_req, void *p_param)
{

    void *p_client_handle = GET_CLIENT_HANDLE(p_req);
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);
    uint32_t sensor_id = GET_SENSOR_ID(p_req->sensor);

    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(p_req->sensor);
    if(p_list == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* On-board sensor list may need to be built up here  */
            p_list = ss_sensor_list_add(sensor_type,sensor_id);
            /* On-board sensor default status is PAIRED */
            if(p_list != NULL)
                p_list->result.conn_status = PAIRED;
        }
    }
    if(p_list == NULL) {
        SS_PRINT_ERR("The sensor(0x%x) is not in sensor list", (uint32_t)p_req->sensor);
        goto EXIT;
    }

    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);

    if (l == NULL) {
        if ((0x01 << sensor_type) & BOARD_SENSOR_MASK) {
            /* Save client connection info*/
            l = ss_arbit_info_list_add(p_list, p_client_handle);
            if (l == NULL) {
                SS_PRINT_ERR("Saving sensor arbitration failed");
                return;
            }
            /* Keep On-board sensor status being same with wireless sensor */
            l->arbit_info.conn_status = PAIRED;
        }
    }
    if(l == NULL) {
        SS_PRINT_ERR("Client(0x%x) does not support the sensor(0x%x)",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
        goto EXIT;
    }

    if (!IS_CONNECTED_STATUS(l->arbit_info.conn_status)) {
        SS_PRINT_ERR("Client(0x%x) does not connect to sensor(0x%x). sensor status:%d",
                (uint32_t)p_client_handle, (uint32_t)p_req->sensor, l->arbit_info.conn_status);
        goto EXIT;
    }
    l->arbit_info.flag |= SENSOR_CALIBRATION_FLAG_MASK;
    clb_sensor_type_flag++;

    svc_platform_handler_t * p_platform_handler =(svc_platform_handler_t *)p_param;
    p_platform_handler->sensor_calibration(p_req, NULL);
    return;
EXIT:
    SS_PRINT_ERR("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_CALIBRATION_RSP, SS_STATUS_ERROR, p_req->sensor);
}

#ifdef CONFIG_SENSOR_SERVICE_RESERVED_FUNC
static void ss_send_sensor_info_rsp_msg_to_client(ss_sensor_t sensor_handle, sensor_info_type_t type,
        const uint8_t *str, uint8_t status, uint32_t msg_id, void *client_handle);

static void ss_svc_sensor_pair_handler(ss_sensor_pair_req_t *p_req, void *p_param)
{
    void *p_client_handle;
    uint8_t sensor_type;

    p_client_handle = GET_CLIENT_HANDLE(p_req);
    sensor_type = p_req->pair_param.sensor_type;
    if ( !(IS_ANT_SENSOR(sensor_type) || IS_BLE_SENSOR(sensor_type))) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Sensor(type:%d, id:%d)doesn't support the operation",sensor_type);
#endif
        ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_PAIR_RSP,
                SS_STATUS_ERROR, p_client_handle);
        return;
    }
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    p_platform_handler->sensor_pair(p_req, NULL);
}

static void ss_svc_unpair_handler(ss_sensor_unpair_req_t *p_req, void *p_param)
{
    void *p_client_handle;
    ss_sensor_dev_list_t *p_list;
    p_client_handle = GET_CLIENT_HANDLE(p_req);
    uint8_t sensor_type = GET_SENSOR_TYPE(p_req->sensor);

    if ( !(((0x01 << sensor_type) & BLE_TYPE_MASK) ||
            ((0x01 << sensor_type) & ANT_TYPE_MASK))) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("Sensor type:%d does not support unpair operation", sensor_type);
#endif
            goto EXIT;
     }
    p_list = ss_get_sensor_dev_list(p_req->sensor);

    if (p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Can not find the sensor to unpair");
#endif
        goto EXIT;
    }

    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);
    if (l == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("The client may not connect to the sensor,which can not request unpair");
#endif
        goto EXIT;
    }
    if (ss_client_con_status_update(l, UNPAIRING) == SS_STATUS_ERROR) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("l->arbit_info.conn_status : %d", l->arbit_info.conn_status);
        SS_PRINT_ERR("The client can not switch to UNPAIRING status from %d",
                                                    l->arbit_info.conn_status);
#endif
        goto EXIT;
    }
    uint8_t arbitrating_is_ok = ss_sensor_new_status_arbit(p_list, UNPAIRING);

    switch(arbitrating_is_ok) {
        case SS_STATUS_SUCCESS:
            {
                client_arbit_info_list_t *p_client_arbit_list  = (client_arbit_info_list_t *)
                        p_list->arbit_info_list_header.head;

                while(p_client_arbit_list) {
                    if (p_client_arbit_list->arbit_info.conn_status == READY ||
                        p_client_arbit_list->arbit_info.conn_status == UNPAIRING     ||
                        p_client_arbit_list->arbit_info.conn_status == UNPAIRED      ||
                        p_client_arbit_list->arbit_info.conn_status == RELEASING) {
                        p_client_arbit_list = (client_arbit_info_list_t *)
                                            p_client_arbit_list->list.next;
                    } else {
                        break;
                    }
                }
                if (p_client_arbit_list == NULL) {
                    svc_platform_handler_t * p_platform_handler =
                                    (svc_platform_handler_t *)p_param;
                    p_platform_handler->sensor_unpair(p_req, NULL);
                    return;
                }
            } break;
        case SS_STATUS_CMD_REPEAT:
            ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_UNPAIR_RSP,
                                       SS_STATUS_SUCCESS, p_client_handle);
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, UNPAIRING, SS_STATUS_SUCCESS);
            return;
            break;
        case SS_STATUS_ERROR:
            CLIENT_FSM_SWITCH(l->arbit_info.conn_status, UNPAIRING, SS_STATUS_ERROR);
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_ERR("Sensor(0x%x) status arbitrate unsuccessfully", (uint32_t)p_list->sensor_handle);
#endif
            goto EXIT;
            break;
        default:
            break;
    }
EXIT:
    SS_PRINT_LOG("Wrong executing");
    ss_send_rsp_msg_to_client(p_client_handle, MSG_ID_SS_SENSOR_UNPAIR_RSP,
            SS_STATUS_ERROR, p_req->sensor);
}

static uint8_t ss_get_sensor_info_handle(ss_sensor_get_info_req_t *p_req,
                                            void (*sensor_info_handler)(ss_sensor_t, void *),
                                            uint8_t info_type)
{
    void *p_client_handle = GET_CLIENT_HANDLE(p_req);
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(p_req->sensor);

    if (IS_ON_BOARD_SENSOR(GET_SENSOR_TYPE(p_req->sensor))) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("On board sensors do not support sensor vendor info");
#endif
        goto EXIT;
    }

    if(p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Client(0x%x) does not support the sensor(0x%x)",
            (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
#endif
        goto EXIT;
    }
    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, p_client_handle);

    if (l == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Can't find client(0x%x) connection info to sensor(0x%x)",
            (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
#endif
        goto EXIT;
    }
    if (!IS_CONNECTED_STATUS(l->arbit_info.conn_status)) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Client(0x%x) does not connect to sensor(0x%x)",
            (uint32_t)p_client_handle, (uint32_t)p_req->sensor);
#endif
        goto EXIT;
    }
    l->arbit_info.flag |= (1 << info_type);
    sensor_info_handler(p_req->sensor, NULL);
    return SS_STATUS_SUCCESS;
EXIT:
    SS_PRINT_LOG("Wrong executing");
    return SS_STATUS_ERROR;
}

static void ss_get_sensor_device_id_handle(ss_sensor_get_device_id_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_device_id, SENSOR_DEVICE_ID)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_DEVICE_ID, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_DEVICE_ID_RSP, client_handle);
    }
}

static void ss_get_sensor_product_id_handle(ss_sensor_get_product_id_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler =
                                    (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_product_id, SENSOR_PRODUCT_ID)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_PRODUCT_ID, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_PRODUCT_ID_RSP, client_handle);
    }
}

static void ss_get_sensor_manufacture_id_handle(ss_sensor_get_manufacturer_id_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_manufacturer_id, SENSOR_MANUFACTURER_ID)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_MANUFACTURER_ID, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_RSP, client_handle);
    }
}

static void ss_get_sensor_serial_number_handle(ss_sensor_get_serial_number_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_serial_number, SENSOR_SERIAL_NUMBER)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_SERIAL_NUMBER, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_RSP, client_handle);
    }
}

static void ss_get_sensor_sw_version_handle(ss_sensor_get_sw_version_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_sw_version, SENSOR_SW_VERSION)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_SW_VERSION, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_SW_VERSION_RSP, client_handle);
    }
}

static void ss_get_sensor_hw_version_handle(ss_sensor_get_hw_version_req_t *p_req, void *p_param)
{
    svc_platform_handler_t * p_platform_handler = (svc_platform_handler_t *)p_param;
    if (ss_get_sensor_info_handle(p_req, p_platform_handler->sensor_hw_version, SENSOR_HW_VERSION)) {
        void *client_handle = GET_CLIENT_HANDLE(p_req);
        ss_send_sensor_info_rsp_msg_to_client(p_req->sensor, SENSOR_HW_VERSION, NULL, SS_STATUS_ERROR,
                MSG_ID_SS_SENSOR_GET_HW_VERSION_RSP, client_handle);
    }
}

static void ss_send_sensor_info_rsp_msg_to_client(ss_sensor_t sensor_handle, sensor_info_type_t type,
                                        const uint8_t *str, uint8_t status, uint32_t msg_id, void *client_handle)
{
    uint8_t len = 0;
    if (status == SS_STATUS_SUCCESS) {
        if(str != NULL) {
            len = strlen((const char *)str) + 1;
        }
    }
    ss_sensor_device_id_rsp_t *p_msg = (ss_sensor_device_id_rsp_t *)balloc(
                sizeof(ss_sensor_device_id_rsp_t) + len, NULL);

    if(len) {
        data_cpy(p_msg->string, str, len);
    }
    p_msg->handle = sensor_handle;
    p_msg->info_type = type;
    p_msg->status = status;

    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Sensor(0x%x) not in sensor list", (uint32_t)sensor_handle);
#endif
        goto EXIT;
    }
    client_arbit_info_list_t *l = ss_get_client_con_info(p_list, client_handle);
    if (l == NULL) {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_ERR("Client(0x%x) is not in sensor list", (uint32_t)sensor_handle);
#endif
        goto EXIT;
    }
    l->arbit_info.flag &= ~(1 << type);
EXIT:
    SS_PRINT_LOG("Wrong executing");
    send_evt_msg_to_client((struct cfw_message *)p_msg, client_handle, msg_id);

}

static void ss_send_sensor_info_rsp_msg_to_clients(ss_sensor_t sensor_handle, sensor_info_type_t type,
                                                    uint8_t *str, uint8_t status, uint32_t msg_id)
{
    uint8_t len = 0;
    if (status == SS_STATUS_SUCCESS) {
        if(str != NULL) {
            len = strlen((const char *)str);
        }
    }
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(sensor_handle);
    if (p_list == NULL) {
        SS_PRINT_ERR("Excuting failed");
        return;
    }
    client_arbit_info_list_t *l =
        (client_arbit_info_list_t *)p_list->arbit_info_list_header.head;
    svc_foreach_list(l) {
        if (l->arbit_info.flag & (0x01 << type)) {
            l->arbit_info.flag &= ~(0x01 << type);
            ss_sensor_info_msg_t *p_msg = (ss_sensor_info_msg_t *)balloc(
                sizeof(ss_sensor_info_msg_t) + len + 1, NULL);
            if(len) {
                data_cpy(p_msg->string, str, len);
            }
            p_msg->string[len] = '\0'; /* Fill the end of string */
            p_msg->string_len = len;
            p_msg->handle = sensor_handle;
            p_msg->info_type = type;
            p_msg->status = status;
            send_evt_msg_to_client((struct cfw_message *)p_msg, l->p_handle, msg_id);
        }
    }
}

void ss_send_device_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_DEVICE_ID, str, status,
                                           MSG_ID_SS_SENSOR_GET_DEVICE_ID_RSP);
}
void ss_send_product_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_PRODUCT_ID, str, status,
                                           MSG_ID_SS_SENSOR_GET_PRODUCT_ID_RSP);
}
void ss_send_manufacturer_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_MANUFACTURER_ID, str, status,
                                           MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_RSP);
}
void ss_send_serial_number_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_SERIAL_NUMBER, str, status,
                                           MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_RSP);
}

void ss_send_sw_version_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_SW_VERSION, str, status,
                                           MSG_ID_SS_SENSOR_GET_SW_VERSION_RSP);
}

void ss_send_hw_version_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status)
{
    ss_send_sensor_info_rsp_msg_to_clients(sensor_handle, SENSOR_HW_VERSION, str, status,
                                           MSG_ID_SS_SENSOR_GET_HW_VERSION_RSP);
}
#endif/* CONFIG_SENSOR_SERVICE_RESERVED_FUNC*/

static void ss_svc_msg_handler(struct cfw_message *p_msg, void *p_param)
{
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
    SS_PRINT_LOG("Excueting msg[%d] handle", CFW_MESSAGE_ID(p_msg));
#endif
    switch (CFW_MESSAGE_ID(p_msg)) {
    case MSG_ID_SS_START_SENSOR_SCANNING_REQ:
        ss_svc_start_scanning_req_handler((ss_start_sensor_scanning_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_STOP_SENSOR_SCANNING_REQ:
        ss_svc_stop_scanning_req_handler((ss_stop_sensor_scanning_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_REQ:
        ss_svc_subscribe_handler((ss_sensor_subscribe_data_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_REQ:
        ss_svc_unsubscribe_handler((ss_sensor_unsubscribe_data_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_CALIBRATION_REQ:
        ss_svc_calibration_handle((ss_sensor_calibration_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_SET_PROPERTY_REQ:
        ss_svc_set_property_handle((ss_sensor_set_property_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_PROPERTY_REQ:
        ss_svc_get_property_handle((ss_sensor_get_property_req_t *)p_msg, p_param);
        break;
#if defined(BLE_SERVICE) && (BLE_SERVICE == 1)
    case MSG_ID_SS_BLE_RSP_MSG:
        ss_ble_resp_msg_handler((ble_status_msg_t *)p_msg);
        break;
    case MSG_ID_SS_BLE_EVT_MSG:
        ss_ble_evt_msg_handler((ble_evt_msg_t *)p_msg);
        break;
#endif
#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
   case MSG_ID_SS_SC_RSP_MSG:
        ss_sc_resp_msg_handler((sc_rsp_t *)p_msg);
        break;
    case MSG_ID_SS_SC_EVT_MSG:
        ss_sc_resp_msg_handler((sc_rsp_t *)p_msg);
        break;
#endif
#ifdef CONFIG_SENSOR_SERVICE_RESERVED_FUNC
    case MSG_ID_SS_SENSOR_PAIR_REQ:
        ss_svc_sensor_pair_handler((ss_sensor_pair_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_UNPAIR_REQ:
        ss_svc_unpair_handler((ss_sensor_unpair_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_DEVICE_ID_REQ:
        ss_get_sensor_device_id_handle((ss_sensor_get_device_id_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_PRODUCT_ID_REQ:
        ss_get_sensor_product_id_handle((ss_sensor_get_product_id_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_REQ:
        ss_get_sensor_manufacture_id_handle((ss_sensor_get_manufacturer_id_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_REQ:
        ss_get_sensor_serial_number_handle((ss_sensor_get_serial_number_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_SW_VERSION_REQ:
        ss_get_sensor_sw_version_handle((ss_sensor_get_sw_version_req_t *)p_msg, p_param);
        break;
    case MSG_ID_SS_SENSOR_GET_HW_VERSION_REQ:
        ss_get_sensor_hw_version_handle((ss_sensor_get_hw_version_req_t *)p_msg, p_param);
        break;
#endif
    default:
        SS_PRINT_ERR("Illegal");
        break;
    }
    cfw_msg_free(p_msg);
}

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
void register_svc_callback(void * param)
{
    /* Initialise the service part */
    SS_PRINT_LOG("QRK: register_service");
    cfw_register_service(param, get_svc(), ss_svc_msg_handler, get_svc_handler());
    ss_svc_port_id = get_svc()->port_id;
    SS_PRINT_LOG("svc_port_id=%d", ss_svc_port_id);
}
#endif

void ss_service_init(void *p_queue)
{
    svc_platform_handler_t *p_handler = get_svc_handler();

    if(!p_handler) {
        SS_PRINT_ERR("Illegal platform handler");
        return;
    }
    service_t *p_service = get_svc();

    if(p_service == NULL) {
        SS_PRINT_ERR("ERROR! Illegal platform service");
        return;
    }
#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == QRK_SENSOR_SERVICE)
    if(!sensor_svc_open_sensor_core_svc(p_queue, register_svc_callback, p_queue)){
        SS_PRINT_ERR("ARC Score_svc init open failed");
        return;
    }
#endif

#if defined(SENSOR_SERVICE) && (SENSOR_SERVICE == ARC_SENSOR_CORE_SERVICE)
    SS_PRINT_LOG("ARC: register_service");
    /* Initialise the service part */
    cfw_register_service(p_queue, p_service, ss_svc_msg_handler, p_handler);
    ss_svc_port_id = p_service->port_id;
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
    SS_PRINT_LOG("svc_port_id=%d", ss_svc_port_id);
#endif
#endif
    sensor_svc_clb_init(p_queue);
}
