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
#include "drivers/sensor/bme280_support.h"

static int deactivate(struct phy_sensor_t * sensor);

struct bme280_sensor_drv_t {
    struct phy_sensor_t sensor;
    uint8_t bme280_type;
};

static int generic_open(struct phy_sensor_t *sensor)
{
    return bme280_sensor_open(sensor);
}

static void generic_close(struct phy_sensor_t *sensor)
{
    deactivate(sensor);
}


static int activate(struct phy_sensor_t * sensor)
{
    struct bme280_t *p_bme280 = bme280_get_ptr();

    if(sensor->type==SENSOR_BAROMETER){
        if(p_bme280->osrs_p==BME280_OVERSAMPLING_SKIPPED){
            p_bme280->osrs_p = BME280_OVERSAMPLING_16X;
            pr_debug(LOG_MODULE_BME280, "Use default osr[16x] for barometer");
        }
    }else if(sensor->type==SENSOR_TEMPERATURE){
        if(p_bme280->osrs_t==BME280_OVERSAMPLING_SKIPPED){
            p_bme280->osrs_t = BME280_OVERSAMPLING_2X;
            pr_debug(LOG_MODULE_BME280, "Use default osr[2x] for temperature");
        }
    }else if(sensor->type==SENSOR_HUMIDITY){
        if(p_bme280->osrs_h==BME280_OVERSAMPLING_SKIPPED){
            p_bme280->osrs_h = BME280_OVERSAMPLING_1X;
            pr_debug(LOG_MODULE_BME280, "Use default osr[1x] for humidity");
        }
    }else{
        pr_debug(LOG_MODULE_BME280, "Sensor type unsupport");
        return DRV_RC_INVALID_CONFIG;
    }

    p_bme280->mode = BME280_NORMAL_MODE;

    if(bme280_set_workmode(sensor))
        return DRV_RC_FAIL;

    return DRV_RC_OK;
}

static int deactivate(struct phy_sensor_t * sensor)
{
    struct bme280_t *p_bme280 = bme280_get_ptr();

    if(sensor->type==SENSOR_BAROMETER)
        p_bme280->osrs_p=BME280_OVERSAMPLING_SKIPPED;
    else if(sensor->type==SENSOR_TEMPERATURE)
        p_bme280->osrs_t=BME280_OVERSAMPLING_SKIPPED;
    else if(sensor->type==SENSOR_HUMIDITY)
        p_bme280->osrs_h=BME280_OVERSAMPLING_SKIPPED;
    else{
        pr_debug(LOG_MODULE_BME280, "Sensor type unsupport");
        return DRV_RC_INVALID_CONFIG;
    }

    if(!(p_bme280->osrs_p|p_bme280->osrs_t|p_bme280->osrs_h))
      p_bme280->mode = BME280_SLEEP_MODE;

    if(bme280_set_workmode(sensor))
        return DRV_RC_FAIL;

    return DRV_RC_OK;
}

static int generic_activate(struct phy_sensor_t * sensor, bool enable)
{
    if(enable)
        return activate(sensor);

    return deactivate(sensor);
}

static int generic_read_data(struct phy_sensor_t *sensor, uint8_t * buf, uint16_t buff_len)
{
    struct bme280_sensor_drv_t *bme280_sensor = (struct bme280_sensor_drv_t *)sensor;
    BME280_RETURN_FUNCTION_TYPE com_rslt = 0;

    if(bme280_sensor->bme280_type==BME280_SENSOR_TEMP)
        com_rslt = bme280_read_t((int32_t *)buf);
    else if(bme280_sensor->bme280_type==BME280_SENSOR_PRESS)
        com_rslt = bme280_read_p((uint32_t *)buf);
    else
        com_rslt = bme280_read_h((uint32_t *)buf);

    if(com_rslt)
        return 0;

    return sizeof(uint32_t);
}

static int generic_get_property(struct phy_sensor_t * sensor, uint8_t type, void *value)
{
    phy_sensor_range_property_t *property_range;
    int ret = DRV_RC_OK;

    switch(type){
    case SENSOR_PROP_SENSING_RANGE:
        property_range = (phy_sensor_range_property_t *)value;
        if(sensor->type==SENSOR_BAROMETER){
            //300 hPa ~ 1100 hPa
            property_range->low = 30000;
            property_range->high = 110000;
        }else if(sensor->type==SENSOR_HUMIDITY){
            //relative humidity: 0%~100%
            property_range->low = 0;
            property_range->high = 100;
        }else if(sensor->type==SENSOR_TEMPERATURE){
            //0~65 degrees Celsius
            property_range->low = 0;
            property_range->high = 65;
        }else
            ret = DRV_RC_INVALID_CONFIG;
    break;
    default:
        ret = DRV_RC_INVALID_OPERATION;
    }

    return ret;
}

