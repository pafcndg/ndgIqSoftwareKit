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

#include "cfw/cfw_client.h"
#include "services/sensor_service/sensor_svc.h"

int ss_start_sensor_scanning(cfw_service_conn_t *p_service_conn,
                                    void *p_priv, uint32_t sensor_type_bit_map)
{
    ss_start_sensor_scanning_req_t *p_msg;

    /* Allocate sensor scanning request message */
    p_msg = (ss_start_sensor_scanning_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_START_SENSOR_SCANNING_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor_type_bit_map = sensor_type_bit_map;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_stop_sensor_scanning(cfw_service_conn_t *p_service_conn, void *p_priv, uint32_t sensor_type_bit_map)
{
    ss_stop_sensor_scanning_req_t *p_msg;

    /* Allocate stopping sensor scanning request message */
    p_msg = (ss_stop_sensor_scanning_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_STOP_SENSOR_SCANNING_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor_type_bit_map = sensor_type_bit_map;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_subscribe_data(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor,
                             uint8_t *data_type, uint8_t data_type_nr, uint16_t sampling_interval,
                             uint16_t reporting_interval)
{
    ss_sensor_subscribe_data_req_t *p_msg;

    if (data_type_nr == 0) {
        return -1;
    }
    /* Allocate sensor subscribing data request message */
    p_msg = (ss_sensor_subscribe_data_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_REQ, (sizeof(*p_msg) + (data_type_nr - 1) *
                                                          sizeof(uint8_t)), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->data_type_nr = data_type_nr;
    p_msg->sampling_interval = sampling_interval;
    p_msg->reporting_interval = reporting_interval;

    /* Fill Request Parameter */
    memcpy(p_msg->data_type ,data_type ,sizeof(uint8_t) * data_type_nr);

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_unsubscribe_data(cfw_service_conn_t *p_service_conn, void *p_priv,
                                        ss_sensor_t sensor, uint8_t *data_type, uint8_t data_type_nr)
{
    ss_sensor_unsubscribe_data_req_t *p_msg;

    if (data_type_nr == 0) {
        return -1;
    }
    /* Allocate sensor unsubscribing data request message */
    p_msg = (ss_sensor_unsubscribe_data_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_REQ,  (sizeof(*p_msg) + (data_type_nr - 1) *
                                                             sizeof(uint8_t)), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->data_type_nr = data_type_nr;
    /* Fill Request Parameter */
    memcpy(p_msg->data_type ,data_type ,sizeof(uint8_t) * data_type_nr);

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_set_property(cfw_service_conn_t *p_service_conn, void *p_priv,
                            ss_sensor_t sensor,  uint8_t len, uint8_t *value)
{
    ss_sensor_set_property_req_t *p_msg;

    /* Allocate sensor setting calibration request message */
    p_msg = (ss_sensor_set_property_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_SET_PROPERTY_REQ, sizeof(*p_msg)+len, p_priv);
    if ((p_msg == NULL) || (value == NULL) || (len == 0)) {
        return -1;
    }
    p_msg->sensor = sensor;
    p_msg->data_length= len;
    /* Fill cal param*/
    memcpy(p_msg->value, value, len);

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_property(cfw_service_conn_t *p_service_conn, void *p_priv,
                            ss_sensor_t sensor)
{
    ss_sensor_get_property_req_t *p_msg;

    /* Allocate sensor setting calibration request message */
    p_msg = (ss_sensor_get_property_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_GET_PROPERTY_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    p_msg->sensor = sensor;
    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_set_calibration(cfw_service_conn_t *p_service_conn, void *p_priv,
                            ss_sensor_t sensor, uint8_t calibration_type, uint8_t len, uint8_t *value)
{
    ss_sensor_calibration_req_t *p_msg;

    /* Allocate sensor setting calibration request message */
    p_msg = (ss_sensor_calibration_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_CALIBRATION_REQ, sizeof(*p_msg)+len, p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    p_msg->sensor = sensor;
    p_msg->calibration_type = calibration_type;
    p_msg->clb_cmd = SET_CALIBRATION_CMD;
    p_msg->data_length= len;
    /* Fill cal param*/
    memcpy(p_msg->value, value, len);

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_opt_calibration(cfw_service_conn_t *p_service_conn, void *p_priv,
                                    ss_sensor_t sensor, uint8_t clb_cmd, uint8_t calibration_type)
{
    ss_sensor_calibration_req_t *p_msg;

    /* Allocate sensor getting calibration capability request message */
    p_msg = (ss_sensor_calibration_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_CALIBRATION_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->clb_cmd= clb_cmd;
    p_msg->calibration_type = calibration_type;
    /* Send the message */
    return cfw_send_message(p_msg);
}


#if CONFIG_SENSOR_SERVICE_RESERVED_FUNC
int ss_sensor_subscribe_status(cfw_service_conn_t *p_service_conn, void *p_priv,
                                        ss_sensor_t sensor, uint8_t status_type)
{
    ss_sensor_subscribe_status_req_t *p_msg;

    /* Allocate sensor subscribing status request message */
    p_msg = (ss_sensor_subscribe_status_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_SUBSCRIBE_STATUS_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->status_type = status_type;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_unsubscribe_status(cfw_service_conn_t *p_service_conn, void *p_priv,
                                            ss_sensor_t sensor, uint8_t status_type)
{
    ss_sensor_unsubscribe_status_req_t *p_msg;

    /* Allocate sensor unsubscribing status request message */
    p_msg = (ss_sensor_unsubscribe_status_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_UNSUBSCRIBE_STATUS_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->status_type = status_type;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_send_sensor_cmd(cfw_service_conn_t *p_service_conn, void *p_priv,
        ss_sensor_t sensor, uint32_t cmd_id, cmd_parameter_t parameter)
{
    ss_send_sensor_cmd_req_t *p_msg;

    int msg_size = parameter.length + sizeof(*p_msg) + sizeof(parameter.length);

    /* Allocate sending sensor cmd request message */
    p_msg = (ss_send_sensor_cmd_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SEND_SENSOR_CMD_REQ, msg_size, p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;
    p_msg->cmd_id = cmd_id;
    p_msg->parameter = parameter;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_release_sensor(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor)
{
    ss_release_sensor_req_t *p_msg;

    /* Allocate releasing sensor request message */
    p_msg = (ss_release_sensor_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_RELEASE_SENSOR_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_device_id(cfw_service_conn_t *p_service_conn, void *p_priv,
                                    ss_sensor_t sensor)
{
    ss_sensor_get_device_id_req_t *p_msg;

    /* Allocate sensor getting device id request message */
    p_msg = (ss_sensor_get_device_id_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_DEVICE_ID_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_product_id(cfw_service_conn_t *p_service_conn, void *p_priv,
                                            ss_sensor_t sensor)
{
    ss_sensor_get_product_id_req_t *p_msg;

    /* Allocate sensor getting product id request message */
    p_msg = (ss_sensor_get_product_id_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_PRODUCT_ID_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;
    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_manufacturer_id(cfw_service_conn_t *p_service_conn, void *p_priv,
                                            ss_sensor_t sensor)
{
    ss_sensor_get_manufacturer_id_req_t *p_msg;

    /* Allocate sensor getting manufacturer id request message */
    p_msg = (ss_sensor_get_manufacturer_id_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_MANUFACTURER_ID_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_serial_number(cfw_service_conn_t *p_service_conn, void *p_priv,
                                        ss_sensor_t sensor)
{
    ss_sensor_get_serial_number_req_t *p_msg;

    /* Allocate sensor getting serial number request message */
    p_msg = (ss_sensor_get_serial_number_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_SERIAL_NUMBER_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_sw_version(cfw_service_conn_t *p_service_conn, void *p_priv,
                                    ss_sensor_t sensor)
{
    ss_sensor_get_sw_version_req_t *p_msg;

    /* Allocate sensorgeting software version request message */
    p_msg = (ss_sensor_get_sw_version_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_SW_VERSION_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_get_hw_version(cfw_service_conn_t *p_service_conn, void *p_priv,
                                        ss_sensor_t sensor)
{
    ss_sensor_get_hw_version_req_t *p_msg;

    /* Allocate sensor getting hardware version request message */
    p_msg = (ss_sensor_get_hw_version_req_t *) cfw_alloc_message_for_service(p_service_conn,
                                MSG_ID_SS_SENSOR_GET_HW_VERSION_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor= sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_pair(cfw_service_conn_t *p_service_conn, void *p_priv, ss_pair_param_t *p_pair_param)
{
    ss_sensor_pair_req_t *p_msg;

    /* Allocate sensor pairing request message */
    p_msg = (ss_sensor_pair_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_PAIR_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->pair_param = *p_pair_param;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_unpair(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor)
{
    ss_sensor_unpair_req_t *p_msg;

    /* Allocate sensor unpairing request message */
    p_msg = (ss_sensor_unpair_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_UNPAIR_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}
int ss_sensor_start(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor)
{
    ss_sensor_start_req_t *p_msg;

    /* Allocate sensor starting request message */
    p_msg = (ss_sensor_start_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_START_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_sensor_stop(cfw_service_conn_t *p_service_conn, void *p_priv, ss_sensor_t sensor)
{
    ss_sensor_stop_req_t *p_msg;

    /* Allocate sensor stopping request message */
    p_msg = (ss_sensor_stop_req_t *) cfw_alloc_message_for_service(p_service_conn,
                    MSG_ID_SS_SENSOR_STOP_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->sensor = sensor;

    /* Send the message */
    return cfw_send_message(p_msg);
}

int ss_set_mode(cfw_service_conn_t *p_service_conn, void *p_priv, uint8_t tether_mode)
{
    ss_set_mode_req_t *p_msg;

    /* Allocate sensor setting calibration request message */
    p_msg = (ss_set_mode_req_t *) cfw_alloc_message_for_service(p_service_conn,
            MSG_ID_SS_SET_MODE_REQ, sizeof(*p_msg), p_priv);
    if (p_msg == NULL) {
        return -1;
    }
    /* Fill Request Parameter */
    p_msg->tether_mode = tether_mode;

    /* Send the message */
    return cfw_send_message(p_msg);
}
#endif/**CONFIG_SENSOR_SERVICE_RESERVED_FUNC*/
