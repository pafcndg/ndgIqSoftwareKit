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

#include "drivers/sensor/bme280_support.h"

static struct bme280_t bme280_s;

static inline void bme280_default_config(void)
{
	struct bme280_t *p_bme280 = &bme280_s;
    p_bme280->osrs_t = BME280_OVERSAMPLING_SKIPPED; //BME280_OVERSAMPLING_2X
    p_bme280->osrs_p = BME280_OVERSAMPLING_SKIPPED; //BME280_OVERSAMPLING_16X;
    p_bme280->osrs_h = BME280_OVERSAMPLING_SKIPPED; //BME280_OVERSAMPLING_1X;

    p_bme280->filter = BME280_FILTERCOEFF_16;
    p_bme280->mode = BME280_SLEEP_MODE;
    p_bme280->t_sb = BME280_STANDBYTIME_20_MS;
}

DRIVER_API_RC bme280_set_workmode(struct phy_sensor_t * sensor)
{
    BME280_RETURN_FUNCTION_TYPE comres = SUCCESS;
    uint8_t cfg  = BME280_Zero_U8X;
    uint8_t ctrl_meas = BME280_Zero_U8X;
    uint8_t ctrl_hum  = BME280_Zero_U8X;
    uint8_t cfg_check = BME280_Zero_U8X;
    uint8_t ctrl_meas_check = BME280_Zero_U8X;
    uint8_t ctrl_hum_check = BME280_Zero_U8X;
    uint8_t mismatch = 0;

	struct bme280_t *p_bme280 = &bme280_s;

    /* Only support FORCED MODE in driver */
    if(p_bme280->mode == BME280_FORCED_MODE){
        pr_debug(LOG_MODULE_BME280, "Forced mode not supported!");
        return E_BME280_OUT_OF_RANGE;
    }

    comres += bme280_read_reg(BME280_CTRLHUM_REG, &ctrl_hum);
    comres += bme280_read_reg(BME280_CTRLMEAS_REG, &ctrl_meas);
    comres += bme280_read_reg(BME280_CONFIG_REG, &cfg);

    if(comres)
        return comres;

    //switch to sleep mode for config reg write
    if(BME280_GET_BITSLICE(ctrl_meas, BME280_CTRLMEAS_REG_MODE)!=BME280_SLEEP_MODE){
        bme280_set_softreset();
        sensor_delay_ms(3);
    }
    cfg = BME280_SET_BITSLICE(cfg, BME280_CONFIG_REG_TSB, p_bme280->t_sb);
    cfg = BME280_SET_BITSLICE(cfg, BME280_CONFIG_REG_FILTER, p_bme280->filter);
    comres += bme280_write_reg(BME280_CONFIG_REG, &cfg);



    ctrl_hum = BME280_SET_BITSLICE(ctrl_hum, BME280_CTRLHUM_REG_OSRSH, p_bme280->osrs_h);
    comres += bme280_write_reg(BME280_CTRLHUM_REG, &ctrl_hum);

    ctrl_meas = BME280_SET_BITSLICE(ctrl_meas, BME280_CTRLMEAS_REG_OSRST, p_bme280->osrs_t);
    ctrl_meas = BME280_SET_BITSLICE(ctrl_meas, BME280_CTRLMEAS_REG_OSRSP, p_bme280->osrs_p);
    ctrl_meas = BME280_SET_BITSLICE(ctrl_meas, BME280_CTRLMEAS_REG_MODE, p_bme280->mode);
    comres += bme280_write_reg(BME280_CTRLMEAS_REG, &ctrl_meas);

    bme280_read_reg(BME280_CTRLHUM_REG, &ctrl_hum_check);
    if(ctrl_hum_check!=ctrl_hum)
        mismatch |= (1<<0);

    bme280_read_reg(BME280_CTRLMEAS_REG, &ctrl_meas_check);
    if(ctrl_meas_check!=ctrl_meas)
        mismatch |= (1<<1);

    bme280_read_reg(BME280_CONFIG_REG, &cfg_check);
    if(cfg_check!=cfg)
        mismatch |= (1<<2);

    if(comres || mismatch){
        pr_debug(LOG_MODULE_BME280, "set wm mismatch=%x", mismatch);
        return DRV_RC_FAIL;
    }

    p_bme280->t_period_us = 1000;
    if(p_bme280->osrs_t)
        p_bme280->t_period_us += 2000*p_bme280->osrs_t;
    if(p_bme280->osrs_p)
        p_bme280->t_period_us += (2000*p_bme280->osrs_p+500);
    if(p_bme280->osrs_h)
        p_bme280->t_period_us += (2000*p_bme280->osrs_h+500);

    switch(p_bme280->t_sb){
        case 0:
            p_bme280->t_period_us += 500;
        break;
        case 6:
            p_bme280->t_period_us += 10000;
        break;
        case 7:
            p_bme280->t_period_us += 20000;
        break;
        default:
            p_bme280->t_period_us += (62500 << (p_bme280->t_sb-1));
    }

    sensor->odr_hz_x10 = (uint16_t)(10000000/p_bme280->t_period_us);

    return DRV_RC_OK;
}

DRIVER_API_RC bme280_sensor_open(struct phy_sensor_t * sensor)
{
    DRIVER_API_RC com_rslt = 0;
    static uint8_t bme280_initilized = 0;

    if (bme280_initilized)
        return DRV_RC_OK;


    com_rslt = __bme280_init(&bme280_s);
    bme280_default_config();
    bme280_set_workmode(sensor);

    if (!com_rslt)
        bme280_initilized = 1;

    return com_rslt;
}

struct bme280_t* bme280_get_ptr(void)
{
    return &bme280_s;
}