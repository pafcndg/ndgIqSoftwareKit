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

#include "cfw/cfw.h"
#include "os/os.h"

#include "services/sensor_service/sensor_svc_list.h"
#include "services/sensor_service/sensor_svc_utils.h"

static list_head_t ss_client_list_head;
static list_head_t ss_sensor_list_head;

/***************************************************************************\
 * client list APIs
\***************************************************************************/
ss_client_list_t *ss_get_client_list_first_node()
{
    return (ss_client_list_t *)ss_client_list_head.head;
}

ss_client_list_t *ss_get_client_list(void *p_handle)
{
    ss_client_list_t *l = (ss_client_list_t *)ss_client_list_head.head;

    while (l) {
        if (l->p_client_handle == p_handle) {
            return l;
        }
        l = (ss_client_list_t *) l->list.next;
    }
    return NULL;
}

ss_client_list_t * ss_client_list_add(void *p_handle)
{
    ss_client_list_t *p_list = (ss_client_list_t *)
                        balloc(sizeof(ss_client_list_t), NULL);
    if (p_list == NULL) {
        return NULL;
    }

    p_list->p_client_handle = p_handle;
    p_list->list.next = NULL;
    p_list->unconn_status1 = UNUSED;
    p_list->unconn_status2 = UNUSED;
    p_list->unconn_status3 = UNUSED;
    list_add(&ss_client_list_head,(list_t *) p_list);
    return p_list;
}

/**
 * @brief  Set client list status
 * @param  p_list: Client list pointer
 *         sensor_type_bit_map: Specify the bitmap of sensor types.
 *         When updating two or more sensor type status, use bit OR operation,
 *         such as : 1 << SENSOR_ACCELEROMETER | 1 << SENSOR_BAROMETER
 *         status: Service unconnected status
 * @retval SS_LIST_SUCCESS if no error, otherwise SS_LIST_ERROR
 */
static int ss_set_client_list_status(ss_client_list_t * p_list,
                                     uint32_t sensor_type_bit_map,
                                     svc_status_t status)
{
    if (p_list == NULL) {
        return SS_LIST_ERROR;
    }

    uint8_t bit_offset = 0;

    while(sensor_type_bit_map) {
        if (sensor_type_bit_map & 0x01) {
            p_list->unconn_status1 &= ~(0x01 << bit_offset);
            p_list->unconn_status1 |= (status & 0x01) << bit_offset;
            p_list->unconn_status2 &= ~(0x01 << bit_offset);
            p_list->unconn_status2 |= ((status >> 1) & 0x01) << bit_offset;
            p_list->unconn_status3 &= ~(0x01 << bit_offset);
            p_list->unconn_status3 |= ((status >> 2) & 0x01) << bit_offset;
        }
        bit_offset++;
        sensor_type_bit_map >>= 1;
    }
    return SS_LIST_SUCCESS;
}

int ss_update_client_list_status(ss_client_list_t * p_list,
                                 uint32_t sensor_type_bit_map,
                                 svc_status_t new_status)
{
    uint8_t unconn_status[32];
    uint8_t bit_offset = 0;

    if (p_list == NULL) {
        return SS_LIST_ERROR;
    }
    uint8_t update_success = 0;
    uint8_t need_to_update;

    ss_get_client_list_status_ext(p_list,
            sensor_type_bit_map, unconn_status);
    while(sensor_type_bit_map) {
        if (sensor_type_bit_map & 0x01) {
            need_to_update = 0;
            switch (new_status) {
            /* The commands must be accepted in any situation */
            case SCAN_REQ:
            case SCAN_STOP_REQ:
                need_to_update = 1;
                break;
            case SCANNING:
            case SCAN_STOPPED:
                if (unconn_status[bit_offset] == new_status -1)
                   need_to_update = 1;
                break;
            case SCANNED:
                if (unconn_status[bit_offset] == SCANNING ||
                    unconn_status[bit_offset] == SCANNED)
                    need_to_update = 1;
                break;
            default:
                break;
            }
            if (need_to_update == 1) {
                ss_set_client_list_status(p_list, 1 << bit_offset, new_status);
                update_success = 1;
            }
        }
        bit_offset++;
        sensor_type_bit_map >>= 1;
    }
    if (update_success == 1) {
        return SS_LIST_SUCCESS;
    } else {
        return SS_LIST_ERROR;
    }
}

int ss_get_client_list_status(ss_client_list_t * p_list,
                   uint8_t sensor_type, uint8_t *p_unconn_status)
{
    if (p_list == NULL) {
        return SS_LIST_ERROR;
    }

    *p_unconn_status = ((p_list->unconn_status1 >> sensor_type) & 0x01)|
                       ((p_list->unconn_status2 >> sensor_type) & 0x01) << 1 |
                       ((p_list->unconn_status3 >> sensor_type) & 0x01) << 2;
    return SS_LIST_SUCCESS;
}

int ss_get_client_list_status_ext(ss_client_list_t * p_list,
                                  uint32_t sensor_type_bit_map,
                                  uint8_t unconn_status[32])
{
    if (p_list == NULL) {
        return SS_LIST_ERROR;
    }
    uint8_t bit_offset = 0;

    while(sensor_type_bit_map) {
        if (sensor_type_bit_map & 0x01) {
            unconn_status[bit_offset] =
                ((p_list->unconn_status1 >> bit_offset) & 0x01)|
                ((p_list->unconn_status2 >> bit_offset) & 0x01) << 1 |
                ((p_list->unconn_status3 >> bit_offset) & 0x01) << 2;
        }
        sensor_type_bit_map >>= 1;
        bit_offset++;
    }
    return SS_LIST_SUCCESS;
}

