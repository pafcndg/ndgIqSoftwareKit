/*
Copyright (c) 2015, Intel Corporation. All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions are met:
*
*1. Redistributions of source code must retain the above copyright notice,
*this list of conditions and the following disclaimer.
*
*2. Redistributions in binary form must reproduce the above copyright notice,
*this list of conditions and the following disclaimer in the documentation
*and/or other materials provided with the distribution.
*
*3. Neither the name of the copyright holder nor the names of its contributors
*may be used to endorse or promote products derived from this software without
*specific prior written permission.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS'
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*POSSIBILITY OF SUCH DAMAGE.
*/
/* Utility API */
#include "services/sensor_service/sensor_svc.h"
#include "services/sensor_service/sensor_svc_utils.h"
#include "services/sensor_service/sensor_svc_calibration.h"
/* Properties Service will allow to read/store sensor data to flash */
#include "services/properties_service/properties_service_api.h"

/*Flash size and address*/
static short accel_clb_flash[3] = {0,0,0};
static int gyro_clb_flash[3] = {0, 0, 0};

/*Flag for doing erase first*/
#define ERASE_FLASH_FIRST 0
/*Flag for turning read flash*/
static uint8_t ready_to_read_flag = 0;
/*Flag to start first props_clb_dev number*/
static uint8_t prop_id_index = 0;

/*
 * nonpersist_service_id = 1 ,false, false
 * */
#define PERSIST_SERVICE_ID 2

/*Sensor_clb_read_flash call back */
typedef void (*props_read_cb)(uint8_t, uint8_t ,void *, uint16_t );

#define MAX_DEV_NUM  (MAX_CLB_PROP_ID-BASE_CLB_PROP_ID-1)
/*Gloable struct to store everything*/
struct props_clb_dev_s
{
    uint8_t prop_id;
    uint8_t sensor_type;
    uint8_t dev_id;
    void *data_addr;
    uint16_t data_len;
    props_read_cb read_back_func;     /*This func will be defined by users,but it excute in this file*/
    uint8_t exc_flag;
    uint8_t sist_service_id;
}props_clb_dev[MAX_DEV_NUM];

static cfw_service_conn_t *props_service_conn = NULL;
static cfw_client_t * props_client;

static uint8_t auto_set_clb_to_sensor_core(uint8_t sensor_type, uint8_t dev_id, uint8_t clb_type, void *data, uint16_t data_len);

static void props_read_flash_cb(uint8_t sensor_type, uint8_t dev_id, void *data_addr, uint16_t len)
{
    auto_set_clb_to_sensor_core(sensor_type, dev_id, 0, data_addr, len);
}

/*props service call back*/
static void props_handle_connect_cb(cfw_service_conn_t *conn, void *param)
{
    int i = 0;
    props_service_conn = conn;
#if defined(ERASE_FLASH_FIRST) && (ERASE_FLASH_FIRST== 1)
    for(i = 0; i < MAX_DEV_NUM; i++)
        sensor_clb_clean_flash(props_clb_dev[i].sensor_type,props_clb_dev[i].dev_id);
#else
    for(i = 0 ;i < MAX_DEV_NUM; i++)
        sensor_clb_add_flash(props_clb_dev[i].sensor_type,props_clb_dev[i].dev_id);
#endif
}

