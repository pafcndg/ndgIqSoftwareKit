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

#include "infra/device.h"
#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmi160_drv.h"

int bmi160_drv_generic_open(struct phy_sensor_t *sensor)
{
    struct bmi160_sensor_drv_t *bmi160_sensor = (struct bmi160_sensor_drv_t *)sensor;
    return bmi160_sensor_open(bmi160_sensor->bmi160_type);
}

void bmi160_drv_generic_close(struct phy_sensor_t *sensor)
{
    struct bmi160_sensor_drv_t *bmi160_sensor = (struct bmi160_sensor_drv_t *)sensor;
    bmi160_sensor_close(bmi160_sensor->bmi160_type);
}

int bmi160_drv_generic_set_odr(struct phy_sensor_t *sensor, uint16_t odr_hz_x10)
{
    struct bmi160_sensor_drv_t *bmi160_sensor = (struct bmi160_sensor_drv_t *)sensor;
    return bmi160_sensor_set_rate(odr_hz_x10, bmi160_sensor->bmi160_type);
}

int bmi160_drv_generic_query_odr(struct phy_sensor_t * sensor, uint16_t odr_target, uint16_t *odr_support)
{
    struct bmi160_sensor_drv_t *bmi160_sensor = (struct bmi160_sensor_drv_t *)sensor;
    return bmi160_sensor_query_rate(odr_target, odr_support, bmi160_sensor->bmi160_type);
}

int bmi160_drv_generic_enable_int_mode(struct phy_sensor_t *sensor,
                                   generic_read_data_cb callback, void *priv_data,
                                   uint16_t sample_rate_x10, bool enable)
{
    struct bmi160_sensor_drv_t *bmi160_sensor = (struct bmi160_sensor_drv_t *)sensor;
    if (enable) {
        if (sample_rate_x10 != bmi160_sensor->sensor.odr_hz_x10)
            if (bmi160_drv_generic_set_odr(sensor, sample_rate_x10))
                return DRV_RC_FAIL;
        return bmi160_register_read_cb(callback, priv_data, bmi160_sensor->bmi160_type);
    }else
        return bmi160_unregister_read_cb(bmi160_sensor->bmi160_type);
}

static int accel_enable_fifo(struct phy_sensor_t *sensor, uint8_t* buffer, uint16_t len,  bool enable)
{
    if (enable)
        return bmi160_fifo_enable(BMI160_SENSOR_ACCEL, buffer, len, BMI160_USER_FIFO_ACCEL_ENABLE__MSK);
    else
        return bmi160_fifo_disable(BMI160_SENSOR_ACCEL, ~BMI160_USER_FIFO_ACCEL_ENABLE__MSK);
}

static int accel_read_data(struct phy_sensor_t *sensor, uint8_t * buf,
                           uint16_t buff_len)
{
    return bmi160_accle_read_data(buf, buff_len);
}

static int accel_read_fifo(struct phy_sensor_t *sensor, uint8_t * buf,
                           uint16_t buff_len)
{
    return bmi160_sensor_read_fifo(BMI160_SENSOR_ACCEL, buf, buff_len);
}

