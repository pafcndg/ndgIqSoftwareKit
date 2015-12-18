/*
****************************************************************************
* Copyright (C) 2013 - 2014 Bosch Sensortec GmbH
*
* bme280.c
* Date: 2014/07/21
* Revision: 2.0(Pressure and Temperature compensation code revision is 1.1
*               and Humidity compensation code revision is 1.0)
*
* Usage: Sensor Driver file for BME280 sensor
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

#include "drivers/sensor/bme280_support.h"

static struct bme280_t *p_bme280 = BME280_NULL;      /**< pointer to BME280 */

/*******************************************************************************
 *  Description: *//**\brief This API is used to
 *  calibration parameters used for calculation in the registers
 *  parameter   Register address    bit
 *  dig_T1          0x88/0x89       0 : 7 / 8: 15
 *  dig_T2          0x8A/0x8B       0 : 7 / 8: 15
 *  dig_T3          0x8C/0x8D       0 : 7 / 8: 15
 *  dig_P1          0x8E/0x8F       0 : 7 / 8: 15
 *  dig_P2          0x90/0x91       0 : 7 / 8: 15
 *  dig_P3          0x92/0x93       0 : 7 / 8: 15
 *  dig_P4          0x94/0x95       0 : 7 / 8: 15
 *  dig_P5          0x96/0x97       0 : 7 / 8: 15
 *  dig_P6          0x98/0x99       0 : 7 / 8: 15
 *  dig_P7          0x9A/0x9B       0 : 7 / 8: 15
 *  dig_P8          0x9C/0x9D       0 : 7 / 8: 15
 *  dig_P9          0x9E/0x9F       0 : 7 / 8: 15
 *  dig_H1              0xA1            0 : 7
 *  dig_H2          0xE1/0xE2       0 : 7 / 8: 15
 *  dig_H3              0xE3            0 : 7
 *
 *  \param:  None
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static BME280_RETURN_FUNCTION_TYPE bme280_get_calib_param()
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t a_data_uint8_tr[26] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0};

    comres += bme280_bus_burst_read(BME280_DIG_T1_LSB_REG, a_data_uint8_tr, 26);

    p_bme280->cal_param.dig_T1 = (uint16_t)(((
    (uint16_t)((uint8_t)a_data_uint8_tr[1])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[0]);
    p_bme280->cal_param.dig_T2 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[3])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[2]);
    p_bme280->cal_param.dig_T3 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[5])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[4]);
    p_bme280->cal_param.dig_P1 = (uint16_t)(((
    (uint16_t)((uint8_t)a_data_uint8_tr[7])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[6]);
    p_bme280->cal_param.dig_P2 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[9])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[8]);
    p_bme280->cal_param.dig_P3 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[11])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[10]);
    p_bme280->cal_param.dig_P4 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[13])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[12]);
    p_bme280->cal_param.dig_P5 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[15])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[14]);
    p_bme280->cal_param.dig_P6 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[17])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[16]);
    p_bme280->cal_param.dig_P7 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[19])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[18]);
    p_bme280->cal_param.dig_P8 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[21])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[20]);
    p_bme280->cal_param.dig_P9 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[23])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[22]);
    p_bme280->cal_param.dig_H1 = a_data_uint8_tr[25];

    comres += bme280_bus_burst_read(BME280_DIG_H2_LSB_REG, a_data_uint8_tr, 7);

    p_bme280->cal_param.dig_H2 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[1])) <<
    SHIFT_LEFT_8_POSITION) | a_data_uint8_tr[0]);
    p_bme280->cal_param.dig_H3 = a_data_uint8_tr[2];
    p_bme280->cal_param.dig_H4 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[3])) <<
    SHIFT_LEFT_4_POSITION) | (((uint8_t)0x0F)
    & a_data_uint8_tr[4]));
    p_bme280->cal_param.dig_H5 = (int16_t)(((
    (int16_t)((int8_t)a_data_uint8_tr[5])) <<
    SHIFT_LEFT_4_POSITION) | (a_data_uint8_tr[4] >>
    SHIFT_RIGHT_4_POSITION));
    p_bme280->cal_param.dig_H6 = (int8_t)a_data_uint8_tr[6];

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This function is used for initialize
 *  the bus read and bus write functions
 *  and assign the chip id and I2C address of the BME280 sensor
 *  chip id is read in the register 0xD0 bit from 0 to 7
 *
 *   \param p_bme280 *bme280 structure pointer.
 *
 *  While changing the parameter of the p_bme280
 *  consider the following point:
 *  Changing the reference value of the parameter
 *  will changes the local copy or local reference
 *  make sure your changes will not
 *  affect the reference value of the parameter
 *  (Better case don't change the reference value of the parameter)
 *
 *
 *
 *
 * \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE __bme280_init(struct bme280_t *bme280)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t v_data_uint8_tr = BME280_Zero_U8X;

    if (p_bme280 != BME280_NULL)
        return E_BME280_REINIT;
    p_bme280 = bme280;
    /* assign BME280 ptr */
    comres += bme280_read_reg(BME280_CHIPID_REG, &v_data_uint8_tr);
    /* read Chip Id */
    p_bme280->chip_id = v_data_uint8_tr;
    pr_debug(LOG_MODULE_DRV,"bme280 chip id=%x\n", p_bme280->chip_id);

    /* readout bme280 calibparam structure */
    bme280_get_calib_param();
    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to read uncompensated temperature
 *  in the registers 0xFA, 0xFB and 0xFC
 *  0xFA -> MSB -> bit from 0 to 7
 *  0xFB -> LSB -> bit from 0 to 7
 *  0xFC -> LSB -> bit from 4 to 7
 *
 * \param int32_t utemperature : Pointer holding
 *          the uncompensated temperature.
 *
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static BME280_RETURN_FUNCTION_TYPE bme280_read_ut(int32_t *utemperature)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t a_data_uint8_tr[3] = {0, 0, 0};

    comres += bme280_bus_read(BME280_TEMPERATURE_MSB_REG, a_data_uint8_tr, 3);
    *utemperature = (int32_t)(((
    (uint32_t) (a_data_uint8_tr[0]))
    << SHIFT_LEFT_12_POSITION) |
    (((uint32_t)(a_data_uint8_tr[1]))
    << SHIFT_LEFT_4_POSITION)
    | ((uint32_t)a_data_uint8_tr[2] >>
    SHIFT_RIGHT_4_POSITION));

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to read uncompensated pressure.
 *  in the registers 0xF7, 0xF8 and 0xF9
 *  0xF7 -> MSB -> bit from 0 to 7
 *  0xF8 -> LSB -> bit from 0 to 7
 *  0xF9 -> LSB -> bit from 4 to 7
 *
 *
 *
 *  \param int32_t upressure : Pointer holding the uncompensated pressure.
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static BME280_RETURN_FUNCTION_TYPE bme280_read_up(int32_t *upressure)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t a_data_uint8_tr[3] = {0, 0, 0};

    comres += bme280_bus_read(BME280_PRESSURE_MSB_REG, a_data_uint8_tr, 3);
    *upressure = (int32_t)((
    ((uint32_t)(a_data_uint8_tr[0]))
    << SHIFT_LEFT_12_POSITION) |
    (((uint32_t)(a_data_uint8_tr[1]))
    << SHIFT_LEFT_4_POSITION)  |
    ((uint32_t)a_data_uint8_tr[2] >>
    SHIFT_RIGHT_4_POSITION));

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to read uncompensated humidity.
 *  in the registers 0xF7, 0xF8 and 0xF9
 *  0xFD -> MSB -> bit from 0 to 7
 *  0xFE -> LSB -> bit from 0 to 7
 *
 *
 *
 *  \param int32_t uhumidity : Pointer holding the uncompensated humidity.
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static BME280_RETURN_FUNCTION_TYPE bme280_read_uh(int32_t *uhumidity)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t a_data_uint8_tr[2] = {0, 0};
    comres += bme280_bus_read(BME280_HUMIDITY_MSB_REG, a_data_uint8_tr, 2);
    *uhumidity = (int32_t)(
    (((uint32_t)(a_data_uint8_tr[0]))
    << SHIFT_LEFT_8_POSITION)|
    ((uint32_t)(a_data_uint8_tr[1])));

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to set
 *  the temperature oversampling in the register 0xF4
 *  bits from 5 to 7
 *
 *  bit                 temperature oversampling
 *  0x00                        Skipped
 *  0x01                        BME280_OVERSAMPLING_1X
 *  0x02                        BME280_OVERSAMPLING_2X
 *  0x03                        BME280_OVERSAMPLING_4X
 *  0x04                        BME280_OVERSAMPLING_8X
 *  0x05,0x06 and 0x07          BME280_OVERSAMPLING_16X
 *
 *
 *  \param uint8_t value : the osrs_t value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_t(uint8_t value)
{
    BME280_RETURN_FUNCTION_TYPE comres = SUCCESS;
    uint8_t v_data_uint8_tr = BME280_Zero_U8X;

    comres += bme280_read_reg(BME280_CTRLMEAS_REG_OSRST__REG, &v_data_uint8_tr);
    if (comres == SUCCESS) {
        v_data_uint8_tr =
        BME280_SET_BITSLICE(v_data_uint8_tr,
        BME280_CTRLMEAS_REG_OSRST, value);
        comres += bme280_write_reg(BME280_CTRLMEAS_REG_OSRST__REG, &v_data_uint8_tr);
        p_bme280->osrs_t = value;
    }
    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to set
 *  the pressure oversampling in the register 0xF4
 *  bits from 2 to 4
 *
 *  bit                 pressure oversampling
 *  0x00                        Skipped
 *  0x01                        BME280_OVERSAMPLING_1X
 *  0x02                        BME280_OVERSAMPLING_2X
 *  0x03                        BME280_OVERSAMPLING_4X
 *  0x04                        BME280_OVERSAMPLING_8X
 *  0x05,0x06 and 0x07          BME280_OVERSAMPLING_16X
 *
 *
 *  \param uint8_t value : the osrs_p value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_p(uint8_t value)
{
    BME280_RETURN_FUNCTION_TYPE comres = SUCCESS;
    uint8_t v_data_uint8_tr = BME280_Zero_U8X;

    comres += bme280_read_reg(BME280_CTRLMEAS_REG_OSRSP__REG, &v_data_uint8_tr);
    if (comres == SUCCESS) {
        v_data_uint8_tr =
        BME280_SET_BITSLICE(v_data_uint8_tr,
        BME280_CTRLMEAS_REG_OSRSP, value);
        comres += bme280_write_reg(BME280_CTRLMEAS_REG_OSRSP__REG, &v_data_uint8_tr);
        p_bme280->osrs_p = value;
    }

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API is used to set
 *  the humidity oversampling setting in the register 0xF2
 *  bits from 0 to 2
 *
 *  bit                 pressure oversampling
 *  0x00                        Skipped
 *  0x01                        BME280_OVERSAMPLING_1X
 *  0x02                        BME280_OVERSAMPLING_2X
 *  0x03                        BME280_OVERSAMPLING_4X
 *  0x04                        BME280_OVERSAMPLING_8X
 *  0x05,0x06 and 0x07          BME280_OVERSAMPLING_16X
 *
 *
 *
 * The "BME280_CTRLHUM_REG_OSRSH" register sets the humidity
 * data acquisition options of the device.
 * changes to this registers only become effective after a write operation to
 * "BME280_CTRLMEAS_REG" register.
 * In the code automated reading and writing of "BME280_CTRLHUM_REG_OSRSH"
 * register first set the "BME280_CTRLHUM_REG_OSRSH" and then read and write
 * the "BME280_CTRLMEAS_REG" register in the function.
 *
 *
 *
 *  \param uint8_t value : Value of the humidity oversampling setting
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_h(uint8_t value)
{
    BME280_RETURN_FUNCTION_TYPE comres = SUCCESS;
    uint8_t v_data_uint8_tr = BME280_Zero_U8X;

    comres += bme280_read_reg(BME280_CTRLHUM_REG_OSRSH__REG, &v_data_uint8_tr);
    if (comres == SUCCESS) {
        v_data_uint8_tr = BME280_SET_BITSLICE(v_data_uint8_tr,
        BME280_CTRLHUM_REG_OSRSH, value);
        comres += bme280_write_reg(BME280_CTRLHUM_REG_OSRSH__REG, &v_data_uint8_tr);
        p_bme280->osrs_h = value;
    }

    comres += bme280_read_reg(BME280_CTRLMEAS_REG, &v_data_uint8_tr);
    if (comres == SUCCESS)
        comres +=bme280_write_reg(BME280_CTRLMEAS_REG, &v_data_uint8_tr);

    return comres;
}

/*******************************************************************************
 *  Description: *//**\brief This API used to set the
 *  Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *  \param uint8_t *mode : Pointer holding the mode value.
 *  0x00            ->  BME280_SLEEP_MODE
 *  0x01 and 0x02   ->  BME280_FORCED_MODE
 *  0x03            ->  BME280_NORMAL_MODE
 *
 *
 *  \return : results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_mode(uint8_t mode)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    uint8_t v_mode_uint8_tr = BME280_Zero_U8X;

    v_mode_uint8_tr = (p_bme280->osrs_t << SHIFT_LEFT_5_POSITION) +
                      (p_bme280->osrs_p << SHIFT_LEFT_2_POSITION) + mode;
    comres += bme280_write_reg(BME280_CTRLMEAS_REG_MODE__REG, &v_mode_uint8_tr);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief Used to reset the sensor
 * The value 0xB6 is written to the 0xE0 register the device is reset using the
 * complete power-on-reset procedure.
 * Softreset can be easily set using bme280_set_softreset().
 * Usage Hint : bme280_set_softreset()
 *
 *
 *  \param: None
 *
 *
 *
 *  \return: result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_softreset()
{
    uint8_t v_data_uint8_tr = BME280_SOFT_RESET_CODE;
    return bme280_write_reg(BME280_RESET_REG, &v_data_uint8_tr);
}

#ifdef BME280_ENABLE_FLOAT
/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *                          and returns the value in Degree centigrade
 *                          Output value of "51.23" equals 51.23 DegC.
 *
 *
 *
 *  \param signed long : value of uncompensated temperature
 *
 *
 *
 *  \return
 *          double : actual temperature in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static double bme280_compensate_T_double(int32_t adc_t)
{
    double v_x1_uint32_tr = BME280_Zero_U8X;
    double v_x2_uint32_tr = BME280_Zero_U8X;
    double temperature = BME280_Zero_U8X;

    v_x1_uint32_tr  = (((double)adc_t) / 16384.0 -
    ((double)p_bme280->cal_param.dig_T1) / 1024.0) *
    ((double)p_bme280->cal_param.dig_T2);
    v_x2_uint32_tr  = ((((double)adc_t) / 131072.0 -
    ((double)p_bme280->cal_param.dig_T1) / 8192.0) *
    (((double)adc_t) / 131072.0 -
    ((double)p_bme280->cal_param.dig_T1) / 8192.0)) *
    ((double)p_bme280->cal_param.dig_T3);
    p_bme280->cal_param.t_fine = (int32_t)(v_x1_uint32_tr + v_x2_uint32_tr);
    temperature  = (v_x1_uint32_tr + v_x2_uint32_tr) / 5120.0;

    return temperature;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *                          and returns pressure in Pa as double.
 *                          Output value of "96386.2"
 *                          equals 96386.2 Pa = 963.862 hPa.
 *
 *
 *  \param signed int : value of uncompensated pressure
 *
 *
 *
 *  \return
 *          double : actual pressure in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static double bme280_compensate_P_double(int32_t adc_p)
{
    double v_x1_uint32_tr = BME280_Zero_U8X;
    double v_x2_uint32_tr = BME280_Zero_U8X;
    double pressure = BME280_Zero_U8X;

    v_x1_uint32_tr = ((double)p_bme280->cal_param.t_fine/2.0) - 64000.0;
    v_x2_uint32_tr = v_x1_uint32_tr * v_x1_uint32_tr *
    ((double)p_bme280->cal_param.dig_P6) / 32768.0;
    v_x2_uint32_tr = v_x2_uint32_tr + v_x1_uint32_tr *
    ((double)p_bme280->cal_param.dig_P5) * 2.0;
    v_x2_uint32_tr = (v_x2_uint32_tr / 4.0) +
    (((double)p_bme280->cal_param.dig_P4) * 65536.0);
    v_x1_uint32_tr = (((double)p_bme280->cal_param.dig_P3) *
    v_x1_uint32_tr * v_x1_uint32_tr / 524288.0 +
    ((double)p_bme280->cal_param.dig_P2) * v_x1_uint32_tr) / 524288.0;
    v_x1_uint32_tr = (1.0 + v_x1_uint32_tr / 32768.0) *
    ((double)p_bme280->cal_param.dig_P1);
    pressure = 1048576.0 - (double)adc_p;
    /* Avoid exception caused by division by zero */
    if (v_x1_uint32_tr != BME280_Zero_U8X)
        pressure = (pressure - (v_x2_uint32_tr / 4096.0)) * 6250.0 / v_x1_uint32_tr;
    else
        return 0;
    v_x1_uint32_tr = ((double)p_bme280->cal_param.dig_P9) *
    pressure * pressure / 2147483648.0;
    v_x2_uint32_tr = pressure * ((double)p_bme280->cal_param.dig_P8) / 32768.0;
    pressure = pressure + (v_x1_uint32_tr + v_x2_uint32_tr +
    ((double)p_bme280->cal_param.dig_P7)) / 16.0;

    return pressure;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual humidity from uncompensated humidity
 *                          and returns the value in relative humidity (%rH)
 *                          Output value of "42.12" equals 42.12 %rH
 *
 *  \param signed int : value of uncompensated humidity
 *
 *
 *
 *  \return
 *          double : actual humidity in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static double bme280_compensate_H_double(int32_t adc_h)
{
    double var_h = BME280_Zero_U8X;
    var_h = (((double)p_bme280->cal_param.t_fine)-76800.0);
    if (var_h != BME280_Zero_U8X)
        var_h = (adc_h-(((double)p_bme280->cal_param.dig_H4)*64.0 +
        ((double)p_bme280->cal_param.dig_H5) / 16384.0 * var_h))*
        (((double)p_bme280->cal_param.dig_H2)/65536.0*(1.0 + ((double)
        p_bme280->cal_param.dig_H6)/67108864.0*var_h*(1.0+((double)
        p_bme280->cal_param.dig_H3)/67108864.0*var_h)));
    else
        return 0;
    var_h = var_h * (1.0-((double)
    p_bme280->cal_param.dig_H1)*var_h/524288.0);
    if (var_h > 100.0)
        var_h = 100.0;
    else if (var_h < 0.0)
        var_h = 0.0;
    return var_h;

}

/*******************************************************************************
 * Description: *//**\brief reads  temperature .
 *
 *
 *
 *
 *  \param double temperature : Pointer holding
 *                      the compensated temperature.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_t_double(double *temperature)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t utemperature = BME280_Zero_U8X;

    comres += bme280_read_ut(&utemperature);
    *temperature = bme280_compensate_T_double(utemperature);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief reads  pressure .
 *
 *
 *
 *
 *  \param double pressure : Pointer holding
 *                          the compensated pressure.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_p_double(double *pressure)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t utemperature = BME280_Zero_U8X;

    comres += bme280_read_up(&utemperature);
    *pressure = bme280_compensate_P_double(utemperature);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief reads  humidity .
 *
 *
 *
 *
 *  \param double humidity : Pointer holding
 *                          the compensated humidity.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_h_double(double *humidity)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t uhumidity = BME280_Zero_U8X;

    comres += bme280_read_uh(&uhumidity);
    *humidity = bme280_compensate_H_double(uhumidity);

    return comres;
}
#else
/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *                    and returns the value in 0.01 degree Centigrade
 *                    Output value of "5123" equals 51.23 DegC.
 *
 *
 *
 *  \param int32_t : value of uncompensated temperature
 *
 *
 *  \return
 *          int32_t : actual temperature
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static int32_t bme280_compensate_T_int32(int32_t adc_t)
{
    int32_t v_x1_uint32_tr = BME280_Zero_U8X;
    int32_t v_x2_uint32_tr = BME280_Zero_U8X;
    int32_t temperature = BME280_Zero_U8X;

    v_x1_uint32_tr  = ((((adc_t >> 3) - ((int32_t)
    p_bme280->cal_param.dig_T1 << 1))) *
    ((int32_t)p_bme280->cal_param.dig_T2)) >> 11;
    v_x2_uint32_tr  = (((((adc_t >> 4) -
    ((int32_t)p_bme280->cal_param.dig_T1)) * ((adc_t >> 4) -
    ((int32_t)p_bme280->cal_param.dig_T1))) >> 12) *
    ((int32_t)p_bme280->cal_param.dig_T3)) >> 14;
    p_bme280->cal_param.t_fine = v_x1_uint32_tr + v_x2_uint32_tr;
    temperature  = (p_bme280->cal_param.t_fine * 5 + 128) >> 8;
    return temperature;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *                          and returns the value in Pascal(Pa)
 *                          Output value of "96386" equals 96386 Pa =
 *                          963.86 hPa = 963.86 millibar
 *
 *
 *
 *  \param int32_t : value of uncompensated pressure
 *
 *
 *
 *  \return
 *          uint32_t : actual pressure
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static uint32_t bme280_compensate_P_int32(int32_t adc_p)
{
    int32_t v_x1_uint32_tr = BME280_Zero_U8X;
    int32_t v_x2_uint32_tr = BME280_Zero_U8X;
    uint32_t pressure = BME280_Zero_U8X;

    v_x1_uint32_tr = (((int32_t)p_bme280->cal_param.t_fine) >> 1) - (int32_t)64000;
    v_x2_uint32_tr = (((v_x1_uint32_tr >> 2) * (v_x1_uint32_tr >> 2)) >> 11) *
    ((int32_t)p_bme280->cal_param.dig_P6);
    v_x2_uint32_tr = v_x2_uint32_tr + ((v_x1_uint32_tr *
    ((int32_t)p_bme280->cal_param.dig_P5)) << 1);
    v_x2_uint32_tr = (v_x2_uint32_tr >> 2) +
    (((int32_t)p_bme280->cal_param.dig_P4) << 16);
    v_x1_uint32_tr = (((p_bme280->cal_param.dig_P3 * (((v_x1_uint32_tr >> 2) *
    (v_x1_uint32_tr >> 2)) >> 13)) >> 3) +
    ((((int32_t)p_bme280->cal_param.dig_P2) *
    v_x1_uint32_tr) >> 1)) >> 18;
    v_x1_uint32_tr = ((((32768+v_x1_uint32_tr)) *
    ((int32_t)p_bme280->cal_param.dig_P1))  >> 15);
    pressure = (((uint32_t)(((int32_t)1048576) - adc_p) -
    (v_x2_uint32_tr >> 12))) * 3125;
    if (pressure < 0x80000000)
        /* Avoid exception caused by division by zero */
        if (v_x1_uint32_tr != BME280_Zero_U8X)
            pressure = (pressure << 1) / ((uint32_t)v_x1_uint32_tr);
        else
            return BME280_Zero_U8X;
    else
        /* Avoid exception caused by division by zero */
        if (v_x1_uint32_tr != BME280_Zero_U8X)
            pressure = (pressure / (uint32_t)v_x1_uint32_tr) * 2;
        else
            return BME280_Zero_U8X;

        v_x1_uint32_tr = (((int32_t)p_bme280->cal_param.dig_P9) *
        ((int32_t)(((pressure >> 3) * (pressure >> 3)) >> 13)))
        >> 12;
        v_x2_uint32_tr = (((int32_t)(pressure >> 2)) *
        ((int32_t)p_bme280->cal_param.dig_P8)) >> 13;
        pressure = (uint32_t)((int32_t)pressure +
        ((v_x1_uint32_tr + v_x2_uint32_tr + p_bme280->cal_param.dig_P7) >> 4));

    return pressure;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual humidity from
 *        uncompensated humidity
 *        and returns the value in %rH as unsigned 32bit integer
 *        in Q22.10 format(22 integer 10 fractional bits).
 *        An output value of 42313
 *        represents 42313 / 1024 = 41.321 %rH
 *
 *
 *
 *  \param int32_t : value of uncompensated humidity
 *
 *  \return
 *          uint32_t : actual relative humidity
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
static uint32_t bme280_compensate_H_int32(int32_t adc_h)
{
    int32_t v_x1_uint32_tr;
    v_x1_uint32_tr = (p_bme280->cal_param.t_fine - ((int32_t)76800));
    v_x1_uint32_tr = (((((adc_h << 14) -
    (((int32_t)p_bme280->cal_param.dig_H4) << 20) -
    (((int32_t)p_bme280->cal_param.dig_H5) * v_x1_uint32_tr)) +
    ((int32_t)16384)) >> 15) *
    (((((((v_x1_uint32_tr *
    ((int32_t)p_bme280->cal_param.dig_H6)) >> 10) *
    (((v_x1_uint32_tr * ((int32_t)p_bme280->cal_param.dig_H3)) >> 11) +
    ((int32_t)32768))) >> 10) +
    ((int32_t)2097152)) *
    ((int32_t)p_bme280->cal_param.dig_H2) + 8192) >> 14));
    v_x1_uint32_tr = (v_x1_uint32_tr - (((((v_x1_uint32_tr >> 15) *
    (v_x1_uint32_tr >> 15)) >> 7) *
    ((int32_t)p_bme280->cal_param.dig_H1)) >> 4));
    v_x1_uint32_tr = (v_x1_uint32_tr < 0 ? 0 : v_x1_uint32_tr);
    v_x1_uint32_tr = (v_x1_uint32_tr > 419430400 ? 419430400 : v_x1_uint32_tr);
    return (uint32_t)(v_x1_uint32_tr>>12);
}

/*******************************************************************************
 * Description: *//**\brief reads pressure, temperature and humidity.
 *
 *
 *
 *
 *  \param int32_t temperature : Pointer holding
 *                      the compensated temperature.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_t(int32_t *temperature)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t utemperature = BME280_Zero_U8X;

    comres += bme280_read_ut(&utemperature);
    *temperature = bme280_compensate_T_int32(utemperature);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief reads pressure.
 *
 *
 *
 *
 *  \param uint32_t pressure : Pointer holding
 *                          the compensated pressure.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_p(uint32_t *pressure)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t upressure = BME280_Zero_U8X;

    comres += bme280_read_up(&upressure);
    *pressure = bme280_compensate_P_int32(upressure);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief reads humidity.
 *
 *
 *
 *
 *  \param uint32_t pressure : Pointer holding
 *                          the compensated humidity.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_h(uint32_t *humidity)
{
    BME280_RETURN_FUNCTION_TYPE comres = BME280_Zero_U8X;
    int32_t uhumidity = BME280_Zero_U8X;

    comres += bme280_read_uh(&uhumidity);
    *humidity = bme280_compensate_H_int32(uhumidity);

    return comres;
}

#endif
