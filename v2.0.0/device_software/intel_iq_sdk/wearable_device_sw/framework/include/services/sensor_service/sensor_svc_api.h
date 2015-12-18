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

#ifndef __SENSOR_SVC_API_H__
#define __SENSOR_SVC_API_H__

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "services/services_ids.h"
#include "services/sensor_service/sensor_svc_request.h"
#include "services/sensor_service/sensor_data_format.h"

/**
 * @defgroup SS_SERVICE Sensor Service
 *
 * Handles sensors data and make them available.
 *
 * @ingroup services
 */

/**
 * @defgroup SS_SERVICE_API Sensor Service API
 * Sensor Service API.
 * @ingroup SS_SERVICE
 * @{
 */

/*
 * 1:Open the tcmd client
 * 0:Close the tcmd client
 * */
#define TEST_SS_SVC         0

#if defined(TEST_SS_SVC) && (TEST_SS_SVC !=0)
int sensor_tcmd_app_init(void *p_queue);
#endif

/**
 * Response Message Ids
 */
#define MSG_ID_SS_START_SENSOR_SCANNING_RSP         (MSG_ID_SS_RSP | 0x01)
#define MSG_ID_SS_GET_SENSOR_LIST_RSP               (MSG_ID_SS_RSP | 0x02)
#define MSG_ID_SS_STOP_SENSOR_SCANNING_RSP          (MSG_ID_SS_RSP | 0x03)
#define MSG_ID_SS_SENSOR_PAIR_RSP                   (MSG_ID_SS_RSP | 0x05)
#define MSG_ID_SS_SENSOR_UNPAIR_RSP                 (MSG_ID_SS_RSP | 0x06)
#define MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP         (MSG_ID_SS_RSP | 0x07)
#define MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP       (MSG_ID_SS_RSP | 0x08)
#define MSG_ID_SS_SENSOR_CALIBRATION_RSP            (MSG_ID_SS_RSP | 0x09)

#define MSG_ID_SS_SENSOR_SET_PROPERTY_RSP           (MSG_ID_SS_RSP | 0x0a)
#define MSG_ID_SS_SENSOR_GET_PROPERTY_RSP           (MSG_ID_SS_RSP | 0x0b)

/**
 * Asynchronous Message Ids
 */
#define MSG_ID_SS_START_SENSOR_SCANNING_EVT         (MSG_ID_SS_EVT | 0x01)
#define MSG_ID_SS_GET_SENSOR_LIST_EVT               (MSG_ID_SS_EVT | 0x02)
#define MSG_ID_SS_STOP_SENSOR_SCANNING_EVT          (MSG_ID_SS_EVT | 0x03)
#define MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_EVT         (MSG_ID_SS_EVT | 0x04)
#define MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_EVT       (MSG_ID_SS_EVT | 0x05)
#define MSG_ID_SS_SENSOR_CALIBRATION_EVT            (MSG_ID_SS_EVT | 0x06)
#define MSG_ID_SS_SENSOR_GET_PROPERTY_EVT           (MSG_ID_SS_EVT | 0x07)

typedef enum {
    SENSOR_DEVICE_ID,
    SENSOR_PRODUCT_ID,
    SENSOR_MANUFACTURER_ID,
    SENSOR_SERIAL_NUMBER,
    SENSOR_SW_VERSION,
    SENSOR_HW_VERSION,
    SENSOR_CALIBRATION_FLAG,
    SENSOR_SET_PROPERTY_FLAG,
    SENSOR_GET_PROPERTY_FLAG
} sensor_info_type_t;

#define SENSOR_DEVICE_ID_REQ_MASK           (1 << SENSOR_DEVICE_ID)
#define SENSOR_PRODUCT_ID_REQ_MASK          (1 << SENSOR_PRODUCT_ID)
#define SENSOR_MANUFACTURE_ID_REQ_MASK      (1 << SENSOR_MANUFACTURER_ID)
#define SENSOR_SERIAL_NUMBER_REQ_MASK       (1 << SENSOR_SERIAL_NUMBER)
#define SENSOR_SW_VERSION_REQ_MASK          (1 << SENSOR_SW_VERSION)
#define SENSOR_HW_VERSION_REQ_MASK          (1 << SENSOR_HW_VERSION)
#define SENSOR_CALIBRATION_FLAG_MASK        (1 << SENSOR_CALIBRATION_FLAG)
#define SENSOR_SET_PROPERTY_FLAG_MASK        (1 << SENSOR_SET_PROPERTY_FLAG)
#define SENSOR_GET_PROPERTY_FLAG_MASK        (1 << SENSOR_GET_PROPERTY_FLAG)
/**
 * The type for supported sensor device
 */
typedef void * ss_sensor_t; /*!< Composed of (sensor_type << 24) | (sensor_id & 0xFFFFFF) */

typedef struct {
    uint8_t count;        /*!< The number of sensor devices */
    ss_sensor_t sensors[];/*!< array to hold returned sensor devices of type sensor_t */
} ss_sensor_list_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_SCANNING_REQ
 */
