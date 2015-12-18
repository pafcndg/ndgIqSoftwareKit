/*
****************************************************************************
* Copyright (C) 2013 - 2014 Bosch Sensortec GmbH
*
* File : bme280.h
*
* Date : 2014/07/21
*
* Revision : 2.0(Pressure and Temperature compensation code revision is 1.1
*               and Humidity compensation code revision is 1.0)
*
* Usage: Sensor Driver for BME280 sensor
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/

#ifndef __BME280_H__
#define __BME280_H__

#include <stdint.h>
#include <limits.h>

/* If the user wants to support floating point calculations, please set
    the following #define. If floating point
    calculation is not wanted or allowed
    (e.g. in Linux kernel), please do not set the define. */
/*#define BME280_ENABLE_FLOAT*/
/* If the user wants to support 64 bit integer calculation
    (needed for optimal pressure accuracy) please set
    the following #define. If int64 calculation is not wanted
    (e.g. because it would include
    large libraries), please do not set the define. */
//#define BME280_ENABLE_INT64

//#define BME280_ENABLE_FLOAT

/** defines the return parameter type of the BME280_WR_FUNCTION */
#define BME280_BUS_WR_RETURN_TYPE int8_t

/**\brief links the order of parameters defined in
BME280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BME280_BUS_WR_PARAM_TYPES uint8_t, uint8_t,\
        uint8_t *, uint8_t

/**\brief links the order of parameters defined in
BME280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BME280_BUS_WR_PARAM_ORDER(device_addr, register_addr,\
        register_data, wr_len)

/* never change this line */
#define BME280_BUS_WRITE_FUNC(device_addr, register_addr,\
register_data, wr_len) bus_write(device_addr, register_addr,\
        register_data, wr_len)

/**\brief defines the return parameter type of the BME280_RD_FUNCTION
*/
#define BME280_BUS_RD_RETURN_TYPE int8_t

/**\brief defines the calling parameter types of the BME280_RD_FUNCTION
*/
#define BME280_BUS_RD_PARAM_TYPES (uint8_t, uint8_t,\
        uint8_t *, uint8_t)

/**\brief links the order of parameters defined in \
BME280_BUS_RD_PARAM_TYPE to function calls used inside the API
*/
#define BME280_BUS_RD_PARAM_ORDER (device_addr, register_addr,\
        register_data)

/* never change this line */
#define BME280_BUS_READ_FUNC(device_addr, register_addr,\
        register_data, rd_len)bus_read(device_addr, register_addr,\
        register_data, rd_len)

/**\brief defines the return parameter type of the BME280_DELAY_FUNCTION
*/
#define BME280_DELAY_RETURN_TYPE void

/**\brief defines the calling parameter types of the BME280_DELAY_FUNCTION
*/
#define BME280_DELAY_PARAM_TYPES uint16_t

/* never change this line */
#define BME280_DELAY_FUNC(delay_in_msec)\
        delay_func(delay_in_msec)

