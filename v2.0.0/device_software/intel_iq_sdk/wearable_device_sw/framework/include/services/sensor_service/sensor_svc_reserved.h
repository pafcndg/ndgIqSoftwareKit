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
#ifndef __SENSOR_SVC_RESERVED_H
#define __SENSOR_SVC_RESERVED_H

/**
 * Sensor Service Message definitions
 */
#define MSG_ID_SS_BASE                              (MSG_ID_SS_SERVICE_BASE + 0x00)
#define MSG_ID_SS_RSP                               (MSG_ID_SS_SERVICE_BASE + 0x40)
#define MSG_ID_SS_EVT                               (MSG_ID_SS_SERVICE_BASE + 0x80)

/**
 * Request Message Ids
 */
#define MSG_ID_SS_GET_SENSOR_LIST_REQ               (MSG_ID_SS_BASE | 0x10)
#define MSG_ID_SS_RELEASE_SENSOR_REQ                (MSG_ID_SS_BASE | 0x11)
#define MSG_ID_SS_SENSOR_GET_TYPE_REQ               (MSG_ID_SS_BASE | 0x12)
#define MSG_ID_SS_SENSOR_GET_DEVICE_ID_REQ          (MSG_ID_SS_BASE | 0x13)
#define MSG_ID_SS_SENSOR_GET_PRODUCT_ID_REQ         (MSG_ID_SS_BASE | 0x14)
#define MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_REQ    (MSG_ID_SS_BASE | 0x15)
#define MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_REQ      (MSG_ID_SS_BASE | 0x16)
#define MSG_ID_SS_SENSOR_GET_SW_VERSION_REQ         (MSG_ID_SS_BASE | 0x17)
#define MSG_ID_SS_SENSOR_GET_HW_VERSION_REQ         (MSG_ID_SS_BASE | 0x18)

#define MSG_ID_SS_SENSOR_PAIR_REQ                   (MSG_ID_SS_BASE | 0x19)
#define MSG_ID_SS_SENSOR_UNPAIR_REQ                 (MSG_ID_SS_BASE | 0x1A)


#define MSG_ID_SS_SET_MODE_REQ                      (MSG_ID_SS_BASE | 0x1B)
#define MSG_ID_SS_SEND_SENSOR_CMD_REQ               (MSG_ID_SS_BASE | 0x1C)
#define MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_REQ       (MSG_ID_SS_BASE | 0x1D)
#define MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_REQ     (MSG_ID_SS_BASE | 0x1E)

/**
 * Response Message Ids
 */
#define MSG_ID_SS_RELEASE_SENSOR_RSP                (MSG_ID_SS_RSP | 0x10)
#define MSG_ID_SS_SENSOR_GET_TYPE_RSP               (MSG_ID_SS_RSP | 0x11)
#define MSG_ID_SS_SENSOR_GET_DEVICE_ID_RSP          (MSG_ID_SS_RSP | 0x12)
#define MSG_ID_SS_SENSOR_GET_PRODUCT_ID_RSP         (MSG_ID_SS_RSP | 0x13)
#define MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_RSP    (MSG_ID_SS_RSP | 0x14)
#define MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_RSP      (MSG_ID_SS_RSP | 0x15)
#define MSG_ID_SS_SENSOR_GET_SW_VERSION_RSP         (MSG_ID_SS_RSP | 0x16)
#define MSG_ID_SS_SENSOR_GET_HW_VERSION_RSP         (MSG_ID_SS_RSP | 0x17)

#define MSG_ID_SS_SENSOR_START_RSP                  (MSG_ID_SS_RSP | 0x18)
#define MSG_ID_SS_SENSOR_STOP_RSP                   (MSG_ID_SS_RSP | 0x19)

#define MSG_ID_SS_SET_MODE_RSP                      (MSG_ID_SS_RSP | 0x1A)
#define MSG_ID_SS_SEND_SENSOR_CMD_RSP               (MSG_ID_SS_RSP | 0x1B)
#define MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_RSP       (MSG_ID_SS_RSP | 0x1C)
#define MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_RSP     (MSG_ID_SS_RSP | 0x1D)

/**
 * Asynchronous Message Ids
 */
#define MSG_ID_SS_RELEASE_SENSOR_EVT                (MSG_ID_SS_EVT | 0x10)
#define MSG_ID_SS_SENSOR_GET_TYPE_EVT               (MSG_ID_SS_EVT | 0x11)
#define MSG_ID_SS_SENSOR_GET_DEVICE_ID_EVT          (MSG_ID_SS_EVT | 0x12)
#define MSG_ID_SS_SENSOR_GET_PRODUCT_ID_EVT         (MSG_ID_SS_EVT | 0x13)
#define MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_EVT    (MSG_ID_SS_EVT | 0x14)
#define MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_EVT      (MSG_ID_SS_EVT | 0x15)
#define MSG_ID_SS_SENSOR_GET_SW_VERSION_EVT         (MSG_ID_SS_EVT | 0x16)
#define MSG_ID_SS_SENSOR_GET_HW_VERSION_EVT         (MSG_ID_SS_EVT | 0x17)

