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
#include "drivers/sensor/bmi160_gpio.h"

#define BMI160_DEFAULT_INT_CONFIG   \
    ( BMI160_INT1_TRIGGER_LEVEL     \
    | BMI160_INT1_ACTIVE_HIGH       \
    | BMI160_INT1_DIR_OUTPUT        \
    | BMI160_INT2_TRIGGER_LEVEL     \
    | BMI160_INT2_ACTIVE_HIGH       \
    | BMI160_INT2_DIR_OUTPUT)

/* default undersampling average cycle 2^BMI160_DEFAULT_AVG_CONFIG */
#define BMI160_DEFAULT_AVG_CONFIG 2

#define ACCEL_CONVERT_CONST 9800
#define GYRO_CONVERT_CONST 15625

#define DELAY_ACCEL_SUSPEND2NORMAL 4
#define DELAY_GYRO_SUSPEND2NORMAL  60
#define DELAY_GYRO_FASTUP2NORMAL   10
#define DELAY_MAG_PM_SWITCH        1

#define BMI160_NO_MOTION_DUR_IN_MS 5120
#define BMI160_ANY_MOTION_DUR_SLOPE 2
#define BMI160_ANY_MOTION_THRESHOLD_IN_MICRO_G 64000
#define BMI160_SLOW_NO_MOTION_THRESHOLD_IN_MICRO_G 64000

static struct bmi160_rt_t s_bmi160_rt;
static struct bmi160_rt_t *p_bmi160_rt;

static uint8_t suspend_cmds[BMI160_SENSOR_COUNT] =
{
    ACCEL_MODE_SUSPEND,
    GYRO_MODE_SUSPEND,
#if BMI160_ENABLE_MAG
    MAG_MODE_SUSPEND
#endif
};

static uint8_t normal_cmds[BMI160_SENSOR_COUNT] =
{
    ACCEL_MODE_NORMAL,
    GYRO_MODE_NORMAL,
#if BMI160_ENABLE_MAG
    MAG_MODE_NORMAL
#endif
};

static uint8_t lowpower_cmds[BMI160_SENSOR_COUNT] =
{
    ACCEL_MODE_LOWPOWER,
    0,
#if BMI160_ENABLE_MAG
    MAG_MODE_LOWPOWER
#endif
};

/* unit is ms */
static int8_t pm_delay[BMI160_SENSOR_COUNT] =
{
    DELAY_ACCEL_SUSPEND2NORMAL,
    DELAY_GYRO_SUSPEND2NORMAL,
#if BMI160_ENABLE_MAG
    DELAY_MAG_PM_SWITCH
#endif
};

/* Assign the fifo time */
uint32_t bmi160_fifo_time = 0;

#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
/* Used to store as accel fifo data */
struct bmi160_accel_t bmi160_accel_fifo[FIFO_FRAME_CNT];
/* Used to store as gyro fifo data */
struct bmi160_gyro_t bmi160_gyro_fifo[FIFO_FRAME_CNT];
struct bmi160_s32_xyz_t bmi160_mag_fifo[MAG_FIFO_FRAME_CNT];
uint8_t bmi160_accel_index = 0;
uint8_t bmi160_gyro_index = 0;
uint8_t bmi160_mag_index = 0;
#endif

/* FIFO data read for FIFO_FRAME of data */
uint8_t bmi160_fifo_data[FIFO_FRAME] = {0};
static uint16_t bmi160_fifo_data_start = 0;
static uint16_t bmi160_fifo_data_end = 0;

static uint8_t fifo_config1 = BMI160_USER_FIFO_TIME_ENABLE__MSK |
                              BMI160_USER_FIFO_HEADER_ENABLE__MSK;

static uint8_t acc_sensitivity_shift = BMI160_SENSITIVITY_ACCEL_8G_SHIFT;
static uint8_t acc_range = BMI160_ACCEL_RANGE_8G;
static uint8_t gyro_range = BMI160_GYRO_RANGE_1000_DEG_SEC;
static uint16_t gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_1K_NUMERATOR;
static uint16_t gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_1K_DENOMINATOR;

 /*!
 *  @brief This API writes value to the register 0x7E bit 0 to 7
 *
 *
 *  @param  v_command_reg_u8 : The value to write command register
 *  value   |  Description
 * ---------|--------------------------------------------------------
 *  0x03    |   START_FOC_ACCEL_GYRO
 *  0x10    |   ACCEL_MODE_SUSPEND
 *  0x11    |   ACCEL_MODE_NORMAL
 *  0x12    |   ACCEL_MODE_LOWPOWER
 *  0x14    |   GYRO_MODE_SUSPEND
 *  0x15    |   GYRO_MODE_NORMAL
 *  0x17    |   GYRO_MODE_FASTSTARTUP
 *  0x18    |   MAG_MODE_SUSPEND
 *  0x19    |   MAG_MODE_NORMAL
 *  0x1A    |   MAG_MODE_LOWPOWER
 *  0xB0    |   FLUSH_FIFO
 *  0xB1    |   RESET_INT_ENGINE
 *  0xB2    |   CLEAR_SETP_CNT
 *  0xB6    |   TRIGGER_RESET
 *  0xC0    |   ENABLE_EXT_MODE
 *  0xC4    |   ERASE_NVM_CELL
 *  0xC8    |   LOAD_NVM_CELL
 *  0xF0    |   RESET_ACCEL_DATA_PATH
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_set_command_register(uint8_t v_command_reg_u8)
{
    return bmi160_write_reg( BMI160_CMD_COMMANDS__REG, &v_command_reg_u8);
}

DRIVER_API_RC bmi160_set_paging_enable(uint8_t page_enable)
{
    uint8_t value = 0;
    if(bmi160_read_reg(BMI160_CMD_PAGING_EN__REG, &value))
        return DRV_RC_FAIL;
    value = BMI160_SET_BITSLICE(value, BMI160_CMD_PAGING_EN, page_enable);
    return bmi160_write_reg(BMI160_CMD_PAGING_EN__REG, &value);
}

DRIVER_API_RC bmi160_set_target_page(uint8_t target_page)
{
    uint8_t value = 0;

    if(bmi160_read_reg(BMI160_CMD_TARGET_PAGE__REG, &value))
        return DRV_RC_FAIL;

    value = BMI160_SET_BITSLICE(value, BMI160_CMD_TARGET_PAGE, target_page);
    return bmi160_write_reg(BMI160_CMD_TARGET_PAGE__REG, &value);
}

DRIVER_API_RC bmi160_set_mag_manual_enable(uint8_t enable, uint8_t burst)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t reg = 0;
    uint8_t mag_offset = 0;

    if(bmi160_read_reg(BMI160_USER_MAG_MANUAL_ENABLE__REG, &reg))
        return DRV_RC_FAIL;

    reg = BMI160_SET_BITSLICE(reg, BMI160_USER_MAG_MANUAL_ENABLE, enable);
    reg = BMI160_SET_BITSLICE(reg, BMI160_USER_MAG_OFFSET, mag_offset);
    reg = BMI160_SET_BITSLICE(reg, BMI160_USER_MAG_BURST, burst);

    com_rslt = bmi160_write_reg(BMI160_USER_MAG_MANUAL_ENABLE__REG, &reg);

    if(!com_rslt){
        p_bmi160_rt->mag_manual_enable = enable;
        bmi160_delay_ms(1);
    }

    return com_rslt;
}

static DRIVER_API_RC bmi160_read_accel_xyz(struct bmi160_s16_xyz_t *data, uint8_t sensor_type)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t a_data[BMI160_ACCEL_RAW_DATA_SIZE] = {0};
    uint8_t reg_addr = BMI160_USER_DATA_14_ACCEL_X_LSB__REG;
        /* read the gyro xyz data*/
    com_rslt = bmi160_bus_read(reg_addr, a_data, BMI160_ACCEL_RAW_DATA_SIZE);
    /* Data X */
    data->x = (int16_t)
            ((((int32_t)((int8_t)a_data[1]))<< 8) | (a_data[0]));
    /* Data Y */
    data->y = (int16_t)
            ((((int32_t)((int8_t)a_data[3]))<< 8) | (a_data[2]));
    /* Data Z */
    data->z = (int16_t)
            ((((int32_t)((int8_t)a_data[5]))<< 8) | (a_data[4]));
    return com_rslt;
}

