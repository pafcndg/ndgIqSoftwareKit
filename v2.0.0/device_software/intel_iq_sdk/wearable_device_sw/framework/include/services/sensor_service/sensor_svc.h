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

#ifndef __SENSOR_SVC_H__
#define __SENSOR_SVC_H__
#include <string.h>
#include "services/sensor_service/sensor_data_format.h"
#include "services/sensor_service/sensor_svc_platform.h"
#include "services/sensor_service/sensor_svc_calibration.h"

/* Return status of sensor funcs*/
typedef enum {
    NO_ERROR,
    NO_CLIENT_SUPPORT,    /*Client does't support the sensor*/
    NO_CLIENT_INFO,    /*Can't find the client info*/
    NO_SENSOR_INFO    /*Can't find the sensor info*/
} err_info_t;

/* Sensor service status */
typedef enum {
    SS_STATUS_SUCCESS = 0,
    SS_STATUS_CMD_REPEAT,
    SS_STATUS_ERROR            /* Generic Error */
} ss_service_status_t;

#define NEXT_STATUS_SCANNING 1
#define NEXT_STATUS_PAIRING  2

typedef struct {
    uint8_t next_status;
    void *p_param;
} open_service_param_t;

int ss_send_rsp_msg_to_client(void *p_handle, uint16_t msg_id,
         ss_service_status_t status, void *p_sensor_handle);

void ss_send_scan_rsp_msg_to_clients(uint32_t sensor_type_bit_map,
                                      ss_service_status_t status);

void ss_send_scan_data_to_clients(uint8_t sensor_type, void *p_data);


void ss_send_stop_scan_rsp_msg_to_clients(uint32_t sensor_type_bit_map,
                                           ss_service_status_t status);

void ss_send_pair_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t status);

void ss_send_subscribing_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                    ss_service_status_t status);

void ss_send_subscribing_evt_msg_to_clients(ss_sensor_t sensor_handle,
                           uint8_t data_type, uint32_t timestamp, void *p_data, uint16_t len);

void ss_send_unsubscribing_rsp_msg_to_clients(ss_sensor_t sensor_handle,
                                                    ss_service_status_t status);

void ss_send_cal_rsp_and_data_to_clients(ss_sensor_t sensor_handle, uint8_t calibration_type,
                                        uint8_t length, uint8_t *cal_data, uint8_t status);

void ss_send_set_property_rsp_to_clients(ss_sensor_t ,uint8_t );

void ss_send_get_property_data_to_clients(ss_sensor_t ,uint8_t , uint8_t *, uint8_t );

void ss_send_unpair_rsp_msg_to_clients(ss_sensor_t sensor_handle, ss_service_status_t status);

void ss_send_unlink_rsp_msg_to_clients(ss_sensor_t sensor_handle, ss_service_status_t status);

void ss_send_device_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

void ss_send_product_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

void ss_send_manufacturer_id_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

void ss_send_serial_number_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

void ss_send_hw_version_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

void ss_send_sw_version_rsp_msg_to_clients(ss_sensor_t sensor_handle, uint8_t *str, uint8_t status);

#endif