static uint8_t _delete_list(list_head_t *p_head, list_t *p_element)
{
    int err = 0;

    /* list_remove function has no lock. */
    list_remove(p_head, p_element);
    err = bfree(p_element);

    if (err != E_OS_OK)
        return SS_LIST_ERROR;
    return SS_LIST_SUCCESS;
}

int ss_list_delete_ext(list_head_t *p_head)
{
    list_t *l = NULL;
    list_t *p_cur;
    int err;

    l = p_head->head;
    while(l) {
        list_remove(p_head, l);
        p_cur = l;
        l = l->next;
        err = bfree(p_cur);
        if (err != E_OS_OK) {
            return SS_LIST_ERROR;
        }
    }
    return SS_LIST_SUCCESS;
}

int ss_delete_client_list(void *p_handle)
{
    ss_client_list_t *p_client_list;

    p_client_list = ss_get_client_list(p_handle);

    if (p_client_list == NULL) {
        #if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
        SS_PRINT_LOG("Client handle is NULL");
        #endif
        return SS_LIST_ERROR;
    }
    return _delete_list(&ss_client_list_head, (list_t *)p_client_list);
}

int  ss_arbit_info_list_delete(list_head_t *p_arbit_info_list, list_t *p_element)
{
    if (p_arbit_info_list == NULL) {
        return SS_LIST_ERROR;
    }
    return _delete_list(p_arbit_info_list, p_element);
}

int ss_sensor_node_delete(list_t *p_element)
{
    return _delete_list(&ss_sensor_list_head, p_element);
}

int ss_arbit_info_list_length(list_head_t* list)
{
    int i = 0;
    list_t * l= list->head;
    while(l) {
        l = l->next;
        i++;
    }
    return i;
}

/***************************************************************************\
 * sensor list APIs
\***************************************************************************/
ss_sensor_dev_list_t *ss_get_sensor_dev_list(void *p_handle)
{
    ss_sensor_dev_list_t *l = (ss_sensor_dev_list_t *) ss_sensor_list_head.head;

    while(l != NULL) {
        if (l->sensor_handle == p_handle) {
            return l;
        }
        l = (ss_sensor_dev_list_t *)l->list.next;
    }
    return NULL;
}

ss_sensor_dev_list_t *ss_sensor_list_add(uint8_t type, uint32_t id)
{
    ss_sensor_dev_list_t *p_list = (ss_sensor_dev_list_t *)
                                balloc(sizeof(ss_sensor_dev_list_t), NULL);
    if (p_list == NULL) {
        return NULL;
    }
    p_list->sensor_handle = GET_SENSOR_HANDLE(type,id);
    p_list->result.conn_status = READY;
    list_init(&p_list->arbit_info_list_header);
    p_list->list.next = NULL;
    list_add(&ss_sensor_list_head,(list_t *)p_list);
    return p_list;
}

int ss_sensor_list_delete(void *p_handle)
{
    ss_sensor_dev_list_t *p_list = ss_get_sensor_dev_list(p_handle);
    int err;

    if (p_list == NULL) {
        //SS_PRINT_ERR("Sensor device handle is NULL");
        return SS_LIST_ERROR;
    }
    if (ss_list_delete_ext(&p_list->arbit_info_list_header) == SS_LIST_ERROR) {
        //SS_PRINT_ERR("Arbitration list deletion failed");
    }
    list_remove(&ss_sensor_list_head, (list_t *)p_list);
    err = bfree(p_list);
    if (err != 0)
        return SS_LIST_ERROR;
    return SS_LIST_SUCCESS;
}

client_arbit_info_list_t *ss_get_client_con_info(
                                            ss_sensor_dev_list_t *p_list,
                                            void *p_client_handle)
{
    client_arbit_info_list_t *l = (client_arbit_info_list_t *)
                        p_list->arbit_info_list_header.head;
    while(l) {
        if (l->p_handle == p_client_handle) {
            return l;
        }
        l = (client_arbit_info_list_t *)l->list.next;
    }

    return NULL;
}

client_arbit_info_list_t *ss_arbit_info_list_add(ss_sensor_dev_list_t *p_list,
                                                    void *p_client_handle)
{
    client_arbit_info_list_t *p_arbit_info_list =
        (client_arbit_info_list_t *) balloc(sizeof(client_arbit_info_list_t),NULL);
    if (p_arbit_info_list == NULL) {
        return NULL;
    }
    p_arbit_info_list->arbit_info.conn_status = READY;
    p_arbit_info_list->list.next = NULL;
    p_arbit_info_list->p_handle = p_client_handle;
    p_arbit_info_list->arbit_info.flag = 0;
    list_add(&p_list->arbit_info_list_header,
            (list_t *)p_arbit_info_list);
    return p_arbit_info_list;
}

list_head_t get_client_list_head(void)
{
    return ss_client_list_head;
}

list_head_t get_sensor_list_head(void)
{
    return ss_sensor_list_head;
}