static void props_handle_msg(struct cfw_message *msg, void *data)
{
    int i = 0;
    switch (CFW_MESSAGE_ID(msg)) {
       case MSG_ID_PROP_SERVICE_ADD_PROP_RSP:
           {
               int add_prop =((add_property_rsp_msg_t *)msg)->rsp_header.status;
               if(!ready_to_read_flag){
                   for(i = 0; i < MAX_DEV_NUM; i++){
                       if(i == (MAX_DEV_NUM - 1)) ready_to_read_flag = 1;
                       if(props_clb_dev[i].exc_flag == EXC_TODO_ADD){
                           /*clear the current prop_id status*/
                           props_clb_dev[i].exc_flag = EXC_DONE_ADD;
                           /*DRV_RC_INVALID_OPERATION means that prop_id had been added*/
                           if ( (add_prop != DRV_RC_OK) && (add_prop != DRV_RC_INVALID_OPERATION) ) {
                               pr_info(LOG_MODULE_SS_SVC,"Rsp_prop[%d] add[%d]", props_clb_dev[i].prop_id, add_prop);
                           }
                           break;/*Just do one time*/
                       }
                   }
               }
               /*Make sure that the props_add were done, and next step is read first.*/
               if(ready_to_read_flag){
                   for(i = 0; i < MAX_DEV_NUM; i++){
                       if(props_clb_dev[i].exc_flag == EXC_DONE_ADD){
                           sensor_clb_read_flash(props_clb_dev[i].sensor_type, props_clb_dev[i].dev_id, props_read_flash_cb);
                       }
                   }
               }
               break;
           }
        case MSG_ID_PROP_SERVICE_READ_PROP_RSP:
            {
                int read_prop =((read_property_rsp_msg_t *)msg)->rsp_header.status;
                for(i = 0; i < MAX_DEV_NUM; i++){
                    if(props_clb_dev[i].exc_flag == EXC_TODO_READ){
                        props_clb_dev[i].exc_flag = EXC_DONE_READ;
                        if (read_prop == DRV_RC_OK) {
                            int read_size =((read_property_rsp_msg_t *)msg)->property_size;
                            uint8_t *read_data = (uint8_t*)&((read_property_rsp_msg_t *)msg)->start_of_values;
                            props_clb_dev[i].read_back_func(props_clb_dev[i].sensor_type, props_clb_dev[i].dev_id, read_data,read_size);
                        }else{
                            pr_debug(LOG_MODULE_SS_SVC,"Rsp_prop[%d] rd[%d]",props_clb_dev[i].prop_id, read_prop);
                        }
                        break;/*when it find one result,break*/
                    }
                }
            }
            break;
        case MSG_ID_PROP_SERVICE_WRITE_PROP_RSP:
            {
                int wr_prop =((write_property_rsp_msg_t *)msg)->rsp_header.status;
                for(i = 0; i < MAX_DEV_NUM; i++){
                    if(props_clb_dev[i].exc_flag == EXC_TODO_WRITE){
                        props_clb_dev[i].exc_flag = EXC_DONE_WRITE;
                        if (wr_prop != DRV_RC_OK) {
                            pr_error(LOG_MODULE_SS_SVC,"Rsp_prop[%d] wr[%d]",props_clb_dev[i].prop_id,wr_prop);
                        }
                        break;/*when it find one result,break*/
                    }
                }
                break;
            }
        case MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP:
            {
                int rm_prop =((remove_property_rsp_msg_t *)msg)->rsp_header.status;
                for(i = 0; i < MAX_DEV_NUM; i++){
                    if(props_clb_dev[i].exc_flag == EXC_TODO_CLEAN){
                        props_clb_dev[i].exc_flag = EXC_DONE_CLEAN;
                        if(rm_prop != DRV_RC_OK){
                            pr_debug(LOG_MODULE_SS_SVC,"Rsp_prop[%d] rm%d ",props_clb_dev[i].prop_id, rm_prop);
                        }
                    }
#if defined(ERASE_FLASH_FIRST) && (ERASE_FLASH_FIRST== 1)
                    /*When clean the prop_id. we need to create a new one*/
                    if( i == MAX_DEV_NUM - 1 ){
                        for(i = 0 ;i < MAX_DEV_NUM; i++)
                            sensor_clb_add_flash(prop_clb_dev[i].sensor_type, props_clb_dev[i].dev_id);
                    }
#endif
                }
                break;
            }
        default:
            break;
    }
    cfw_msg_free(msg);
}

static uint8_t find_clb_dev(uint8_t sensor_type, uint8_t dev_id)
{
    uint8_t index = 0;
    while(index < MAX_DEV_NUM){
       if(props_clb_dev[index].sensor_type == sensor_type){
           if( props_clb_dev[index].dev_id == dev_id )
               return index;
       }else
           index++;
    }
    return MAX_DEV_NUM;
}

void sensor_clb_write_flash(uint8_t sensor_type, uint8_t dev_id, void *data, uint32_t len)
{
    uint8_t index = find_clb_dev(sensor_type, dev_id);
    if(!props_service_conn || (index == MAX_DEV_NUM)) return;

    props_clb_dev[index].exc_flag = EXC_TODO_WRITE;
    properties_service_write(props_service_conn, props_clb_dev[index].sist_service_id,
            props_clb_dev[index].prop_id, data, len, NULL);
}

