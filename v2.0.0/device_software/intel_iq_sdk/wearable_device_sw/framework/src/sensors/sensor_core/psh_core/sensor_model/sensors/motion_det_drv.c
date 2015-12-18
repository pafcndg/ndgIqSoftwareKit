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

#ifdef CONFIG_PSH_CORE_PWM
static sensor_t real_sensor = NULL;
sensor_id_t sensor_list[1];
static int no_move = 0;

int motion_detect_test_callback (struct sensor_data * sensor_data, void *priv_data)
{
    uint8_t *event = sensor_data->data;
    char *str_event;
    if(*event==PHY_SENSOR_EVENT_DOUBLE_TAP){
        str_event = "double tap";
        no_move = 0;
    } else if(*event==PHY_SENSOR_EVENT_NO_MOTION) {
        str_event = "no motion";
        no_move = 1;
    } else if(*event==PHY_SENSOR_EVENT_ANY_MOTION){
        str_event = "any motion";
        no_move = 0;
    }else
        str_event = "unkown";
    pr_info(LOG_MODULE_PSH_CORE, "Event: [%s]", str_event);
    uint8_t sid = sensor_get_sid(MOVE_DETECT_NAME_STR);
    phy_sensor_report(sid);

    return 0;
}

static int motion_det_init(struct phy_driver *phy_drv){
    int ret;
    uint8_t  device_id;
    phy_sensor_type_t sensor_type;
    sensor_t my_sensor;

    ret = get_sensor_list((1 << SENSOR_MOTION_DETECTOR), sensor_list,1);
    if(ret <= 0){
        psh_err2(PSH_ITSELF,"NO really sensor_list was got\n");
        return 0;
    }

    device_id = sensor_list[0].dev_id;
    sensor_type = sensor_list[0].sensor_type;


    my_sensor = phy_sensor_open(sensor_type, device_id);
    real_sensor = my_sensor;
    if(my_sensor == NULL){
        psh_err2(PSH_ITSELF,"NO really sensor was found,open failed\n");
        return 0;
    }
    phy_sensor_enable(real_sensor, 1);
    phy_sensor_data_register_callback(real_sensor, motion_detect_test_callback, NULL, 0);

    return 0;
}

static int motion_det_config(struct phy_driver *phy_drv,struct phy_cfg_param *cfg){
    psh_info2(PSH_ITSELF,"Enable the move_:%d\n",cfg->freq);
    return 0;
}
static int motion_det_read_data(struct phy_driver *phy_drv,void *data_buf,u8 data_size){
    struct md_data *data = (struct md_data*) data_buf;
    data->state = no_move;
    if(!no_move)
        sensor_pwm_option(SENSOR_PWM_REQUEST);
    return sizeof(struct md_data);
}
#else
int motion_detect_test_callback (struct sensor_data * sensor_data, void *priv_data)
{
    return 0;
}

static int motion_det_init(struct phy_driver *phy_drv){
    return 0;
}

static int motion_det_config(struct phy_driver *phy_drv,struct phy_cfg_param *cfg){
    return 0;
}
static int motion_det_read_data(struct phy_driver *phy_drv,void *data_buf,u8 data_size){
    return 0;
}

#endif
static struct phy_sensor_funcs motion_det_phy_funcs={
    .init=motion_det_init,
    .config=motion_det_config,
    .read_data=motion_det_read_data,
};
define_drvops(motion_det,motion_det_phy_funcs);