int bmi160_drv_generic_set_property(struct phy_sensor_t * sensor, uint8_t type, const void *value)
{
    uint8_t sub_type;
    bmi160_avg_property_t *avg;
    phy_sensor_watermark_property_t *wm;
    struct bmi160_rt_t *p_bmi160_rt = bmi160_get_ptr();
    int ret = DRV_RC_OK;
    enum bmi160_sensor_type _type;

    if(sensor->type==SENSOR_ACCELEROMETER)
        _type = BMI160_SENSOR_ACCEL;
    else if(sensor->type==SENSOR_GYROSCOPE)
        _type = BMI160_SENSOR_GYRO;
#if BMI160_ENABLE_MAG
    else if(sensor->type==SENSOR_MAGNETOMETER)
        _type = BMI160_SENSOR_MAG;
#endif
    else
        return DRV_RC_INVALID_CONFIG;


    switch(type){
        case SENSOR_PROP_FIFO_WATERMARK:
            wm = (phy_sensor_watermark_property_t *)value;
            p_bmi160_rt->watermark_trigger_cnt[_type] = wm->count;
            p_bmi160_rt->wm_callback[_type] = wm->callback;
            p_bmi160_rt->wm_cb_priv_data[_type] = wm->priv_data;
            bmi160_update_fifo_watermark();
        break;
        case SENSOR_PROP_DRIVER_SPECIFIC:
            sub_type = *((uint8_t*)value);
            switch(sub_type) {
            case BMI160_SUBPROP_AVG:
                avg = (bmi160_avg_property_t *)value;
                if(sensor->type==SENSOR_ACCELEROMETER){
                    /* this value will be used next time accel enter into lowpower mode */
                    p_bmi160_rt->undersampling_avg = avg->avg_value;
                    pr_debug(LOG_MODULE_BMI160, "set avg to %d", (1<<p_bmi160_rt->undersampling_avg));
                }else
                    ret = DRV_RC_INVALID_OPERATION;
                break;
            default:
                pr_debug(LOG_MODULE_BMI160, "property subtype#%d not support");
                ret = DRV_RC_INVALID_OPERATION;
            }
        break;
        default:
            pr_debug(LOG_MODULE_BMI160, "property type#%d not support");
            ret = DRV_RC_INVALID_OPERATION;
    }
    return ret;
}

int bmi160_drv_generic_get_property(struct phy_sensor_t * sensor, uint8_t type, void *value)
{
    phy_sensor_range_property_t *property_range;
    struct bmi160_rt_t *p_bmi160_rt = bmi160_get_ptr();
    int ret = DRV_RC_OK;

    switch(type){
    case SENSOR_PROP_SENSING_RANGE:
        property_range = (phy_sensor_range_property_t *)value;
        if(sensor->type==SENSOR_ACCELEROMETER){
            property_range->high = p_bmi160_rt->range_native[BMI160_SENSOR_ACCEL];
            property_range->low = - property_range->high;
        }
        else if(sensor->type==SENSOR_GYROSCOPE){
            property_range->high = p_bmi160_rt->range_native[BMI160_SENSOR_GYRO];
            property_range->low = - property_range->high;
        }
#if BMI160_ENABLE_MAG
        else if(sensor->type==SENSOR_MAGNETOMETER){
            property_range->high = 1300;
            property_range->low = -1300;
        }
#endif
        else
            ret = DRV_RC_INVALID_CONFIG;
    break;
    default:
        pr_debug(LOG_MODULE_BMI160, "property type#%d not support");
        ret = DRV_RC_INVALID_OPERATION;
    }

    return ret;
}

struct bmi160_sensor_drv_t bmi160_accel = {
    .sensor = {
               .type = SENSOR_ACCELEROMETER,
               .raw_data_len = sizeof(phy_accel_data_t),
               .hw_fifo_len = 1024,
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK  |
                                   PHY_SENSOR_REPORT_MODE_INT_REG_MASK   |
                                   PHY_SENSOR_REPORT_MODE_POLL_FIFO_MASK |
                                   PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK,
               .api = {
                       .open = bmi160_drv_generic_open,
                       .close = bmi160_drv_generic_close,
                       .query_odr = bmi160_drv_generic_query_odr,
                       .set_odr = bmi160_drv_generic_set_odr,
                       .enable_int_mode = bmi160_drv_generic_enable_int_mode,
                       .enable_fifo = accel_enable_fifo,
                       .read = accel_read_data,
                       .fifo_read = accel_read_fifo,
                       .set_property = bmi160_drv_generic_set_property,
                       .get_property = bmi160_drv_generic_get_property,
                       },
               },
    .bmi160_type = BMI160_SENSOR_ACCEL,
};