static int generic_set_property(struct phy_sensor_t * sensor, uint8_t type, const void *value)
{
    uint8_t sub_type;
    struct bme280_t *p_bme280 = bme280_get_ptr();
    bme280_osr_property_t *osr;
    bme280_tsb_property_t *tsb;
    bme280_filter_property_t *filter;

    switch(type){
        case SENSOR_PROP_DRIVER_SPECIFIC:
            sub_type = *((uint8_t*)value);

            switch(sub_type) {
                case BME280_SUBPROP_OSR:
                    osr = (bme280_osr_property_t *)value;
                    if(osr->value > 5){
                        pr_debug(LOG_MODULE_BME280, "%s, tsb out of range", __func__);
                        return DRV_RC_INVALID_CONFIG;
                    }
                    if(sensor->type==SENSOR_BAROMETER)
                        p_bme280->osrs_p = osr->value;
                    else if(sensor->type==SENSOR_TEMPERATURE)
                        p_bme280->osrs_t = osr->value;
                    else if(sensor->type==SENSOR_HUMIDITY)
                        p_bme280->osrs_h = osr->value;
                    else{
                        pr_debug(LOG_MODULE_BME280, "%s: sensor type not supported", __func__);
                        return DRV_RC_INVALID_CONFIG;
                    }
                break;

                case BME280_SUBPROP_TSB:
                    tsb = (bme280_tsb_property_t *)value;
                    if(tsb->value > 7){
                        pr_debug(LOG_MODULE_BME280, "%s, tsb out of range", __func__);
                        return DRV_RC_INVALID_CONFIG;
                    }
                    p_bme280->t_sb = tsb->value;
                break;

                case BME280_SUBPROP_FILTER:
                    filter = (bme280_filter_property_t *)value;
                    if(filter->value > 4){
                        pr_debug(LOG_MODULE_BME280, "%s, filter out of range", __func__);
                        return DRV_RC_INVALID_CONFIG;
                    }
                    p_bme280->filter = filter->value;
                break;

                default:
                    pr_debug(LOG_MODULE_BME280, "property subtype#%d not support");
                    return DRV_RC_INVALID_CONFIG;
            }

        break;
        default:
            pr_debug(LOG_MODULE_BME280, "property type#%d not support");
            return DRV_RC_INVALID_CONFIG;
    }

    return 0;
}

struct bme280_sensor_drv_t bme280_temp = {
    .sensor = {
               .type = SENSOR_TEMPERATURE,
               .raw_data_len = sizeof(phy_temp_data_t),
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK,
               .api = {
                       .open = generic_open,
                       .close = generic_close,
                       .activate = generic_activate,
                       .read = generic_read_data,
                       .set_property = generic_set_property,
                       .get_property = generic_get_property,
                       },
               },
    .bme280_type = BME280_SENSOR_TEMP,
};

struct bme280_sensor_drv_t bme280_humi = {
    .sensor = {
               .type = SENSOR_HUMIDITY,
               .raw_data_len = sizeof(phy_humi_data_t),
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK,
               .api = {
                       .open = generic_open,
                       .close = generic_close,
                       .activate = generic_activate,
                       .read = generic_read_data,
                       .set_property = generic_set_property,
                       .get_property = generic_get_property,
                       },
               },
    .bme280_type = BME280_SENSOR_HUMID,
};

struct bme280_sensor_drv_t bme280_baro = {
    .sensor = {
               .type = SENSOR_BAROMETER,
               .raw_data_len = sizeof(phy_baro_data_t),
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK,
               .api = {
                       .open = generic_open,
                       .close = generic_close,
                       .activate = generic_activate,
                       .read = generic_read_data,
                       .set_property = generic_set_property,
                       .get_property = generic_get_property,
                       },
               },
    .bme280_type = BME280_SENSOR_PRESS,
};

int bme280_sensor_register(void)
{
    int ret = 0;
    ret =  sensor_register(&bme280_temp.sensor);
    ret += sensor_register(&bme280_humi.sensor);
    ret += sensor_register(&bme280_baro.sensor);
    return ret;
}
