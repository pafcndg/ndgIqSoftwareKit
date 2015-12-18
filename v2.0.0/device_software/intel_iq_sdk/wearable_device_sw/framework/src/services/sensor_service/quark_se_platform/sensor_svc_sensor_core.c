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

#include "string.h"
#include "os/os.h"

#include "cfw/cfw_service.h"

#include "services/sensor_service/sensor_svc.h"
#include "services/sensor_service/sensor_svc_sensor_core.h"
#include "services/sensor_service/sensor_svc_utils.h"

int send_request_cmd_to_core(uint8_t tran_id,
                                    uint8_t sensor_id,
                                    uint32_t param1,
                                    uint32_t param2,
                                    uint8_t* addr,
                                    uint8_t cmd_id)
{
    struct ia_cmd *cmd = NULL;
    uint16_t length;

    switch( cmd_id ){
        case CMD_GET_SENSOR_LIST:{
            length = sizeof(struct ia_cmd)+sizeof(struct sensor_type_bit_map);
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct sensor_type_bit_map *bitmap= (struct sensor_type_bit_map *)cmd->param;
            bitmap->bit_map = param1;
            SS_PRINT_LOG("CMD_GET_SENSOR_LIST:bit_map=%d",param1);
            break;
        }case CMD_START_SENSOR:
        case CMD_STOP_SENSOR:{
            length = sizeof(struct ia_cmd)+sizeof(struct sensor_id);
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct sensor_id *sensor = (struct sensor_id*)cmd->param;
            sensor->sensor_type = tran_id;
            sensor->dev_id = sensor_id;
            break;
        }case CMD_SUBSCRIBE_SENSOR_DATA:{
            length = sizeof(struct ia_cmd)+sizeof(struct subscription);
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct subscription *ia_param = (struct subscription*)cmd->param;
            ia_param->sensor.sensor_type = tran_id;
            ia_param->sensor.dev_id = sensor_id;
            ia_param->sampling_interval = param1;
            ia_param->reporting_interval= param2;
            break;
        }case CMD_UNSUBSCRIBE_SENSOR_DATA:{
            length = sizeof(struct ia_cmd)+sizeof(struct unsubscription);
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct unsubscription *ia_param = (struct unsubscription*)cmd->param;
            ia_param->sensor.sensor_type = tran_id;
            ia_param->sensor.dev_id = sensor_id;
            break;
       }case CMD_GET_PROPERTY:{
            length = sizeof(struct ia_cmd)+sizeof(struct get_property);
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct get_property *ia_param = (struct get_property*)cmd->param;
            ia_param->sensor.sensor_type = tran_id;
            ia_param->sensor.dev_id = sensor_id;
            break;
        }case CMD_SET_PROPERTY:{
            length = sizeof(struct ia_cmd)+sizeof(struct set_property)+param2;
            cmd = (struct ia_cmd*)balloc(length,NULL);
            struct set_property *ia_param = (struct set_property *)cmd->param;
            ia_param->sensor.sensor_type = tran_id;
            ia_param->sensor.dev_id = sensor_id;
            ia_param->length= param2;
            memcpy(ia_param->property_params, addr, param2);
            break;
        }case CMD_DEBUG:{
            SS_PRINT_LOG("Returning as command debug sent\n");
            return IPC_STS_ERR_EMPTY;
        }default:{
            length = sizeof(struct ia_cmd);
            cmd = (struct ia_cmd*)balloc(length,NULL);
        }
    }
    cmd->length = length;
    cmd->cmd_id = cmd_id;
    return ipc_2core_send(cmd);
}

int svc_send_scan_cmd_to_core(uint32_t sensor_type_bit_map)
{
    return send_request_cmd_to_core(0, 0, sensor_type_bit_map, 0, NULL,CMD_GET_SENSOR_LIST);
}

int svc_send_start_cmd_to_core(uint8_t tran_id,
                               uint8_t sensor_id)
{
    return send_request_cmd_to_core(tran_id,
                                    sensor_id,
                                    0,0,NULL,
                                    CMD_START_SENSOR);
}

int svc_send_stop_cmd_to_core(uint8_t tran_id,
                              uint8_t sensor_id)
{
    return send_request_cmd_to_core(tran_id,
                                    sensor_id,
                                    0,0,NULL,
                                    CMD_STOP_SENSOR );
}

