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

#ifndef __SENSOR_SVC_LIST_H__
#define __SENSOR_SVC_LIST_H__

#define SCAN_RSP_FLAG       (0x1 << 0)

typedef enum {
    /*
      * Below statuses only are used for client
      */
    UNUSED,
    SCAN_REQ,
    SCANNING,
    SCANNED,
    SCAN_STOP_REQ,
    SCAN_STOPPED,
    /*
         * Below statuses are shared betweent client and sensor
      */
    READY,
    PAIRING,
    PAIRED,
    SUBSCRIBING,
    SUBSCRIBED,
    SUBSCRIBE_EVENT,
    UNSUBSCRIBING,
    UNSUBSCRIBED,
    SENSOR_STOPPING,
    SENSOR_STOPPED,
    UNPAIRING,
    UNPAIRED,
    UNLINKED,
    RELEASING
} svc_status_t;

typedef struct {
    list_t list;
    void *p_client_handle;
    uint32_t unconn_status1;
    uint32_t unconn_status2;
    uint32_t unconn_status3;
    uint8_t rsp_status;
    /*
     * Store the sensor handle which to be paired,
     * the value will be updated after request to pair a new sensor.
     */
    void *p_sensor_handle;
} ss_client_list_t;

typedef struct {
    uint16_t sampling_interval;
    uint16_t reporting_interval;
} subscribe_data_param_t;

typedef struct {
    uint8_t conn_status;
    uint32_t flag;  /* Flag the client if waiting for some describe or alarm info */
    union {
        subscribe_data_param_t subscribe_data_param;
    };
} client_arbit_info_t;

typedef struct {
    list_t list;
    void *p_handle;
    client_arbit_info_t arbit_info;
} client_arbit_info_list_t;

typedef struct
{
    list_t list;
    void *sensor_handle;
    list_head_t arbit_info_list_header;
    client_arbit_info_t result;
} ss_sensor_dev_list_t;

typedef enum {
    SS_LIST_SUCCESS = 0,
    SS_LIST_ERROR
} list_err_status_t;

ss_client_list_t *ss_get_client_list_first_node(void);

/**
* @brief  Get client list by client handle
* @param  p_handle: client identification
* @retval A client list struct pointer if no error, otherwise NULL
*/
ss_client_list_t *ss_get_client_list(void *p_handle);

/**
* @brief  Insert a client handle into client list
* @param  p_handle: Client identification
* @retval SS_LIST_SUCCESS if no error, otherwise SS_LIST_ERROR
*/
ss_client_list_t *ss_client_list_add(void *p_handle);

/**
* @brief  Remove a client from client list
* @param  p_handle: Client identification
* @retval SS_LIST_SUCCESS if no error, otherwise SS_LIST_ERROR
*/
int ss_delete_client_list(void *p_handle);

 /**
  * @brief  Update Client list status
  * @param  p_list: Client list pointer
  *         sensor_type_bit_map: Specify the bitmap of sensor types.
  *         new_status: Sensor service unconnected status
  * @retval SS_LIST_ERROR : update failed
  *         SS_LIST_SUCCESS :update successfull
  */
 int ss_update_client_list_status(ss_client_list_t * p_list,
          uint32_t sensor_type_bit_map, svc_status_t new_status);

/**
 * @brief  Get the status of a given sensor type
 * @param  sensor_type: Specify sensor type.
 *         p_unconn_status:  Return the status of the given sensor type
 * @retval SS_LIST_SUCCESS if no error, otherwise SS_LIST_ERROR
 */
int ss_get_client_list_status(ss_client_list_t * p_list,
        uint8_t sensor_type, uint8_t *p_unconn_status);
/**
 * @brief  Get the status of  given sensor types
 * @param  sensor_type_bit_map: Specify the bitmap of sensor types.
 *         unconn_status:  Return the status of the given sensor types
 * @retval SS_LIST_SUCCESS if no error, otherwise SS_LIST_ERROR
 */
int ss_get_client_list_status_ext(ss_client_list_t * p_list,
              uint32_t sensor_type_bit_map, uint8_t unconn_status[32]);

/**
 * @brief  Get client list head
 * @param  None
 * @retval Client list header
 */
list_head_t get_client_list_head(void);

/**
 * @brief  Get sensor list head.
 * @param  None
 * @retval Client list header
 */
list_head_t get_sensor_list_head(void);

/**
 * @brief  Get sensor list by sensor identify(type +id)
 * @param  p_handle: Sensor identification
 * @retval A sensor list struct pointer if no error, otherwise NULL
 */
ss_sensor_dev_list_t * ss_get_sensor_dev_list(void *p_handle);

/**
 * @brief  Insert a sensor handle into sensor list
 * @param  type: Sensor type
 *         id: Sensor id
 * @retval Sensor device list pointer if no error, otherwise NULL
 */
ss_sensor_dev_list_t *ss_sensor_list_add(uint8_t type, uint32_t id);

/**
 * @brief  Delete a sensor handle from sensor list
 * @param  p_handle: Sensor identification
 * @retval 0 if no error, otherwise error code
 */
int ss_sensor_list_delete(void *p_handle);

/**
 * @brief  Get the arbitration infomation for a given client handle
 * @param  p_list: Sensor device list pointer
 *         p_client_handle: Client handle pointer
 * @retval Arbitration list pointer if no error, otherwise NULL
 */
client_arbit_info_list_t *ss_get_client_con_info(
          ss_sensor_dev_list_t *p_list, void *p_client_handle);

/**
 * @brief  Insert an elment into arbit info list
 * @param  p_list: Sensor device list pointer
 *         p_client_handle: Client hander pointer
 * @retval Client arbitration infomation list point if no error, otherwise NULL
 */
client_arbit_info_list_t *ss_arbit_info_list_add(
        ss_sensor_dev_list_t *p_list, void *p_client_handle);

/**
 * @brief  Delete and free memory arbit info list
 * @param  p_arbit_info_list: arbit_info_list_header pointer
 *         p_element: client_arbit_info_list_t hander pointer
 * @retval 0 if no error, otherwise error code
 */
int  ss_arbit_info_list_delete(list_head_t *p_arbit_info_list, list_t *p_element);

/**
 * @brief  Delete and free memory sensor client list
 * @param  p_element: ss_sensor_dev_list_t pointer
 * @retval 0 if no error, otherwise error code
 */
int ss_sensor_node_delete(list_t *p_element);

/**
 * @brief  Get arbit_info_list header length
 * @param  list_head_t: arbit_info_list_header pointer
 * @retval arbit_info_list_header length
 */
int ss_arbit_info_list_length(list_head_t* list);

#endif
