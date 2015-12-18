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

#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmi160_bus.h"
#include "drivers/sensor/bmm150_support.h"

static struct bmi160_rt_t *p_bmi160_rt;

static struct trim_data_t mag_trim;

static uint8_t bmm150_parse_mag_s32_xyz_data(uint8_t *out_buf, uint8_t *out_index, uint16_t *fifo_index);
static DRIVER_API_RC bmm150_change_powermode(uint8_t power_mode);

DRIVER_API_RC bmm150_bus_access_manual(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt, uint8_t read)
{
    /* check if mag interface is in setup mode */
    if (!p_bmi160_rt->mag_manual_enable) {
        if(bmi160_set_mag_manual_enable(1, BMM150_MAG_READ_BURST_1))
          return DRV_RC_FAIL;
    }

    if(read){
        bmi160_write_reg(BMI160_USER_MAG_IF_2_ADDR, &reg_addr);
        bmi160_delay_ms(1);
        bmi160_bus_read(BMI160_USER_DATA_0_ADDR, reg_data, cnt);

    }else {
        bmi160_write_reg(BMI160_USER_MAG_IF_4_ADDR, reg_data);
        bmi160_write_reg(BMI160_USER_MAG_IF_3_ADDR, &reg_addr);
        bmi160_delay_ms(1);
    }

    return DRV_RC_OK;
}

static DRIVER_API_RC bmm150_read_trim(void)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t a_data[2] = {0};

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_X1, (uint8_t *)&mag_trim.dig_x1);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Y1, (uint8_t *)&mag_trim.dig_y1);

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_X2, (uint8_t *)&mag_trim.dig_x2);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Y2, (uint8_t *)&mag_trim.dig_y2);

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_XY1, (uint8_t *)&mag_trim.dig_xy1);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_XY2, (uint8_t *)&mag_trim.dig_xy2);

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z1_LSB, &a_data[0]);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z1_MSB, &a_data[1]);
    mag_trim.dig_z1 = (uint16_t)((((uint32_t)(a_data[1]))<< 8) | (a_data[0]));

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z2_LSB, &a_data[0]);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z2_MSB, &a_data[1]);
    mag_trim.dig_z2 = (int16_t)((((int32_t)((int8_t)a_data[1]))<< 8) | (a_data[0]));

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z3_LSB, &a_data[0]);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z3_MSB, &a_data[1]);
    mag_trim.dig_z3 = (int16_t)((((int32_t)((int8_t)a_data[1]))<< 8) | (a_data[0]));

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z4_LSB, &a_data[0]);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_Z4_MSB, &a_data[1]);
    mag_trim.dig_z4 = (int16_t)((((int32_t)((int8_t)a_data[1]))<< 8) | (a_data[0]));

    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_XYZ1_LSB, &a_data[0]);
    com_rslt += bmm150_read_reg(BMM150_MAG_DIG_XYZ1_MSB, &a_data[1]);
    mag_trim.dig_xyz1 = (uint16_t)((((uint32_t)(a_data[1]))<< 8) | (a_data[0]));

    return com_rslt;
}

/* power_mode:
 *              0 --> suspend
 *              1 --> sleep
 */
static DRIVER_API_RC bmm150_mag_power_control(uint8_t power_on, uint8_t soft_reset)
{
    uint8_t value;
    DRIVER_API_RC com_rslt = DRV_RC_OK;

    value = power_on;
    if(soft_reset)
        value |= BMI150_SOFTRESET_MASK;
    com_rslt += bmm150_write_reg(BMM150_POWER_CONTROL, &value);
    bmi160_delay_ms(5);
    return com_rslt;
}

static DRIVER_API_RC bmm150_mag_control(uint8_t op_mode, uint8_t data_rate, uint8_t selftest, uint8_t adv_selftest)
{
    uint8_t value;
    value = (adv_selftest<<6) | (data_rate<<3) | (op_mode<<1) | selftest;
    return bmm150_write_reg(BMM150_CONTROL, &value);
}


