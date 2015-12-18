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

#ifndef __SENSOR_SVC_SENSOR_CORE_H
#define __SENSOR_SVC_SENSOR_CORE_H

#include "services/sensor_service/sensor_svc_api.h"
#include <stdint.h>
#include <stdbool.h>
#include "util/compiler.h"


typedef struct {
    struct cfw_message head;
    uint16_t msg_id;
    uint8_t tran_id;
    uint8_t param[] __aligned(4);
}__aligned(4) sc_rsp_t;

typedef struct {
    struct cfw_message head;
    uint16_t msg_id;
    uint8_t sensor_type;
    uint8_t sensor_id;
    uint8_t len;
    uint8_t data[0];
} sc_evt_t;

typedef sc_evt_t sc_scan_evt_t;

typedef sc_evt_t sc_subscribe_evt_t;

int svc_send_scan_cmd_to_core(uint32_t sensor_type_bit_map);

int svc_send_start_cmd_to_core(uint8_t sensor_type,
                                        uint8_t sensor_id);

int svc_send_subscribe_data_cmd_to_core(uint8_t sensor_type,
                                        uint8_t sensor_id,
                                        uint16_t sample_freq,
                                        uint16_t buf_delay);

int svc_send_unsubscribe_data_cmd_to_core(uint8_t sensor_type,
                                          uint8_t sensor_id,
                                          uint8_t data_type);

int svc_send_calibration_cmd_to_core(ss_sensor_calibration_req_t *req);

int svc_send_stop_cmd_to_core(uint8_t sensor_type,
                              uint8_t sensor_id);

void ss_sc_resp_msg_handler(sc_rsp_t *p_msg);

void ss_sc_evt_msg_handler(sc_evt_t *p_msg);

int send_request_cmd_to_core(uint8_t tran_id,
                                    uint8_t sensor_id,
                                    uint32_t param1,
                                    uint32_t param2,
                                    uint8_t* addr,
                                    uint8_t cmd_id);

/**
 * take sensor_service as a client of sensor_core svc
 *
 * sensor service open the sensor core service, so that this
 * sensor_svc will be a client of sensor_core_svc.
 */
bool sensor_svc_open_sensor_core_svc(void * queue);
#endif