static int gyro_enable_fifo(struct phy_sensor_t *sensor, uint8_t* buffer, uint16_t len,
                            bool enable)
{
    if (enable)
        return bmi160_fifo_enable(BMI160_SENSOR_GYRO, buffer, len, BMI160_USER_FIFO_GYRO_ENABLE__MSK);
    else
        return bmi160_fifo_disable(BMI160_SENSOR_GYRO, (uint8_t)(~BMI160_USER_FIFO_GYRO_ENABLE__MSK));
}

static int gyro_read_data(struct phy_sensor_t *sensor, uint8_t * buf,
                          uint16_t buff_len)
{
    return bmi160_gyro_read_data(buf, buff_len);
}

static int gyro_read_fifo(struct phy_sensor_t *sensor, uint8_t * buf,
                           uint16_t buff_len)
{
    return bmi160_sensor_read_fifo(BMI160_SENSOR_GYRO, buf, buff_len);
}

struct bmi160_sensor_drv_t bmi160_gyro = {
    .sensor = {
               .type = SENSOR_GYROSCOPE,
               .raw_data_len = sizeof(phy_gyro_data_t),
               .hw_fifo_len = 1024,
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK  |
                                   PHY_SENSOR_REPORT_MODE_INT_REG_MASK   |
                                   PHY_SENSOR_REPORT_MODE_POLL_FIFO_MASK |
                                   PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK,
               .api = {
                       .open = bmi160_drv_generic_open,
                       .close = bmi160_drv_generic_close,
                       .query_odr = bmi160_drv_generic_query_odr,
                       .set_odr = bmi160_drv_generic_set_odr,
                       .enable_int_mode = bmi160_drv_generic_enable_int_mode,
                       .enable_fifo = gyro_enable_fifo,
                       .fifo_read = gyro_read_fifo,
                       .read = gyro_read_data,
                       .set_property = bmi160_drv_generic_set_property,
                       .get_property = bmi160_drv_generic_get_property,
                       },
               },
    .bmi160_type = BMI160_SENSOR_GYRO,
};

#ifdef CONFIG_BMI160_PM
static int motion_enable_int_mode(struct phy_sensor_t *sensor,
                                   generic_read_data_cb callback, void *priv_data,
                                   uint16_t sample_rate_x10, bool enable)
{
    int ret = 0;
    if (enable) {
        if(bmi160_accel.sensor.odr_hz_x10==0)
            ret = bmi160_drv_generic_set_odr(&bmi160_accel.sensor,
                            BMI160_MOTION_DETECTION_DEFAULT_ODR_HZ*10);
        ret += bmi160_register_motion_cb(callback, priv_data);
    }else{
        ret = bmi160_unregister_motion_cb();
        if(bmi160_accel.sensor.odr_hz_x10==0)
            ret += bmi160_drv_generic_set_odr(&bmi160_accel.sensor, 0);
    }

    return ret;
}

/* this driver using bmi160 accelerometer sensor for motion detection
 * the motion detection is for power management of bmi160
 */
struct bmi160_sensor_drv_t bmi160_motion = {
    .sensor = {
               .type = SENSOR_MOTION_DETECTOR,
               .raw_data_len = sizeof(phy_event_data_t),
               .hw_fifo_len = 0,
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_INT_REG_MASK,
               .api = {
                       .open = bmi160_drv_generic_open,
                       .close = bmi160_drv_generic_close,
                       .enable_int_mode = motion_enable_int_mode,
        },
    },
    .bmi160_type = BMI160_SENSOR_MOTION,
};
#endif

int bmi160_sensor_register(void)
{
    int ret;
    ret = sensor_register(&bmi160_accel.sensor);
    ret += sensor_register(&bmi160_gyro.sensor);
#ifdef CONFIG_BMI160_PM
    ret += sensor_register(&bmi160_motion.sensor);
#endif
    return ret;
}