static DRIVER_API_RC bmi160_read_gyro_xyz(struct bmi160_s32_xyz_t *data, uint8_t sensor_type)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t a_data[BMI160_GYRO_RAW_DATA_SIZE] = {0};
    uint8_t reg_addr = BMI160_USER_DATA_8_GYRO_X_LSB__REG;
        /* read the gyro xyz data*/
    com_rslt = bmi160_bus_read(reg_addr, a_data, BMI160_GYRO_RAW_DATA_SIZE);
    /* Data X */
    data->x = ((((int32_t)((int8_t)a_data[1]))<< 8) | (a_data[0]));
    /* Data Y */
    data->y = ((((int32_t)((int8_t)a_data[3]))<< 8) | (a_data[2]));
    /* Data Z */
    data->z = ((((int32_t)((int8_t)a_data[5]))<< 8) | (a_data[4]));
    return com_rslt;
}

DRIVER_API_RC bmi16_set_user_sensor_config(uint8_t type, uint8_t powermode, uint8_t change_bwp_odr, uint8_t bwp_odr)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t cmd = 0;
    uint8_t reg;
    uint32_t delay = 1;
    static uint32_t need_to_delay_ms = 0;
    static uint32_t pm_switch_timestamp = 0;

    if (type >= BMI160_SENSOR_COUNT)
        return DRV_RC_INVALID_CONFIG;

    if(type==BMI160_SENSOR_ACCEL){
        reg = BMI160_USER_ACCEL_CONFIG_ADDR;
        if(powermode==BMI160_POWER_LOWPOWER)
            bwp_odr = (0x80 | bwp_odr);
    }
    else if(type==BMI160_SENSOR_GYRO)
        reg = BMI160_USER_GYRO_CONFIG_ADDR;
    else
        reg = BMI160_USER_MAG_CONFIG_ADDR;

    if(change_bwp_odr){
        com_rslt += bmi160_write_reg(reg, &bwp_odr);
        pr_debug(LOG_MODULE_BMI160, "%s: reg[0x%x], 0x%x", __func__, reg, bwp_odr);
        bmi160_delay_ms(1);
    }

    if(p_bmi160_rt->power_status[type] == powermode)
        return DRV_RC_OK;

    switch(powermode){
    case BMI160_POWER_SUSPEND:
        cmd = suspend_cmds[type];
        break;
    case BMI160_POWER_NORMAL:
        cmd = normal_cmds[type];
        if(p_bmi160_rt->power_status[type] == BMI160_POWER_FAST_STARTUP)
            delay = DELAY_GYRO_FASTUP2NORMAL;
        else
            delay = pm_delay[type];
        break;
    case BMI160_POWER_LOWPOWER:
        cmd = lowpower_cmds[type];
        if(type==BMI160_SENSOR_ACCEL)
            delay = 4;
        break;
    case BMI160_POWER_FAST_STARTUP:
        cmd = GYRO_MODE_FASTSTARTUP;
        break;
    default:
        pr_debug(LOG_MODULE_BMI160, "powermode %u not supported", powermode);
        return DRV_RC_INVALID_CONFIG;
    }

    if(need_to_delay_ms){
        uint32_t delayed_ms = get_uptime_ms() - pm_switch_timestamp;
        if(delayed_ms < need_to_delay_ms){
            pr_debug(LOG_MODULE_BMI160, "still need to delay %d ms", need_to_delay_ms - delayed_ms);
            bmi160_delay_ms(need_to_delay_ms - delayed_ms);
        }
    }

    com_rslt += bmi160_set_command_register(cmd);

    p_bmi160_rt->power_status[type] = powermode;

    if(powermode==BMI160_POWER_NORMAL){
        bmi160_delay_ms(delay);
        need_to_delay_ms = 0;
    } else{
        pm_switch_timestamp = get_uptime_ms();
        need_to_delay_ms = delay;
    }

    return com_rslt;
}

DRIVER_API_RC bmi160_change_sensor_powermode(uint8_t type, uint8_t powermode)
{
    return bmi16_set_user_sensor_config(type, powermode, 0, 0);
}

inline DRIVER_API_RC bmi160_get_fifo_data(uint8_t *fifo_data, uint32_t size)
{
    return bmi160_bus_burst_read( BMI160_USER_FIFO_DATA__REG, fifo_data, size);
}

static DRIVER_API_RC bmi160_set_accel_range(uint8_t v_range_u8)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t data = 0;
    com_rslt = bmi160_read_reg(BMI160_USER_ACCEL_RANGE__REG, &data);
    if (com_rslt == DRV_RC_OK) {
        data  = BMI160_SET_BITSLICE(data, BMI160_USER_ACCEL_RANGE, v_range_u8);
        /* write the accel range*/
        com_rslt += bmi160_write_reg(BMI160_USER_ACCEL_RANGE__REG, &data);
    }

    return com_rslt;
}

DRIVER_API_RC bmi160_change_accel_sensing_range(uint8_t range)
{
    pr_debug(LOG_MODULE_BMI160, "%s: range:%d", __func__, range);
    if(range <= 2) {
        acc_range = BMI160_ACCEL_RANGE_2G;
        p_bmi160_rt->range_native[BMI160_SENSOR_ACCEL] = 2;
        acc_sensitivity_shift = BMI160_SENSITIVITY_ACCEL_2G_SHIFT;
    }else if(range <= 4) {
        acc_range = BMI160_ACCEL_RANGE_4G;
        p_bmi160_rt->range_native[BMI160_SENSOR_ACCEL] = 4;
        acc_sensitivity_shift = BMI160_SENSITIVITY_ACCEL_4G_SHIFT;

    }else if(range <= 8) {
        acc_range = BMI160_ACCEL_RANGE_8G;
        p_bmi160_rt->range_native[BMI160_SENSOR_ACCEL] = 8;
        acc_sensitivity_shift = BMI160_SENSITIVITY_ACCEL_8G_SHIFT;
    }else{
        acc_range = BMI160_ACCEL_RANGE_16G;
        p_bmi160_rt->range_native[BMI160_SENSOR_ACCEL] = 16;
        acc_sensitivity_shift = BMI160_SENSITIVITY_ACCEL_16G_SHIFT;
    }

    return bmi160_set_accel_range(acc_range);
}

static DRIVER_API_RC bmi160_set_gyro_range(uint8_t v_range_u8)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t data = 0;
    /* write the gyro range value */
    com_rslt = bmi160_read_reg(BMI160_USER_GYRO_RANGE__REG, &data);
    if (com_rslt == DRV_RC_OK) {
        data = BMI160_SET_BITSLICE(data, BMI160_USER_GYRO_RANGE, v_range_u8);
        com_rslt += bmi160_write_reg(BMI160_USER_GYRO_RANGE__REG, &data);
    }

    return com_rslt;
}