int svc_send_subscribe_data_cmd_to_core(uint8_t tran_id,
                                        uint8_t sensor_id,
                                        uint16_t sample_freq,
                                        uint16_t reporting_interval)
{
    //return ipc_2core_send(&cmd);
    return send_request_cmd_to_core(tran_id,
                                    sensor_id,
                                    sample_freq,
                                    reporting_interval,
                                    NULL,
                                    CMD_SUBSCRIBE_SENSOR_DATA);
    return 0;
}

int svc_send_unsubscribe_data_cmd_to_core(uint8_t tran_id,
                                          uint8_t sensor_id,
                                          uint8_t data_type)
{
    return send_request_cmd_to_core(tran_id,
                                    sensor_id,
                                    0,0,NULL,
                                    CMD_UNSUBSCRIBE_SENSOR_DATA);
}

int svc_send_calibration_cmd_to_core(ss_sensor_calibration_req_t *req)
{
    struct ia_cmd *cmd = NULL;
    uint8_t sensor_type = GET_SENSOR_TYPE(req->sensor);
    uint8_t sensor_id = GET_SENSOR_ID(req->sensor);

    uint16_t length = sizeof(struct ia_cmd)+sizeof(struct calibration)+req->data_length;
    cmd = (struct ia_cmd*)balloc(length,NULL);
    struct calibration *ia_param = (struct calibration*)cmd->param;
    ia_param->sensor.sensor_type = sensor_type;
    ia_param->sensor.dev_id = sensor_id;
    ia_param->calibration_type= req->calibration_type;
    ia_param->clb_cmd= req->clb_cmd;
    cmd->cmd_id = CMD_CALIBRATION;
    if((req->clb_cmd) == SET_CALIBRATION_CMD || (req->clb_cmd == REBOOT_AUTO_CALIBRATION_CMD)){
        ia_param->data_length = req->data_length;
        memcpy(ia_param->calib_params, req->value, ia_param->data_length);
        /*write the clb data to falsh*/
        if(ia_param->clb_cmd == SET_CALIBRATION_CMD)
            sensor_clb_write_flash(sensor_type, sensor_id, ia_param->calib_params, ia_param->data_length);
    }else{
        ia_param->data_length = 0;
    }
    return ipc_2core_send(cmd);
}

void ss_sc_resp_msg_handler(sc_rsp_t *p_msg)
{
    switch(p_msg->msg_id) {
        case RESP_GET_SENSOR_LIST: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct sensor_list *list = (struct sensor_list*)cmd->param;
            if(list->count == 0){
                SS_PRINT_LOG("Resplist->count=%d,NO sensor list ,break",list->count);
                break;
            }
            int i = 0;
            int old_sensor_type = -1;
            for(i=0; i<(list->count); i++){
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
                SS_PRINT_LOG("Resp list->count=%d,sensor_type=%d,dev_id=%d", \
                                                                list->count,list->sensor_list[i].sensor_type, \
                                                                list->sensor_list[i].dev_id);
#endif
                ss_on_board_scan_data_t scan_data;
                scan_data.ch_id = list->sensor_list[i].dev_id;
                /*First, send a rsp msg to service.Rsp msg is a very important symbol to judge the sensor status*/
                if( old_sensor_type != list->sensor_list[i].sensor_type)
                    ss_send_scan_rsp_msg_to_clients(1 << (list->sensor_list[i].sensor_type), SS_STATUS_SUCCESS);
                /*Second, we can send the scan data*/
                ss_send_scan_data_to_clients(list->sensor_list[i].sensor_type, &scan_data);
                old_sensor_type = list->sensor_list[i].sensor_type;
            }

            break;
        } case RESP_START_SENSOR: {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct return_value *ret_v = (struct return_value*)cmd->param;
            ss_send_scan_rsp_msg_to_clients(1 << (ret_v->sensor.sensor_type), ret_v->ret);
            SS_PRINT_LOG("[RSP_START_SENSOR]: sensor_type: %d, sensor_id: %d, status: %d",
                         ret_v->sensor.sensor_type, ret_v->sensor.dev_id, ret_v->ret);
#endif
            break;
        } case RESP_STOP_SENSOR: {
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct return_value *ret_v = (struct return_value*)cmd->param;
            SS_PRINT_LOG("[RSP_STOP_SENSOR]: sensor_type: %d, sensor_id: %d, status: %d",
                         ret_v->sensor.sensor_type, ret_v->sensor.dev_id, ret_v->ret);
#endif
            break;
        } case RESP_SUBSCRIBE_SENSOR_DATA: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct return_value *ret_v = (struct return_value*)cmd->param;
            ss_send_subscribing_rsp_msg_to_clients(GET_SENSOR_HANDLE(ret_v->sensor.sensor_type,
                    ret_v->sensor.dev_id), ret_v->ret);
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("[RSP_SUBCRIBE_DATA]: sensor_type: %d, sensor_id: %d, status: %d",
                         ret_v->sensor.sensor_type, ret_v->sensor.dev_id, ret_v->ret);
#endif
            break;
        } case RESP_UNSUBSCRIBE_SENSOR_DATA: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct return_value *ret_v = (struct return_value*)cmd->param;
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("[RSP_USUBCRIBE_DATA]: sensor_type: %d, sensor_id: %d, status: %d",
                         ret_v->sensor.sensor_type, ret_v->sensor.dev_id, ret_v->ret);
