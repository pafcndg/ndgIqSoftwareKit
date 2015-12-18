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

#include "options.h"
#include "sensor_core.h"
#include "runtime.h"
#include "sensor_bit_cfg.h"
#include "sensors/phy_sensor_api/phy_sensor_api.h"
#include "plat_sensor.h"
#include "runtime.h"

static bool fifo_on = false;
extern struct sensor_funcs async_phy_funcs;
extern int sim_connected;

static sensor_t real_sensor = NULL;
sensor_id_t sensor_list[1];

static int accel_phy_init(struct phy_driver *phy_drv){
    int ret;
    uint8_t  device_id;
    phy_sensor_type_t sensor_type;
    sensor_t my_sensor;
    psh_log("Init \n");

    ret = get_sensor_list((1 << SENSOR_ACCELEROMETER), sensor_list,1);
    if(ret <= 0){
        psh_err2(PSH_ITSELF,"NO really sensor_list was got\n");
        goto err;
    }

    device_id = sensor_list[0].dev_id;
    sensor_type = sensor_list[0].sensor_type;

    my_sensor = phy_sensor_open(sensor_type, device_id);
    real_sensor = my_sensor;
    if(my_sensor == NULL){
        psh_err2(PSH_ITSELF,"NO really sensor was found,open failed\n");
        goto err;
    }

    return 0;

err:
    return E_HW;
}

static u16 accel_freq_query(struct phy_driver *phy_drv, u16 freq_to)
{
    u16 freq_supported = 0;

    if(phy_sensor_query_odr_value(real_sensor, freq_to*10, &freq_supported))
        return 0;

    return freq_supported/10;
}

static int accel_phy_config(struct phy_driver *phy_drv,struct phy_cfg_param *cfg){
    int ret = 1;
    u16 freq_supported = 0;

    if(real_sensor==NULL)
        goto err;

    if (cfg->bit_cfg & SNR_BIT_CFG_FIFO_ON){
        phy_sensor_enable_hwfifo(real_sensor, 1, 1000);
        psh_info2(PSH_ITSELF,"Enable fifo mode");
        fifo_on = true;
    }else{
        psh_info2(PSH_ITSELF,"Disable fifo mode");
        fifo_on = false;
    }

    if(cfg->freq != FREQ_ASYNC){
        if(phy_sensor_query_odr_value(real_sensor, cfg->freq*10, &freq_supported))
            goto err;
        cfg->freq = freq_supported/10;
    }
    else if(phy_sensor_query_odr_value(real_sensor, 1000, &freq_supported))
        goto err;

    if(phy_sensor_set_odr_value(real_sensor, freq_supported))
        goto err;

    if(phy_sensor_enable(real_sensor, 1)){
        psh_info2(PSH_ITSELF,"accel enable sensor polling mode ERR\n");
        goto err;
    }

#ifdef CONFIG_BMI160
    bmi160_avg_property_t avg;
    avg.sub_type = BMI160_SUBPROP_AVG;
    avg.avg_value = CONFIG_BMI160_LOWPOWER_AVG;
    ret = phy_sensor_set_property(real_sensor, SENSOR_PROP_DRIVER_SPECIFIC, (void*)&avg);
#endif

    if(ret!=0)
        goto err;

    return 0;
err:
    psh_err2(PSH_ITSELF,"Config err,it will result in reading data err\n");
    return E_HW;
}

static int accel_phy_read_data(struct phy_driver *phy_drv,void *data_buf,u8 data_size){
    uint16_t data_len =(uint16_t) data_size;
    char buff[sizeof(struct sensor_data)+sizeof(phy_accel_data_t)];
    struct sensor_data *sdata = (struct sensor_data*) buff;
    sdata->data_length= sizeof(phy_accel_data_t);

    if(real_sensor==NULL)
        goto err;

   if(fifo_on){
       data_len = phy_sensor_fifo_read(real_sensor, (uint8_t*)data_buf, data_len);
       if( data_len <= 0)
           goto err;
   }else{
        static int count = 0;
        struct accel_phy_data* data = (struct accel_phy_data*)data_buf;
        data_len = phy_sensor_data_read(real_sensor,sdata);
        if(data_len <= 0)
            goto err;

        phy_accel_data_t *accel_data = (phy_accel_data_t *)sdata->data;
        data->ax = accel_data->x;
        data->ay = accel_data->y;
        data->az = accel_data->z;
        while((++count)%1000 == 0){
            psh_log("accel read data,x=%d,y=%d,z=%d",data->ax,data->ay,data->az);
            count = 0;
        }
   }
   return data_len;
err:
    psh_err2(PSH_ITSELF,"accel read data err");
    return 0;
}

static struct phy_sensor_funcs accel_phy_phy_funcs={
    .init=accel_phy_init,
    .config=accel_phy_config,
    .read_data=accel_phy_read_data,
    .freq_query=accel_freq_query,
};
define_drvops(accel_phy,accel_phy_phy_funcs);