typedef struct
{
    struct cfw_message header;
    uint32_t sensor_type_bit_map; /*refer to the sensor type MASK definition*/
} ss_start_sensor_scanning_req_t;

/**
 * Parameters of MSG_ID_SS_STOP_SENSOR_SCANNING_REQ
 */
typedef ss_start_sensor_scanning_req_t ss_stop_sensor_scanning_req_t;

/**
 * On board sensor scan data
 */
typedef struct {
    uint32_t ch_id;     /*!< GET_SENSOR_HANDLE(sensor_type, dev_id) */
} ss_on_board_scan_data_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_REQ
 */
typedef struct
{
    struct cfw_message header;
    uint16_t sampling_interval;     /*!< Sensor data sample frequence, unit: HZ*/
    uint16_t reporting_interval;    /*!< sensor data reporting interval, unit: ms*/
    ss_sensor_t sensor;
    uint8_t data_type_nr;           /*!< The number of data_type for one sensor*/
    uint8_t data_type[1];           /*!< The specific data_type*/
} ss_sensor_subscribe_data_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
    uint8_t data_type_nr;       /*!< The number of data_type for one sensor*/
    uint8_t data_type[1];       /*!< The specific data_type*/
} ss_sensor_unsubscribe_data_req_t;

/**
 *Calibration cmd
 *Generally Speaking, start-->get--->stop----for(;;)set
 */
enum calibration_cmd_e{
    IGNORE_CALIBRATION_CMD,
    START_CALIBRATION_CMD,
    GET_CALIBRATION_CMD,
    STOP_CALIBRATION_CMD,
    SET_CALIBRATION_CMD,
    REBOOT_AUTO_CALIBRATION_CMD
};

/**
 * Parameters of MSG_ID_SS_SENSOR_CALIBRATION_CAL_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
    uint8_t clb_cmd;                    /*!< enum CALIBRAITON_CMD_E*/
    uint8_t calibration_type;           /*!< different calibration type for one sensor*/
    uint8_t data_length;
    uint8_t value[0];
} ss_sensor_calibration_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
    uint8_t status_type;
} ss_sensor_subscribe_status_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_REQ
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t sensor;
    uint8_t status_type;
} ss_sensor_unsubscribe_status_req_t;

/**
 * Sensor service response message
 */
typedef struct
{
    struct cfw_message header;
    ss_sensor_t handle;
    uint8_t status;
} ss_svc_message_general_rsp_t;

typedef ss_svc_message_general_rsp_t ss_start_sensor_scanning_rsp_t;
typedef ss_svc_message_general_rsp_t ss_stop_scanning_rsp_t;
typedef ss_svc_message_general_rsp_t ss_sensor_subscribe_data_rsp_t;
typedef ss_svc_message_general_rsp_t ss_sensor_unsubscribe_data_rsp_t;

/*
 * Sensor service send event
 */
typedef struct
{
    struct cfw_message head;
    uint8_t sensor_type;
    /**
     * Just used for compiling successfully.
     * App can not use them any longer,
     * because sensor service did not set them with available value.
     */
    ss_sensor_t handle;
    uint16_t len;
    union {
        uint8_t data[0];
        ss_on_board_scan_data_t on_board_data;
    };
} ss_sensor_scan_event_t;

typedef struct
{
    struct cfw_message head;
    ss_sensor_t handle;
    ss_sensor_data_header_t sensor_data_header;
} ss_sensor_subscribe_data_event_t;

/*
 * sensor service send get_calibration event to app
 */
typedef struct
{
    struct cfw_message head;
    ss_sensor_t handle;
    uint8_t calibration_type;
    uint8_t data_length;
    uint8_t value[0];
} ss_sensor_get_cal_data_evt_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_SET_PROPERTY_REQ
 */
typedef struct
{
    struct cfw_message head;
    ss_sensor_t sensor;
    uint8_t data_length;
    uint8_t value[0];
} ss_sensor_set_property_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_PROPERTY_REQ
 */
typedef struct
{
    struct cfw_message head;
    ss_sensor_t sensor;
} ss_sensor_get_property_req_t;

/**
 * Parameters of MSG_ID_SS_SENSOR_GET_PROPERTY_EVT
 */
typedef struct
{
    struct cfw_message head;
    ss_sensor_t handle;
    uint8_t data_length;
    uint8_t value[0];
} ss_sensor_get_property_evt_t;

#include "services/sensor_service/sensor_svc_reserved.h"

/**
 * Used by application to start the sensor scanning.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor_type_bit_map  Specify the bitmap of sensor types.
 *                              bitmap refer to \ref ss_sensor_type_t.
 *                              eg:SENSOR_ACCELEROMETER|SENSOR_GYROSCOPE,...
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_START_SENSOR_SCANNING_REQ
 */
int ss_start_sensor_scanning(cfw_service_conn_t *p_service_conn, void *p_priv, uint32_t sensor_type_bit_map);