#endif
            ss_send_unsubscribing_rsp_msg_to_clients(GET_SENSOR_HANDLE(ret_v->sensor.sensor_type,
                                                    ret_v->sensor.dev_id), ret_v->ret);
            break;
        } case RESP_CALIBRATION: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct resp_calibration  *resp_clb = (struct resp_calibration*)cmd->param;
            if(resp_clb->ret.ret)
                SS_PRINT_LOG("clb_cmd :%d, clb_type:%d,type: %d, id: %d, status: %d", \
                                    resp_clb->clb_cmd, resp_clb->calibration_type, \
                                    resp_clb->ret.sensor.sensor_type, \
                                    resp_clb->ret.sensor.dev_id, resp_clb->ret.ret);
            /*Service should ignore this clb_cmd*/
            if(resp_clb->clb_cmd == REBOOT_AUTO_CALIBRATION_CMD)
                break;
            /*This func will check the data and len*/
            ss_send_cal_rsp_and_data_to_clients(GET_SENSOR_HANDLE(resp_clb->ret.sensor.sensor_type,
                                                                    resp_clb->ret.sensor.dev_id),
                                                                    resp_clb->calibration_type,
                                                                    resp_clb->length,
                                                                    resp_clb->calib_params,resp_clb->ret.ret);
            /*When get the cal data, we should write it into flash.It belongs a coarse tuning
             *Maybe users will use SET_CALIBRATION_CMD to set a fine tuning*/
            if( resp_clb->clb_cmd == GET_CALIBRATION_CMD ){
                sensor_clb_write_flash( resp_clb->ret.sensor.sensor_type, resp_clb->ret.sensor.dev_id,
                                        resp_clb->calib_params, resp_clb->length);
            }
            break;
        } case RESP_GET_PROPERTY: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct resp_get_property *resp= (struct resp_get_property*)cmd->param;
            //return rsp and evt
            ss_send_get_property_data_to_clients(GET_SENSOR_HANDLE(resp->ret.sensor.sensor_type,
                        resp->ret.sensor.dev_id), resp->length, resp->property_params, resp->ret.ret);
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("[RSP_PROP]: sensor_type: %d, sensor_id: %d, status: %d",
                    resp->ret.sensor.sensor_type, resp->ret.sensor.dev_id, resp->ret.ret);
#endif
            break;
        } case RESP_SET_PROPERTY: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct return_value *ret_v = (struct return_value*)cmd->param;
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
            SS_PRINT_LOG("[RSP_PROP]: sensor_type: %d, sensor_id: %d, status: %d",
                    ret_v->sensor.sensor_type, ret_v->sensor.dev_id, ret_v->ret);
#endif
            ss_send_set_property_rsp_to_clients(GET_SENSOR_HANDLE(ret_v->sensor.sensor_type,
                                                    ret_v->sensor.dev_id), ret_v->ret);
            break;
        } case RESP_DEBUG: {
            break;
        } case SENSOR_DATA: {
            struct ia_cmd *cmd = (struct ia_cmd*)p_msg->param;
            struct sensor_data *data = (struct sensor_data*)cmd->param;
#if defined(SENSOR_SERVICE_DEBUG) && (SENSOR_SERVICE_DEBUG == 1)
          SS_PRINT_LOG("[SENSOR_DATA]: sensor_type: %d, sensor_id: %d, data_len: %d",
                  data->sensor.sensor_type, data->sensor.dev_id, data->data_length);
#endif
            ss_send_subscribing_evt_msg_to_clients(GET_SENSOR_HANDLE(data->sensor.sensor_type, data->sensor.dev_id),
                                                        0,
                                                        data->timestamp,
                                                        (void*)data->data,
                                                        data->data_length);
            break;
        }default:
            break;
        }
}