void sensor_clb_read_flash(uint8_t sensor_type, uint8_t dev_id, void (*cb)(uint8_t, uint8_t, void *, uint16_t ))
{
    uint8_t index = find_clb_dev(sensor_type,dev_id);
    if(!props_service_conn || (index == MAX_DEV_NUM)) return;

    props_clb_dev[index].exc_flag = EXC_TODO_READ;
    //register the callback
    props_clb_dev[index].read_back_func= cb;
    properties_service_read(props_service_conn, props_clb_dev[index].sist_service_id, props_clb_dev[index].prop_id, NULL);
}

void sensor_clb_clean_flash(uint8_t sensor_type, uint8_t dev_id)
{
    uint8_t index = find_clb_dev(sensor_type,dev_id);
    if(!props_service_conn || (index == MAX_DEV_NUM)) return;

    props_clb_dev[index].exc_flag = EXC_TODO_CLEAN;
    properties_service_remove(props_service_conn, props_clb_dev[index].sist_service_id, props_clb_dev[index].prop_id, NULL);
}

void sensor_clb_add_flash(uint8_t sensor_type, uint8_t dev_id)
{
    uint8_t index = find_clb_dev(sensor_type, dev_id);
    if(!props_service_conn || (index == MAX_DEV_NUM)) return;

    props_clb_dev[index].exc_flag = EXC_TODO_ADD;
    properties_service_add(props_service_conn, props_clb_dev[index].sist_service_id,
            props_clb_dev[index].prop_id, true, (uint8_t *)props_clb_dev[index].data_addr, props_clb_dev[index].data_len, NULL);
}

/*why not we excute the properties_service_add in this func,
 * beacuse we canot make sure that the props service will be init when the sencond continuous invoking*/
void sensor_clb_attr_init( enum calibration_prop_id prop_id, uint8_t sensor_type, uint8_t dev_id, void *data, uint16_t len)
{
    if(prop_id_index < MAX_DEV_NUM){
        props_clb_dev[prop_id_index].prop_id = prop_id;
        props_clb_dev[prop_id_index].sensor_type= sensor_type;
        props_clb_dev[prop_id_index].dev_id= dev_id;
        props_clb_dev[prop_id_index].data_addr= data;
        props_clb_dev[prop_id_index].data_len= len;
        props_clb_dev[prop_id_index].sist_service_id= PERSIST_SERVICE_ID;
        props_clb_dev[prop_id_index].exc_flag = EXC_IDLE;
    }
    /*For next prop_id*/
    prop_id_index ++;
}

static uint8_t auto_set_clb_to_sensor_core( uint8_t sensor_type,
                                            uint8_t dev_id,
                                            uint8_t clb_type,
                                            void *data,
                                            uint16_t data_len)
{
    ss_sensor_calibration_req_t *p_msg;

    /* Allocate sensor setting calibration request message */
    p_msg = (ss_sensor_calibration_req_t *) balloc(sizeof(*p_msg)+data_len,NULL);
    if (p_msg == NULL) {
        return -1;
    }
    p_msg->sensor = GET_SENSOR_HANDLE(sensor_type, dev_id);
    p_msg->calibration_type = clb_type;
    p_msg->clb_cmd = REBOOT_AUTO_CALIBRATION_CMD;
    p_msg->data_length= data_len;
    /* Fill cal param*/
    memcpy(p_msg->value, data, data_len);
    svc_send_calibration_cmd_to_core(p_msg);
    bfree(p_msg);
    return 0;
}

void sensor_svc_clb_init(T_QUEUE queue)
{
    if(!props_service_conn){
        props_client = cfw_client_init(queue, props_handle_msg, NULL);
        /* Open properties service when available */
        cfw_open_service_helper(props_client, PROPERTIES_SERVICE_ID,
                props_handle_connect_cb, NULL);
    }
/*we need to know the dev_id and sensor_type from sensor_core*/
    sensor_clb_attr_init(ACCEL_CLB_PROP_ID, SENSOR_ACCELEROMETER, 1, accel_clb_flash, 3*(sizeof(short)));
    sensor_clb_attr_init(GYRO_CLB_PROP_ID,  SENSOR_GYROSCOPE, 2, gyro_clb_flash, 3*(sizeof(int)));
}