DRIVER_API_RC bmi160_change_gyro_sensing_range(uint16_t range)
{
    pr_debug(LOG_MODULE_BMI160, "%s: range:%d", __func__, range);
    if(range <= 125){
        gyro_range = BMI160_GYRO_RANGE_125_DEG_SEC;
        gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_125_NUMERATOR;
        gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_125_DENOMINATOR;
    }else if(range <= 250){
        gyro_range = BMI160_GYRO_RANGE_250_DEG_SEC;
        gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_250_NUMERATOR;
        gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_250_DENOMINATOR;
    }else if(range <= 500){
        gyro_range = BMI160_GYRO_RANGE_500_DEG_SEC;
        gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_500_NUMERATOR;
        gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_500_DENOMINATOR;
    }else if(range <=1000){
        gyro_range = BMI160_GYRO_RANGE_1000_DEG_SEC;
        gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_1K_NUMERATOR;
        gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_1K_DENOMINATOR;
    }else {
        gyro_range = BMI160_GYRO_RANGE_2000_DEG_SEC;
        gyro_convert_numerator = BMI160_SENSITIVITY_GYRO_2K_NUMERATOR;
        gyro_convert_denominator = BMI160_SENSITIVITY_GYRO_2K_DENOMINATOR;
    }

    p_bmi160_rt->range_native[BMI160_SENSOR_GYRO] = (2000 >> gyro_range);

    return bmi160_set_gyro_range(gyro_range);
}

DRIVER_API_RC bmi160_set_if_mode(uint8_t v_if_mode_u8)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t data = 0;
    /* write if mode*/
    if(!bmi160_read_reg(BMI160_USER_IF_CONFIG_IF_MODE__REG, &data)){
        data = BMI160_SET_BITSLICE(data, BMI160_USER_IF_CONFIG_IF_MODE, v_if_mode_u8);
        com_rslt = bmi160_write_reg(BMI160_USER_IF_CONFIG_IF_MODE__REG,&data);
    }

    return com_rslt;
}

/* Parse accel data, from s16_xyz to s16_syz */
static int8_t parse_accel_xyz_data(uint8_t *out_buf, uint8_t *out_index, uint16_t *fifo_index)
{
    struct bmi160_accel_t* s16_xyz_buf = (struct bmi160_accel_t*)out_buf;
    uint16_t i = *fifo_index;

    s16_xyz_buf[*out_index].x = (int16_t)
                                ((((int32_t)((int8_t)bmi160_fifo_data[i + 1]))<< 8) | bmi160_fifo_data[i + 0]);
    s16_xyz_buf[*out_index].y = (int16_t)
                                ((((int32_t)((int8_t)bmi160_fifo_data[i + 3]))<< 8) | bmi160_fifo_data[i + 2]);
    s16_xyz_buf[*out_index].z = (int16_t)
                                ((((int32_t)((int8_t)bmi160_fifo_data[i + 5]))<< 8) | bmi160_fifo_data[i + 4]);

    *fifo_index = i + BMI160_ACCEL_RAW_DATA_SIZE;
    *out_index = *out_index + 1;
    return 0;
}

/* Parse gyro data, from s16_xyz to s32_syz */
static int8_t parse_gyro_xyz_data(uint8_t *out_buf, uint8_t *out_index, uint16_t *fifo_index)
{
    struct bmi160_gyro_t* s32_xyz_buf = (struct bmi160_gyro_t*)out_buf;
    uint16_t i = *fifo_index;

    s32_xyz_buf[*out_index].x = ((((int32_t)((int8_t)bmi160_fifo_data[i + 1]))<< 8) | bmi160_fifo_data[i + 0]);
    s32_xyz_buf[*out_index].y = ((((int32_t)((int8_t)bmi160_fifo_data[i + 3]))<< 8) | bmi160_fifo_data[i + 2]);
    s32_xyz_buf[*out_index].z = ((((int32_t)((int8_t)bmi160_fifo_data[i + 5]))<< 8) | bmi160_fifo_data[i + 4]);

    *fifo_index = i + BMI160_GYRO_RAW_DATA_SIZE;
    *out_index = *out_index + 1;
    return 0;
}

static DRIVER_API_RC bmi160_support_init(struct bmi160_rt_t * bmi160_rt)
{
    DRIVER_API_RC com_rslt = 0;

    p_bmi160_rt = bmi160_rt;

#ifdef CONFIG_BMI160_CONVERT_ACCEL_DATA
    p_bmi160_rt->convert_data_funs[BMI160_SENSOR_ACCEL] = bmi160_convert_accel_data;
#endif

#ifdef CONFIG_BMI160_CONVERT_GYRO_DATA
    p_bmi160_rt->convert_data_funs[BMI160_SENSOR_GYRO] = bmi160_convert_gyro_data;
#endif

    p_bmi160_rt->reg_data_read_funs[BMI160_SENSOR_ACCEL] = bmi160_accle_read_data;
    p_bmi160_rt->reg_data_read_funs[BMI160_SENSOR_GYRO] = bmi160_gyro_read_data;

    /* if avg is not set, use BMI160_DEFAULT_AVG_CONFIG when enter into lowpower mode */
    p_bmi160_rt->undersampling_avg = BMI160_DEFAULT_AVG_CONFIG;

    bmi160_delay_ms(100);

#ifdef CONFIG_BMI160_SPI
    uint8_t dummy = 0;
    /* Perform a dummy read from 0x7f to switch to spi interface */
    com_rslt += bmi160_read_reg(0x7F,&dummy);
#endif

    com_rslt += bmi160_read_reg(BMI160_USER_CHIP_ID__REG, &p_bmi160_rt->chip_id);
    pr_debug(LOG_MODULE_BMI160, "BMI160 Chip ID: 0x%x", p_bmi160_rt->chip_id);

    bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_NORMAL);

    com_rslt += bmi160_change_accel_sensing_range(CONFIG_BMI160_DEFAULT_ACCEL_RANGE);
    com_rslt += bmi160_change_gyro_sensing_range(CONFIG_BMI160_DEFAULT_GYRO_RANGE);
    bmi160_delay_ms(1);
    return com_rslt;
}

static DRIVER_API_RC set_int_motion0(uint32_t slo_no_mot_dur, uint8_t anym_dur)
{
    uint8_t int_motion_0 = 0;
    if(slo_no_mot_dur<=20480)
        int_motion_0 =(slo_no_mot_dur/1280 - 1);
    else if(slo_no_mot_dur<=102400)
        int_motion_0 = (1<<4) | (slo_no_mot_dur/5120 - 5);
    else
        int_motion_0 = (1<<5) | (slo_no_mot_dur/10240 - 11);

    int_motion_0 = (int_motion_0<<2)|anym_dur;
    return bmi160_write_reg(BMI160_USER_INTR_MOTION_0_ADDR, &int_motion_0);
}


static DRIVER_API_RC bmi160_interrupt_configuration(uint8_t int_config)
{
    /* This variable used for provide the communication
       results */
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t ctrl = int_config;

    com_rslt = bmi160_write_reg(BMI160_USER_INTR_OUT_CTRL_ADDR, &ctrl);

#if BMI160_USE_INT_PIN1
    /* Map interrupts: data ready, fifo_wm, fifo full */
    ctrl = BMI160_USER_INTR_MAP_1_INTR1_FIFO_FULL__MSK |
           BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__MSK   |
           BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__MSK;
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MAP_1_ADDR, &ctrl);
#ifdef CONFIG_BMI160_PM
    /* Map interrupts: double tap, no motion, any motion */
    ctrl = BMI160_USER_INTR_MAP_0_INTR1_DOUBLE_TAP__MSK |
           BMI160_USER_INTR_MAP_0_INTR1_NOMOTION__MSK   |
           BMI160_USER_INTR_MAP_0_INTR1_ANY_MOTION__MSK;
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MAP_0_ADDR, &ctrl);
#endif
#else
    /* Map interrupts: data ready, fifo_wm, fifo full */
    ctrl = BMI160_USER_INTR_MAP_1_INTR2_FIFO_FULL__MSK |
           BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__MSK |
           BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__MSK;
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MAP_1_ADDR, &ctrl);
#ifdef CONFIG_BMI160_PM
    /* Map interrupts: double tap, no motion, any motion */
    ctrl = BMI160_USER_INTR_MAP_2_INTR2_DOUBLE_TAP__MSK |
           BMI160_USER_INTR_MAP_2_INTR2_NOMOTION__MSK   |
           BMI160_USER_INTR_MAP_2_INTR2_ANY_MOTION__MSK;
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MAP_2_ADDR, &ctrl);
#endif
#endif

    /* latch the interrupt signals and flags */
    ctrl = BMI160_LATCH_DUR_10_MILLI_SEC;
    bmi160_write_reg(BMI160_USER_INTR_LATCH_ADDR, &ctrl);