#define MSG_ID_SS_SENSOR_PAIR_EVT                   (MSG_ID_SS_EVT | 0x18)
#define MSG_ID_SS_SENSOR_UNPAIR_EVT                 (MSG_ID_SS_EVT | 0x19)
#define MSG_ID_SS_SENSOR_START_EVT                  (MSG_ID_SS_EVT | 0x1A)
#define MSG_ID_SS_SENSOR_STOP_EVT                   (MSG_ID_SS_EVT | 0x1B)

#define MSG_ID_SS_SET_MODE_EVT                      (MSG_ID_SS_EVT | 0x1C)
#define MSG_ID_SS_SEND_SENSOR_CMD_EVT               (MSG_ID_SS_EVT | 0x1D)
#define MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_EVT       (MSG_ID_SS_EVT | 0x1E)
#define MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_EVT     (MSG_ID_SS_EVT | 0x1F)
#define MSG_ID_SS_SENSOR_UNLINK_EVT                 (MSG_ID_SS_EVT | 0x20)

#define MSG_ID_SS_BLE_RSP_MSG                       (MSG_ID_SS_EVT | 0x80)
#define MSG_ID_SS_BLE_EVT_MSG                       (MSG_ID_SS_EVT | 0x81)
#define MSG_ID_SS_SC_RSP_MSG                        (MSG_ID_SS_EVT | 0x82)
#define MSG_ID_SS_SC_EVT_MSG                        (MSG_ID_SS_EVT | 0x83)

/*Reserved sturcts and funcs*/
#ifdef CONFIG_SENSOR_SERVICE_RESERVED_FUNC
/**
 * Parameters of MSG_ID_SS_GET_SENSOR_LIST_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_list_t **sensor_list;
} ss_get_sensor_list_req_t;

/**
 * Parameters of MSG_ID_SS_RELEASE_SENSOR_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
} ss_release_sensor_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_TYPE_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
} ss_sensor_get_info_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_DEVICE_ID_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_device_id_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_PRODUCT_ID_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_product_id_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_manufacturer_id_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_serial_number_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_SW_VERSION_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_sw_version_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_HW_VERSION_REQ
 */
typedef ss_sensor_get_info_req_t ss_sensor_get_hw_version_req_t;

typedef ss_svc_message_general_rsp_t ss_sensor_start_rsp_t;
typedef ss_svc_message_general_rsp_t ss_sensor_stop_rsp_t;
typedef ss_svc_message_general_rsp_t ss_sensor_pair_rsp_t;
typedef ss_svc_message_general_rsp_t ss_sensor_unpair_rsp_t;

/**
 * Just used for compatible with Coach app,
 * It will be removed after it is not used in app any longer.
 */
typedef ss_sensor_pair_rsp_t         ss_sensor_pair_event_t;


/**
 * Parameters of MSG_ID_SS_SENSOR_START
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
} ss_sensor_start_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_STOP
 */
typedef ss_sensor_start_req_t ss_sensor_stop_req_t;

/**
 * Sensor pair param
 * This structure will be expanded when it support ble/ant
 */
typedef struct {
    uint8_t sensor_type;
} ss_pair_param_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_PAIR_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_pair_param_t pair_param;
} ss_sensor_pair_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_UNPAIR_REQ
 */
typedef  struct {
    struct cfw_message header;
    ss_sensor_t sensor;
}ss_sensor_unpair_req_t;

/**
 * Parameters of MSG_ID_SS_SET_MODE_REQ
 */
typedef struct
{
    struct cfw_message header;
    uint8_t tether_mode;
} ss_set_mode_req_t;

typedef struct {
  uint8_t length;
  uint8_t parameter[0];
} cmd_parameter_t;

/**
 * Parameters of MSG_ID_SS_SEND_SENSOR_CMD_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
    uint32_t cmd_id;
    cmd_parameter_t parameter;
} ss_send_sensor_cmd_req_t;

typedef struct {
    struct cfw_message head;
    ss_sensor_t handle;
    sensor_info_type_t info_type;
    uint16_t string_len;
    uint8_t status;
    /*
     * It stores sensor info while status = 0.
     * In the other case, It is a null string.
     */
    uint8_t string[0];
} ss_sensor_info_msg_t;

typedef ss_sensor_info_msg_t ss_sensor_device_id_rsp_t;
typedef ss_sensor_info_msg_t ss_sensor_product_id_rsp_t;
typedef ss_sensor_info_msg_t ss_sensor_manufacturer_id_rsp_t;
typedef ss_sensor_info_msg_t ss_sensor_serial_number_rsp_t;
typedef ss_sensor_info_msg_t ss_sensor_sw_version_rsp_t;
typedef ss_sensor_info_msg_t ss_sensor_hw_version_rsp_t;

