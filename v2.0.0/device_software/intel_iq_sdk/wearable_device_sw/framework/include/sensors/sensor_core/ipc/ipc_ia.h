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

#ifndef __IPC_IA_H
#define __IPC_IA_H
#include "os/os.h"
#include "sensors/phy_sensor_api/phy_sensor_common.h"
#include "util/compiler.h"

/**
 * @defgroup sensor_core_ipc  Sensor Core IPC
 * Common data structure in sensor layer,service<--Ipc-->sensor_core.
 * @ingroup SS_SERVICE
 * @{
 */

#define CMD_PARAM_MAX_SIZE 62
#define RESP_PARAM_MAX_SIZE 1022

/** sensor core response cmd_id to service  */
enum ipc_resp_type{
    RESP_GET_SENSOR_LIST,
    RESP_START_SENSOR,
    RESP_STOP_SENSOR,
    RESP_SUBSCRIBE_SENSOR_DATA,
    RESP_UNSUBSCRIBE_SENSOR_DATA,
    RESP_CALIBRATION,
    RESP_GET_PROPERTY,
    RESP_SET_PROPERTY,
    RESP_DEBUG,
    SENSOR_DATA,
    RESP_ID_MAX,
};
/** service request cmd for sensor_core*/
enum ipc_cmd_type{
    CMD_GET_SENSOR_LIST = 0,
    CMD_START_SENSOR,
    CMD_STOP_SENSOR,
    CMD_SUBSCRIBE_SENSOR_DATA,
    CMD_UNSUBSCRIBE_SENSOR_DATA,
    CMD_CALIBRATION,
    CMD_GET_PROPERTY,
    CMD_SET_PROPERTY,
    CMD_DEBUG,
    CMD_TRIGGER_ISR,
    CMD_ID_MAX,
};

/** Common status to indicates the sensor_core status , when we try to request a cmd*/
enum ipc_ret_type{
    RESPERROR_SUCCESS = 0,
    RESPERROR_NO_DEVICE,
    RESPERROR_DEVICE_EXCEPTION,
};

/** unique struct data used to transport all cmd/response/data  */
struct ia_cmd {
    uint8_t tran_id;
    uint8_t cmd_id;
    uint16_t length;
    uint8_t param[] __aligned(4);
} __aligned(4);

/**
 * GET LIST OF AVAILABLE SENSORS
 *
 * This struct is parameter to specify the bitmap of sensor types
 * For example, if sensor core service wants to get the sensor list of SENSOR_ACCELEROMETER and SENSOR_GYROSCOPE, the input value should be,
    (1 << SENSOR_ACCELEROMETER) | (1 << SENSOR_GYROSCOPE)
 * */
struct sensor_type_bit_map {
        uint32_t bit_map;
} __aligned(4);

/**
 * RETURN LIST OF AVAILABLE SENSORS
 */
struct sensor_list {
    uint8_t count;                                               /**<  indicates the number of returned sensor_id */
    struct sensor_id sensor_list[] __aligned(4); /**<  the array of return of type sensor_id */
} __aligned(4);

/**
 * THE RETURN VALUE OF STARTING A SENSOR
 * THE RETURN VALUE OF STOPPING A SENSOR
 * THE RETURN VALUE OF SUBSCRIBING SENSOR DATA
 * THE RETURN VALUE OF UNSUBSCRIBING SENSOR DATA
 * THE RETURN VALUE OF SETTING CALIBRATION OF A SENSOR
 * THE RETURNED VALUE OF SETTING A PROPERTY VALUE OF A SENSOR
 * */
struct return_value {
    struct sensor_id sensor;
    uint8_t ret;
} __aligned(4);

/**
 * THE RETURNED CALIBRATION PARAMETERS OF A SENSOR
 */
struct resp_calibration {
    struct return_value ret;
    uint8_t clb_cmd;                                        /*!< reter to enum calibration_cmd_e*/
    uint8_t calibration_type;                               /*!< refer to enum calibration_type_e*/
    uint8_t length;
    uint8_t calib_params[] __aligned(4);    /*!<  is specific to sensor device */
}__aligned(4);

/**
 * THE RETURNED PROPERTY VALUES OF A SENSOR
 * */
struct resp_get_property {
    struct return_value ret;
    uint8_t length;
    uint8_t property_params[] __aligned(4);
} __aligned(4);

/**
 * SUBSCRIBE SENSOR DATA
 *
 * The unit of sampling_interval and reporting_interval is ms.
 * */
struct subscription {
    struct sensor_id sensor;
    uint16_t sampling_interval;
    uint16_t reporting_interval;
} __aligned(4);

/**
 * UNSUBSCRIBE SENSOR data
 * */
struct unsubscription {
    struct sensor_id sensor;
} __packed;

/**
 * CALIBRATION PARAMETERS OF A SENSOR
 * */
struct calibration {
    struct sensor_id sensor;
    uint8_t clb_cmd;                                        /*!< refer to enum calibration_cmd_e*/
    uint8_t calibration_type;                               /*!< refer to enum calibration_type_e*/
    uint8_t data_length;
    uint8_t calib_params[] __aligned(4);    /*!<  calibration result*/
}__aligned(4);

/**
 * GET A PROPERTY VALUE OF A SENSOR
 * */
struct get_property {
    struct sensor_id sensor;
    uint8_t property;               /**<  is specific to sensor device */
} __aligned(4);

/**
 * SET A PROPERTY VALUE OF A SENSOR
 * */
struct set_property {
    struct sensor_id sensor;
    uint8_t length;
    uint8_t property_params[] __aligned(4);
}__aligned(4);

/** @} */
#endif