#ifdef CONFIG_BMI160_PM
    uint8_t tap_ctrl = 0xC6; /* Double Tapping configuration */
    uint8_t anym_th_ctrl;    /* no_motion_threshold */
    uint8_t slo_no_mot_th_ctrl;
    uint8_t int_motion_3 = 0x15;    /* Select any-motion, no-motion */
    uint8_t pmu_trigger = 0;
    bmi160_write_reg(BMI160_USER_INTR_TAP_0_ADDR, &tap_ctrl);

    set_int_motion0(BMI160_NO_MOTION_DUR_IN_MS, BMI160_ANY_MOTION_DUR_SLOPE-1);

    if(acc_range==BMI160_ACCEL_RANGE_2G){
        tap_ctrl = 8;     /* 62.5mg x tap_threshold */
        anym_th_ctrl = BMI160_ANY_MOTION_THRESHOLD_IN_MICRO_G/3910;
        slo_no_mot_th_ctrl = BMI160_SLOW_NO_MOTION_THRESHOLD_IN_MICRO_G/3910;
    } else if(acc_range==BMI160_ACCEL_RANGE_4G){
        tap_ctrl = 4;      /* 125mg x tap_threshold */
        anym_th_ctrl = BMI160_ANY_MOTION_THRESHOLD_IN_MICRO_G/7810;
        slo_no_mot_th_ctrl = BMI160_SLOW_NO_MOTION_THRESHOLD_IN_MICRO_G/7810;
    } else if(acc_range==BMI160_ACCEL_RANGE_8G){
        tap_ctrl = 2;      /* 250mg x tap_threshold */
        anym_th_ctrl = BMI160_ANY_MOTION_THRESHOLD_IN_MICRO_G/15630;
        slo_no_mot_th_ctrl = BMI160_SLOW_NO_MOTION_THRESHOLD_IN_MICRO_G/15630;
    } else {
        tap_ctrl = 1;      /* 500mg x tap_threshold */
        anym_th_ctrl = BMI160_ANY_MOTION_THRESHOLD_IN_MICRO_G/31250;
        slo_no_mot_th_ctrl = BMI160_SLOW_NO_MOTION_THRESHOLD_IN_MICRO_G/31250;
    }

    com_rslt += bmi160_write_reg(BMI160_USER_INTR_TAP_1_ADDR, &tap_ctrl);
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MOTION_1_ADDR, &anym_th_ctrl);
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MOTION_2_ADDR, &slo_no_mot_th_ctrl);
    com_rslt += bmi160_write_reg(BMI160_USER_INTR_MOTION_3_ADDR, &int_motion_3);

    com_rslt += bmi160_write_reg(BMI160_USER_PMU_TRIGGER_ADDR, &pmu_trigger);
#endif

    bmi160_delay_ms(1);
    return com_rslt;
}

static DRIVER_API_RC bmi160_config_intr(uint8_t int_set)
{

    uint8_t retry = 0;
    uint8_t bmi160_config_intr_reg = 0;
    uint8_t bmi160_config_intr_en_check = 0;
    uint8_t en = 0;
    DRIVER_API_RC com_rslt = 0;

    if(int_set==0){         /* interrupts in set 0 */
        if(p_bmi160_rt->int_en[BMI160_OFFSET_DOUBLE_TAP])
            en |= BMI160_USER_INTR_ENABLE_0_DOUBLE_TAP_ENABLE__MSK;
        if(p_bmi160_rt->int_en[BMI160_OFFSET_ANYMOTION])
            en |= (BMI160_USER_INTR_ENABLE_0_ANY_MOTION_X_ENABLE__MSK |
                   BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Y_ENABLE__MSK |
                   BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Z_ENABLE__MSK);
        bmi160_config_intr_reg = BMI160_USER_INTR_ENABLE_0_ADDR;
    }else if(int_set==1) {  /* interrupts in set 1 */
        if(p_bmi160_rt->int_en[BMI160_OFFSET_DRDY])
            en |= BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__MSK;
        if(p_bmi160_rt->int_en[BMI160_OFFSET_FIFO_WM])
            en |= BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__MSK;
        if(p_bmi160_rt->int_en[BMI160_OFFSET_FIFO_FULL])
            en |= BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__MSK;
        bmi160_config_intr_reg = BMI160_USER_INTR_ENABLE_1_ADDR;
    }else if(int_set==2) {  /* interrupts in set 2 */
        if(p_bmi160_rt->int_en[BMI160_OFFSET_NOMOTION])
            en |= (BMI160_USER_INTR_ENABLE_2_NOMOTION_X_ENABLE__MSK |
                   BMI160_USER_INTR_ENABLE_2_NOMOTION_Y_ENABLE__MSK |
                   BMI160_USER_INTR_ENABLE_2_NOMOTION_Z_ENABLE__MSK);
        bmi160_config_intr_reg = BMI160_USER_INTR_ENABLE_2_ADDR;
    }

RETRY:
    if(retry<10){
        com_rslt = bmi160_write_reg(bmi160_config_intr_reg, &en);
        bmi160_delay_ms(1);
        bmi160_read_reg(bmi160_config_intr_reg, &bmi160_config_intr_en_check);
        if(bmi160_config_intr_en_check!=en){
            retry++;
            pr_debug(LOG_MODULE_BMI160, "int_reg[0x%x], expect[%x], readback[%x], retry=[%d]",
                bmi160_config_intr_reg, en, bmi160_config_intr_en_check, retry);
            com_rslt = DRV_RC_FAIL;
            goto RETRY;
        }else
            com_rslt = DRV_RC_OK;
    }

    if(com_rslt){
        pr_debug(LOG_MODULE_BMI160, "failed to set reg[%x], expect[%x], readback[%x]",
            bmi160_config_intr_reg, en, bmi160_config_intr_en_check);
    }

    return com_rslt;
}

DRIVER_API_RC bmi160_int_enable(uint8_t int_set, uint8_t int_offset, uint8_t enable_mask)
{
    p_bmi160_rt->int_en[int_offset] |= enable_mask;
    return bmi160_config_intr(int_set);
}

DRIVER_API_RC bmi160_int_disable(uint8_t int_set, uint8_t int_offset, uint8_t disable_mask)
{
    p_bmi160_rt->int_en[int_offset] &= disable_mask;
    return bmi160_config_intr(int_set);
}

DRIVER_API_RC bmi160_flush_fifo(void)
{
    DRIVER_API_RC com_rslt = 0;
    com_rslt = bmi160_set_command_register(FLUSH_FIFO);
    bmi160_delay_ms(1);
    return com_rslt;
}

DRIVER_API_RC bmi160_fifo_length(uint32_t *v_fifo_length_u32)
{
    /* variable used for return the status of communication result*/
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    /* Array contains the fifo length data */
    uint8_t a_data[2] = {0};
    /* read fifo length */
    com_rslt = bmi160_bus_read(BMI160_USER_FIFO_BYTE_COUNTER_LSB__REG, a_data, 2);
    a_data[1] = BMI160_GET_BITSLICE(a_data[1], BMI160_USER_FIFO_BYTE_COUNTER_MSB);
    *v_fifo_length_u32 = (uint32_t)(((uint32_t)((uint8_t) (a_data[1]) << 8))| a_data[0]);

    return com_rslt;
}

