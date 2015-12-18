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
#include "cfw/cfw_debug.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_debug.h"
#include "cfw/cproxy.h"
#include "infra/port.h"
#include "infra/log.h"

#include "services/sensor_service/sensor_svc_api.h"
#include "services/sensor_service/sensor_svc_utils.h"
#include "services/sensor_service/sensor_svc_platform.h"

#ifdef CONFIG_TCMD
#include "infra/tcmd/handler.h"
#endif

static ss_sensor_t sensor_handles[32];

#ifdef CONFIG_SERVICES_SENSOR
#define SENSOR_SVC_ID ARC_SC_SVC_ID
#endif

/*start
 *attr of tcmd*/
#define UNKNOWN_SENSOR 255
#define UNKNOWN_DATA_TYPE 255
#define SUBSCRIBE_SENSOR_OFFSET 2
//just ignore the data_type
#define SUBSCRIBE_DATA_TYPE 2
#define SUBSCRIBE_SAMPLE_INTR 3
#define SUBSCRIBE_REPORT_INTR 4

/*attr of calibration*/
#define CALIBRATION_CMD_OFFSET  2
#define CALIBRATION_TYPE_OFFSET  3
#define CALIBRATION_SENSOR_TYPE_OFFSET  4
#define CALIBRATION_PARAM_OFFSET  5

/*attr of unsubcribe*/
#define UNSUBSCRIBE_REQ_LENGTH 3

/*attr of property*/
#define PROPERTY_SENSOR_TYPE_OFFSET  2
#define PROPERTY_PARAM_OFFSET  3

/*attr of tcmd
 *end*/