/**
 * Set mode for sensor service (tethered or untethered).
 *
 * In TETHERED_MODE, sensor service would send data to host application through BT;
 * In UNTETHERED_MODE, sensor service would send data to coaching application running
 * locally on Intel&reg; Curie&trade; device.
 *
 * @param p_service_conn service connection
 * @param p_priv       pointer to private data
 * @param tether_mode  1 tether mode; 0 untether mode
 *
 * @return cfw_send_message() status
 */
int ss_set_mode(cfw_service_conn_t *p_service_conn, void *p_priv, uint8_t tether_mode);

/**
 * Send sensor command.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @param  cmd_id       Specific to sensor type.
 * @param  parameter    Specific to cmd_id.
 * @return cfw_send_message() status
 */
int ss_send_sensor_cmd(cfw_service_conn_t *p_service_conn, void *p_priv,
                     ss_sensor_t sensor, uint32_t cmd_id, cmd_parameter_t parameter);


/**
 * Used by application to release the sensor when it finishes using the sensor.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_RELEASE_SENSOR_RSP
 */
int ss_release_sensor(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor);

/**
 * Get sensor type.
 *
 * @param  p_service_conn    service connection
 * @param  p_priv          pointer to private data
 * @param[in]  sensor      which is retrieved by ss_get_sensor_list()
 * @param[out] sensor_type It is allocated by application and returns the sensor type
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_GET_TYPE_RSP
 */
int ss_sensor_get_type(cfw_service_conn_t *p_service_conn, void *p_priv,
                            ss_sensor_t sensor, ss_sensor_type_t *sensor_type);

/**
 * Get sensor device ID.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return It is allocated by application and returns the sensor device id.
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_GET_DEVICE_ID_RSP
 */
int ss_sensor_get_device_id(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor);

/**
 * Get sensor product ID.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return It is allocated by application and returns the sensor product id.
 * @return cfw_send_message() status,  MSG: MSG_ID_SS_SENSOR_GET_PRODUCT_ID_RSP
 */
int ss_sensor_get_product_id(cfw_service_conn_t *p_service_conn, void *p_priv,
                                    ss_sensor_t sensor);

/**
 * Get sensor manufacturer ID.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return It is allocated by application and returns the sensor manufacturer id.
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_RSP
 */
int ss_sensor_get_manufacturer_id(cfw_service_conn_t *p_service_conn, void *p_priv,
                                                    ss_sensor_t sensor);

/**
 * Get sensor serial number.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_RSP
 */
int ss_sensor_get_serial_number(cfw_service_conn_t *p_service_conn, void *p_priv,
                                            ss_sensor_t sensor);

/**
 * Get sensor software version.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return cfw_send_message() status,  MSG: MSG_ID_SS_SENSOR_GET_SW_VERSION_RSP
 */
int ss_sensor_get_sw_version(cfw_service_conn_t *p_service_conn, void *p_priv,
                                    ss_sensor_t sensor);

/**
 * Get sensor hardware version.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_GET_HW_VERSION_RSP
 */
int ss_sensor_get_hw_version(cfw_service_conn_t *p_service_conn, void *p_priv,
                                        ss_sensor_t sensor);

/**
 * Pair a sensor.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  p_pair_param Pairing information.
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_PAIR_RSP
 */
int ss_sensor_pair(cfw_service_conn_t *p_service_conn, void *p_priv,
                    ss_pair_param_t *p_pair_param);

/**
 * Unpair a sensor.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return cfw_send_message() status,  MSG: MSG_ID_SS_SENSOR_UNPAIR_RSP
 */
int ss_sensor_unpair(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor);

/**
 * Start a sensor.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 *
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_START_RSP
 */
int ss_sensor_start(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor);

/**
 * Stop a sensor.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_STOP_RSP
 */
int ss_sensor_stop(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor);
/**
 * Subscribe to status.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @param  status_type  Shared by all sensor types.
 * @return cfw_send_message() status  MSG:MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_RSP
 */
int ss_sensor_subscribe_status(cfw_service_conn_t *p_service_conn, void *p_priv,
                                     ss_sensor_t sensor, uint8_t status_type);

/**
 * Unsubscribe from status.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @param  status_type  Shared by all sensor types.
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_RSP
 */
int ss_sensor_unsubscribe_status(cfw_service_conn_t *p_service_conn, void *p_priv,
                                 ss_sensor_t sensor, uint8_t status_type);
#endif   /*CONFIG_SENSOR_SERVICE_RESERVED_FUNC*/

#endif /*__SENSOR_SVC_RESERVED_H*/