static DRIVER_API_RC bmm150_set_mag_presetmode(uint8_t presetmode)
{
    uint8_t rep_xy, rep_z;
    //uint8_t data_rate;
    DRIVER_API_RC com_rslt;
    switch (presetmode) {
    case BMM150_PRESETMODE_LOWPOWER:
        /* write the XY and Z repetitions*/
        rep_xy = BMM150_MAG_LOWPOWER_REPXY;
        rep_z = BMM150_MAG_LOWPOWER_REPZ;
        //data_rate = BMM150_MAG_LOWPOWER_DR;
        break;
    case BMM150_PRESETMODE_REGULAR:
        rep_xy = BMM150_MAG_REGULAR_REPXY;
        rep_z = BMM150_MAG_REGULAR_REPZ;
        //data_rate = BMM150_MAG_REGULAR_DR;
        break;
    case BMM150_PRESETMODE_HIGHACCURACY:
        rep_xy = BMM150_MAG_HIGHACCURACY_REPXY;
        rep_z = BMM150_MAG_HIGHACCURACY_REPZ;
        //data_rate = BMM150_MAG_HIGHACCURACY_DR;
    case BMM150_PRESETMODE_ENHANCED:
        rep_xy = BMM150_MAG_ENHANCED_REPXY;
        rep_z = BMM150_MAG_ENHANCED_REPZ;
        //data_rate = BMM150_MAG_ENHANCED_DR;
    default:
        return DRV_RC_FAIL;
    }

    com_rslt = bmm150_write_reg(BMM150_REP_XY, &rep_xy);
    com_rslt += bmm150_write_reg(BMM150_REP_Z, &rep_z);
    //com_rslt += bmm150_write_reg(BMM150_CONTROL, &data_rate);
    return com_rslt;
}

DRIVER_API_RC bmm150_sensor_open(void)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t value = 0;

    p_bmi160_rt = bmi160_get_ptr();

    p_bmi160_rt->parse_mag_sensor_data = bmm150_parse_mag_s32_xyz_data;
    p_bmi160_rt->change_mag_powermode = bmm150_change_powermode;
    p_bmi160_rt->reg_data_read_funs[BMI160_SENSOR_MAG] = bmm150_mag_read_data;

    /* switch mag interface to normal mode */
    com_rslt += bmi160_change_sensor_powermode(BMI160_SENSOR_MAG, BMI160_POWER_NORMAL);
    /* Mag init commands */
    com_rslt += bmi160_set_command_register(BMI160_COMMAND_REG_ONE);
    bmi160_delay_ms(1);
    com_rslt += bmi160_set_command_register(BMI160_COMMAND_REG_TWO);
    bmi160_delay_ms(1);
    bmi160_set_command_register(BMI160_COMMAND_REG_THREE);
    bmi160_delay_ms(1);
    /* switch to page 1*/
    bmi160_set_target_page(1);
    bmi160_delay_ms(1);
    bmi160_set_paging_enable(1);
    bmi160_delay_ms(1);
    /* enable the pullup configuration from the register 0x85 bit 4 and 5 */
    bmi160_read_reg(BMI160_COM_C_TRIM_FIVE__REG, &value);
    value = value | BMI160_PULL_UP_DATA;
    bmi160_write_reg(BMI160_COM_C_TRIM_FIVE__REG, &value);
    bmi160_delay_ms(1);

    /* switch to page 0*/
    bmi160_set_target_page(0);
    bmi160_delay_ms(1);
    /* Write the BMM150 i2c address*/
    value = (BMM150_I2C_ADDRESS<<1);
    bmi160_write_reg(BMI160_USER_I2C_DEVICE_ADDR__REG, &value);
    bmi160_delay_ms(1);
    bmm150_resume(BMM150_DO_INIT);
    return com_rslt;
}

