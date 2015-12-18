/*
*
****************************************************************************
* Copyright (C) 2011 - 2015 Bosch Sensortec GmbH
*
* File : bmm150_regs.h
*
* Date : 2015/05/21
*
* Revision : 2.0.4 $
*
* Usage: Sensor Driver for BMM150 and BMM150 sensor
*
****************************************************************************
*
* \section License
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
/****************************************************************************/
/*! \file bmm150_regs.h
    \brief BMM150 Sensor Driver Support Header File */

#ifndef __BMM150_H__
#define __BMM150_H__

#define BMM150_I2C_ADDRESS                 (0x10)
/***************************************************************/
/**\name    REGISTER ADDRESS DEFINITION        */
/***************************************************************/
/********************************************/
/**\name    CHIP ID       */
/********************************************/
/* Fixed Data Registers */
#define BMM150_REG_CHIPID                     (0x40)
/********************************************/
/**\name    DATA REGISTERS       */
/********************************************/
/* Data Registers*/
#define BMM150_DATA_X_LSB                   (0x42)
#define BMM150_DATA_X_MSB                   (0x43)
#define BMM150_DATA_Y_LSB                   (0x44)
#define BMM150_DATA_Y_MSB                   (0x45)
#define BMM150_DATA_Z_LSB                   (0x46)
#define BMM150_DATA_Z_MSB                   (0x47)
#define BMM150_DATA_R_LSB                   (0x48)
#define BMM150_DATA_R_MSB                   (0x49)

/********************************************/
/**\name    INTERRUPT STATUS      */
/********************************************/
/* Status Registers */
#define BMM150_INT_STAT_REG                    (0x4A)
/********************************************/
/**\name    POWER MODE DEFINITIONS      */
/********************************************/
/* Control Registers */
#define BMM150_POWER_CONTROL               (0x4B)
#define BMM150_CONTROL                     (0x4C)
#define BMM150_INT_CONTROL                 (0x4D)
#define BMM150_SENS_CONTROL                (0x4E)
#define BMM150_LOW_THRES                   (0x4F)
#define BMM150_HIGH_THRES                  (0x50)
/********************************************/
/**\name XY AND Z REPETITIONS DEFINITIONS  */
/********************************************/
#define BMM150_REP_XY                      (0x51)
#define BMM150_REP_Z                       (0x52)
/**************************************************/
/**\name	BMM150 TRIM DATA DEFINITIONS      */
/*************************************************/
#define BMM150_MAG_DIG_X1                      (0x5D)
#define BMM150_MAG_DIG_Y1                      (0x5E)
#define BMM150_MAG_DIG_Z4_LSB                  (0x62)
#define BMM150_MAG_DIG_Z4_MSB                  (0x63)
#define BMM150_MAG_DIG_X2                      (0x64)
#define BMM150_MAG_DIG_Y2                      (0x65)
#define BMM150_MAG_DIG_Z2_LSB                  (0x68)
#define BMM150_MAG_DIG_Z2_MSB                  (0x69)
#define BMM150_MAG_DIG_Z1_LSB                  (0x6A)
#define BMM150_MAG_DIG_Z1_MSB                  (0x6B)
#define BMM150_MAG_DIG_XYZ1_LSB                (0x6C)
#define BMM150_MAG_DIG_XYZ1_MSB                (0x6D)
#define BMM150_MAG_DIG_Z3_LSB                  (0x6E)
#define BMM150_MAG_DIG_Z3_MSB                  (0x6F)
#define BMM150_MAG_DIG_XY2                     (0x70)
#define BMM150_MAG_DIG_XY1                     (0x71)

#define BMM150_DATA_RATE_10HZ        (0x00)
#define BMM150_DATA_RATE_02HZ        (0x01)
#define BMM150_DATA_RATE_06HZ        (0x02)
#define BMM150_DATA_RATE_08HZ        (0x03)
#define BMM150_DATA_RATE_15HZ        (0x04)
#define BMM150_DATA_RATE_20HZ        (0x05)
#define BMM150_DATA_RATE_25HZ        (0x06)
#define BMM150_DATA_RATE_30HZ        (0x07)

/**************************************************/
/**\name    BMM150 PRESET MODES - DATA RATES    */
/*************************************************/
#define BMM150_MAG_LOWPOWER_DR                       0x02
#define BMM150_MAG_REGULAR_DR                        0x02
#define BMM150_MAG_HIGHACCURACY_DR                   0x2A
#define BMM150_MAG_ENHANCED_DR                       0x02
/**************************************************/
/**\name	BMM150 PRESET MODES - REPETITIONS-XY RATES */
/*************************************************/
#define BMM150_MAG_LOWPOWER_REPXY                    (1)
#define BMM150_MAG_REGULAR_REPXY                     (4)
#define BMM150_MAG_HIGHACCURACY_REPXY                (23)
#define BMM150_MAG_ENHANCED_REPXY                    (7)
/**************************************************/
/**\name	BMM150 PRESET MODES - REPETITIONS-Z RATES */
/*************************************************/
#define BMM150_MAG_LOWPOWER_REPZ                     (2)
#define BMM150_MAG_REGULAR_REPZ                      (14)
#define BMM150_MAG_HIGHACCURACY_REPZ                 (82)
#define BMM150_MAG_ENHANCED_REPZ                     (26)
/**************************************************/
/**\name    USED FOR MAG OVERFLOW CHECK FOR BMM150  */
/*************************************************/
#define BMI160_MAG_OVERFLOW_OUTPUT          ((int16_t)-32768)
#define BMI160_MAG_OVERFLOW_OUTPUT_S32      ((int32_t)(-2147483647-1))
#define BMI160_MAG_NEGATIVE_SATURATION_Z   ((int16_t)-32767)
#define BMI160_MAG_POSITIVE_SATURATION_Z   ((uint16_t)32767)
#define BMI160_MAG_FLIP_OVERFLOW_ADCVAL     ((int16_t)-4096)
#define BMI160_MAG_HALL_OVERFLOW_ADCVAL     ((int16_t)-16384)

#define BMM150_PM_MODE_SLEEP    (0x01)

#define BMM150_CHIPID           (0x32)
#endif