DRIVER_API_RC bmi160_update_fifo_watermark(void)
{
    uint32_t wm_value = 0;
    uint32_t wm_value_debug;
    uint8_t wm_value_reg;
    DRIVER_API_RC com_rslt = 0;
    uint8_t enable_mask = 0;
    uint8_t raw_data_size[3] = { BMI160_ACCEL_RAW_DATA_SIZE,
                                 BMI160_GYRO_RAW_DATA_SIZE,
                                 BMI160_MAG_RAW_DATA_SIZE };

    for(int i=0; i<BMI160_SENSOR_COUNT; i++){
#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
        if(p_bmi160_rt->fifo_en[i] && p_bmi160_rt->read_callbacks[i]){
#else
        if(p_bmi160_rt->fifo_en[i] && p_bmi160_rt->wm_callback[i]){
#endif
            enable_mask |= (1<<i);
            wm_value += p_bmi160_rt->watermark_trigger_cnt[i]*(raw_data_size[i]+1);
        }
    }

    wm_value_debug = wm_value;
    if(wm_value > BMI160_FIFO_WATERMARK_MAX)
        wm_value = BMI160_FIFO_WATERMARK_MAX;

    wm_value_reg = (wm_value>>2);
    if(wm_value_reg)
        com_rslt += bmi160_write_reg(BMI160_USER_FIFO_WM__REG, &wm_value_reg);

    com_rslt += bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, 0);
    com_rslt += bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, enable_mask);

    pr_debug(LOG_MODULE_BMI160, "change wm: %d --> %d --> %d", wm_value_debug, wm_value, wm_value_reg);
    return com_rslt;
}

DRIVER_API_RC bmi160_sensor_open(uint8_t sensor_type)
{
    DRIVER_API_RC com_rslt = 0;
    static uint8_t bmi160_initilized = 0;

    if (bmi160_initilized){
        s_bmi160_rt.sensor_opened |= (1<<sensor_type);
        return DRV_RC_OK;
    }

    com_rslt += bmi160_support_init(&s_bmi160_rt);
    com_rslt += bmi160_interrupt_configuration(BMI160_DEFAULT_INT_CONFIG);
    com_rslt += bmi160_config_gpio();
    /* Accel was set to Normal mode in bmi160_support_init
     * suspend accel after init to save power if it is opened but not used
     */
    com_rslt += bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_SUSPEND);

    if (!com_rslt){
        bmi160_initilized = 1;
        s_bmi160_rt.sensor_opened |= (1<<sensor_type);
    }

    return com_rslt;
}

DRIVER_API_RC bmi160_sensor_close(uint8_t sensor_type)
{
    p_bmi160_rt->sensor_opened &= ~(1<<sensor_type);

    if(sensor_type==BMI160_SENSOR_GYRO){
        bmi160_change_sensor_powermode(BMI160_SENSOR_GYRO, BMI160_POWER_SUSPEND);
    }
#if BMI160_ENABLE_MAG
    else if(sensor_type==BMI160_SENSOR_MAG && p_bmi160_rt->change_mag_powermode)
        p_bmi160_rt->change_mag_powermode(BMI160_POWER_SUSPEND);
#endif
    else if(!(s_bmi160_rt.sensor_opened &
             ((1<<BMI160_SENSOR_ACCEL) | (1<<BMI160_SENSOR_MOTION)))){
        bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_SUSPEND);
        p_bmi160_rt->sensor_odr[BMI160_SENSOR_ACCEL] = 0;
    }

    return DRV_RC_OK;
}

DRIVER_API_RC bmi160_register_read_cb(read_data callback, void *priv_data, uint8_t sensor_type)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t enable_mask = (1<<sensor_type);

    p_bmi160_rt->read_callbacks[sensor_type] = callback;
    p_bmi160_rt->cb_priv_data[sensor_type] = priv_data;

#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
    if (p_bmi160_rt->fifo_en[sensor_type])
        com_rslt += bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, enable_mask);
    else
#endif
        com_rslt += bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_DRDY, enable_mask);
    return com_rslt;
}

DRIVER_API_RC bmi160_unregister_read_cb(uint8_t sensor_type)
{

    DRIVER_API_RC com_rslt = 0;
    uint8_t disable_mask = ~(1<<sensor_type);

#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
    if (p_bmi160_rt->fifo_en[sensor_type])
        com_rslt += bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, disable_mask);
    else
#endif
        com_rslt += bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_DRDY, disable_mask);

    p_bmi160_rt->read_callbacks[sensor_type] = NULL;
    p_bmi160_rt->cb_priv_data[sensor_type] = NULL;

    return com_rslt;
}

DRIVER_API_RC bmi160_register_motion_cb(read_data callback, void *priv_data)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t enable_mask = (1<<BMI160_SENSOR_MOTION);

    p_bmi160_rt->motion_callback = callback;
    p_bmi160_rt->motion_cb_data = priv_data;
    p_bmi160_rt->motion_state = 1;

    /* enable no-motion/slow motion detection */
    com_rslt += bmi160_int_enable(BMI160_INT_SET_2, BMI160_OFFSET_NOMOTION, enable_mask);
#if BMI160_ENABLE_DOUBLE_TAPPING
    com_rslt += bmi160_int_enable(BMI160_INT_SET_0, BMI160_OFFSET_DOUBLE_TAP, enable_mask);
#endif
    return com_rslt;
}

DRIVER_API_RC bmi160_unregister_motion_cb(void)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t disable_mask = 0;

    com_rslt += bmi160_int_disable(BMI160_INT_SET_2, BMI160_OFFSET_NOMOTION, disable_mask);
    com_rslt += bmi160_int_disable(BMI160_INT_SET_0, BMI160_OFFSET_ANYMOTION, disable_mask);
#if BMI160_ENABLE_DOUBLE_TAPPING
    com_rslt += bmi160_int_disable(BMI160_INT_SET_0, BMI160_OFFSET_DOUBLE_TAP, disable_mask);
#endif

    p_bmi160_rt->motion_callback = NULL;
    p_bmi160_rt->motion_cb_data = NULL;
    return com_rslt;
}

DRIVER_API_RC bmi160_sensor_query_rate(uint16_t odr_target, uint16_t *odr_support, uint8_t sensor_type)
{
    uint16_t odr_min;
    uint16_t odr_max = 16000;
    uint16_t odr = 0;
    uint16_t shift = 0;

    if(_Rarely(odr_target==0)){
        *odr_support = 0;
        return DRV_RC_OK;
    }

    if(sensor_type==BMI160_SENSOR_GYRO)
        odr_min = 250;
    else
        odr_min =125;

    odr = odr_min;
    for(shift = 0; odr < odr_max;){
        if(odr_target <= odr)
            break;
        shift++;
        odr = (odr_min<<shift);
    }
    *odr_support = odr;

#if BMI160_ENABLE_DOUBLE_TAPPING
    /* do not change odr of accel if double tapping is enalbed
     * in no motion state
     */
    if(sensor_type==BMI160_SENSOR_ACCEL &&
       p_bmi160_rt->int_en[BMI160_OFFSET_DOUBLE_TAP] &&
       !p_bmi160_rt->motion_state &&
       odr!=p_bmi160_rt->sensor_odr[BMI160_SENSOR_ACCEL]){
        *odr_support = p_bmi160_rt->sensor_odr[BMI160_SENSOR_ACCEL];
    }
#endif

#if DEBUG_BMI160
    if(*odr_support!=odr_target) {
        pr_debug(LOG_MODULE_BMI160, "target odr[%d/10]HZ, supported odr[%d/10]HZ", odr_target, *odr_support);
	}
#endif

    return DRV_RC_OK;
}