DRIVER_API_RC bmm150_resume(uint8_t is_init)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t value = 0;
    /* switch mag interface to normal mode */
    com_rslt += bmi160_change_sensor_powermode(BMI160_SENSOR_MAG, BMI160_POWER_NORMAL);
    /*Enable the MAG interface */
    com_rslt += bmi160_set_if_mode(BMI160_PRIMARY_AUTO_SECONDARY_MAG);
    com_rslt += bmm150_mag_power_control(BMM150_POWER_ON, !BMM150_DO_SOFT_RESET);

    if(is_init){
        /* Read the BMM150 device id is 0x32*/
        bmm150_read_reg(BMM150_REG_CHIPID, &value);
        if(value!=BMM150_DEVICE_ID){
            pr_debug(LOG_MODULE_BMI160, "bmm150 chip id mismatch");
            return DRV_RC_FAIL;
        }
        pr_debug(LOG_MODULE_BMI160, "bmm150 chip id = 0x%x", value);

        //bmm150_mag_control(BMM150_POWER_MODE_SLEEP, BMM150_DATA_RATE_10HZ, BMM150_NO_SELFTEST, BMM150_NO_SELFTEST);
        bmm150_read_trim();
        bmm150_set_mag_presetmode(BMM150_PRESETMODE_REGULAR);
    }

    bmm150_mag_control(BMM150_POWER_MODE_FORCED, BMM150_DATA_RATE_30HZ, BMM150_NO_SELFTEST, BMM150_NO_SELFTEST);
    value = BMM150_DATA_X_LSB;
    bmi160_write_reg(BMI160_USER_READ_ADDR__REG, &value);

    if(p_bmi160_rt->mag_manual_enable)
        bmi160_set_mag_manual_enable(0, BMM150_MAG_READ_BURST_8);

    return com_rslt;
}

DRIVER_API_RC bmm150_suspend(void)
{
    DRIVER_API_RC com_rslt = 0;
    /* power off bmm150 */
    com_rslt += bmm150_mag_power_control(BMM150_POWER_OFF, !BMM150_DO_SOFT_RESET);
    /* Disable the MAG interface */
    com_rslt += bmi160_set_if_mode(BMI160_PRIMARY_AUTO_SECONDARY_OFF);
    /* switch mag interface to normal mode */
    com_rslt += bmi160_change_sensor_powermode(BMI160_SENSOR_MAG, BMI160_POWER_SUSPEND);

    return com_rslt;
}

static DRIVER_API_RC bmm150_change_powermode(uint8_t powermode)
{
    switch(powermode){
    case BMI160_POWER_SUSPEND:
        return bmm150_suspend();
    case BMI160_POWER_NORMAL:
        return bmm150_resume(0);
    default:
        pr_debug(LOG_MODULE_BMI160, "powermode %d not supported [bmm150]", powermode);
        return DRV_RC_INVALID_CONFIG;
    }
}

static DRIVER_API_RC bmm150_read_mag_xyzr(struct bmm150_mag_xyzr_t* data)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t a_data[8] = {0};
        /* read the gyro xyz data*/
    com_rslt = bmi160_bus_read(BMI160_USER_DATA_MAG_X_LSB__REG, a_data, 8);
    bmm150_8bytes_to_xyzr(a_data, data);

    return com_rslt;
}