#define SS_TCMD_LOG(format ,...) \
    do{ \
        pr_info(LOG_MODULE_SS_SVC,"[TCMD]"format,##__VA_ARGS__); \
    }while(0)

static cfw_service_conn_t *p_ss_service_conn = NULL;

#define SENSOR_TYPE_NUM 11
struct sensors {
    char sensor_name[10];
    uint8_t sensor_type;
} sensors_info[SENSOR_TYPE_NUM] = {
    {"GESTURE", SENSOR_PATTERN_MATCHING_GESTURE},
    {"TAP", SENSOR_ABS_TAPPING},
    {"SIMPLEGES", SENSOR_ABS_SIMPLEGES},
    {"STEP", SENSOR_ABS_STEPCOUNTER},
    {"ACTIVITY", SENSOR_ABS_ACTIVITY},
    {"ACCEL", SENSOR_ACCELEROMETER},
    {"GYRO", SENSOR_GYROSCOPE},
    {"MAG", SENSOR_MAGNETOMETER},
    {"BARO", SENSOR_BAROMETER},
    {"TEMP", SENSOR_TEMPERATURE},
    {"HUM", SENSOR_HUMIDITY},
};

enum err_out_e {
    NO_ERR,
    ERR_OPEN_SVC,
    ERR_SVC_ID,
    ERR_RSP_STOPPING,
    ERR_SENSOR_EVENT,
    ERR_MSG_ID,
};

static void sensor_test_handle_message(struct cfw_message *p_msg, void *p_param)
{
    int err_out = NO_ERR;
    switch (CFW_MESSAGE_ID(p_msg))
    {
    case MSG_ID_SS_START_SENSOR_SCANNING_RSP:
        break;
    case MSG_ID_SS_START_SENSOR_SCANNING_EVT:
        {
            ss_sensor_scan_event_t *p_evt = (ss_sensor_scan_event_t *)p_msg;
            ss_on_board_scan_data_t on_board_data = p_evt->on_board_data;
            uint8_t sensor_type = p_evt->sensor_type;
            p_evt->handle = GET_SENSOR_HANDLE(sensor_type, on_board_data.ch_id);

            sensor_handles[sensor_type] = p_evt->handle;
        } break;
    case MSG_ID_SS_STOP_SENSOR_SCANNING_RSP:
        {
            ss_stop_scanning_rsp_t *p_rsp = (ss_stop_scanning_rsp_t *)p_msg;
            if (p_rsp->status != 0) {
                err_out = ERR_RSP_STOPPING;
                goto err_out;
            }
        } break;
    case MSG_ID_SS_SENSOR_SET_PROPERTY_RSP:
        SS_TCMD_LOG("set prop Rsp");
        break;
    case MSG_ID_SS_SENSOR_GET_PROPERTY_RSP:
        SS_TCMD_LOG("get prop Rsp");
        break;
    case MSG_ID_SS_SENSOR_GET_PROPERTY_EVT:
        SS_TCMD_LOG("Get prop evt");
        break;
    case MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_RSP:
        SS_TCMD_LOG("Sbc Rsp");
        break;
    case MSG_ID_SS_SENSOR_SUBSCRIBE_DATA_EVT:
        {
            ss_sensor_subscribe_data_event_t *p_evt =
                 (ss_sensor_subscribe_data_event_t *)p_msg;
            ss_sensor_data_header_t *p_data_header = &p_evt->sensor_data_header;
            uint8_t sensor_type = GET_SENSOR_TYPE(p_evt->handle);
            switch(sensor_type) {
               case SENSOR_ACCELEROMETER:
                {
                    int data_len = p_data_header->data_length;
                    int data_num = data_len / sizeof(struct accel_phy_data);
                    for(int i = 0 ; i < data_num ; i++){
                        struct accel_phy_data *p_data = (struct accel_phy_data *)p_data_header->data + i;
                        SS_TCMD_LOG("T(ms):%d,xyz(mg):%d,%d,%d",p_data_header->timestamp,(p_data->ax),(p_data->ay),(p_data->az));
                    }
                } break;
                case SENSOR_GYROSCOPE:
                {
                    struct gyro_phy_data *p_data = (struct gyro_phy_data *)p_data_header->data;
                    SS_TCMD_LOG("T(ms):%d,xyz(mg):%d,%d,%d",p_data_header->timestamp,(p_data->gx),(p_data->gy),(p_data->gz));
                } break;
                 case SENSOR_PATTERN_MATCHING_GESTURE:
                {
                    struct gs_personalize *p =(struct gs_personalize *)p_data_header->data;
                    SS_TCMD_LOG("%s=%d,size=%d",sensors_info[0].sensor_name,p->nClassLabel,p->size);
                } break;
                case SENSOR_ABS_TAPPING:
                {
                    struct tapping_result *p =(struct tapping_result *)p_data_header->data;
                    SS_TCMD_LOG("%s=%d",sensors_info[1].sensor_name, p->tapping_cnt);
                } break;
                case SENSOR_ABS_SIMPLEGES:
                {
                    struct simpleges_result *p =(struct simpleges_result *)p_data_header->data;
                    SS_TCMD_LOG("%s=%d",sensors_info[2].sensor_name, p->type);
                } break;
                case SENSOR_ABS_STEPCOUNTER:
                {
                   struct stepcounter_result *p =(struct stepcounter_result *)p_data_header->data;
                     SS_TCMD_LOG("%s=%d,activity:%d", sensors_info[3].sensor_name,p->steps, p->activity);
                } break;
                case SENSOR_ABS_ACTIVITY:
                {
                   struct activity_result *p =(struct activity_result *)p_data_header->data;
                    SS_TCMD_LOG("%s=%d",sensors_info[4].sensor_name, p->type);
                } break;
                case SENSOR_MAGNETOMETER:
                {
                   struct mag_phy_data *p_data = (struct mag_phy_data *)p_data_header->data;
                   SS_TCMD_LOG("T(ms):%d,xyz(uT):%d,%d,%d",p_data_header->timestamp, (p_data->mx),(p_data->my),(p_data->mz));
                } break;
               case SENSOR_BAROMETER:
                {
                   struct baro_phy_data *p_data = (struct baro_phy_data*)p_data_header->data;
                   SS_TCMD_LOG("%s=%d",sensors_info[8].sensor_name,(p_data->value));
                } break;
               case SENSOR_TEMPERATURE:
                {
                   struct temp_phy_data *p_data = (struct temp_phy_data*)p_data_header->data;
                   SS_TCMD_LOG("%s=%d",sensors_info[9].sensor_name,(p_data->value));
                } break;
               case SENSOR_HUMIDITY:
                {
                   struct humi_phy_data *p_data = (struct humi_phy_data*)p_data_header->data;
                   SS_TCMD_LOG("%s=%d",sensors_info[10].sensor_name,(p_data->value));
                } break;
                default:
                err_out = ERR_SENSOR_EVENT;
                break;
            }
        } break;
    case MSG_ID_SS_SENSOR_UNSUBSCRIBE_DATA_RSP:
        {
            SS_TCMD_LOG("Unsbc rsp");
        } break;
    case MSG_ID_SS_SENSOR_CALIBRATION_RSP:
        {
            ss_svc_message_general_rsp_t *rsp_msg = (ss_svc_message_general_rsp_t *)p_msg;
            SS_TCMD_LOG("Clb rsp status[%d]", rsp_msg->status);
        }break;
    case MSG_ID_SS_SENSOR_CALIBRATION_EVT:
        {
            ss_sensor_get_cal_data_evt_t* cal_msg= (ss_sensor_get_cal_data_evt_t*)p_msg;
            uint8_t sensor_type = GET_SENSOR_TYPE(cal_msg->handle);
            uint8_t dev_id= GET_SENSOR_ID(cal_msg->handle);
            uint8_t* ptr = cal_msg->value;
            switch(sensor_type) {
                case SENSOR_ACCELEROMETER:
                    SS_TCMD_LOG("Accel id[%d] offset:x/%d,y/%d,z/%d", dev_id,*((short*)ptr), *((short*)(ptr)+1), *((short*)(ptr)+2));
                    break;
                case SENSOR_GYROSCOPE:
                    SS_TCMD_LOG("Gyro id[%d] offst:x/%d,y/%d,z/%d", dev_id,*((int*)ptr), *((int*)(ptr)+1), *((int*)(ptr)+2));
                    break;
            }
        }break;
    default:
        err_out = ERR_MSG_ID;
        break;
    }
err_out:
    if(err_out)
        SS_TCMD_LOG("ERR_ID:%d",err_out);
    cfw_msg_free(p_msg);
}

#ifdef CONFIG_TCMD
static uint8_t get_clb_cmd(char* name)
{
    if (strncmp(name, "start", sizeof("start")) == 0 ) {
        return START_CALIBRATION_CMD;
    }
    if (strncmp(name, "set", sizeof("set")) == 0) {
        return SET_CALIBRATION_CMD;
    }
    if (strncmp(name, "stop", sizeof("stop")) == 0 ) {
        return STOP_CALIBRATION_CMD;
    }
    if (strncmp(name, "get", sizeof("get")) == 0 ) {
        return GET_CALIBRATION_CMD;
    }
    SS_TCMD_LOG("[Clb cmd]:start stop set get");
    return IGNORE_CALIBRATION_CMD;
}

static uint8_t get_clb_type(char* name)
{
   return 0;
}

static uint32_t get_sensor_type(char* sensor_name)
{
    int i = 0;
    for(i = 0; i< SENSOR_TYPE_NUM; i++){
        if (strncmp(sensor_name, sensors_info[i].sensor_name, strlen(sensors_info[i].sensor_name))== 0 ) {
            return sensors_info[i].sensor_type;
        }
    }
    SS_TCMD_LOG("[Sensor type]:");
    for(i =0; i < SENSOR_TYPE_NUM ; i++){
        if(sensors_info[i].sensor_name)
            SS_TCMD_LOG("%s",sensors_info[i].sensor_name);
    }
    return UNKNOWN_SENSOR;
}

static uint8_t get_data_type(char* data_type)
{
    return ACCEL_DATA;
}

void sscmd_scan(bool is_start,int argc, char **argv)
{
    int ss_num = 0;
    uint32_t sensor_type = 0;
    uint8_t sensor_type_tmp = UNKNOWN_SENSOR;
    for(ss_num=2; ss_num<argc; ss_num++)
    {
        sensor_type_tmp = get_sensor_type(argv[ss_num]);
        if(sensor_type_tmp == UNKNOWN_SENSOR)
            return;
        sensor_type |= (0x01 << sensor_type_tmp);
    }
    if(is_start)
        ss_start_sensor_scanning(p_ss_service_conn, NULL, sensor_type);
    else
        ss_stop_sensor_scanning(p_ss_service_conn, NULL,sensor_type);
}

#include "misc/printk.h"
void sscmd_property(bool is_set, int argc, char **argv)
{
    uint32_t sensor_type = get_sensor_type(argv[PROPERTY_SENSOR_TYPE_OFFSET]);
    if ( sensor_type == UNKNOWN_SENSOR ) {
        return;
    }

    if(!is_set){
        ss_sensor_get_property(p_ss_service_conn, NULL, sensor_handles[sensor_type]);
        return;
    }
    uint8_t value[24] = {0};
    uint8_t i = 0;
    uint8_t param_num = argc - PROPERTY_PARAM_OFFSET;
    uint8_t data_len = 0;
    for(i = PROPERTY_PARAM_OFFSET; i < argc; i++){
        value[i-PROPERTY_PARAM_OFFSET] = (uint8_t )strtol(argv[i], NULL, 10);
        printk("value[%d]=%d\n",i-PROPERTY_PARAM_OFFSET,value[i-PROPERTY_PARAM_OFFSET]);
    }
    data_len = param_num*sizeof(uint8_t);
    printk("num=%d,data_len=%d\n",param_num, data_len);
    ss_sensor_set_property(p_ss_service_conn, NULL, sensor_handles[sensor_type], data_len, value);
}

void sscmd_subscribe(bool is_unsub, int argc, char **argv)
{
    uint32_t sensor_type = get_sensor_type(argv[SUBSCRIBE_SENSOR_OFFSET]);
    if ( sensor_type == UNKNOWN_SENSOR ) {
        return;
    }
    uint8_t data_type =  get_data_type(argv[SUBSCRIBE_DATA_TYPE]);
    if ( data_type == UNKNOWN_DATA_TYPE ) {
        return;
    }
    if(is_unsub){
        ss_sensor_unsubscribe_data(p_ss_service_conn, NULL, sensor_handles[sensor_type], &data_type, 1);
        return;
    }
    uint16_t sampling_interval = strtol(argv[SUBSCRIBE_SAMPLE_INTR], NULL, 0);
    uint16_t reporting_interval = strtol(argv[SUBSCRIBE_REPORT_INTR], NULL, 0);
    ss_sensor_subscribe_data(p_ss_service_conn, NULL, sensor_handles[sensor_type],&data_type, 1, sampling_interval, reporting_interval);
}

void sscmd_calibration(int argc, char **argv)
{
    uint32_t sensor_type = get_sensor_type(argv[CALIBRATION_SENSOR_TYPE_OFFSET]);
    if ( sensor_type == UNKNOWN_SENSOR ) {
        return;
    }
    uint8_t clb_type = get_clb_type(argv[CALIBRATION_TYPE_OFFSET]);
    uint8_t clb_cmd = get_clb_cmd(argv[CALIBRATION_CMD_OFFSET]);

    if(clb_cmd == SET_CALIBRATION_CMD && argc <= CALIBRATION_PARAM_OFFSET){
        return;
    }
    char value[24] = {0};
    switch(clb_cmd){
       case SET_CALIBRATION_CMD:{
            uint8_t i = 0;
            uint8_t data_len = 0;
            if(sensor_type == SENSOR_ACCELEROMETER){
                for(i = CALIBRATION_PARAM_OFFSET; i < argc; i++){
                    *(((short*)value)+(i-CALIBRATION_PARAM_OFFSET)) = (short)strtol(argv[i], NULL, 0);
                }
                data_len = 3*sizeof(short);
            }else if(sensor_type == SENSOR_GYROSCOPE){
                for(i = CALIBRATION_PARAM_OFFSET; i < argc; i++){
                    *(((int*)value)+(i-CALIBRATION_PARAM_OFFSET)) = (int)strtol(argv[i], NULL, 0);
                }
                data_len = 3*sizeof(int);
            }
            ss_sensor_set_calibration(p_ss_service_conn, NULL, sensor_handles[sensor_type], clb_type, data_len, (uint8_t*)value);
            break;
        }default:
            ss_sensor_opt_calibration(p_ss_service_conn, NULL, sensor_handles[sensor_type], clb_cmd, clb_type);
            break;
    }
}

static int check_svc_handle(struct tcmd_handler_ctx *ctx)
{
    /*One way to create a sensor tcmd_app*/
    if(!p_ss_service_conn)
        p_ss_service_conn = cproxy_connect(SENSOR_SVC_ID, sensor_test_handle_message, NULL);
    if(!p_ss_service_conn){
        TCMD_RSP_ERROR(ctx, "Cproxy err");
        return 1;
    }
    return 0;
}
void clb_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx)
{
    if(!check_svc_handle(ctx))
        sscmd_calibration(argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}

void startsc_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_scan(true, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}
void stopsc_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_scan(false, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}

void sbc_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_subscribe(false, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}

void unsbc_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_subscribe(true, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}

void setprop_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_property(true, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}

void getprop_handle(int argc,char **argv, struct tcmd_handler_ctx *ctx){
    if(!check_svc_handle(ctx))
        sscmd_property(false, argc, argv);
    TCMD_RSP_FINAL(ctx, NULL);
}


/*Only DECLARE_TEST_COMMAND is compiled into curie_reference_release,DECLARE_TEST_COMMAND_ENG is invalid*/
DECLARE_TEST_COMMAND_ENG(ss, startsc, startsc_handle);
DECLARE_TEST_COMMAND_ENG(ss, stopsc, stopsc_handle);
DECLARE_TEST_COMMAND_ENG(ss, sbc, sbc_handle);
DECLARE_TEST_COMMAND_ENG(ss, unsbc, unsbc_handle);
DECLARE_TEST_COMMAND_ENG(ss, clb, clb_handle);
DECLARE_TEST_COMMAND_ENG(ss, setprop, setprop_handle);
DECLARE_TEST_COMMAND_ENG(ss, getprop, getprop_handle);
#endif
