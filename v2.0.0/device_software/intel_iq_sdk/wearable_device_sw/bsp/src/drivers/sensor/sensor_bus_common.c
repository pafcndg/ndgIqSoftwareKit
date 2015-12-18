/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential information
 * of Intel or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 *
 ******************************************************************************/
#include "util/compiler.h"
#include "machine.h"
#include "os/os.h"
#include "os/os_types.h"
#include "infra/log.h"
#include "infra/time.h"
#include "drivers/data_type.h"
#include "string.h"
#include "drivers/serial_bus_access.h"
#include "drivers/sensor/sensor_bus_common.h"

/************************* Use Serial Bus Access API *******************/

#define SENSOR_BUS_TIMEOUT 30000 //30s

static void sensor_bus_callback(struct sba_request *req)
{
    struct sensor_sba_req *sensor_req = (struct sensor_sba_req *)req;
    semaphore_give(sensor_req->sem, NULL);
}

static uint8_t get_sba_req(uint8_t *req_bitmap, uint8_t req_count)
{
    uint8_t i;
    uint32_t saved = interrupt_lock();
    for(i=0; i<req_count; i++) {
        if(!(*req_bitmap & (1<<i))){
            *req_bitmap |= (1<<i);
            break;
        }
    }
    interrupt_unlock(saved);
    return i;
}

static void release_sba_req(uint8_t *req_bitmap, uint8_t i)
{
    uint32_t saved = interrupt_lock();
    *req_bitmap &= ~(1<<i);
    interrupt_unlock(saved);
}

static inline void config_req_read(uint8_t *reg_addr, uint8_t* reg_data, uint32_t cnt, sba_request_t *req, SENSOR_BUS_TYPE bus_type)
{
    if(bus_type==SENSOR_BUS_TYPE_SPI)
        *reg_addr |= SPI_READ_CMD;

    req->request_type = SBA_TRANSFER;
    req->tx_buff = reg_addr;
    req->tx_len = 1;
    req->rx_buff = reg_data;
    req->rx_len = cnt;
    req->status = 1;
}

static inline void config_req_write(uint8_t *reg_addr, uint8_t* reg_data, uint32_t cnt, sba_request_t *req, uint8_t *buffer, SENSOR_BUS_TYPE bus_type)
{
    if(bus_type==SENSOR_BUS_TYPE_SPI) {
        int i;
        *reg_addr &= 0x7F;
        for(i=0; i<cnt; i++){
            buffer[i<<1]     = *reg_addr+i;
            buffer[(i<<1)+1] = reg_data[i];
        }
        req->tx_len          = (cnt<<1);
    }else{
        buffer[0]         = *reg_addr;
        memcpy(&buffer[1], reg_data, cnt);
        req->tx_len       = cnt+1;
    }
    req->request_type = SBA_TX;
    req->tx_buff         = buffer;
    req->rx_len          = 0;
    req->status          = 1;
}

DRIVER_API_RC sensor_bus_access(struct sensor_sba_info *info, uint8_t reg_addr, uint8_t * reg_data,
                                uint32_t cnt, bool req_read, uint8_t *write_buffer)
{
    int i;
    sba_request_t *req;
    DRIVER_API_RC ret = DRV_RC_OK;
    OS_ERR_TYPE err;
    if((i = get_sba_req(&info->bitmap, info->req_cnt)) >= info->req_cnt) {
        pr_debug(LOG_MODULE_DRV, "%s:DEV[%d] No req left", __func__, info->dev_id);
        return DRV_RC_FAIL;
    }
    req = &info->reqs[i].req;
    if(req_read)
        config_req_read(&reg_addr, reg_data, cnt, req, info->bus_type);
    else
        config_req_write(&reg_addr, reg_data, cnt, req, write_buffer, info->bus_type);

    if(sba_exec_request(req)) {
        pr_debug(LOG_MODULE_DRV, "%s:DEV[%d] request exec error", __func__, info->dev_id);
        release_sba_req(&info->bitmap, i);
        return DRV_RC_FAIL;
    }

    if((err = semaphore_take(info->reqs[i].sem, SENSOR_BUS_TIMEOUT))) {
        pr_debug(LOG_MODULE_DRV, "%s:DEV[%d] take semaphore err#%d", __func__, info->dev_id, err);
        return DRV_RC_FAIL;
    }

    if(req->status) {
        pr_error(LOG_MODULE_DRV, "%s:DEV[%d] state error", __func__,info->dev_id);
        ret = DRV_RC_FAIL;
    }
    release_sba_req(&info->bitmap, i);

    return ret;
}

struct sensor_sba_info * sensor_config_bus(uint32_t dev_addr, uint8_t dev_id, SBA_BUSID bus_id, SENSOR_BUS_TYPE bus_type, int req_num)
{

    struct sensor_sba_req *reqs = NULL;
    struct sensor_sba_info *info = NULL;
    int i;

    reqs = (struct sensor_sba_req *) balloc(sizeof(struct sensor_sba_req)*req_num, NULL);

    if(!reqs)
        return NULL;

    info = (struct sensor_sba_info *) balloc(sizeof(struct sensor_sba_info), NULL);

    if(!info)
        goto FAIL;

    info->reqs = reqs;
    info->bitmap = 0;
    info->req_cnt = req_num;
    info->bus_type = bus_type;

    for(i=0; i<req_num; i++){
        reqs[i].req.bus_id         = bus_id;
        reqs[i].req.status         = 1;
        reqs[i].req.callback       = sensor_bus_callback;
        reqs[i].req.full_duplex    = 0;
        reqs[i].sem                = semaphore_create(0, NULL);

        if(!reqs[i].sem)
            goto FAIL;

        if(bus_type==SENSOR_BUS_TYPE_I2C)
            reqs[i].req.addr.slave_addr = dev_addr;
        else
            reqs[i].req.addr.cs         = dev_addr;
    }

    pr_debug(LOG_MODULE_DRV, "%s: Serial BUS[%d] initialized", __func__, bus_id);

    return info;

FAIL:
    for(i=0; i<req_num; i++){
        if(reqs[i].sem){
            semaphore_delete(reqs[i].sem, NULL);
            reqs[i].sem = NULL;
        }
    }
    if(reqs)
        bfree(reqs);

    if(info)
        bfree(info);

    pr_debug(LOG_MODULE_DRV, "%s: Serial BUS[%d] init failed", __func__, bus_id);
    return NULL;
}

static void wait_ticks(uint32_t ticks)
{
    uint32_t start = get_uptime_32k();

    while((get_uptime_32k()-start) < ticks) ;
}

void sensor_delay_ms(uint32_t msecs)
{
    if(msecs >= MS_PER_NANO_SYS_TIMER_TICK) {
        T_SEMAPHORE sem = semaphore_create (0, NULL);
        semaphore_take(sem, (int)msecs);
        semaphore_delete(sem, NULL);
    }else {
        wait_ticks(msecs * AON_CNT_TICK_PER_MS);
    }
}