int32_t bmm150_mag_compensate_X_Y(int16_t value_s16, uint16_t mag_r, uint8_t type)
{
    int32_t inter_retval = 0;
    int16_t mag_trim_dig1 = 0;
    int16_t mag_trim_dig2 = 0;

    if(type==BMM150_COMPENSATE_X){
        mag_trim_dig1 = (int16_t)mag_trim.dig_x1;
        mag_trim_dig2 = (int16_t)mag_trim.dig_x2;
    }
    else{
        mag_trim_dig1 = (int16_t)mag_trim.dig_y1;
        mag_trim_dig2 = (int16_t)mag_trim.dig_y2;
    }
    /* no overflow */
    if (value_s16 != BMI160_MAG_FLIP_OVERFLOW_ADCVAL) {
        if ((mag_r != 0) && (mag_trim.dig_xyz1 != 0)) {
            inter_retval = ((int32_t)(((uint16_t)((((int32_t)mag_trim.dig_xyz1) << 14)/
                            (mag_r != 0 ? mag_r : mag_trim.dig_xyz1)))
                            -((uint16_t)0x4000)));
        } else {
            inter_retval = BMI160_MAG_OVERFLOW_OUTPUT;
            return inter_retval;
        }
        inter_retval = ((int32_t)((((int32_t)value_s16) *
                        ((((((((int32_t)mag_trim.dig_xy2) *
                        ((((int32_t)inter_retval) *
                        ((int32_t)inter_retval)) >> 7)) +
                        (((int32_t)inter_retval) *
                        ((int32_t)(((int16_t)mag_trim.dig_xy1) << 7)))) >> 9) +
                        ((int32_t)0x100000)) *
                        ((int32_t)(mag_trim_dig2 +
                        ((int16_t)0xA0)))) >> 12)) >> 13)) +
                        (mag_trim_dig1 << 3);
        /* check the overflow output */
        if (inter_retval == (int32_t)BMI160_MAG_OVERFLOW_OUTPUT)
            inter_retval = BMI160_MAG_OVERFLOW_OUTPUT_S32;
    } else {
        /* overflow */
        inter_retval = BMI160_MAG_OVERFLOW_OUTPUT;
    }
    return inter_retval;
}

int32_t bmm150_mag_compensate_Z(int16_t mag_z, uint16_t mag_r)
{
    int32_t retval = 0;

    if (mag_z != BMI160_MAG_HALL_OVERFLOW_ADCVAL) {
        if ((mag_r != 0) &&
            (mag_trim.dig_z2 != 0) &&
            (mag_trim.dig_z3 != 0) &&
            (mag_trim.dig_z1 != 0) &&
            (mag_trim.dig_xyz1 != 0)) {
            retval = (((((int32_t)(mag_z - mag_trim.dig_z4)) << 15) -
            ((((int32_t)mag_trim.dig_z3) * ((int32_t)(((int16_t)mag_r) -
            ((int16_t)mag_trim.dig_xyz1)))) >> 2))/
            (mag_trim.dig_z2 + ((int16_t)(((((int32_t)mag_trim.dig_z1) *
            ((((int16_t)mag_r) << 1))) + (1 << 15)) >> 16))));
        }
    } else {
        retval = BMI160_MAG_OVERFLOW_OUTPUT;
    }
        return retval;
}


DRIVER_API_RC bmm150_read_mag_compensate_xyz(struct bmi160_s32_xyz_t *mag_comp_xyz)
{
    struct bmm150_mag_xyzr_t mag_xyzr;

    if(bmm150_read_mag_xyzr(&mag_xyzr))
        return DRV_RC_FAIL;
    bmm150_xyzr_to_s32xyz(&mag_xyzr, mag_comp_xyz);
    return DRV_RC_OK;
}

static uint8_t bmm150_parse_mag_s32_xyz_data(uint8_t *out_buf, uint8_t *out_index, uint16_t *fifo_index)
{
    struct bmi160_s32_xyz_t* s32_xyz_buf = (struct bmi160_s32_xyz_t*)out_buf;
    struct bmm150_mag_xyzr_t mag_xyzr;
    uint16_t i = *fifo_index;

    bmm150_8bytes_to_xyzr(&bmi160_fifo_data[i], &mag_xyzr);

    *fifo_index = i+8;

    bmm150_xyzr_to_s32xyz(&mag_xyzr, &s32_xyz_buf[*out_index]);

    *out_index = *out_index + 1;
    return 0;
}

int bmm150_mag_read_data(uint8_t * buf, uint16_t buff_len)
{
    if(bmm150_read_mag_compensate_xyz((struct bmi160_s32_xyz_t *)buf))
        return 0;
    return sizeof(struct bmi160_s32_xyz_t);
}