/**
 * Used by application to stop the sensor scanning.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor_type_bit_map  Specify the bitmap of sensor types.
 *                              bitmap refer to \ref ss_sensor_type_t.
 * @return The sensor list of a certain sensor type. MSG:MSG_ID_SS_STOP_SENSOR_SCANNING_RSP
 */
int ss_stop_sensor_scanning(cfw_service_conn_t *p_service_conn, void *p_priv, uint32_t sensor_type_bit_map);

/**
 * Set the parameter of sensor subscribing data.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @param  data_type    Specific to sensor type.
 * @param  data_type_nr        the number of data type requested
 * @param  sampling_interval   The frequency of sensor data sampling,unit[HZ].
 * @param  reporting_interval  The frequency of sensor data reporting,unit[ms].
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP
 */
int ss_sensor_subscribe_data(cfw_service_conn_t *p_service_conn, void *p_priv,
ss_sensor_t sensor, uint8_t *data_type, uint8_t data_type_nr, uint16_t sampling_interval,
uint16_t reporting_interval);

/**
 * Unset the parameter of sensor subscribing data.
 *
 * @param  p_service_conn   service connection
 * @param  p_priv         pointer to private data
 * @param  sensor         Input parameter.
 * @param  data_type      Specific to sensor type.
 * @param  data_type_nr   Size of data type
 * @return cfw_send_message() status,  MSG:MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP
 */
int ss_sensor_unsubscribe_data(cfw_service_conn_t *p_service_conn, void *p_priv,
                                ss_sensor_t sensor, uint8_t *data_type ,uint8_t data_type_nr);

/**
 * Set the sensor property.
 *
 * @param  p_service_conn     service connection
 * @param  p_priv           pointer to private data
 * @param  sensor           Input parameter.
 * @param  len              Property parameter length.
 * @param  value            Property parameter.
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_SET_PROPERTY_REQ
 */
int ss_sensor_set_property(cfw_service_conn_t *p_service_conn, void *p_priv,
                 ss_sensor_t sensor, uint8_t len, uint8_t *value);
/**
 * Set the sensor property.
 *
 * @param  p_service_conn     service connection
 * @param  p_priv           pointer to private data
 * @param  sensor           Input parameter.
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_GET_PROPERTY_REQ
 */
int ss_sensor_get_property(cfw_service_conn_t *p_service_conn, void *p_priv,
                 ss_sensor_t sensor);

/**
 * Set the calibration .
 *
 * @param  p_service_conn     service connection
 * @param  p_priv           pointer to private data
 * @param  sensor           Input parameter.
 * @param  calibration_type Specific to sensor type.
 * @param  len              Calibration parameter length. (Only valid for SET_CALIBRATION_CMD).
 * @param  value            Calibration parameter.(Only valid for SET_CALIBRATION_CMD).
 * @return cfw_send_message() status,   MSG:MSG_ID_SS_SENSOR_CALIBRATION_REQ
 */
int ss_sensor_set_calibration(cfw_service_conn_t *p_service_conn, void *p_priv,
                 ss_sensor_t sensor, uint8_t calibration_type, uint8_t len, uint8_t *value);

/**
 * Operation the calibration parameter.
 *
 * @param  p_service_conn service connection
 * @param  p_priv       pointer to private data
 * @param  sensor       Input parameter.
 * @param  clb_cmd      START_CALIBRATION_CMD/STOP_CALIBRATION_CMD/SET_CALIBRATION_CMD
 * @param  calibration_type  Specific to sensor type.
 * @return Indicates whether the operation is success of failure.
 *         MSG: MSG_ID_SS_SENSOR_CALIBRATION_REQ
 */
int ss_sensor_opt_calibration(cfw_service_conn_t *p_service_conn, void *p_priv,
                             ss_sensor_t sensor, uint8_t clb_cmd, uint8_t calibration_type);

/**
  * Initialize the sensor framework client to connect
  * to sensor framework and allocate resources.
  *
  * It should be the first API to be called before other sensor
  * framework functions.
  *
  * @param[in]  p_queue  Queue of service
  * @return  none
  */
void ss_service_init(void *p_queue);

#define IS_ON_BOARD_SENSOR_TYPE(type) ((((type) > ON_BOARD_SENSOR_TYPE_START) &&    \
                                        ((type) < ON_BOARD_SENSOR_TYPE_END)) ? 1 : 0)
#define IS_BLE_SENSOR_TYPE(type)    ((((type) > BLE_SENSOR_TYPE_START) &&   \
                                    ((type) < BLE_SENSOR_TYPE_END)) ? 1 : 0)
#define IS_ANT_SENSOR_TYPE(type)    ((((type) > ANT_SENSOR_TYPE_START) &&   \
                                    ((type) < ANT_SENSOR_TYPE_END)) ? 1 : 0)
/** @} */

#endif   /*__SENSOR_SVC_API_H__*/