#define BME280_GET_BITSLICE(regvar, bitname)\
        ((regvar & bitname##__MSK) >> bitname##__POS)

#define BME280_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


/* Constants */
#define BME280_NULL                          0
#define BME280_RETURN_FUNCTION_TYPE          int8_t

#define SHIFT_RIGHT_4_POSITION               4
#define SHIFT_LEFT_2_POSITION                2
#define SHIFT_LEFT_4_POSITION                4
#define SHIFT_LEFT_5_POSITION                5
#define SHIFT_LEFT_8_POSITION                8
#define SHIFT_LEFT_12_POSITION               12
#define SHIFT_LEFT_16_POSITION               16
#define BME280_Four_U8X                      4
#define BME280_Zero_U8X                      0
#define BME280_Eight_U8X                     8
#define SUCCESS                             ((uint8_t)0)

#define E_BME280_NULL_PTR                   ((int8_t)-127)
#define E_BME280_COMM_RES                   ((int8_t)-1)
#define E_BME280_OUT_OF_RANGE               ((int8_t)-2)
#define E_BME280_REINIT                     ((int8_t)-3)

#define BME280_I2C_ADDRESS1                  0x76
#define BME280_I2C_ADDRESS2                  0x77

/* Sensor Specific constants */
#define BME280_SLEEP_MODE                    0x00
#define BME280_FORCED_MODE                   0x01
#define BME280_NORMAL_MODE                   0x03
#define BME280_SOFT_RESET_CODE               0xB6

#define BME280_STANDBYTIME_0_5_MS            0x00
#define BME280_STANDBYTIME_62_5_MS           0x01
#define BME280_STANDBYTIME_125_MS            0x02
#define BME280_STANDBYTIME_250_MS            0x03
#define BME280_STANDBYTIME_500_MS            0x04
#define BME280_STANDBYTIME_1000_MS           0x05
#define BME280_STANDBYTIME_10_MS             0x06
#define BME280_STANDBYTIME_20_MS             0x07

#define BME280_OVERSAMPLING_SKIPPED          0x00
#define BME280_OVERSAMPLING_1X               0x01
#define BME280_OVERSAMPLING_2X               0x02
#define BME280_OVERSAMPLING_4X               0x03
#define BME280_OVERSAMPLING_8X               0x04
#define BME280_OVERSAMPLING_16X              0x05

#define BME280_STANDARD_OSRS_H               BME280_OVERSAMPLING_1X
#define BME280_FILTERCOEFF_OFF               0x00
#define BME280_FILTERCOEFF_2                 0x01
#define BME280_FILTERCOEFF_4                 0x02
#define BME280_FILTERCOEFF_8                 0x03
#define BME280_FILTERCOEFF_16                0x04

#define T_INIT_MAX                             20
        /* 20/16 = 1.25 ms */
#define T_MEASURE_PER_OSRS_MAX                 37
        /* 37/16 = 2.3125 ms*/

#define T_SETUP_PRESSURE_MAX                   10
        /* 10/16 = 0.625 ms */

#define T_SETUP_HUMIDITY_MAX                   10
        /* 10/16 = 0.625 ms */

typedef enum bme280_work_mode_t {
    WEATHER_MONITOR,
    HUMIDITY_SENSING,
    INDOOR_NAVIGATION,
    GAMING,
    ERROR_MD
}BME280_WK_MD;

/*calibration parameters */
#define BME280_DIG_T1_LSB_REG                0x88
#define BME280_DIG_T1_MSB_REG                0x89
#define BME280_DIG_T2_LSB_REG                0x8A
#define BME280_DIG_T2_MSB_REG                0x8B
#define BME280_DIG_T3_LSB_REG                0x8C
#define BME280_DIG_T3_MSB_REG                0x8D
#define BME280_DIG_P1_LSB_REG                0x8E
#define BME280_DIG_P1_MSB_REG                0x8F
#define BME280_DIG_P2_LSB_REG                0x90
#define BME280_DIG_P2_MSB_REG                0x91
#define BME280_DIG_P3_LSB_REG                0x92
#define BME280_DIG_P3_MSB_REG                0x93
#define BME280_DIG_P4_LSB_REG                0x94
#define BME280_DIG_P4_MSB_REG                0x95
#define BME280_DIG_P5_LSB_REG                0x96
#define BME280_DIG_P5_MSB_REG                0x97
#define BME280_DIG_P6_LSB_REG                0x98
#define BME280_DIG_P6_MSB_REG                0x99
#define BME280_DIG_P7_LSB_REG                0x9A
#define BME280_DIG_P7_MSB_REG                0x9B
#define BME280_DIG_P8_LSB_REG                0x9C
#define BME280_DIG_P8_MSB_REG                0x9D
#define BME280_DIG_P9_LSB_REG                0x9E
#define BME280_DIG_P9_MSB_REG                0x9F

#define BME280_DIG_H1_REG                    0xA1

#define BME280_DIG_H2_LSB_REG                0xE1
#define BME280_DIG_H2_MSB_REG                0xE2
#define BME280_DIG_H3_REG                    0xE3
#define BME280_DIG_H4_MSB_REG                0xE4
#define BME280_DIG_H5_LSB_H4_LSB_REG         0xE5
#define BME280_DIG_H5_MSB_REG                0xE6
#define BME280_DIG_H6_REG                    0xE7


#define BME280_CHIPID_REG                    0xD0  /*Chip ID Register */
#define BME280_RESET_REG                     0xE0  /*Softreset Register */
#define BME280_STATUS_REG                    0xF3  /*Status Register */
#define BME280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */
#define BME280_CTRLHUM_REG                   0xF2  /*Ctrl Humidity Register*/
#define BME280_CONFIG_REG                    0xF5  /*Configuration Register */
#define BME280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */
#define BME280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */
#define BME280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */
#define BME280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */
#define BME280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */
#define BME280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */
#define BME280_HUMIDITY_MSB_REG              0xFD  /*Humidity MSB Reg */
#define BME280_HUMIDITY_LSB_REG              0xFE  /*Humidity LSB Reg */

/* Status Register */
#define BME280_STATUS_REG_MEASURING__POS           3
#define BME280_STATUS_REG_MEASURING__MSK           0x08
#define BME280_STATUS_REG_MEASURING__LEN           1
#define BME280_STATUS_REG_MEASURING__REG           BME280_STATUS_REG

#define BME280_STATUS_REG_IMUPDATE__POS            0
#define BME280_STATUS_REG_IMUPDATE__MSK            0x01
#define BME280_STATUS_REG_IMUPDATE__LEN            1
#define BME280_STATUS_REG_IMUPDATE__REG            BME280_STATUS_REG

/* Control Measurement Register */
#define BME280_CTRLMEAS_REG_OSRST__POS             5
#define BME280_CTRLMEAS_REG_OSRST__MSK             0xE0
#define BME280_CTRLMEAS_REG_OSRST__LEN             3
#define BME280_CTRLMEAS_REG_OSRST__REG             BME280_CTRLMEAS_REG

#define BME280_CTRLMEAS_REG_OSRSP__POS             2
#define BME280_CTRLMEAS_REG_OSRSP__MSK             0x1C
#define BME280_CTRLMEAS_REG_OSRSP__LEN             3
#define BME280_CTRLMEAS_REG_OSRSP__REG             BME280_CTRLMEAS_REG

#define BME280_CTRLMEAS_REG_MODE__POS              0
#define BME280_CTRLMEAS_REG_MODE__MSK              0x03
#define BME280_CTRLMEAS_REG_MODE__LEN              2
#define BME280_CTRLMEAS_REG_MODE__REG              BME280_CTRLMEAS_REG

#define BME280_CTRLHUM_REG_OSRSH__POS              0
#define BME280_CTRLHUM_REG_OSRSH__MSK              0x07
#define BME280_CTRLHUM_REG_OSRSH__LEN              3
#define BME280_CTRLHUM_REG_OSRSH__REG              BME280_CTRLHUM_REG

/* Configuration Register */
#define BME280_CONFIG_REG_TSB__POS                 5
#define BME280_CONFIG_REG_TSB__MSK                 0xE0
#define BME280_CONFIG_REG_TSB__LEN                 3
#define BME280_CONFIG_REG_TSB__REG                 BME280_CONFIG_REG

#define BME280_CONFIG_REG_FILTER__POS              2
#define BME280_CONFIG_REG_FILTER__MSK              0x1C
#define BME280_CONFIG_REG_FILTER__LEN              3
#define BME280_CONFIG_REG_FILTER__REG              BME280_CONFIG_REG

#define BME280_CONFIG_REG_SPI3WEN__POS             0
#define BME280_CONFIG_REG_SPI3WEN__MSK             0x01
#define BME280_CONFIG_REG_SPI3WEN__LEN             1
#define BME280_CONFIG_REG_SPI3WEN__REG             BME280_CONFIG_REG

/* Data Register */
#define BME280_PRESSURE_XLSB_REG_DATA__POS         4
#define BME280_PRESSURE_XLSB_REG_DATA__MSK         0xF0
#define BME280_PRESSURE_XLSB_REG_DATA__LEN         4
#define BME280_PRESSURE_XLSB_REG_DATA__REG         BME280_PRESSURE_XLSB_REG

#define BME280_TEMPERATURE_XLSB_REG_DATA__POS      4
#define BME280_TEMPERATURE_XLSB_REG_DATA__MSK      0xF0
#define BME280_TEMPERATURE_XLSB_REG_DATA__LEN      4
#define BME280_TEMPERATURE_XLSB_REG_DATA__REG      BME280_TEMPERATURE_XLSB_REG

#define BME280_WR_FUNC_PTR\
        int8_t (*bus_write)(uint8_t, uint8_t,\
        uint8_t *, uint8_t)

#define BME280_RD_FUNC_PTR\
        int8_t (*bus_read)(uint8_t, uint8_t,\
        uint8_t *, uint8_t)

#define BME280_MDELAY_DATA_TYPE uint16_t
/** this structure holds all device specific calibration parameters */
struct bme280_calibration_param_t {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t  dig_H1;
    int16_t dig_H2;
    uint8_t  dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t  dig_H6;

    int32_t t_fine;
};
/** BME280 image registers data structure */
struct bme280_t {
    struct bme280_calibration_param_t cal_param;

    uint8_t chip_id;
    uint8_t dev_addr;

    uint8_t osrs_t;
    uint8_t osrs_p;
    uint8_t osrs_h;
    uint8_t t_sb;
    uint8_t filter;
    uint8_t mode;

    uint32_t t_period_us;
};

BME280_RETURN_FUNCTION_TYPE __bme280_init(struct bme280_t *bme280);

BME280_RETURN_FUNCTION_TYPE bme280_read_t(int32_t *temperature);

BME280_RETURN_FUNCTION_TYPE bme280_read_p(uint32_t *pressure);

BME280_RETURN_FUNCTION_TYPE bme280_read_h(uint32_t *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_read_pth(uint32_t *pressure,
int32_t *temperature, uint32_t *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_get_forced_pth(uint32_t *pressure,
int32_t *temperature, uint32_t *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_read_t_double(double *temperature);

BME280_RETURN_FUNCTION_TYPE bme280_read_p_double(double *pressure);

BME280_RETURN_FUNCTION_TYPE bme280_read_h_double(double *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_read_pth_double(double *pressure,
double *temperature, double *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_get_forced_pth_double(double *pressure,
double *temperature, double *humidity);

BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_t(uint8_t *value);

BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_t(uint8_t value);

BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_p(uint8_t *value);

BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_p(uint8_t value);

BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_h(uint8_t *value);

BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_h(uint8_t value);

BME280_RETURN_FUNCTION_TYPE bme280_get_mode(uint8_t *mode);

BME280_RETURN_FUNCTION_TYPE bme280_set_mode(uint8_t mode);

BME280_RETURN_FUNCTION_TYPE bme280_set_softreset(void);

BME280_RETURN_FUNCTION_TYPE bme280_get_filter(uint8_t *value);

BME280_RETURN_FUNCTION_TYPE bme280_set_filter(uint8_t value);

BME280_RETURN_FUNCTION_TYPE bme280_get_standbydur(uint8_t *time);

BME280_RETURN_FUNCTION_TYPE bme280_read_pt(uint32_t *pressure, int32_t *temperature);

BME280_RETURN_FUNCTION_TYPE bme280_sleep(void);

int8_t bme280_read_id(void);
#endif