DRIVER_API_RC bmi160_sensor_set_rate(uint16_t odr_hz_x10, uint8_t sensor_type)
{
    uint8_t v_output_data_rate_u8 = 0;
    DRIVER_API_RC com_rslt = 0;
    uint8_t powermode = BMI160_POWER_SUSPEND;

    if (sensor_type >= BMI160_SENSOR_COUNT)
        return DRV_RC_FAIL;

    if (odr_hz_x10 == p_bmi160_rt->sensor_odr[sensor_type])
        return DRV_RC_OK;

    if (odr_hz_x10 == 0) {
        /* disable the sensor if set output data rate to 0 */
        if (p_bmi160_rt->sensor_odr[sensor_type]) {
            /* If accel will be set to 0, but motion is need, do not suspend accel */
            if(sensor_type==BMI160_SENSOR_ACCEL && p_bmi160_rt->motion_callback)
                return DRV_RC_OK;
            if(bmi160_change_sensor_powermode(sensor_type, BMI160_POWER_SUSPEND))
                return DRV_RC_FAIL;
        }
        p_bmi160_rt->sensor_odr[sensor_type] = 0;
        if(sensor_type == BMI160_SENSOR_ACCEL)
            p_bmi160_rt->accel_config = 0;
        return DRV_RC_OK;
    }

    /* The MACRO value is the same of ACCEL/GYRO/MAG @ same rate */
    switch(odr_hz_x10) {
    case 125:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ;
        break;
    case 250:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ;
        break;
    case 500:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ;
        break;
    case 1000:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ;
        break;
    case 2000:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ;
        break;
    case 4000:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ;
        break;
    case 8000:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ;
        break;
    case 16000:
        v_output_data_rate_u8 = BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ;
        break;
    default:
        goto ODR_NOT_SUPPORT;
    }

    powermode = BMI160_POWER_NORMAL;


    if(sensor_type==BMI160_SENSOR_ACCEL) {
#ifdef CONFIG_BMI160_PM
        /* Default will set accel to low power mode */
        powermode = BMI160_POWER_LOWPOWER;
        v_output_data_rate_u8 |= ((p_bmi160_rt->undersampling_avg)<<4);
#else
        v_output_data_rate_u8 |= (BMI160_ACCEL_NORMAL_AVG4<<4);
#endif
        p_bmi160_rt->accel_config = v_output_data_rate_u8;
    } else if(sensor_type==BMI160_SENSOR_GYRO) {
        if(odr_hz_x10 < 250)
            goto ODR_NOT_SUPPORT;
        /* Set the gryo bandwidth as Normal along with odr, no undersampling */
        v_output_data_rate_u8 |= (BMI160_GYRO_NORMAL_MODE<<4);
    }
#if BMI160_ENABLE_MAG
    else if(sensor_type==BMI160_SENSOR_MAG) {
        if(odr_hz_x10>8000)
            goto ODR_NOT_SUPPORT;
    }
#endif

    if(bmi16_set_user_sensor_config(sensor_type, powermode, 1, v_output_data_rate_u8))
        return DRV_RC_FAIL;

    p_bmi160_rt->sensor_odr[sensor_type] = odr_hz_x10;

    return com_rslt;

ODR_NOT_SUPPORT:
    pr_debug(LOG_MODULE_BMI160, "sensor[%u] doesn't support ODR (%u/10)", sensor_type, odr_hz_x10);
    return DRV_RC_FAIL;
}

DRIVER_API_RC bmi160_fifo_enable(uint8_t sensor_type, uint8_t *buffer,
                                 uint16_t buffer_len, uint8_t fifo_en_mask)
{
    DRIVER_API_RC com_rslt = 0;
    uint8_t int_enable_mask = (1<<sensor_type);

    if((fifo_config1 & 0xE0) ==0){
        bmi160_flush_fifo();
        bmi160_delay_ms(1);
    }

    p_bmi160_rt->fifo_en[sensor_type] = 1;
    p_bmi160_rt->fifo_ubuffer[sensor_type] = buffer;
    p_bmi160_rt->fifo_ubuffer_len[sensor_type] = buffer_len;

    fifo_config1 |= fifo_en_mask;

    com_rslt+= bmi160_write_reg(BMI160_USER_FIFO_CONFIG_1_ADDR, &fifo_config1);
    bmi160_delay_ms(1);

#if BMI160_ENABLE_DOUBLE_TAPPING
    /* fifo full interrupt is used to show data lost */
    com_rslt += bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, int_enable_mask);
#endif

    /* If fifo is enabled for one sensor,
     * the interrupt mode register data report will be disabled
     */
    if (p_bmi160_rt->read_callbacks[sensor_type])
        com_rslt += bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_DRDY, ~int_enable_mask);

    com_rslt+= bmi160_update_fifo_watermark();
    return com_rslt;
}

DRIVER_API_RC bmi160_fifo_disable(uint8_t sensor_type, uint8_t fifo_disable_mask)
{
    DRIVER_API_RC com_rslt = 0;

    if(sensor_type >= BMI160_SENSOR_COUNT)
        return com_rslt;

    uint8_t int_enable_mask = (1<<sensor_type);

    p_bmi160_rt->fifo_en[sensor_type] = 0;
    fifo_config1 &= fifo_disable_mask;


    com_rslt+= bmi160_write_reg(BMI160_USER_FIFO_CONFIG_1_ADDR, &fifo_config1);
    bmi160_delay_ms(1);

#if BMI160_ENABLE_DOUBLE_TAPPING
    com_rslt += bmi160_int_disable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, ~int_enable_mask);
#endif

    if (p_bmi160_rt->read_callbacks[sensor_type])
        com_rslt += bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_DRDY, int_enable_mask);

    com_rslt+= bmi160_update_fifo_watermark();

    p_bmi160_rt->watermark_trigger_cnt[sensor_type] = 0;
    p_bmi160_rt->wm_callback[sensor_type] = NULL;
    p_bmi160_rt->wm_cb_priv_data[sensor_type] = NULL;

    p_bmi160_rt->fifo_ubuffer[sensor_type] = NULL;
    p_bmi160_rt->fifo_ubuffer_len[sensor_type] = 0;
    return com_rslt;
}

int bmi160_accle_read_data(uint8_t * buf, uint16_t buff_len)
{
    if (_Rarely(!p_bmi160_rt->sensor_odr[BMI160_SENSOR_ACCEL]))
        return 0;
    if (bmi160_read_accel_xyz((struct bmi160_s16_xyz_t *)buf, BMI160_SENSOR_ACCEL))
        return 0;
#ifdef CONFIG_BMI160_CONVERT_ACCEL_DATA
    bmi160_convert_accel_data(buf);
#endif
    return sizeof(struct bmi160_accel_t);
}

int bmi160_gyro_read_data(uint8_t * buf, uint16_t buff_len)
{
    if (_Rarely(!p_bmi160_rt->sensor_odr[BMI160_SENSOR_GYRO]))
        return 0;
    if (bmi160_read_gyro_xyz((struct bmi160_s32_xyz_t *)buf, BMI160_SENSOR_GYRO))
        return 0;
#ifdef CONFIG_BMI160_CONVERT_GYRO_DATA
    bmi160_convert_gyro_data(buf);
#endif
    return sizeof(struct bmi160_gyro_t);
}

static inline int bmi160_abstract_fifo_data(uint8_t type, uint8_t size,
                                            uint8_t *out_buf, uint16_t frame_cnt_max, uint8_t *out_index,
                                            uint16_t *fifo_index)
{
    if(out_buf && (*out_index < frame_cnt_max)){
#if BMI160_ENABLE_MAG
        if(type==BMI160_SENSOR_MAG)
            return p_bmi160_rt->parse_mag_sensor_data(out_buf, out_index, fifo_index);
        else
#endif
        if(type==BMI160_SENSOR_ACCEL)
            return parse_accel_xyz_data(out_buf, out_index, fifo_index);
        else
            return parse_gyro_xyz_data(out_buf, out_index, fifo_index);
    }

    if(!p_bmi160_rt->fifo_en[type]){
#if BMI160_ENABLE_MAG
        if(type==BMI160_SENSOR_MAG)
            *fifo_index+=8;
        else
#endif
            *fifo_index+=6;
        pr_debug(LOG_MODULE_BMI160, "fifo disabled type[%d], abandon data", type);
        return 0;
    }

    return FIFO_BUFFER_OVERFLOW;
}

static int parse_data_from_fifo(uint8_t *buffers[3], uint16_t frame_cnt_max[3], uint8_t actual_frame[3])
{
    uint16_t fifo_index = 0;
    uint8_t frame_head;
    int8_t last_stat = 0;
    uint8_t read_mask = 0;
    uint16_t fifo_index_save = 0;

    uint8_t *buf_accel = buffers[BMI160_SENSOR_ACCEL];
    uint8_t *buf_gyro  = buffers[BMI160_SENSOR_GYRO];

    uint8_t *accel_index = &actual_frame[BMI160_SENSOR_ACCEL];
    uint8_t *gyro_index = &actual_frame[BMI160_SENSOR_GYRO];

#if BMI160_ENABLE_MAG
    uint8_t *buf_mag = buffers[BMI160_SENSOR_MAG];
    uint8_t *mag_index = &actual_frame[BMI160_SENSOR_MAG];
#endif

    for (fifo_index = bmi160_fifo_data_start; fifo_index < bmi160_fifo_data_end;) {
        frame_head = bmi160_fifo_data[fifo_index];
        fifo_index = fifo_index + 1;
        read_mask = 0;
        switch(frame_head) {
        case FIFO_HEAD_A:
            read_mask = TYPE_MASK_ACCEL;
            break;
        case FIFO_HEAD_G:
            read_mask = TYPE_MASK_GYRO;
            break;
        case FIFO_HEAD_G_A:
            read_mask = TYPE_MASK_ACCEL_GYRO;
            break;
#if BMI160_ENABLE_MAG
        case FIFO_HEAD_M:
            read_mask = TYPE_MASK_MAG;
            break;
            /* Header frame of mag and accel */
        case FIFO_HEAD_M_A:
            read_mask = TYPE_MASK_ACCEL_MAG;
            break;
        /* Header frame of mag and gyro */
        case FIFO_HEAD_M_G:
            read_mask = TYPE_MASK_GYRO_MAG;
            break;
        /* Header frame of mag, gyro and accel */
        case FIFO_HEAD_M_G_A:
            read_mask = TYPE_MASK_ACCEL_GYRO_MAG;
            break;
#endif
            /* Header frame of skip frame */
        case FIFO_HEAD_SKIP_FRAME:
        case FIFO_HEAD_INPUT_CONFIG:
            if (fifo_index + 1 > bmi160_fifo_data_end) {
                last_stat = FIFO_SKIP_OVER_LEN;
                break;
            }
            fifo_index = fifo_index + 1;
            break;
        /* Header frame of over read fifo v_data_u8 */
        case FIFO_HEAD_SENSOR_TIME:
        case FIFO_HEAD_OVER_READ_LSB:
            last_stat = FIFO_OVER_READ_RETURN;
            break;
        default:
            pr_debug(LOG_MODULE_BMI160, "unknown head:0x%x\n", frame_head);
            last_stat = 1;
            break;
        }

        int mix_frame_size = 0;
        if(read_mask & TYPE_MASK_MAG)
            mix_frame_size += BMI160_MAG_RAW_DATA_SIZE;
        if(read_mask & TYPE_MASK_GYRO)
            mix_frame_size += BMI160_GYRO_RAW_DATA_SIZE;
        if(read_mask & TYPE_MASK_ACCEL)
            mix_frame_size += BMI160_ACCEL_RAW_DATA_SIZE;
        if(fifo_index+mix_frame_size > bmi160_fifo_data_end)
            last_stat = FIFO_M_G_A_OVER_LEN;

        fifo_index_save = fifo_index;
#if BMI160_ENABLE_MAG
        if(!last_stat && (read_mask & TYPE_MASK_MAG))
            last_stat = bmi160_abstract_fifo_data(BMI160_SENSOR_MAG, BMI160_MAG_RAW_DATA_SIZE,
                                                  buf_mag, frame_cnt_max[BMI160_SENSOR_MAG], mag_index,
                                                  &fifo_index);
#endif

        if(!last_stat && (read_mask & TYPE_MASK_GYRO))
            last_stat = bmi160_abstract_fifo_data(BMI160_SENSOR_GYRO, BMI160_GYRO_RAW_DATA_SIZE,
                                                  buf_gyro, frame_cnt_max[BMI160_SENSOR_GYRO], gyro_index,
                                                  &fifo_index);

        if(!last_stat && (read_mask & TYPE_MASK_ACCEL))
            last_stat = bmi160_abstract_fifo_data(BMI160_SENSOR_ACCEL, BMI160_ACCEL_RAW_DATA_SIZE,
                                                  buf_accel, frame_cnt_max[BMI160_SENSOR_ACCEL], accel_index,
                                                  &fifo_index);

        if(last_stat)
            break;
    }

    if(last_stat==FIFO_BUFFER_OVERFLOW)
        bmi160_fifo_data_start = fifo_index_save-1;
    else{
        bmi160_fifo_data_start = 0;
        bmi160_fifo_data_end = 0;
    }

    return last_stat;
}

DRIVER_API_RC bmi160_read_fifo_data(uint8_t *buffers[3], uint16_t buffer_len[3], uint8_t actual_frame[3])
{
    uint32_t fifo_len;
    uint16_t frame_cnt_max[3];
    int ret_parse = 0;
    uint16_t read_len = 0;
    frame_cnt_max[BMI160_SENSOR_ACCEL] = buffer_len[BMI160_SENSOR_ACCEL]/BMI160_ACCEL_FRAME_SIZE;
    frame_cnt_max[BMI160_SENSOR_GYRO] = buffer_len[BMI160_SENSOR_GYRO]/BMI160_GYRO_FRAME_SIZE;
#if BMI160_ENABLE_MAG
    frame_cnt_max[BMI160_SENSOR_MAG] = buffer_len[BMI160_SENSOR_MAG]/BMI160_MAG_FRAME_SIZE;
#endif

    /* handle data left in buffer */
    if(bmi160_fifo_data_start!=bmi160_fifo_data_end){
        ret_parse = parse_data_from_fifo(buffers, frame_cnt_max, actual_frame);
        if(ret_parse==FIFO_BUFFER_OVERFLOW){
            goto data_convert;
        }
    }

    bmi160_fifo_data_start = 0;

    if(p_bmi160_rt->fifo_en[BMI160_SENSOR_ACCEL])
        read_len += (frame_cnt_max[BMI160_SENSOR_ACCEL]-actual_frame[BMI160_SENSOR_ACCEL])*(BMI160_ACCEL_RAW_DATA_SIZE+1);
    if(p_bmi160_rt->fifo_en[BMI160_SENSOR_GYRO])
        read_len += (frame_cnt_max[BMI160_SENSOR_GYRO]-actual_frame[BMI160_SENSOR_GYRO])*(BMI160_GYRO_RAW_DATA_SIZE+1);
#if BMI160_ENABLE_MAG
    if(p_bmi160_rt->fifo_en[BMI160_SENSOR_MAG])
        read_len += (frame_cnt_max[BMI160_SENSOR_MAG]-actual_frame[BMI160_SENSOR_MAG])*(BMI160_MAG_RAW_DATA_SIZE+1);
#endif
    bmi160_fifo_length(&fifo_len);

    if(read_len > fifo_len)
        read_len = fifo_len;

    if(read_len > FIFO_FRAME)
        read_len = FIFO_FRAME;

    pr_debug(LOG_MODULE_BMI160, "read %d byte from hw fifo", read_len);

    bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_NORMAL);
    if(bmi160_get_fifo_data(&bmi160_fifo_data[0], read_len)){
        bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_LOWPOWER);
        return DRV_RC_FAIL;
    }
    bmi160_change_sensor_powermode(BMI160_SENSOR_ACCEL, BMI160_POWER_LOWPOWER);
    bmi160_fifo_data_end = read_len;

    ret_parse = parse_data_from_fifo(buffers, frame_cnt_max, actual_frame);
#if DEBUG_BMI160
    if(ret_parse==FIFO_BUFFER_OVERFLOW) {
      pr_debug(LOG_MODULE_BMI160, "reserve fifo data, start=%d, end=%d", bmi160_fifo_data_start, bmi160_fifo_data_end);
    }
#endif

data_convert:
    for(int type=0; type<BMI160_SENSOR_COUNT; type++){
        if(buffers[type] && p_bmi160_rt->fifo_en[type]){
            uint8_t frame_len;
#if BMI160_ENABLE_MAG
            if(type==BMI160_SENSOR_MAG)
                frame_len = BMI160_MAG_FRAME_SIZE;
            else
#endif
            if(type==BMI160_SENSOR_ACCEL)
                frame_len = BMI160_ACCEL_FRAME_SIZE;
            else
                frame_len = BMI160_GYRO_FRAME_SIZE;

            if(p_bmi160_rt->convert_data_funs[type]){
                for(int i=0; i<actual_frame[type]; i++)
                    p_bmi160_rt->convert_data_funs[type](buffers[type]+i*frame_len);
            }
        }
    }

#if !BMI160_ENABLE_DOUBLE_TAPPING
    if(bmi160_any_motion_detected){
        uint8_t int_enable_mask = 0;
        for(uint8_t sensor_type=BMI160_SENSOR_ACCEL; sensor_type<BMI160_SENSOR_COUNT; sensor_type++){
            if(p_bmi160_rt->fifo_en[sensor_type])
                int_enable_mask = (1<<sensor_type);
        }
        bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_FULL, int_enable_mask);
        bmi160_any_motion_detected = 0;
    }
#else
    if(bmi160_wait_double_tap){
        if(get_uptime_ms()-bmi160_any_motion_timestamp >= 600){
            bmi160_wait_double_tap = 0;
            bmi16_set_user_sensor_config(BMI160_SENSOR_ACCEL, BMI160_POWER_LOWPOWER, 1, p_bmi160_rt->accel_config);
        }
    }
#endif

    if(bmi160_need_update_wm_interrupt){
        uint8_t wm_enable_mask = 0;
            for(int i=0; i<BMI160_SENSOR_COUNT; i++){
                if(p_bmi160_rt->fifo_en[i])
                    wm_enable_mask |= (1<<i);
            }
        bmi160_int_enable(BMI160_INT_SET_1, BMI160_OFFSET_FIFO_WM, wm_enable_mask);
    }

    return DRV_RC_OK;
}

int bmi160_sensor_read_fifo(uint8_t sensor_type, uint8_t *buf, uint16_t buff_len)
{
    uint8_t frame_cnt[3] = {0, 0, 0};
    uint8_t *sensor_buffer[3] = {};
    uint16_t buff_lens[3] = {};
    uint8_t frame_size[3] = {BMI160_ACCEL_FRAME_SIZE, BMI160_GYRO_FRAME_SIZE, BMI160_MAG_FRAME_SIZE};
    int actual_len;
    int i;

    if(!p_bmi160_rt->fifo_en[sensor_type]){
        pr_debug(LOG_MODULE_BMI160, "fifo is not enabled!!");
        return 0;
    }

    if(buf && p_bmi160_rt->fifo_ubuffer[sensor_type]
           && buf!=p_bmi160_rt->fifo_ubuffer[sensor_type]){
        pr_debug(LOG_MODULE_BMI160, "[%d]using buffer previous assigned", sensor_type);
    }else if(!buf && !p_bmi160_rt->fifo_ubuffer[sensor_type]){
        pr_debug(LOG_MODULE_BMI160, "[%d]no usable buffer", sensor_type);
        return 0;
    }

    for(i=0; i<BMI160_SENSOR_COUNT; i++){
        sensor_buffer[i] = p_bmi160_rt->fifo_ubuffer[i] + p_bmi160_rt->fifo_ubuffer_ptr[i];
        buff_lens[i] = p_bmi160_rt->fifo_ubuffer_len[i] - p_bmi160_rt->fifo_ubuffer_ptr[i];
    }

    if(!p_bmi160_rt->fifo_ubuffer[sensor_type]){
        sensor_buffer[sensor_type] = buf;
        buff_lens[sensor_type] = buff_len;
    }

    /* only read hw fifo for one type, data of other types will be read out together */
    for(i=0; i<BMI160_SENSOR_COUNT; i++){
        if(p_bmi160_rt->sensor_odr[i] && p_bmi160_rt->fifo_en[i])
            break;
    }
    if(sensor_type==i){
        bmi160_read_fifo_data(sensor_buffer, buff_lens, frame_cnt);
        for(i=0; i<BMI160_SENSOR_COUNT; i++)
            p_bmi160_rt->fifo_ubuffer_ptr[i] += frame_cnt[i]*frame_size[i];
    }

    actual_len = p_bmi160_rt->fifo_ubuffer_ptr[sensor_type];
    /* reset pointer after each polling read */
    p_bmi160_rt->fifo_ubuffer_ptr[sensor_type] = 0;
    return actual_len;
}

#if BMI160_SUPPORT_FIFO_INT_DATA_REPORT
DRIVER_API_RC bmi160_read_fifo_header_data(uint16_t fifo_read_len)
{
    DRIVER_API_RC com_rslt = DRV_RC_FAIL;
    uint8_t frame_cnt[3] = {};
    uint8_t *sensor_buffer[3] = {NULL, NULL, NULL};
    uint16_t buff_lens[3] = {FIFO_FRAME_CNT*BMI160_ACCEL_FRAME_SIZE,
                             FIFO_FRAME_CNT*BMI160_GYRO_FRAME_SIZE,
                             MAG_FIFO_FRAME_CNT*BMI160_MAG_FRAME_SIZE};


    sensor_buffer[BMI160_SENSOR_ACCEL] = (uint8_t*)bmi160_accel_fifo;
    sensor_buffer[BMI160_SENSOR_GYRO] = (uint8_t*)bmi160_gyro_fifo;
#if BMI160_ENABLE_MAG
    sensor_buffer[BMI160_SENSOR_MAG] = (uint8_t*)bmi160_mag_fifo;
#endif

    com_rslt = bmi160_read_fifo_data(sensor_buffer, buff_lens, frame_cnt);

    bmi160_accel_index = frame_cnt[BMI160_SENSOR_ACCEL];
    bmi160_gyro_index = frame_cnt[BMI160_SENSOR_GYRO];
#if BMI160_ENABLE_MAG
    bmi160_mag_index = frame_cnt[BMI160_SENSOR_MAG];
#endif

    return com_rslt;
}
#endif

void bmi160_convert_accel_data(uint8_t *buff)
{
    struct bmi160_accel_t *data = (struct bmi160_accel_t *)buff;
    int32_t x = data->x;
    int32_t y = data->y;
    int32_t z = data->z;

#if BMI160_ACCEL_REPORT_MG_PER_SECOND
    data->x = (int16_t) ((x * 1000) >> acc_sensitivity_shift);
    data->y = (int16_t) ((y * 1000) >> acc_sensitivity_shift);
    data->z = (int16_t) ((z * 1000) >> acc_sensitivity_shift);
#else
    data->x = (int16_t) ((x * ACCEL_CONVERT_CONST) >> acc_sensitivity_shift);
    data->y = (int16_t) ((y * ACCEL_CONVERT_CONST) >> acc_sensitivity_shift);
    data->z = (int16_t) ((z * ACCEL_CONVERT_CONST) >> acc_sensitivity_shift);
#endif
}

void bmi160_convert_gyro_data(uint8_t *buff)
{
    struct bmi160_gyro_t *data = (struct bmi160_gyro_t *)buff;
    int32_t x = data->x;
    int32_t y = data->y;
    int32_t z = data->z;

    data->x = (x * gyro_convert_numerator) / gyro_convert_denominator;
    data->y = (y * gyro_convert_numerator) / gyro_convert_denominator;
    data->z = (z * gyro_convert_numerator) / gyro_convert_denominator;
}

inline struct bmi160_rt_t *bmi160_get_ptr(void)
{
    return  p_bmi160_rt;
}
