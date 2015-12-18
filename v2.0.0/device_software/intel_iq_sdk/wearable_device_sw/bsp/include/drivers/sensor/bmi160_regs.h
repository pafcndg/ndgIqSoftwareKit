/*
*
****************************************************************************
* Copyright (C) 2014 Bosch Sensortec GmbH
*
* File : bmi160.h
*
* Date : 2014/12/12
*
* Revision : 2.0.5 $
*
* Usage: Sensor Driver for BMI160 sensor
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

#ifndef __BMI160_REGS_H__
#define __BMI160_REGS_H__
/****************************************************/
/**\name    REGISTER DEFINITIONS       */
/***************************************************/
/*******************/
/**\name CHIP ID */
/*******************/
#define BMI160_USER_CHIP_ID_ADDR                0x00
/*******************/
/**\name ERROR STATUS */
/*******************/
#define BMI160_USER_ERROR_ADDR                  0X02
/*******************/
/**\name POWER MODE STATUS */
/*******************/
#define BMI160_USER_PMU_STAT_ADDR               0X03
/*******************/
/**\name MAG DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_0_ADDR                 0X04
#define BMI160_USER_DATA_1_ADDR                 0X05
#define BMI160_USER_DATA_2_ADDR                 0X06
#define BMI160_USER_DATA_3_ADDR                 0X07
#define BMI160_USER_DATA_4_ADDR                 0X08
#define BMI160_USER_DATA_5_ADDR                 0X09
#define BMI160_USER_DATA_6_ADDR                 0X0A
#define BMI160_USER_DATA_7_ADDR                 0X0B
/*******************/
/**\name GYRO DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_8_ADDR                 0X0C
#define BMI160_USER_DATA_9_ADDR                 0X0D
#define BMI160_USER_DATA_10_ADDR                0X0E
#define BMI160_USER_DATA_11_ADDR                0X0F
#define BMI160_USER_DATA_12_ADDR                0X10
#define BMI160_USER_DATA_13_ADDR                0X11
#define BMI160_USER_DATA_14_ADDR                0X12
#define BMI160_USER_DATA_15_ADDR                0X13
/*******************/
/**\name ACCEL DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_16_ADDR                0X14
#define BMI160_USER_DATA_17_ADDR                0X15
#define BMI160_USER_DATA_18_ADDR                0X16
#define BMI160_USER_DATA_19_ADDR                0X17
/*******************/
/**\name SENSOR TIME REGISTERS */
/*******************/
#define BMI160_USER_SENSORTIME_0_ADDR           0X18
#define BMI160_USER_SENSORTIME_1_ADDR           0X19
#define BMI160_USER_SENSORTIME_2_ADDR           0X1A
/*******************/
/**\name STATUS REGISTER FOR SENSOR STATUS FLAG */
/*******************/
#define BMI160_USER_STAT_ADDR                   0X1B
/*******************/
/**\name INTERRUPY STATUS REGISTERS */
/*******************/
#define BMI160_USER_INTR_STAT_0_ADDR            0X1C
#define BMI160_USER_INTR_STAT_1_ADDR            0X1D
#define BMI160_USER_INTR_STAT_2_ADDR            0X1E
#define BMI160_USER_INTR_STAT_3_ADDR            0X1F
/*******************/
/**\name TEMPERATURE REGISTERS */
/*******************/
#define BMI160_USER_TEMPERATURE_0_ADDR          0X20
#define BMI160_USER_TEMPERATURE_1_ADDR          0X21
/*******************/
/**\name FIFO REGISTERS */
/*******************/
#define BMI160_USER_FIFO_LENGTH_0_ADDR          0X22
#define BMI160_USER_FIFO_LENGTH_1_ADDR          0X23
#define BMI160_USER_FIFO_DATA_ADDR              0X24
/***************************************************/
/**\name ACCEL CONFIG REGISTERS  FOR ODR, BANDWIDTH AND UNDERSAMPLING*/
/******************************************************/
#define BMI160_USER_ACCEL_CONFIG_ADDR           0X40
/*******************/
/**\name ACCEL RANGE */
/*******************/
#define BMI160_USER_ACCEL_RANGE_ADDR            0X41
/***************************************************/
/**\name GYRO CONFIG REGISTERS  FOR ODR AND BANDWIDTH */
/******************************************************/
#define BMI160_USER_GYRO_CONFIG_ADDR            0X42
/*******************/
/**\name GYRO RANGE */
/*******************/
#define BMI160_USER_GYRO_RANGE_ADDR             0X43
/***************************************************/
/**\name MAG CONFIG REGISTERS  FOR ODR*/
/******************************************************/
#define BMI160_USER_MAG_CONFIG_ADDR             0X44
/***************************************************/
/**\name REGISTER FOR GYRO AND ACCEL DOWNSAMPLING RATES FOR FIFO*/
/******************************************************/
#define BMI160_USER_FIFO_DOWN_ADDR              0X45
/***************************************************/
/**\name FIFO CONFIG REGISTERS*/
/******************************************************/
#define BMI160_USER_FIFO_CONFIG_0_ADDR          0X46
#define BMI160_USER_FIFO_CONFIG_1_ADDR          0X47
/***************************************************/
/**\name MAG INTERFACE REGISTERS*/
/******************************************************/
#define BMI160_USER_MAG_IF_0_ADDR               0X4B
#define BMI160_USER_MAG_IF_1_ADDR               0X4C
#define BMI160_USER_MAG_IF_2_ADDR               0X4D
#define BMI160_USER_MAG_IF_3_ADDR               0X4E
#define BMI160_USER_MAG_IF_4_ADDR               0X4F
/***************************************************/
/**\name INTERRUPT ENABLE REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_ENABLE_0_ADDR          0X50
#define BMI160_USER_INTR_ENABLE_1_ADDR          0X51
#define BMI160_USER_INTR_ENABLE_2_ADDR          0X52
#define BMI160_USER_INTR_OUT_CTRL_ADDR          0X53
/***************************************************/
/**\name LATCH DURATION REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_LATCH_ADDR             0X54
/***************************************************/
/**\name MAP INTERRUPT 1 and 2 REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_MAP_0_ADDR             0X55
#define BMI160_USER_INTR_MAP_1_ADDR             0X56
#define BMI160_USER_INTR_MAP_2_ADDR             0X57
/***************************************************/
/**\name DATA SOURCE REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_DATA_0_ADDR            0X58
#define BMI160_USER_INTR_DATA_1_ADDR            0X59
/***************************************************/
/**\name
INTERRUPT THRESHOLD, HYSTERESIS, DURATION, MODE CONFIGURATION REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_LOWHIGH_0_ADDR         0X5A
#define BMI160_USER_INTR_LOWHIGH_1_ADDR         0X5B
#define BMI160_USER_INTR_LOWHIGH_2_ADDR         0X5C
#define BMI160_USER_INTR_LOWHIGH_3_ADDR         0X5D
#define BMI160_USER_INTR_LOWHIGH_4_ADDR         0X5E
#define BMI160_USER_INTR_MOTION_0_ADDR          0X5F
#define BMI160_USER_INTR_MOTION_1_ADDR          0X60
#define BMI160_USER_INTR_MOTION_2_ADDR          0X61
#define BMI160_USER_INTR_MOTION_3_ADDR          0X62
#define BMI160_USER_INTR_TAP_0_ADDR             0X63
#define BMI160_USER_INTR_TAP_1_ADDR             0X64
#define BMI160_USER_INTR_ORIENT_0_ADDR          0X65
#define BMI160_USER_INTR_ORIENT_1_ADDR          0X66
#define BMI160_USER_INTR_FLAT_0_ADDR            0X67
#define BMI160_USER_INTR_FLAT_1_ADDR            0X68
/***************************************************/
/**\name FAST OFFSET CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_USER_FOC_CONFIG_ADDR             0X69
/***************************************************/
/**\name MISCELLANEOUS CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_USER_CONFIG_ADDR                 0X6A
/***************************************************/
/**\name SERIAL INTERFACE SETTINGS REGISTER*/
/******************************************************/
#define BMI160_USER_IF_CONFIG_ADDR              0X6B
/***************************************************/
/**\name GYRO POWER MODE TRIGGER REGISTER */
/******************************************************/
#define BMI160_USER_PMU_TRIGGER_ADDR            0X6C
/***************************************************/
/**\name SELF_TEST REGISTER*/
/******************************************************/
#define BMI160_USER_SELF_TEST_ADDR              0X6D
/***************************************************/
/**\name SPI,I2C SELECTION REGISTER*/
/******************************************************/
#define BMI160_USER_NV_CONFIG_ADDR              0x70
/***************************************************/
/**\name ACCEL AND GYRO OFFSET REGISTERS*/
/******************************************************/
#define BMI160_USER_OFFSET_0_ADDR               0X71
#define BMI160_USER_OFFSET_1_ADDR               0X72
#define BMI160_USER_OFFSET_2_ADDR               0X73
#define BMI160_USER_OFFSET_3_ADDR               0X74
#define BMI160_USER_OFFSET_4_ADDR               0X75
#define BMI160_USER_OFFSET_5_ADDR               0X76
#define BMI160_USER_OFFSET_6_ADDR               0X77
/***************************************************/
/**\name STEP COUNTER INTERRUPT REGISTERS*/
/******************************************************/
#define BMI160_USER_STEP_COUNT_0_ADDR           0X78
#define BMI160_USER_STEP_COUNT_1_ADDR           0X79
/***************************************************/
/**\name STEP COUNTER CONFIGURATION REGISTERS*/
/******************************************************/
#define BMI160_USER_STEP_CONFIG_0_ADDR          0X7A
#define BMI160_USER_STEP_CONFIG_1_ADDR          0X7B
/***************************************************/
/**\name COMMAND REGISTER*/
/******************************************************/
#define BMI160_CMD_COMMANDS_ADDR                0X7E
/***************************************************/
/**\name PAGE REGISTERS*/
/******************************************************/
#define BMI160_CMD_EXT_MODE_ADDR                0X7F
#define BMI160_COM_C_TRIM_FIVE_ADDR             0X85

/**************************************************************/
/**\name    USER DATA REGISTERS DEFINITION START    */
/**************************************************************/

/**************************************************************/
/**\name    CHIP ID LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Chip ID Description - Reg Addr --> 0x00, Bit --> 0...7 */
#define BMI160_USER_CHIP_ID__POS             0
#define BMI160_USER_CHIP_ID__MSK            0xFF
#define BMI160_USER_CHIP_ID__LEN             8
#define BMI160_USER_CHIP_ID__REG             BMI160_USER_CHIP_ID_ADDR
/**************************************************************/
/**\name    ERROR STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Error Description - Reg Addr --> 0x02, Bit --> 0 */
#define BMI160_USER_ERR_STAT__POS               0
#define BMI160_USER_ERR_STAT__LEN               8
#define BMI160_USER_ERR_STAT__MSK               0xFF
#define BMI160_USER_ERR_STAT__REG               BMI160_USER_ERROR_ADDR

#define BMI160_USER_FATAL_ERR__POS               0
#define BMI160_USER_FATAL_ERR__LEN               1
#define BMI160_USER_FATAL_ERR__MSK               0x01
#define BMI160_USER_FATAL_ERR__REG               BMI160_USER_ERROR_ADDR

/* Error Description - Reg Addr --> 0x02, Bit --> 1...4 */
#define BMI160_USER_ERR_CODE__POS               1
#define BMI160_USER_ERR_CODE__LEN               4
#define BMI160_USER_ERR_CODE__MSK               0x1E
#define BMI160_USER_ERR_CODE__REG               BMI160_USER_ERROR_ADDR

/* Error Description - Reg Addr --> 0x02, Bit --> 5 */
#define BMI160_USER_I2C_FAIL_ERR__POS               5
#define BMI160_USER_I2C_FAIL_ERR__LEN               1
#define BMI160_USER_I2C_FAIL_ERR__MSK               0x20
#define BMI160_USER_I2C_FAIL_ERR__REG               BMI160_USER_ERROR_ADDR

/* Error Description - Reg Addr --> 0x02, Bit --> 6 */
#define BMI160_USER_DROP_CMD_ERR__POS              6
#define BMI160_USER_DROP_CMD_ERR__LEN              1
#define BMI160_USER_DROP_CMD_ERR__MSK              0x40
#define BMI160_USER_DROP_CMD_ERR__REG              BMI160_USER_ERROR_ADDR
/**************************************************************/
/**\name    MAG DATA READY LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Error Description - Reg Addr --> 0x02, Bit --> 7 */
#define BMI160_USER_MAG_DADA_RDY_ERR__POS               7
#define BMI160_USER_MAG_DADA_RDY_ERR__LEN               1
#define BMI160_USER_MAG_DADA_RDY_ERR__MSK               0x80
#define BMI160_USER_MAG_DADA_RDY_ERR__REG               BMI160_USER_ERROR_ADDR
/**************************************************************/
/**\name    MAG POWER MODE LENGTH, POSITION AND MASK    */
/**************************************************************/
/* PMU_Status Description of MAG - Reg Addr --> 0x03, Bit --> 1..0 */
#define BMI160_USER_MAG_POWER_MODE_STAT__POS        0
#define BMI160_USER_MAG_POWER_MODE_STAT__LEN        2
#define BMI160_USER_MAG_POWER_MODE_STAT__MSK        0x03
#define BMI160_USER_MAG_POWER_MODE_STAT__REG        \
BMI160_USER_PMU_STAT_ADDR
/**************************************************************/
/**\name    GYRO POWER MODE LENGTH, POSITION AND MASK    */
/**************************************************************/
/* PMU_Status Description of GYRO - Reg Addr --> 0x03, Bit --> 3...2 */
#define BMI160_USER_GYRO_POWER_MODE_STAT__POS               2
#define BMI160_USER_GYRO_POWER_MODE_STAT__LEN               2
#define BMI160_USER_GYRO_POWER_MODE_STAT__MSK               0x0C
#define BMI160_USER_GYRO_POWER_MODE_STAT__REG             \
BMI160_USER_PMU_STAT_ADDR
/**************************************************************/
/**\name    ACCEL POWER MODE LENGTH, POSITION AND MASK    */
/**************************************************************/
/* PMU_Status Description of ACCEL - Reg Addr --> 0x03, Bit --> 5...4 */
#define BMI160_USER_ACCEL_POWER_MODE_STAT__POS               4
#define BMI160_USER_ACCEL_POWER_MODE_STAT__LEN               2
#define BMI160_USER_ACCEL_POWER_MODE_STAT__MSK               0x30
#define BMI160_USER_ACCEL_POWER_MODE_STAT__REG          \
BMI160_USER_PMU_STAT_ADDR
/**************************************************************/
/**\name    MAG DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Mag_X(LSB) Description - Reg Addr --> 0x04, Bit --> 0...7 */
#define BMI160_USER_DATA_0_MAG_X_LSB__POS           0
#define BMI160_USER_DATA_0_MAG_X_LSB__LEN           8
#define BMI160_USER_DATA_0_MAG_X_LSB__MSK          0xFF
#define BMI160_USER_DATA_0_MAG_X_LSB__REG           BMI160_USER_DATA_0_ADDR

/* Mag_X(LSB) Description - Reg Addr --> 0x04, Bit --> 3...7 */
#define BMI160_USER_DATA_MAG_X_LSB__POS           3
#define BMI160_USER_DATA_MAG_X_LSB__LEN           5
#define BMI160_USER_DATA_MAG_X_LSB__MSK          0xF8
#define BMI160_USER_DATA_MAG_X_LSB__REG          BMI160_USER_DATA_0_ADDR

/* Mag_X(MSB) Description - Reg Addr --> 0x05, Bit --> 0...7 */
#define BMI160_USER_DATA_1_MAG_X_MSB__POS           0
#define BMI160_USER_DATA_1_MAG_X_MSB__LEN           8
#define BMI160_USER_DATA_1_MAG_X_MSB__MSK          0xFF
#define BMI160_USER_DATA_1_MAG_X_MSB__REG          BMI160_USER_DATA_1_ADDR

/* Mag_Y(LSB) Description - Reg Addr --> 0x06, Bit --> 0...7 */
#define BMI160_USER_DATA_2_MAG_Y_LSB__POS           0
#define BMI160_USER_DATA_2_MAG_Y_LSB__LEN           8
#define BMI160_USER_DATA_2_MAG_Y_LSB__MSK          0xFF
#define BMI160_USER_DATA_2_MAG_Y_LSB__REG          BMI160_USER_DATA_2_ADDR

/* Mag_Y(LSB) Description - Reg Addr --> 0x06, Bit --> 3...7 */
#define BMI160_USER_DATA_MAG_Y_LSB__POS           3
#define BMI160_USER_DATA_MAG_Y_LSB__LEN           5
#define BMI160_USER_DATA_MAG_Y_LSB__MSK          0xF8
#define BMI160_USER_DATA_MAG_Y_LSB__REG          BMI160_USER_DATA_2_ADDR

/* Mag_Y(MSB) Description - Reg Addr --> 0x07, Bit --> 0...7 */
#define BMI160_USER_DATA_3_MAG_Y_MSB__POS           0
#define BMI160_USER_DATA_3_MAG_Y_MSB__LEN           8
#define BMI160_USER_DATA_3_MAG_Y_MSB__MSK          0xFF
#define BMI160_USER_DATA_3_MAG_Y_MSB__REG          BMI160_USER_DATA_3_ADDR

/* Mag_Z(LSB) Description - Reg Addr --> 0x08, Bit --> 0...7 */
#define BMI160_USER_DATA_4_MAG_Z_LSB__POS           0
#define BMI160_USER_DATA_4_MAG_Z_LSB__LEN           8
#define BMI160_USER_DATA_4_MAG_Z_LSB__MSK          0xFF
#define BMI160_USER_DATA_4_MAG_Z_LSB__REG          BMI160_USER_DATA_4_ADDR

/* Mag_X(LSB) Description - Reg Addr --> 0x08, Bit --> 1...7 */
#define BMI160_USER_DATA_MAG_Z_LSB__POS           1
#define BMI160_USER_DATA_MAG_Z_LSB__LEN           7
#define BMI160_USER_DATA_MAG_Z_LSB__MSK          0xFE
#define BMI160_USER_DATA_MAG_Z_LSB__REG          BMI160_USER_DATA_4_ADDR

/* Mag_Z(MSB) Description - Reg Addr --> 0x09, Bit --> 0...7 */
#define BMI160_USER_DATA_5_MAG_Z_MSB__POS           0
#define BMI160_USER_DATA_5_MAG_Z_MSB__LEN           8
#define BMI160_USER_DATA_5_MAG_Z_MSB__MSK          0xFF
#define BMI160_USER_DATA_5_MAG_Z_MSB__REG          BMI160_USER_DATA_5_ADDR

/* RHALL(LSB) Description - Reg Addr --> 0x0A, Bit --> 0...7 */
#define BMI160_USER_DATA_6_RHALL_LSB__POS           0
#define BMI160_USER_DATA_6_RHALL_LSB__LEN           8
#define BMI160_USER_DATA_6_RHALL_LSB__MSK          0xFF
#define BMI160_USER_DATA_6_RHALL_LSB__REG          BMI160_USER_DATA_6_ADDR

/* Mag_R(LSB) Description - Reg Addr --> 0x0A, Bit --> 3...7 */
#define BMI160_USER_DATA_MAG_R_LSB__POS           2
#define BMI160_USER_DATA_MAG_R_LSB__LEN           6
#define BMI160_USER_DATA_MAG_R_LSB__MSK          0xFC
#define BMI160_USER_DATA_MAG_R_LSB__REG          BMI160_USER_DATA_6_ADDR

/* RHALL(MSB) Description - Reg Addr --> 0x0B, Bit --> 0...7 */
#define BMI160_USER_DATA_7_RHALL_MSB__POS           0
#define BMI160_USER_DATA_7_RHALL_MSB__LEN           8
#define BMI160_USER_DATA_7_RHALL_MSB__MSK          0xFF
#define BMI160_USER_DATA_7_RHALL_MSB__REG          BMI160_USER_DATA_7_ADDR
/**************************************************************/
/**\name    GYRO DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* GYR_X (LSB) Description - Reg Addr --> 0x0C, Bit --> 0...7 */
#define BMI160_USER_DATA_8_GYRO_X_LSB__POS           0
#define BMI160_USER_DATA_8_GYRO_X_LSB__LEN           8
#define BMI160_USER_DATA_8_GYRO_X_LSB__MSK          0xFF
#define BMI160_USER_DATA_8_GYRO_X_LSB__REG          BMI160_USER_DATA_8_ADDR

/* GYR_X (MSB) Description - Reg Addr --> 0x0D, Bit --> 0...7 */
#define BMI160_USER_DATA_9_GYRO_X_MSB__POS           0
#define BMI160_USER_DATA_9_GYRO_X_MSB__LEN           8
#define BMI160_USER_DATA_9_GYRO_X_MSB__MSK          0xFF
#define BMI160_USER_DATA_9_GYRO_X_MSB__REG          BMI160_USER_DATA_9_ADDR

/* GYR_Y (LSB) Description - Reg Addr --> 0x0E, Bit --> 0...7 */
#define BMI160_USER_DATA_10_GYRO_Y_LSB__POS           0
#define BMI160_USER_DATA_10_GYRO_Y_LSB__LEN           8
#define BMI160_USER_DATA_10_GYRO_Y_LSB__MSK          0xFF
#define BMI160_USER_DATA_10_GYRO_Y_LSB__REG          BMI160_USER_DATA_10_ADDR

/* GYR_Y (MSB) Description - Reg Addr --> 0x0F, Bit --> 0...7 */
#define BMI160_USER_DATA_11_GYRO_Y_MSB__POS           0
#define BMI160_USER_DATA_11_GYRO_Y_MSB__LEN           8
#define BMI160_USER_DATA_11_GYRO_Y_MSB__MSK          0xFF
#define BMI160_USER_DATA_11_GYRO_Y_MSB__REG          BMI160_USER_DATA_11_ADDR

/* GYR_Z (LSB) Description - Reg Addr --> 0x10, Bit --> 0...7 */
#define BMI160_USER_DATA_12_GYRO_Z_LSB__POS           0
#define BMI160_USER_DATA_12_GYRO_Z_LSB__LEN           8
#define BMI160_USER_DATA_12_GYRO_Z_LSB__MSK          0xFF
#define BMI160_USER_DATA_12_GYRO_Z_LSB__REG          BMI160_USER_DATA_12_ADDR

/* GYR_Z (MSB) Description - Reg Addr --> 0x11, Bit --> 0...7 */
#define BMI160_USER_DATA_13_GYRO_Z_MSB__POS           0
#define BMI160_USER_DATA_13_GYRO_Z_MSB__LEN           8
#define BMI160_USER_DATA_13_GYRO_Z_MSB__MSK          0xFF
#define BMI160_USER_DATA_13_GYRO_Z_MSB__REG          BMI160_USER_DATA_13_ADDR
/**************************************************************/
/**\name    ACCEL DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* ACC_X (LSB) Description - Reg Addr --> 0x12, Bit --> 0...7 */
#define BMI160_USER_DATA_14_ACCEL_X_LSB__POS           0
#define BMI160_USER_DATA_14_ACCEL_X_LSB__LEN           8
#define BMI160_USER_DATA_14_ACCEL_X_LSB__MSK          0xFF
#define BMI160_USER_DATA_14_ACCEL_X_LSB__REG          BMI160_USER_DATA_14_ADDR

/* ACC_X (MSB) Description - Reg Addr --> 0x13, Bit --> 0...7 */
#define BMI160_USER_DATA_15_ACCEL_X_MSB__POS           0
#define BMI160_USER_DATA_15_ACCEL_X_MSB__LEN           8
#define BMI160_USER_DATA_15_ACCEL_X_MSB__MSK          0xFF
#define BMI160_USER_DATA_15_ACCEL_X_MSB__REG          BMI160_USER_DATA_15_ADDR

/* ACC_Y (LSB) Description - Reg Addr --> 0x14, Bit --> 0...7 */
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__POS           0
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__LEN           8
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__MSK          0xFF
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__REG          BMI160_USER_DATA_16_ADDR

/* ACC_Y (MSB) Description - Reg Addr --> 0x15, Bit --> 0...7 */
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__POS           0
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__LEN           8
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__MSK          0xFF
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__REG          BMI160_USER_DATA_17_ADDR

/* ACC_Z (LSB) Description - Reg Addr --> 0x16, Bit --> 0...7 */
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__POS           0
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__LEN           8
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__MSK          0xFF
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__REG          BMI160_USER_DATA_18_ADDR

/* ACC_Z (MSB) Description - Reg Addr --> 0x17, Bit --> 0...7 */
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__POS           0
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__LEN           8
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__MSK          0xFF
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__REG          BMI160_USER_DATA_19_ADDR
/**************************************************************/
/**\name    SENSOR TIME LENGTH, POSITION AND MASK    */
/**************************************************************/
/* SENSORTIME_0 (LSB) Description - Reg Addr --> 0x18, Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__POS           0
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__LEN           8
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__MSK          0xFF
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__REG          \
        BMI160_USER_SENSORTIME_0_ADDR

/* SENSORTIME_1 (MSB) Description - Reg Addr --> 0x19, Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__POS           0
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__LEN           8
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__MSK          0xFF
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__REG          \
        BMI160_USER_SENSORTIME_1_ADDR

/* SENSORTIME_2 (MSB) Description - Reg Addr --> 0x1A, Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__POS           0
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__LEN           8
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__MSK          0xFF
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__REG          \
        BMI160_USER_SENSORTIME_2_ADDR
/**************************************************************/
/**\name    GYRO SELF TEST LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 1 */
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__POS          1
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__LEN          1
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__MSK          0x02
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__REG         \
        BMI160_USER_STAT_ADDR
/**************************************************************/
/**\name    MAG MANUAL OPERATION LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 2 */
#define BMI160_USER_STAT_MAG_MANUAL_OPERATION__POS          2
#define BMI160_USER_STAT_MAG_MANUAL_OPERATION__LEN          1
#define BMI160_USER_STAT_MAG_MANUAL_OPERATION__MSK          0x04
#define BMI160_USER_STAT_MAG_MANUAL_OPERATION__REG          \
        BMI160_USER_STAT_ADDR
/**************************************************************/
/**\name    FOC STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 3 */
#define BMI160_USER_STAT_FOC_RDY__POS          3
#define BMI160_USER_STAT_FOC_RDY__LEN          1
#define BMI160_USER_STAT_FOC_RDY__MSK          0x08
#define BMI160_USER_STAT_FOC_RDY__REG          BMI160_USER_STAT_ADDR
/**************************************************************/
/**\name    NVM READY LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 4 */
#define BMI160_USER_STAT_NVM_RDY__POS           4
#define BMI160_USER_STAT_NVM_RDY__LEN           1
#define BMI160_USER_STAT_NVM_RDY__MSK           0x10
#define BMI160_USER_STAT_NVM_RDY__REG           BMI160_USER_STAT_ADDR
/**************************************************************/
/**\name    DATA READY LENGTH, POSITION AND MASK FOR ACCEL, MAG AND GYRO*/
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 5 */
#define BMI160_USER_STAT_DATA_RDY_MAG__POS           5
#define BMI160_USER_STAT_DATA_RDY_MAG__LEN           1
#define BMI160_USER_STAT_DATA_RDY_MAG__MSK           0x20
#define BMI160_USER_STAT_DATA_RDY_MAG__REG           BMI160_USER_STAT_ADDR

/* Status Description - Reg Addr --> 0x1B, Bit --> 6 */
#define BMI160_USER_STAT_DATA_RDY_GYRO__POS           6
#define BMI160_USER_STAT_DATA_RDY_GYRO__LEN           1
#define BMI160_USER_STAT_DATA_RDY_GYRO__MSK           0x40
#define BMI160_USER_STAT_DATA_RDY_GYRO__REG           BMI160_USER_STAT_ADDR

/* Status Description - Reg Addr --> 0x1B, Bit --> 7 */
#define BMI160_USER_STAT_DATA_RDY_ACCEL__POS           7
#define BMI160_USER_STAT_DATA_RDY_ACCEL__LEN           1
#define BMI160_USER_STAT_DATA_RDY_ACCEL__MSK           0x80
#define BMI160_USER_STAT_DATA_RDY_ACCEL__REG           BMI160_USER_STAT_ADDR
/**************************************************************/
/**\name    INTERRUPT STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 0 */
#define BMI160_USER_INTR_STAT_0_STEP_INTR__POS           0
#define BMI160_USER_INTR_STAT_0_STEP_INTR__LEN           1
#define BMI160_USER_INTR_STAT_0_STEP_INTR__MSK          0x01
#define BMI160_USER_INTR_STAT_0_STEP_INTR__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    SIGNIFICANT INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 1 */
#define BMI160_USER_INTR_STAT_0_SIGNIFICANT_INTR__POS       1
#define BMI160_USER_INTR_STAT_0_SIGNIFICANT_INTR__LEN       1
#define BMI160_USER_INTR_STAT_0_SIGNIFICANT_INTR__MSK       0x02
#define BMI160_USER_INTR_STAT_0_SIGNIFICANT_INTR__REG       \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    ANY_MOTION INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 2 */
#define BMI160_USER_INTR_STAT_0_ANY_MOTION__POS           2
#define BMI160_USER_INTR_STAT_0_ANY_MOTION__LEN           1
#define BMI160_USER_INTR_STAT_0_ANY_MOTION__MSK          0x04
#define BMI160_USER_INTR_STAT_0_ANY_MOTION__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    PMU TRIGGER INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 3 */
#define BMI160_USER_INTR_STAT_0_PMU_TRIGGER__POS           3
#define BMI160_USER_INTR_STAT_0_PMU_TRIGGER__LEN           1
#define BMI160_USER_INTR_STAT_0_PMU_TRIGGER__MSK          0x08
#define BMI160_USER_INTR_STAT_0_PMU_TRIGGER__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    DOUBLE TAP INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 4 */
#define BMI160_USER_INTR_STAT_0_DOUBLE_TAP_INTR__POS           4
#define BMI160_USER_INTR_STAT_0_DOUBLE_TAP_INTR__LEN           1
#define BMI160_USER_INTR_STAT_0_DOUBLE_TAP_INTR__MSK          0x10
#define BMI160_USER_INTR_STAT_0_DOUBLE_TAP_INTR__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    SINGLE TAP INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 5 */
#define BMI160_USER_INTR_STAT_0_SINGLE_TAP_INTR__POS           5
#define BMI160_USER_INTR_STAT_0_SINGLE_TAP_INTR__LEN           1
#define BMI160_USER_INTR_STAT_0_SINGLE_TAP_INTR__MSK          0x20
#define BMI160_USER_INTR_STAT_0_SINGLE_TAP_INTR__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    ORIENT INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 6 */
#define BMI160_USER_INTR_STAT_0_ORIENT__POS           6
#define BMI160_USER_INTR_STAT_0_ORIENT__LEN           1
#define BMI160_USER_INTR_STAT_0_ORIENT__MSK          0x40
#define BMI160_USER_INTR_STAT_0_ORIENT__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    FLAT INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_0 Description - Reg Addr --> 0x1C, Bit --> 7 */
#define BMI160_USER_INTR_STAT_0_FLAT__POS           7
#define BMI160_USER_INTR_STAT_0_FLAT__LEN           1
#define BMI160_USER_INTR_STAT_0_FLAT__MSK          0x80
#define BMI160_USER_INTR_STAT_0_FLAT__REG          \
        BMI160_USER_INTR_STAT_0_ADDR
/**************************************************************/
/**\name    HIGH_G INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 2 */
#define BMI160_USER_INTR_STAT_1_HIGH_G_INTR__POS               2
#define BMI160_USER_INTR_STAT_1_HIGH_G_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_HIGH_G_INTR__MSK              0x04
#define BMI160_USER_INTR_STAT_1_HIGH_G_INTR__REG              \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name    LOW_G INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 3 */
#define BMI160_USER_INTR_STAT_1_LOW_G_INTR__POS               3
#define BMI160_USER_INTR_STAT_1_LOW_G_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_LOW_G_INTR__MSK              0x08
#define BMI160_USER_INTR_STAT_1_LOW_G_INTR__REG              \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name    DATA READY INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 4 */
#define BMI160_USER_INTR_STAT_1_DATA_RDY_INTR__POS               4
#define BMI160_USER_INTR_STAT_1_DATA_RDY_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_DATA_RDY_INTR__MSK               0x10
#define BMI160_USER_INTR_STAT_1_DATA_RDY_INTR__REG               \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name    FIFO FULL INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 5 */
#define BMI160_USER_INTR_STAT_1_FIFO_FULL_INTR__POS               5
#define BMI160_USER_INTR_STAT_1_FIFO_FULL_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_FIFO_FULL_INTR__MSK               0x20
#define BMI160_USER_INTR_STAT_1_FIFO_FULL_INTR__REG               \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name FIFO WATERMARK INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 6 */
#define BMI160_USER_INTR_STAT_1_FIFO_WM_INTR__POS               6
#define BMI160_USER_INTR_STAT_1_FIFO_WM_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_FIFO_WM_INTR__MSK               0x40
#define BMI160_USER_INTR_STAT_1_FIFO_WM_INTR__REG               \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name    NO MOTION INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_1 Description - Reg Addr --> 0x1D, Bit --> 7 */
#define BMI160_USER_INTR_STAT_1_NOMOTION_INTR__POS               7
#define BMI160_USER_INTR_STAT_1_NOMOTION_INTR__LEN               1
#define BMI160_USER_INTR_STAT_1_NOMOTION_INTR__MSK               0x80
#define BMI160_USER_INTR_STAT_1_NOMOTION_INTR__REG               \
        BMI160_USER_INTR_STAT_1_ADDR
/**************************************************************/
/**\name    ANY MOTION-XYZ AXIS INTERRUPT STATUS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 0 */
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_X__POS               0
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_X__LEN               1
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_X__MSK               0x01
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_X__REG               \
        BMI160_USER_INTR_STAT_2_ADDR

/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 1 */
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Y__POS               1
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Y__LEN               1
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Y__MSK               0x02
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Y__REG               \
        BMI160_USER_INTR_STAT_2_ADDR

/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 2 */
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Z__POS               2
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Z__LEN               1
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Z__MSK               0x04
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_FIRST_Z__REG               \
        BMI160_USER_INTR_STAT_2_ADDR
/**************************************************************/
/**\name    ANY MOTION SIGN LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 3 */
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_SIGN__POS               3
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_SIGN__LEN               1
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_SIGN__MSK               0x08
#define BMI160_USER_INTR_STAT_2_ANY_MOTION_SIGN__REG               \
        BMI160_USER_INTR_STAT_2_ADDR
/**************************************************************/
/**\name    TAP_XYZ AND SIGN LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 4 */
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_X__POS               4
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_X__LEN               1
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_X__MSK               0x10
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_X__REG               \
        BMI160_USER_INTR_STAT_2_ADDR

/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 5 */
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Y__POS               5
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Y__LEN               1
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Y__MSK               0x20
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Y__REG               \
        BMI160_USER_INTR_STAT_2_ADDR

/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 6 */
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Z__POS               6
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Z__LEN               1
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Z__MSK               0x40
#define BMI160_USER_INTR_STAT_2_TAP_FIRST_Z__REG               \
        BMI160_USER_INTR_STAT_2_ADDR

/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 7 */
#define BMI160_USER_INTR_STAT_2_TAP_SIGN__POS               7
#define BMI160_USER_INTR_STAT_2_TAP_SIGN__LEN               1
#define BMI160_USER_INTR_STAT_2_TAP_SIGN__MSK               0x80
#define BMI160_USER_INTR_STAT_2_TAP_SIGN__REG               \
        BMI160_USER_INTR_STAT_2_ADDR
/**************************************************************/
/**\name    INTERRUPT SATAUS FOR WHOLE 0x1E LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_2 Description - Reg Addr --> 0x1E, Bit --> 0...7 */
#define BMI160_USER_INTR_STAT_2__POS               0
#define BMI160_USER_INTR_STAT_2__LEN               8
#define BMI160_USER_INTR_STAT_2__MSK               0xFF
#define BMI160_USER_INTR_STAT_2__REG               \
        BMI160_USER_INTR_STAT_2_ADDR
/**************************************************************/
/**\name    HIGH_G-XYZ AND SIGN LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 0 */
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_X__POS               0
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_X__LEN               1
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_X__MSK               0x01
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_X__REG               \
        BMI160_USER_INTR_STAT_3_ADDR

/* Int_Status_3 Description - Reg Addr --> 0x1E, Bit --> 1 */
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Y__POS               1
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Y__LEN               1
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Y__MSK               0x02
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Y__REG               \
        BMI160_USER_INTR_STAT_3_ADDR

/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 2 */
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Z__POS               2
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Z__LEN               1
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Z__MSK               0x04
#define BMI160_USER_INTR_STAT_3_HIGH_G_FIRST_Z__REG               \
        BMI160_USER_INTR_STAT_3_ADDR

/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 3 */
#define BMI160_USER_INTR_STAT_3_HIGH_G_SIGN__POS               3
#define BMI160_USER_INTR_STAT_3_HIGH_G_SIGN__LEN               1
#define BMI160_USER_INTR_STAT_3_HIGH_G_SIGN__MSK               0x08
#define BMI160_USER_INTR_STAT_3_HIGH_G_SIGN__REG               \
        BMI160_USER_INTR_STAT_3_ADDR
/**************************************************************/
/**\name    ORIENT XY and Z AXIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 4...5 */
#define BMI160_USER_INTR_STAT_3_ORIENT_XY__POS               4
#define BMI160_USER_INTR_STAT_3_ORIENT_XY__LEN               2
#define BMI160_USER_INTR_STAT_3_ORIENT_XY__MSK               0x30
#define BMI160_USER_INTR_STAT_3_ORIENT_XY__REG               \
        BMI160_USER_INTR_STAT_3_ADDR

/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 6 */
#define BMI160_USER_INTR_STAT_3_ORIENT_Z__POS               6
#define BMI160_USER_INTR_STAT_3_ORIENT_Z__LEN               1
#define BMI160_USER_INTR_STAT_3_ORIENT_Z__MSK               0x40
#define BMI160_USER_INTR_STAT_3_ORIENT_Z__REG               \
        BMI160_USER_INTR_STAT_3_ADDR
/**************************************************************/
/**\name    FLAT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 7 */
#define BMI160_USER_INTR_STAT_3_FLAT__POS               7
#define BMI160_USER_INTR_STAT_3_FLAT__LEN               1
#define BMI160_USER_INTR_STAT_3_FLAT__MSK               0x80
#define BMI160_USER_INTR_STAT_3_FLAT__REG               \
        BMI160_USER_INTR_STAT_3_ADDR
/**************************************************************/
/**\name    0x1F LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Status_3 Description - Reg Addr --> 0x1F, Bit --> 0...7 */
#define BMI160_USER_INTR_STAT_3__POS               0
#define BMI160_USER_INTR_STAT_3__LEN               8
#define BMI160_USER_INTR_STAT_3__MSK               0xFF
#define BMI160_USER_INTR_STAT_3__REG               \
        BMI160_USER_INTR_STAT_3_ADDR
/**************************************************************/
/**\name    TEMPERATURE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Temperature Description - LSB Reg Addr --> 0x20, Bit --> 0...7 */
#define BMI160_USER_TEMP_LSB_VALUE__POS               0
#define BMI160_USER_TEMP_LSB_VALUE__LEN               8
#define BMI160_USER_TEMP_LSB_VALUE__MSK               0xFF
#define BMI160_USER_TEMP_LSB_VALUE__REG               \
        BMI160_USER_TEMPERATURE_0_ADDR

/* Temperature Description - LSB Reg Addr --> 0x21, Bit --> 0...7 */
#define BMI160_USER_TEMP_MSB_VALUE__POS               0
#define BMI160_USER_TEMP_MSB_VALUE__LEN               8
#define BMI160_USER_TEMP_MSB_VALUE__MSK               0xFF
#define BMI160_USER_TEMP_MSB_VALUE__REG               \
        BMI160_USER_TEMPERATURE_1_ADDR
/**************************************************************/
/**\name    FIFO BYTE COUNTER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Length0 Description - Reg Addr --> 0x22, Bit --> 0...7 */
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__POS           0
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__LEN           8
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__MSK          0xFF
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__REG          \
        BMI160_USER_FIFO_LENGTH_0_ADDR

/*Fifo_Length1 Description - Reg Addr --> 0x23, Bit --> 0...2 */
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__POS           0
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__LEN           3
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__MSK          0x07
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__REG          \
        BMI160_USER_FIFO_LENGTH_1_ADDR

/**************************************************************/
/**\name    FIFO DATA LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Data Description - Reg Addr --> 0x24, Bit --> 0...7 */
#define BMI160_USER_FIFO_DATA__POS           0
#define BMI160_USER_FIFO_DATA__LEN           8
#define BMI160_USER_FIFO_DATA__MSK          0xFF
#define BMI160_USER_FIFO_DATA__REG          BMI160_USER_FIFO_DATA_ADDR

/**************************************************************/
/**\name    ACCEL CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Acc_Conf Description - Reg Addr --> 0x40, Bit --> 0...3 */
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__POS               0
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__LEN               4
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__MSK               0x0F
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG             \
BMI160_USER_ACCEL_CONFIG_ADDR

/* Acc_Conf Description - Reg Addr --> 0x40, Bit --> 4...6 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__POS               4
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__LEN               3
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__MSK               0x70
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG  BMI160_USER_ACCEL_CONFIG_ADDR

/* Acc_Conf Description - Reg Addr --> 0x40, Bit --> 7 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__POS           7
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__LEN           1
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__MSK           0x80
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__REG  \
BMI160_USER_ACCEL_CONFIG_ADDR

/* Acc_Range Description - Reg Addr --> 0x41, Bit --> 0...3 */
#define BMI160_USER_ACCEL_RANGE__POS               0
#define BMI160_USER_ACCEL_RANGE__LEN               4
#define BMI160_USER_ACCEL_RANGE__MSK               0x0F
#define BMI160_USER_ACCEL_RANGE__REG               BMI160_USER_ACCEL_RANGE_ADDR
/**************************************************************/
/**\name    GYRO CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Gyro_Conf Description - Reg Addr --> 0x42, Bit --> 0...3 */
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__POS               0
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__LEN               4
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__MSK               0x0F
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG               \
BMI160_USER_GYRO_CONFIG_ADDR

/* Gyro_Conf Description - Reg Addr --> 0x42, Bit --> 4...5 */
#define BMI160_USER_GYRO_CONFIG_BW__POS               4
#define BMI160_USER_GYRO_CONFIG_BW__LEN               2
#define BMI160_USER_GYRO_CONFIG_BW__MSK               0x30
#define BMI160_USER_GYRO_CONFIG_BW__REG               \
BMI160_USER_GYRO_CONFIG_ADDR

/* Gyr_Range Description - Reg Addr --> 0x43, Bit --> 0...2 */
#define BMI160_USER_GYRO_RANGE__POS               0
#define BMI160_USER_GYRO_RANGE__LEN               3
#define BMI160_USER_GYRO_RANGE__MSK               0x07
#define BMI160_USER_GYRO_RANGE__REG               BMI160_USER_GYRO_RANGE_ADDR
/**************************************************************/
/**\name    MAG CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Mag_Conf Description - Reg Addr --> 0x44, Bit --> 0...3 */
#define BMI160_USER_MAG_CONFIG_OUTPUT_DATA_RATE__POS               0
#define BMI160_USER_MAG_CONFIG_OUTPUT_DATA_RATE__LEN               4
#define BMI160_USER_MAG_CONFIG_OUTPUT_DATA_RATE__MSK               0x0F
#define BMI160_USER_MAG_CONFIG_OUTPUT_DATA_RATE__REG               \
BMI160_USER_MAG_CONFIG_ADDR
/**************************************************************/
/**\name    FIFO DOWNS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Downs Description - Reg Addr --> 0x45, Bit --> 0...2 */
#define BMI160_USER_FIFO_DOWN_GYRO__POS               0
#define BMI160_USER_FIFO_DOWN_GYRO__LEN               3
#define BMI160_USER_FIFO_DOWN_GYRO__MSK               0x07
#define BMI160_USER_FIFO_DOWN_GYRO__REG BMI160_USER_FIFO_DOWN_ADDR
/**************************************************************/
/**\name    FIFO FILTER FOR ACCEL AND GYRO LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_filt Description - Reg Addr --> 0x45, Bit --> 3 */
#define BMI160_USER_FIFO_FILTER_GYRO__POS               3
#define BMI160_USER_FIFO_FILTER_GYRO__LEN               1
#define BMI160_USER_FIFO_FILTER_GYRO__MSK               0x08
#define BMI160_USER_FIFO_FILTER_GYRO__REG     BMI160_USER_FIFO_DOWN_ADDR

/* Fifo_Downs Description - Reg Addr --> 0x45, Bit --> 4...6 */
#define BMI160_USER_FIFO_DOWN_ACCEL__POS               4
#define BMI160_USER_FIFO_DOWN_ACCEL__LEN               3
#define BMI160_USER_FIFO_DOWN_ACCEL__MSK               0x70
#define BMI160_USER_FIFO_DOWN_ACCEL__REG    BMI160_USER_FIFO_DOWN_ADDR

/* Fifo_FILT Description - Reg Addr --> 0x45, Bit --> 7 */
#define BMI160_USER_FIFO_FILTER_ACCEL__POS               7
#define BMI160_USER_FIFO_FILTER_ACCEL__LEN               1
#define BMI160_USER_FIFO_FILTER_ACCEL__MSK               0x80
#define BMI160_USER_FIFO_FILTER_ACCEL__REG  BMI160_USER_FIFO_DOWN_ADDR
/**************************************************************/
/**\name    FIFO WATER MARK LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_0 Description - Reg Addr --> 0x46, Bit --> 0...7 */
#define BMI160_USER_FIFO_WM__POS               0
#define BMI160_USER_FIFO_WM__LEN               8
#define BMI160_USER_FIFO_WM__MSK               0xFF
#define BMI160_USER_FIFO_WM__REG    BMI160_USER_FIFO_CONFIG_0_ADDR
/**************************************************************/
/**\name    FIFO STOP ON FULL LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 0 */
#define BMI160_USER_FIFO_STOP_ON_FULL__POS      0
#define BMI160_USER_FIFO_STOP_ON_FULL__LEN      1
#define BMI160_USER_FIFO_STOP_ON_FULL__MSK      0x01
#define BMI160_USER_FIFO_STOP_ON_FULL__REG  BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO TIME LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 1 */
#define BMI160_USER_FIFO_TIME_ENABLE__POS               1
#define BMI160_USER_FIFO_TIME_ENABLE__LEN               1
#define BMI160_USER_FIFO_TIME_ENABLE__MSK               0x02
#define BMI160_USER_FIFO_TIME_ENABLE__REG   BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO TAG INTERRUPT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 2 */
#define BMI160_USER_FIFO_TAG_INTR2_ENABLE__POS               2
#define BMI160_USER_FIFO_TAG_INTR2_ENABLE__LEN               1
#define BMI160_USER_FIFO_TAG_INTR2_ENABLE__MSK               0x04
#define BMI160_USER_FIFO_TAG_INTR2_ENABLE__REG  BMI160_USER_FIFO_CONFIG_1_ADDR

/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 3 */
#define BMI160_USER_FIFO_TAG_INTR1_ENABLE__POS               3
#define BMI160_USER_FIFO_TAG_INTR1_ENABLE__LEN               1
#define BMI160_USER_FIFO_TAG_INTR1_ENABLE__MSK               0x08
#define BMI160_USER_FIFO_TAG_INTR1_ENABLE__REG  BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO HEADER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 4 */
#define BMI160_USER_FIFO_HEADER_ENABLE__POS               4
#define BMI160_USER_FIFO_HEADER_ENABLE__LEN               1
#define BMI160_USER_FIFO_HEADER_ENABLE__MSK               0x10
#define BMI160_USER_FIFO_HEADER_ENABLE__REG              \
BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO MAG ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 5 */
#define BMI160_USER_FIFO_MAG_ENABLE__POS               5
#define BMI160_USER_FIFO_MAG_ENABLE__LEN               1
#define BMI160_USER_FIFO_MAG_ENABLE__MSK               0x20
#define BMI160_USER_FIFO_MAG_ENABLE__REG             \
BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO ACCEL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 6 */
#define BMI160_USER_FIFO_ACCEL_ENABLE__POS               6
#define BMI160_USER_FIFO_ACCEL_ENABLE__LEN               1
#define BMI160_USER_FIFO_ACCEL_ENABLE__MSK               0x40
#define BMI160_USER_FIFO_ACCEL_ENABLE__REG              \
BMI160_USER_FIFO_CONFIG_1_ADDR
/**************************************************************/
/**\name    FIFO GYRO ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 7 */
#define BMI160_USER_FIFO_GYRO_ENABLE__POS               7
#define BMI160_USER_FIFO_GYRO_ENABLE__LEN               1
#define BMI160_USER_FIFO_GYRO_ENABLE__MSK               0x80
#define BMI160_USER_FIFO_GYRO_ENABLE__REG              \
BMI160_USER_FIFO_CONFIG_1_ADDR

/**************************************************************/
/**\name    MAG I2C ADDRESS SELECTION LENGTH, POSITION AND MASK*/
/**************************************************************/

/* Mag_IF_0 Description - Reg Addr --> 0x4b, Bit --> 1...7 */
#define BMI160_USER_I2C_DEVICE_ADDR__POS               1
#define BMI160_USER_I2C_DEVICE_ADDR__LEN               7
#define BMI160_USER_I2C_DEVICE_ADDR__MSK               0xFE
#define BMI160_USER_I2C_DEVICE_ADDR__REG    BMI160_USER_MAG_IF_0_ADDR
/**************************************************************/
/**\name MAG CONFIGURATION FOR SECONDARY
    INTERFACE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Mag_IF_1 Description - Reg Addr --> 0x4c, Bit --> 0...1 */
#define BMI160_USER_MAG_BURST__POS               0
#define BMI160_USER_MAG_BURST__LEN               2
#define BMI160_USER_MAG_BURST__MSK               0x03
#define BMI160_USER_MAG_BURST__REG               BMI160_USER_MAG_IF_1_ADDR

/* Mag_IF_1 Description - Reg Addr --> 0x4c, Bit --> 2...5 */
#define BMI160_USER_MAG_OFFSET__POS               2
#define BMI160_USER_MAG_OFFSET__LEN               4
#define BMI160_USER_MAG_OFFSET__MSK               0x3C
#define BMI160_USER_MAG_OFFSET__REG               BMI160_USER_MAG_IF_1_ADDR

/* Mag_IF_1 Description - Reg Addr --> 0x4c, Bit --> 7 */
#define BMI160_USER_MAG_MANUAL_ENABLE__POS               7
#define BMI160_USER_MAG_MANUAL_ENABLE__LEN               1
#define BMI160_USER_MAG_MANUAL_ENABLE__MSK               0x80
#define BMI160_USER_MAG_MANUAL_ENABLE__REG               \
BMI160_USER_MAG_IF_1_ADDR

/* Mag_IF_2 Description - Reg Addr --> 0x4d, Bit -->0... 7 */
#define BMI160_USER_READ_ADDR__POS               0
#define BMI160_USER_READ_ADDR__LEN               8
#define BMI160_USER_READ_ADDR__MSK               0xFF
#define BMI160_USER_READ_ADDR__REG               BMI160_USER_MAG_IF_2_ADDR

/* Mag_IF_3 Description - Reg Addr --> 0x4e, Bit -->0... 7 */
#define BMI160_USER_WRITE_ADDR__POS               0
#define BMI160_USER_WRITE_ADDR__LEN               8
#define BMI160_USER_WRITE_ADDR__MSK               0xFF
#define BMI160_USER_WRITE_ADDR__REG               BMI160_USER_MAG_IF_3_ADDR

/* Mag_IF_4 Description - Reg Addr --> 0x4f, Bit -->0... 7 */
#define BMI160_USER_WRITE_DATA__POS               0
#define BMI160_USER_WRITE_DATA__LEN               8
#define BMI160_USER_WRITE_DATA__MSK               0xFF
#define BMI160_USER_WRITE_DATA__REG               BMI160_USER_MAG_IF_4_ADDR
/**************************************************************/
/**\name    ANY MOTION XYZ AXIS ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->0 */
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_X_ENABLE__POS               0
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_X_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_X_ENABLE__MSK               0x01
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_X_ENABLE__REG                \
BMI160_USER_INTR_ENABLE_0_ADDR

/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->1 */
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Y_ENABLE__POS               1
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Y_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Y_ENABLE__MSK               0x02
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Y_ENABLE__REG            \
BMI160_USER_INTR_ENABLE_0_ADDR

/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->2 */
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Z_ENABLE__POS               2
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Z_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Z_ENABLE__MSK               0x04
#define BMI160_USER_INTR_ENABLE_0_ANY_MOTION_Z_ENABLE__REG              \
BMI160_USER_INTR_ENABLE_0_ADDR
/**************************************************************/
/**\name    DOUBLE TAP ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->4 */
#define BMI160_USER_INTR_ENABLE_0_DOUBLE_TAP_ENABLE__POS               4
#define BMI160_USER_INTR_ENABLE_0_DOUBLE_TAP_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_DOUBLE_TAP_ENABLE__MSK               0x10
#define BMI160_USER_INTR_ENABLE_0_DOUBLE_TAP_ENABLE__REG            \
BMI160_USER_INTR_ENABLE_0_ADDR
/**************************************************************/
/**\name    SINGLE TAP ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->5 */
#define BMI160_USER_INTR_ENABLE_0_SINGLE_TAP_ENABLE__POS               5
#define BMI160_USER_INTR_ENABLE_0_SINGLE_TAP_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_SINGLE_TAP_ENABLE__MSK               0x20
#define BMI160_USER_INTR_ENABLE_0_SINGLE_TAP_ENABLE__REG           \
BMI160_USER_INTR_ENABLE_0_ADDR
/**************************************************************/
/**\name    ORIENT ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->6 */
#define BMI160_USER_INTR_ENABLE_0_ORIENT_ENABLE__POS               6
#define BMI160_USER_INTR_ENABLE_0_ORIENT_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_ORIENT_ENABLE__MSK               0x40
#define BMI160_USER_INTR_ENABLE_0_ORIENT_ENABLE__REG               \
BMI160_USER_INTR_ENABLE_0_ADDR
/**************************************************************/
/**\name    FLAT ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_0 Description - Reg Addr --> 0x50, Bit -->7 */
#define BMI160_USER_INTR_ENABLE_0_FLAT_ENABLE__POS               7
#define BMI160_USER_INTR_ENABLE_0_FLAT_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_0_FLAT_ENABLE__MSK               0x80
#define BMI160_USER_INTR_ENABLE_0_FLAT_ENABLE__REG             \
BMI160_USER_INTR_ENABLE_0_ADDR
/**************************************************************/
/**\name    HIGH_G XYZ ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->0 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__POS               0
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__MSK               0x01
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__REG             \
BMI160_USER_INTR_ENABLE_1_ADDR

/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->1 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__POS               1
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__MSK               0x02
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__REG             \
BMI160_USER_INTR_ENABLE_1_ADDR

/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->2 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__POS               2
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__MSK               0x04
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__REG             \
BMI160_USER_INTR_ENABLE_1_ADDR
/**************************************************************/
/**\name    LOW_G ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->3 */
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__POS               3
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__MSK               0x08
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__REG           \
BMI160_USER_INTR_ENABLE_1_ADDR
/**************************************************************/
/**\name    DATA READY ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->4 */
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__POS               4
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__MSK               0x10
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG              \
BMI160_USER_INTR_ENABLE_1_ADDR
/**************************************************************/
/**\name    FIFO FULL AND WATER MARK ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->5 */
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__POS               5
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__MSK               0x20
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__REG               \
BMI160_USER_INTR_ENABLE_1_ADDR

/* Int_En_1 Description - Reg Addr --> 0x51, Bit -->6 */
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__POS               6
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__MSK               0x40
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__REG              \
BMI160_USER_INTR_ENABLE_1_ADDR
/**************************************************************/
/**\name    NO MOTION XYZ ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_2 Description - Reg Addr --> 0x52, Bit -->0 */
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_X_ENABLE__POS               0
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_X_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_X_ENABLE__MSK               0x01
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_X_ENABLE__REG      \
BMI160_USER_INTR_ENABLE_2_ADDR

/* Int_En_2 Description - Reg Addr --> 0x52, Bit -->1 */
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Y_ENABLE__POS               1
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Y_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Y_ENABLE__MSK               0x02
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Y_ENABLE__REG      \
BMI160_USER_INTR_ENABLE_2_ADDR

/* Int_En_2 Description - Reg Addr --> 0x52, Bit -->2 */
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Z_ENABLE__POS               2
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Z_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Z_ENABLE__MSK               0x04
#define BMI160_USER_INTR_ENABLE_2_NOMOTION_Z_ENABLE__REG      \
BMI160_USER_INTR_ENABLE_2_ADDR
/**************************************************************/
/**\name    STEP DETECTOR ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_2 Description - Reg Addr --> 0x52, Bit -->3 */
#define BMI160_USER_INTR_ENABLE_2_STEP_DETECTOR_ENABLE__POS               3
#define BMI160_USER_INTR_ENABLE_2_STEP_DETECTOR_ENABLE__LEN               1
#define BMI160_USER_INTR_ENABLE_2_STEP_DETECTOR_ENABLE__MSK               0x08
#define BMI160_USER_INTR_ENABLE_2_STEP_DETECTOR_ENABLE__REG   \
BMI160_USER_INTR_ENABLE_2_ADDR
/**************************************************************/
/**\name    EDGE CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->0 */
#define BMI160_USER_INTR1_EDGE_CTRL__POS               0
#define BMI160_USER_INTR1_EDGE_CTRL__LEN               1
#define BMI160_USER_INTR1_EDGE_CTRL__MSK               0x01
#define BMI160_USER_INTR1_EDGE_CTRL__REG        \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    LEVEL CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->1 */
#define BMI160_USER_INTR1_LEVEL__POS               1
#define BMI160_USER_INTR1_LEVEL__LEN               1
#define BMI160_USER_INTR1_LEVEL__MSK               0x02
#define BMI160_USER_INTR1_LEVEL__REG               \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    OUTPUT TYPE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->2 */
#define BMI160_USER_INTR1_OUTPUT_TYPE__POS               2
#define BMI160_USER_INTR1_OUTPUT_TYPE__LEN               1
#define BMI160_USER_INTR1_OUTPUT_TYPE__MSK               0x04
#define BMI160_USER_INTR1_OUTPUT_TYPE__REG               \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    OUTPUT TYPE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->3 */
#define BMI160_USER_INTR1_OUTPUT_ENABLE__POS               3
#define BMI160_USER_INTR1_OUTPUT_ENABLE__LEN               1
#define BMI160_USER_INTR1_OUTPUT_ENABLE__MSK               0x08
#define BMI160_USER_INTR1_OUTPUT_ENABLE__REG        \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    EDGE CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->4 */
#define BMI160_USER_INTR2_EDGE_CTRL__POS               4
#define BMI160_USER_INTR2_EDGE_CTRL__LEN               1
#define BMI160_USER_INTR2_EDGE_CTRL__MSK               0x10
#define BMI160_USER_INTR2_EDGE_CTRL__REG        \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    LEVEL CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->5 */
#define BMI160_USER_INTR2_LEVEL__POS               5
#define BMI160_USER_INTR2_LEVEL__LEN               1
#define BMI160_USER_INTR2_LEVEL__MSK               0x20
#define BMI160_USER_INTR2_LEVEL__REG               \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    OUTPUT TYPE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->6 */
#define BMI160_USER_INTR2_OUTPUT_TYPE__POS               6
#define BMI160_USER_INTR2_OUTPUT_TYPE__LEN               1
#define BMI160_USER_INTR2_OUTPUT_TYPE__MSK               0x40
#define BMI160_USER_INTR2_OUTPUT_TYPE__REG               \
BMI160_USER_INTR_OUT_CTRL_ADDR

/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->7 */
#define BMI160_USER_INTR2_OUTPUT_EN__POS               7
#define BMI160_USER_INTR2_OUTPUT_EN__LEN               1
#define BMI160_USER_INTR2_OUTPUT_EN__MSK               0x80
#define BMI160_USER_INTR2_OUTPUT_EN__REG        \
BMI160_USER_INTR_OUT_CTRL_ADDR
/**************************************************************/
/**\name    LATCH INTERRUPT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Latch Description - Reg Addr --> 0x54, Bit -->0...3 */
#define BMI160_USER_INTR_LATCH__POS               0
#define BMI160_USER_INTR_LATCH__LEN               4
#define BMI160_USER_INTR_LATCH__MSK               0x0F
#define BMI160_USER_INTR_LATCH__REG               BMI160_USER_INTR_LATCH_ADDR
/**************************************************************/
/**\name    INPUT ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Latch Description - Reg Addr --> 0x54, Bit -->4 */
#define BMI160_USER_INTR1_INPUT_ENABLE__POS               4
#define BMI160_USER_INTR1_INPUT_ENABLE__LEN               1
#define BMI160_USER_INTR1_INPUT_ENABLE__MSK               0x10
#define BMI160_USER_INTR1_INPUT_ENABLE__REG               \
BMI160_USER_INTR_LATCH_ADDR

/* Int_Latch Description - Reg Addr --> 0x54, Bit -->5*/
#define BMI160_USER_INTR2_INPUT_ENABLE__POS               5
#define BMI160_USER_INTR2_INPUT_ENABLE__LEN               1
#define BMI160_USER_INTR2_INPUT_ENABLE__MSK               0x20
#define BMI160_USER_INTR2_INPUT_ENABLE__REG              \
BMI160_USER_INTR_LATCH_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF LOW_G LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->0 */
#define BMI160_USER_INTR_MAP_0_INTR1_LOW_G__POS               0
#define BMI160_USER_INTR_MAP_0_INTR1_LOW_G__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_LOW_G__MSK               0x01
#define BMI160_USER_INTR_MAP_0_INTR1_LOW_G__REG BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF HIGH_G LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->1 */
#define BMI160_USER_INTR_MAP_0_INTR1_HIGH_G__POS               1
#define BMI160_USER_INTR_MAP_0_INTR1_HIGH_G__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_HIGH_G__MSK               0x02
#define BMI160_USER_INTR_MAP_0_INTR1_HIGH_G__REG    \
BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT MAPPIONG OF ANY MOTION_G LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->2 */
#define BMI160_USER_INTR_MAP_0_INTR1_ANY_MOTION__POS               2
#define BMI160_USER_INTR_MAP_0_INTR1_ANY_MOTION__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_ANY_MOTION__MSK               0x04
#define BMI160_USER_INTR_MAP_0_INTR1_ANY_MOTION__REG BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF NO MOTION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->3 */
#define BMI160_USER_INTR_MAP_0_INTR1_NOMOTION__POS               3
#define BMI160_USER_INTR_MAP_0_INTR1_NOMOTION__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_NOMOTION__MSK               0x08
#define BMI160_USER_INTR_MAP_0_INTR1_NOMOTION__REG BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF DOUBLE TAP LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->4 */
#define BMI160_USER_INTR_MAP_0_INTR1_DOUBLE_TAP__POS               4
#define BMI160_USER_INTR_MAP_0_INTR1_DOUBLE_TAP__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_DOUBLE_TAP__MSK               0x10
#define BMI160_USER_INTR_MAP_0_INTR1_DOUBLE_TAP__REG    \
BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF SINGLE TAP LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->5 */
#define BMI160_USER_INTR_MAP_0_INTR1_SINGLE_TAP__POS               5
#define BMI160_USER_INTR_MAP_0_INTR1_SINGLE_TAP__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_SINGLE_TAP__MSK               0x20
#define BMI160_USER_INTR_MAP_0_INTR1_SINGLE_TAP__REG          \
BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF ORIENT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x55, Bit -->6 */
#define BMI160_USER_INTR_MAP_0_INTR1_ORIENT__POS               6
#define BMI160_USER_INTR_MAP_0_INTR1_ORIENT__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_ORIENT__MSK               0x40
#define BMI160_USER_INTR_MAP_0_INTR1_ORIENT__REG              \
BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT MAPPIONG OF FLAT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_0 Description - Reg Addr --> 0x56, Bit -->7 */
#define BMI160_USER_INTR_MAP_0_INTR1_FLAT__POS               7
#define BMI160_USER_INTR_MAP_0_INTR1_FLAT__LEN               1
#define BMI160_USER_INTR_MAP_0_INTR1_FLAT__MSK               0x80
#define BMI160_USER_INTR_MAP_0_INTR1_FLAT__REG  BMI160_USER_INTR_MAP_0_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF PMU TRIGGER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->0 */
#define BMI160_USER_INTR_MAP_1_INTR2_PMU_TRIG__POS               0
#define BMI160_USER_INTR_MAP_1_INTR2_PMU_TRIG__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR2_PMU_TRIG__MSK               0x01
#define BMI160_USER_INTR_MAP_1_INTR2_PMU_TRIG__REG BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF FIFO FULL AND
    WATER MARK LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->1 */
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_FULL__POS               1
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_FULL__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_FULL__MSK               0x02
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_FULL__REG          \
BMI160_USER_INTR_MAP_1_ADDR

/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->2 */
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__POS               2
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__MSK               0x04
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__REG            \
BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF DATA READY LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->3 */
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__POS               3
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__MSK               0x08
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__REG        \
BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF PMU TRIGGER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->4 */
#define BMI160_USER_INTR_MAP_1_INTR1_PMU_TRIG__POS               4
#define BMI160_USER_INTR_MAP_1_INTR1_PMU_TRIG__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR1_PMU_TRIG__MSK               0x10
#define BMI160_USER_INTR_MAP_1_INTR1_PMU_TRIG__REG BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF FIFO FULL AND
    WATER MARK LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->5 */
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_FULL__POS               5
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_FULL__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_FULL__MSK               0x20
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_FULL__REG        \
BMI160_USER_INTR_MAP_1_ADDR

/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->6 */
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__POS               6
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__MSK               0x40
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__REG   \
BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT1 MAPPIONG OF DATA READY LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->7 */
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__POS               7
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__LEN               1
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__MSK               0x80
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__REG  \
BMI160_USER_INTR_MAP_1_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF LOW_G LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->0 */
#define BMI160_USER_INTR_MAP_2_INTR2_LOW_G__POS               0
#define BMI160_USER_INTR_MAP_2_INTR2_LOW_G__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_LOW_G__MSK               0x01
#define BMI160_USER_INTR_MAP_2_INTR2_LOW_G__REG BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF HIGH_G LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->1 */
#define BMI160_USER_INTR_MAP_2_INTR2_HIGH_G__POS               1
#define BMI160_USER_INTR_MAP_2_INTR2_HIGH_G__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_HIGH_G__MSK               0x02
#define BMI160_USER_INTR_MAP_2_INTR2_HIGH_G__REG    \
BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF ANY MOTION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->2 */
#define BMI160_USER_INTR_MAP_2_INTR2_ANY_MOTION__POS               2
#define BMI160_USER_INTR_MAP_2_INTR2_ANY_MOTION__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_ANY_MOTION__MSK               0x04
#define BMI160_USER_INTR_MAP_2_INTR2_ANY_MOTION__REG BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF NO MOTION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->3 */
#define BMI160_USER_INTR_MAP_2_INTR2_NOMOTION__POS               3
#define BMI160_USER_INTR_MAP_2_INTR2_NOMOTION__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_NOMOTION__MSK               0x08
#define BMI160_USER_INTR_MAP_2_INTR2_NOMOTION__REG BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF DOUBLE TAP LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->4 */
#define BMI160_USER_INTR_MAP_2_INTR2_DOUBLE_TAP__POS               4
#define BMI160_USER_INTR_MAP_2_INTR2_DOUBLE_TAP__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_DOUBLE_TAP__MSK               0x10
#define BMI160_USER_INTR_MAP_2_INTR2_DOUBLE_TAP__REG    \
BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF SINGLE TAP LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->5 */
#define BMI160_USER_INTR_MAP_2_INTR2_SINGLE_TAP__POS               5
#define BMI160_USER_INTR_MAP_2_INTR2_SINGLE_TAP__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_SINGLE_TAP__MSK               0x20
#define BMI160_USER_INTR_MAP_2_INTR2_SINGLE_TAP__REG    \
BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF ORIENT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->6 */
#define BMI160_USER_INTR_MAP_2_INTR2_ORIENT__POS               6
#define BMI160_USER_INTR_MAP_2_INTR2_ORIENT__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_ORIENT__MSK               0x40
#define BMI160_USER_INTR_MAP_2_INTR2_ORIENT__REG    \
BMI160_USER_INTR_MAP_2_ADDR
/**************************************************************/
/**\name    INTERRUPT2 MAPPIONG OF FLAT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_2 Description - Reg Addr --> 0x57, Bit -->7 */
#define BMI160_USER_INTR_MAP_2_INTR2_FLAT__POS               7
#define BMI160_USER_INTR_MAP_2_INTR2_FLAT__LEN               1
#define BMI160_USER_INTR_MAP_2_INTR2_FLAT__MSK               0x80
#define BMI160_USER_INTR_MAP_2_INTR2_FLAT__REG  BMI160_USER_INTR_MAP_2_ADDR

/**************************************************************/
/**\name    TAP SOURCE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Data_0 Description - Reg Addr --> 0x58, Bit --> 3 */
#define BMI160_USER_INTR_DATA_0_INTR_TAP_SOURCE__POS               3
#define BMI160_USER_INTR_DATA_0_INTR_TAP_SOURCE__LEN               1
#define BMI160_USER_INTR_DATA_0_INTR_TAP_SOURCE__MSK               0x08
#define BMI160_USER_INTR_DATA_0_INTR_TAP_SOURCE__REG               \
BMI160_USER_INTR_DATA_0_ADDR

/**************************************************************/
/**\name    HIGH SOURCE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Data_0 Description - Reg Addr --> 0x58, Bit --> 7 */
#define BMI160_USER_INTR_DATA_0_INTR_LOW_HIGH_SOURCE__POS           7
#define BMI160_USER_INTR_DATA_0_INTR_LOW_HIGH_SOURCE__LEN           1
#define BMI160_USER_INTR_DATA_0_INTR_LOW_HIGH_SOURCE__MSK           0x80
#define BMI160_USER_INTR_DATA_0_INTR_LOW_HIGH_SOURCE__REG            \
BMI160_USER_INTR_DATA_0_ADDR

/**************************************************************/
/**\name    MOTION SOURCE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Data_1 Description - Reg Addr --> 0x59, Bit --> 7 */
#define BMI160_USER_INTR_DATA_1_INTR_MOTION_SOURCE__POS               7
#define BMI160_USER_INTR_DATA_1_INTR_MOTION_SOURCE__LEN               1
#define BMI160_USER_INTR_DATA_1_INTR_MOTION_SOURCE__MSK               0x80
#define BMI160_USER_INTR_DATA_1_INTR_MOTION_SOURCE__REG               \
        BMI160_USER_INTR_DATA_1_ADDR
/**************************************************************/
/**\name    LOW HIGH DURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_0 Description - Reg Addr --> 0x5a, Bit --> 0...7 */
#define BMI160_USER_INTR_LOWHIGH_0_INTR_LOW_DURN__POS               0
#define BMI160_USER_INTR_LOWHIGH_0_INTR_LOW_DURN__LEN               8
#define BMI160_USER_INTR_LOWHIGH_0_INTR_LOW_DURN__MSK               0xFF
#define BMI160_USER_INTR_LOWHIGH_0_INTR_LOW_DURN__REG               \
        BMI160_USER_INTR_LOWHIGH_0_ADDR
/**************************************************************/
/**\name    LOW THRESHOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_1 Description - Reg Addr --> 0x5b, Bit --> 0...7 */
#define BMI160_USER_INTR_LOWHIGH_1_INTR_LOW_THRES__POS               0
#define BMI160_USER_INTR_LOWHIGH_1_INTR_LOW_THRES__LEN               8
#define BMI160_USER_INTR_LOWHIGH_1_INTR_LOW_THRES__MSK               0xFF
#define BMI160_USER_INTR_LOWHIGH_1_INTR_LOW_THRES__REG               \
        BMI160_USER_INTR_LOWHIGH_1_ADDR
/**************************************************************/
/**\name    LOW HYSTERESIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_2 Description - Reg Addr --> 0x5c, Bit --> 0...1 */
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_HYST__POS               0
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_HYST__LEN               2
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_HYST__MSK               0x03
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_HYST__REG               \
        BMI160_USER_INTR_LOWHIGH_2_ADDR
/**************************************************************/
/**\name    LOW MODE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_2 Description - Reg Addr --> 0x5c, Bit --> 2 */
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_MODE__POS               2
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_MODE__LEN               1
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_MODE__MSK               0x04
#define BMI160_USER_INTR_LOWHIGH_2_INTR_LOW_G_MODE__REG               \
        BMI160_USER_INTR_LOWHIGH_2_ADDR
/**************************************************************/
/**\name    HIGH_G HYSTERESIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_2 Description - Reg Addr --> 0x5c, Bit --> 6...7 */
#define BMI160_USER_INTR_LOWHIGH_2_INTR_HIGH_G_HYST__POS               6
#define BMI160_USER_INTR_LOWHIGH_2_INTR_HIGH_G_HYST__LEN               2
#define BMI160_USER_INTR_LOWHIGH_2_INTR_HIGH_G_HYST__MSK               0xC0
#define BMI160_USER_INTR_LOWHIGH_2_INTR_HIGH_G_HYST__REG               \
        BMI160_USER_INTR_LOWHIGH_2_ADDR
/**************************************************************/
/**\name    HIGH_G DURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_3 Description - Reg Addr --> 0x5d, Bit --> 0...7 */
#define BMI160_USER_INTR_LOWHIGH_3_INTR_HIGH_G_DURN__POS               0
#define BMI160_USER_INTR_LOWHIGH_3_INTR_HIGH_G_DURN__LEN               8
#define BMI160_USER_INTR_LOWHIGH_3_INTR_HIGH_G_DURN__MSK               0xFF
#define BMI160_USER_INTR_LOWHIGH_3_INTR_HIGH_G_DURN__REG               \
        BMI160_USER_INTR_LOWHIGH_3_ADDR
/**************************************************************/
/**\name    HIGH_G THRESHOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_LowHigh_4 Description - Reg Addr --> 0x5e, Bit --> 0...7 */
#define BMI160_USER_INTR_LOWHIGH_4_INTR_HIGH_THRES__POS               0
#define BMI160_USER_INTR_LOWHIGH_4_INTR_HIGH_THRES__LEN               8
#define BMI160_USER_INTR_LOWHIGH_4_INTR_HIGH_THRES__MSK               0xFF
#define BMI160_USER_INTR_LOWHIGH_4_INTR_HIGH_THRES__REG               \
        BMI160_USER_INTR_LOWHIGH_4_ADDR
/**************************************************************/
/**\name    ANY MOTION DURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Motion_0 Description - Reg Addr --> 0x5f, Bit --> 0...1 */
#define BMI160_USER_INTR_MOTION_0_INTR_ANY_MOTION_DURN__POS               0
#define BMI160_USER_INTR_MOTION_0_INTR_ANY_MOTION_DURN__LEN               2
#define BMI160_USER_INTR_MOTION_0_INTR_ANY_MOTION_DURN__MSK               0x03
#define BMI160_USER_INTR_MOTION_0_INTR_ANY_MOTION_DURN__REG               \
        BMI160_USER_INTR_MOTION_0_ADDR
/**************************************************************/
/**\name    SLOW/NO MOTION DURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
    /* Int_Motion_0 Description - Reg Addr --> 0x5f, Bit --> 2...7 */
#define BMI160_USER_INTR_MOTION_0_INTR_SLOW_NO_MOTION_DURN__POS      2
#define BMI160_USER_INTR_MOTION_0_INTR_SLOW_NO_MOTION_DURN__LEN      6
#define BMI160_USER_INTR_MOTION_0_INTR_SLOW_NO_MOTION_DURN__MSK      0xFC
#define BMI160_USER_INTR_MOTION_0_INTR_SLOW_NO_MOTION_DURN__REG       \
        BMI160_USER_INTR_MOTION_0_ADDR
/**************************************************************/
/**\name    ANY MOTION THRESHOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Motion_1 Description - Reg Addr --> 0x60, Bit --> 0...7 */
#define BMI160_USER_INTR_MOTION_1_INTR_ANY_MOTION_THRES__POS               0
#define BMI160_USER_INTR_MOTION_1_INTR_ANY_MOTION_THRES__LEN               8
#define BMI160_USER_INTR_MOTION_1_INTR_ANY_MOTION_THRES__MSK               0xFF
#define BMI160_USER_INTR_MOTION_1_INTR_ANY_MOTION_THRES__REG               \
        BMI160_USER_INTR_MOTION_1_ADDR
/**************************************************************/
/**\name    SLOW/NO MOTION THRESHOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Motion_2 Description - Reg Addr --> 0x61, Bit --> 0...7 */
#define BMI160_USER_INTR_MOTION_2_INTR_SLOW_NO_MOTION_THRES__POS       0
#define BMI160_USER_INTR_MOTION_2_INTR_SLOW_NO_MOTION_THRES__LEN       8
#define BMI160_USER_INTR_MOTION_2_INTR_SLOW_NO_MOTION_THRES__MSK       0xFF
#define BMI160_USER_INTR_MOTION_2_INTR_SLOW_NO_MOTION_THRES__REG       \
        BMI160_USER_INTR_MOTION_2_ADDR
/**************************************************************/
/**\name    SLOW/NO MOTION SELECT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Motion_3 Description - Reg Addr --> 0x62, Bit --> 0 */
#define BMI160_USER_INTR_MOTION_3_INTR_SLOW_NO_MOTION_SELECT__POS   0
#define BMI160_USER_INTR_MOTION_3_INTR_SLOW_NO_MOTION_SELECT__LEN   1
#define BMI160_USER_INTR_MOTION_3_INTR_SLOW_NO_MOTION_SELECT__MSK   0x01
#define BMI160_USER_INTR_MOTION_3_INTR_SLOW_NO_MOTION_SELECT__REG   \
BMI160_USER_INTR_MOTION_3_ADDR
/**************************************************************/
/**\name    SIGNIFICANT MOTION SELECT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Motion_3 Description - Reg Addr --> 0x62, Bit --> 1 */
#define BMI160_USER_INTR_SIGNIFICATION_MOTION_SELECT__POS   1
#define BMI160_USER_INTR_SIGNIFICATION_MOTION_SELECT__LEN       1
#define BMI160_USER_INTR_SIGNIFICATION_MOTION_SELECT__MSK       0x02
#define BMI160_USER_INTR_SIGNIFICATION_MOTION_SELECT__REG       \
        BMI160_USER_INTR_MOTION_3_ADDR

/* Int_Motion_3 Description - Reg Addr --> 0x62, Bit --> 3..2 */
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_SKIP__POS       2
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_SKIP__LEN       2
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_SKIP__MSK       0x0C
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_SKIP__REG       \
        BMI160_USER_INTR_MOTION_3_ADDR

/* Int_Motion_3 Description - Reg Addr --> 0x62, Bit --> 5..4 */
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_PROOF__POS      4
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_PROOF__LEN      2
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_PROOF__MSK      0x30
#define BMI160_USER_INTR_SIGNIFICANT_MOTION_PROOF__REG      \
        BMI160_USER_INTR_MOTION_3_ADDR
/**************************************************************/
/**\name    TAP DURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* INT_TAP_0 Description - Reg Addr --> 0x63, Bit --> 0..2*/
#define BMI160_USER_INTR_TAP_0_INTR_TAP_DURN__POS               0
#define BMI160_USER_INTR_TAP_0_INTR_TAP_DURN__LEN               3
#define BMI160_USER_INTR_TAP_0_INTR_TAP_DURN__MSK               0x07
#define BMI160_USER_INTR_TAP_0_INTR_TAP_DURN__REG   \
BMI160_USER_INTR_TAP_0_ADDR
/**************************************************************/
/**\name    TAP SHOCK LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Tap_0 Description - Reg Addr --> 0x63, Bit --> 6 */
#define BMI160_USER_INTR_TAP_0_INTR_TAP_SHOCK__POS               6
#define BMI160_USER_INTR_TAP_0_INTR_TAP_SHOCK__LEN               1
#define BMI160_USER_INTR_TAP_0_INTR_TAP_SHOCK__MSK               0x40
#define BMI160_USER_INTR_TAP_0_INTR_TAP_SHOCK__REG BMI160_USER_INTR_TAP_0_ADDR
/**************************************************************/
/**\name    TAP QUIET LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Tap_0 Description - Reg Addr --> 0x63, Bit --> 7 */
#define BMI160_USER_INTR_TAP_0_INTR_TAP_QUIET__POS               7
#define BMI160_USER_INTR_TAP_0_INTR_TAP_QUIET__LEN               1
#define BMI160_USER_INTR_TAP_0_INTR_TAP_QUIET__MSK               0x80
#define BMI160_USER_INTR_TAP_0_INTR_TAP_QUIET__REG BMI160_USER_INTR_TAP_0_ADDR
/**************************************************************/
/**\name    TAP THRESHOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Tap_1 Description - Reg Addr --> 0x64, Bit --> 0...4 */
#define BMI160_USER_INTR_TAP_1_INTR_TAP_THRES__POS               0
#define BMI160_USER_INTR_TAP_1_INTR_TAP_THRES__LEN               5
#define BMI160_USER_INTR_TAP_1_INTR_TAP_THRES__MSK               0x1F
#define BMI160_USER_INTR_TAP_1_INTR_TAP_THRES__REG BMI160_USER_INTR_TAP_1_ADDR
/**************************************************************/
/**\name    ORIENT MODE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_0 Description - Reg Addr --> 0x65, Bit --> 0...1 */
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_MODE__POS               0
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_MODE__LEN               2
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_MODE__MSK               0x03
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_MODE__REG               \
        BMI160_USER_INTR_ORIENT_0_ADDR
/**************************************************************/
/**\name    ORIENT BLOCKING LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_0 Description - Reg Addr --> 0x65, Bit --> 2...3 */
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_BLOCKING__POS               2
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_BLOCKING__LEN               2
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_BLOCKING__MSK               0x0C
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_BLOCKING__REG               \
        BMI160_USER_INTR_ORIENT_0_ADDR
/**************************************************************/
/**\name    ORIENT HYSTERESIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_0 Description - Reg Addr --> 0x65, Bit --> 4...7 */
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_HYST__POS               4
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_HYST__LEN               4
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_HYST__MSK               0xF0
#define BMI160_USER_INTR_ORIENT_0_INTR_ORIENT_HYST__REG               \
        BMI160_USER_INTR_ORIENT_0_ADDR
/**************************************************************/
/**\name    ORIENT THETA LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_1 Description - Reg Addr --> 0x66, Bit --> 0...5 */
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_THETA__POS               0
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_THETA__LEN               6
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_THETA__MSK               0x3F
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_THETA__REG               \
        BMI160_USER_INTR_ORIENT_1_ADDR
/**************************************************************/
/**\name    ORIENT UD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_1 Description - Reg Addr --> 0x66, Bit --> 6 */
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_UD_ENABLE__POS               6
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_UD_ENABLE__LEN               1
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_UD_ENABLE__MSK               0x40
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_UD_ENABLE__REG               \
        BMI160_USER_INTR_ORIENT_1_ADDR
/**************************************************************/
/**\name    ORIENT AXIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Orient_1 Description - Reg Addr --> 0x66, Bit --> 7 */
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_AXES_EX__POS               7
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_AXES_EX__LEN               1
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_AXES_EX__MSK               0x80
#define BMI160_USER_INTR_ORIENT_1_INTR_ORIENT_AXES_EX__REG               \
        BMI160_USER_INTR_ORIENT_1_ADDR
/**************************************************************/
/**\name    FLAT THETA LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Flat_0 Description - Reg Addr --> 0x67, Bit --> 0...5 */
#define BMI160_USER_INTR_FLAT_0_INTR_FLAT_THETA__POS               0
#define BMI160_USER_INTR_FLAT_0_INTR_FLAT_THETA__LEN               6
#define BMI160_USER_INTR_FLAT_0_INTR_FLAT_THETA__MSK               0x3F
#define BMI160_USER_INTR_FLAT_0_INTR_FLAT_THETA__REG  \
        BMI160_USER_INTR_FLAT_0_ADDR
/**************************************************************/
/**\name    FLAT HYSTERESIS LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Flat_1 Description - Reg Addr --> 0x68, Bit --> 0...3 */
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HYST__POS     0
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HYST__LEN     4
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HYST__MSK     0x0F
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HYST__REG  \
BMI160_USER_INTR_FLAT_1_ADDR
/**************************************************************/
/**\name    FLAT HOLD LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Flat_1 Description - Reg Addr --> 0x68, Bit --> 4...5 */
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HOLD__POS                4
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HOLD__LEN                2
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HOLD__MSK                0x30
#define BMI160_USER_INTR_FLAT_1_INTR_FLAT_HOLD__REG  \
        BMI160_USER_INTR_FLAT_1_ADDR
/**************************************************************/
/**\name    FOC ACCEL XYZ LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Foc_Conf Description - Reg Addr --> 0x69, Bit --> 0...1 */
#define BMI160_USER_FOC_ACCEL_Z__POS               0
#define BMI160_USER_FOC_ACCEL_Z__LEN               2
#define BMI160_USER_FOC_ACCEL_Z__MSK               0x03
#define BMI160_USER_FOC_ACCEL_Z__REG               BMI160_USER_FOC_CONFIG_ADDR

/* Foc_Conf Description - Reg Addr --> 0x69, Bit --> 2...3 */
#define BMI160_USER_FOC_ACCEL_Y__POS               2
#define BMI160_USER_FOC_ACCEL_Y__LEN               2
#define BMI160_USER_FOC_ACCEL_Y__MSK               0x0C
#define BMI160_USER_FOC_ACCEL_Y__REG               BMI160_USER_FOC_CONFIG_ADDR

/* Foc_Conf Description - Reg Addr --> 0x69, Bit --> 4...5 */
#define BMI160_USER_FOC_ACCEL_X__POS               4
#define BMI160_USER_FOC_ACCEL_X__LEN               2
#define BMI160_USER_FOC_ACCEL_X__MSK               0x30
#define BMI160_USER_FOC_ACCEL_X__REG               BMI160_USER_FOC_CONFIG_ADDR
/**************************************************************/
/**\name    FOC GYRO LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Foc_Conf Description - Reg Addr --> 0x69, Bit --> 6 */
#define BMI160_USER_FOC_GYRO_ENABLE__POS               6
#define BMI160_USER_FOC_GYRO_ENABLE__LEN               1
#define BMI160_USER_FOC_GYRO_ENABLE__MSK               0x40
#define BMI160_USER_FOC_GYRO_ENABLE__REG               \
BMI160_USER_FOC_CONFIG_ADDR
/**************************************************************/
/**\name    NVM PROGRAM LENGTH, POSITION AND MASK*/
/**************************************************************/
/* CONF Description - Reg Addr --> 0x6A, Bit --> 1 */
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__POS               1
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__LEN               1
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__MSK               0x02
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__REG               \
BMI160_USER_CONFIG_ADDR

/*IF_CONF Description - Reg Addr --> 0x6B, Bit --> 0 */

#define BMI160_USER_IF_CONFIG_SPI3__POS               0
#define BMI160_USER_IF_CONFIG_SPI3__LEN               1
#define BMI160_USER_IF_CONFIG_SPI3__MSK               0x01
#define BMI160_USER_IF_CONFIG_SPI3__REG               BMI160_USER_IF_CONFIG_ADDR

/*IF_CONF Description - Reg Addr --> 0x6B, Bit --> 5..4 */
#define BMI160_USER_IF_CONFIG_IF_MODE__POS               4
#define BMI160_USER_IF_CONFIG_IF_MODE__LEN               2
#define BMI160_USER_IF_CONFIG_IF_MODE__MSK               0x30
#define BMI160_USER_IF_CONFIG_IF_MODE__REG      \
BMI160_USER_IF_CONFIG_ADDR
/**************************************************************/
/**\name    GYRO SLEEP CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 0...2 */
#define BMI160_USER_GYRO_SLEEP_TRIGGER__POS               0
#define BMI160_USER_GYRO_SLEEP_TRIGGER__LEN               3
#define BMI160_USER_GYRO_SLEEP_TRIGGER__MSK               0x07
#define BMI160_USER_GYRO_SLEEP_TRIGGER__REG BMI160_USER_PMU_TRIGGER_ADDR

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 3...4 */
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__POS               3
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__LEN               2
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__MSK               0x18
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__REG    BMI160_USER_PMU_TRIGGER_ADDR

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 5 */
#define BMI160_USER_GYRO_SLEEP_STATE__POS               5
#define BMI160_USER_GYRO_SLEEP_STATE__LEN               1
#define BMI160_USER_GYRO_SLEEP_STATE__MSK               0x20
#define BMI160_USER_GYRO_SLEEP_STATE__REG   BMI160_USER_PMU_TRIGGER_ADDR

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 6 */
#define BMI160_USER_GYRO_WAKEUP_INTR__POS               6
#define BMI160_USER_GYRO_WAKEUP_INTR__LEN               1
#define BMI160_USER_GYRO_WAKEUP_INTR__MSK               0x40
#define BMI160_USER_GYRO_WAKEUP_INTR__REG   BMI160_USER_PMU_TRIGGER_ADDR
/**************************************************************/
/**\name    ACCEL SELF TEST LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 0...1 */
#define BMI160_USER_ACCEL_SELFTEST_AXIS__POS               0
#define BMI160_USER_ACCEL_SELFTEST_AXIS__LEN               2
#define BMI160_USER_ACCEL_SELFTEST_AXIS__MSK               0x03
#define BMI160_USER_ACCEL_SELFTEST_AXIS__REG    BMI160_USER_SELF_TEST_ADDR

/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 2 */
#define BMI160_USER_ACCEL_SELFTEST_SIGN__POS               2
#define BMI160_USER_ACCEL_SELFTEST_SIGN__LEN               1
#define BMI160_USER_ACCEL_SELFTEST_SIGN__MSK               0x04
#define BMI160_USER_ACCEL_SELFTEST_SIGN__REG    BMI160_USER_SELF_TEST_ADDR

/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 3 */
#define BMI160_USER_SELFTEST_AMP__POS               3
#define BMI160_USER_SELFTEST_AMP__LEN               1
#define BMI160_USER_SELFTEST_AMP__MSK               0x08
#define BMI160_USER_SELFTEST_AMP__REG       BMI160_USER_SELF_TEST_ADDR
/**************************************************************/
/**\name    GYRO SELF TEST LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 4 */
#define BMI160_USER_GYRO_SELFTEST_START__POS               4
#define BMI160_USER_GYRO_SELFTEST_START__LEN               1
#define BMI160_USER_GYRO_SELFTEST_START__MSK               0x10
#define BMI160_USER_GYRO_SELFTEST_START__REG            \
BMI160_USER_SELF_TEST_ADDR
/**************************************************************/
/**\name    NV_CONFIG LENGTH, POSITION AND MASK*/
/**************************************************************/
/* NV_CONF Description - Reg Addr --> 0x70, Bit --> 0 */
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__POS               0
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__LEN               1
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__MSK               0x01
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__REG    BMI160_USER_NV_CONFIG_ADDR

/*IF_CONF Description - Reg Addr --> 0x70, Bit --> 1 */
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__POS               1
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__LEN               1
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__MSK               0x02
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__REG       \
BMI160_USER_NV_CONFIG_ADDR

/*IF_CONF Description - Reg Addr --> 0x70, Bit --> 2 */
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__POS               2
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__LEN               1
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__MSK               0x04
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__REG       \
BMI160_USER_NV_CONFIG_ADDR

/* NV_CONF Description - Reg Addr --> 0x70, Bit --> 3 */
#define BMI160_USER_NV_CONFIG_SPARE0__POS               3
#define BMI160_USER_NV_CONFIG_SPARE0__LEN               1
#define BMI160_USER_NV_CONFIG_SPARE0__MSK               0x08
#define BMI160_USER_NV_CONFIG_SPARE0__REG   BMI160_USER_NV_CONFIG_ADDR

/* NV_CONF Description - Reg Addr --> 0x70, Bit --> 4...7 */
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__POS               4
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__LEN               4
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__MSK               0xF0
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__REG  BMI160_USER_NV_CONFIG_ADDR
/**************************************************************/
/**\name    ACCEL MANUAL OFFSET LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_0 Description - Reg Addr --> 0x71, Bit --> 0...7 */
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__POS               0
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__LEN               8
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__MSK               0xFF
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__REG   BMI160_USER_OFFSET_0_ADDR

/* Offset_1 Description - Reg Addr --> 0x72, Bit --> 0...7 */
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__POS               0
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__LEN               8
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__MSK               0xFF
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__REG   BMI160_USER_OFFSET_1_ADDR

/* Offset_2 Description - Reg Addr --> 0x73, Bit --> 0...7 */
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__POS               0
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__LEN               8
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__MSK               0xFF
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__REG   BMI160_USER_OFFSET_2_ADDR
/**************************************************************/
/**\name    GYRO MANUAL OFFSET LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_3 Description - Reg Addr --> 0x74, Bit --> 0...7 */
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__POS               0
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__LEN               8
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__MSK               0xFF
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__REG    BMI160_USER_OFFSET_3_ADDR

/* Offset_4 Description - Reg Addr --> 0x75, Bit --> 0...7 */
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__POS               0
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__LEN               8
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__MSK               0xFF
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__REG    BMI160_USER_OFFSET_4_ADDR

/* Offset_5 Description - Reg Addr --> 0x76, Bit --> 0...7 */
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__POS               0
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__LEN               8
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__MSK               0xFF
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__REG    BMI160_USER_OFFSET_5_ADDR


/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 0..1 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__POS               0
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__LEN               2
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__MSK               0x03
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__REG    BMI160_USER_OFFSET_6_ADDR

/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 2...3 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__POS               2
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__LEN               2
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__MSK               0x0C
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__REG    BMI160_USER_OFFSET_6_ADDR

/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 4...5 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__POS               4
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__LEN               2
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__MSK               0x30
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__REG     BMI160_USER_OFFSET_6_ADDR
/**************************************************************/
/**\name    ACCEL OFFSET  ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 6 */
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__POS               6
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__LEN               1
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__MSK               0x40
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__REG   \
BMI160_USER_OFFSET_6_ADDR
/**************************************************************/
/**\name    GYRO OFFSET  ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_6 Description - Reg Addr --> 0x77, Bit -->  7 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__POS               7
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__LEN               1
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__MSK               0x80
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__REG    BMI160_USER_OFFSET_6_ADDR
/**************************************************************/
/**\name    STEP COUNTER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* STEP_CNT_0  Description - Reg Addr --> 0x78, Bit -->  0 to 7 */
#define BMI160_USER_STEP_COUNT_LSB__POS               0
#define BMI160_USER_STEP_COUNT_LSB__LEN               7
#define BMI160_USER_STEP_COUNT_LSB__MSK               0xFF
#define BMI160_USER_STEP_COUNT_LSB__REG  BMI160_USER_STEP_COUNT_0_ADDR

/* STEP_CNT_1  Description - Reg Addr --> 0x79, Bit -->  0 to 7 */
#define BMI160_USER_STEP_COUNT_MSB__POS               0
#define BMI160_USER_STEP_COUNT_MSB__LEN               7
#define BMI160_USER_STEP_COUNT_MSB__MSK               0xFF
#define BMI160_USER_STEP_COUNT_MSB__REG  BMI160_USER_STEP_COUNT_1_ADDR
/**************************************************************/
/**\name    STEP COUNTER CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* STEP_CONFIG_0  Description - Reg Addr --> 0x7A, Bit -->  0 to 7 */
#define BMI160_USER_STEP_CONFIG_ZERO__POS               0
#define BMI160_USER_STEP_CONFIG_ZERO__LEN               7
#define BMI160_USER_STEP_CONFIG_ZERO__MSK               0xFF
#define BMI160_USER_STEP_CONFIG_ZERO__REG    BMI160_USER_STEP_CONFIG_0_ADDR


/* STEP_CONFIG_1  Description - Reg Addr --> 0x7B, Bit -->  0 to 2 and
4 to 7 */
#define BMI160_USER_STEP_CONFIG_ONE_CNF1__POS               0
#define BMI160_USER_STEP_CONFIG_ONE_CNF1__LEN               3
#define BMI160_USER_STEP_CONFIG_ONE_CNF1__MSK               0x07
#define BMI160_USER_STEP_CONFIG_ONE_CNF1__REG    BMI160_USER_STEP_CONFIG_1_ADDR

#define BMI160_USER_STEP_CONFIG_ONE_CNF2__POS               4
#define BMI160_USER_STEP_CONFIG_ONE_CNF2__LEN               4
#define BMI160_USER_STEP_CONFIG_ONE_CNF2__MSK               0xF0
#define BMI160_USER_STEP_CONFIG_ONE_CNF2__REG    BMI160_USER_STEP_CONFIG_1_ADDR
/**************************************************************/
/**\name    STEP COUNTER ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* STEP_CONFIG_1  Description - Reg Addr --> 0x7B, Bit -->  0 to 2 */
#define BMI160_USER_STEP_CONFIG_1_STEP_COUNT_ENABLE__POS        3
#define BMI160_USER_STEP_CONFIG_1_STEP_COUNT_ENABLE__LEN        1
#define BMI160_USER_STEP_CONFIG_1_STEP_COUNT_ENABLE__MSK        0x08
#define BMI160_USER_STEP_CONFIG_1_STEP_COUNT_ENABLE__REG    \
BMI160_USER_STEP_CONFIG_1_ADDR

/* USER REGISTERS DEFINITION END */
/**************************************************************************/
/* CMD REGISTERS DEFINITION START */
/**************************************************************/
/**\name    COMMAND REGISTER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Command description address - Reg Addr --> 0x7E, Bit -->  0....7 */
#define BMI160_CMD_COMMANDS__POS              0
#define BMI160_CMD_COMMANDS__LEN              8
#define BMI160_CMD_COMMANDS__MSK              0xFF
#define BMI160_CMD_COMMANDS__REG     BMI160_CMD_COMMANDS_ADDR
/**************************************************************/
/**\name    PAGE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_CMD_TARGET_PAGE__POS           4
#define BMI160_CMD_TARGET_PAGE__LEN           2
#define BMI160_CMD_TARGET_PAGE__MSK           0x30
#define BMI160_CMD_TARGET_PAGE__REG   BMI160_CMD_EXT_MODE_ADDR

/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_CMD_PAGING_EN__POS           7
#define BMI160_CMD_PAGING_EN__LEN           1
#define BMI160_CMD_PAGING_EN__MSK           0x80
#define BMI160_CMD_PAGING_EN__REG       BMI160_CMD_EXT_MODE_ADDR

/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_COM_C_TRIM_FIVE__POS           4
#define BMI160_COM_C_TRIM_FIVE__LEN           2
#define BMI160_COM_C_TRIM_FIVE__MSK           0x30
#define BMI160_COM_C_TRIM_FIVE__REG     BMI160_COM_C_TRIM_FIVE_ADDR

/**************************************************************************/
/* CMD REGISTERS DEFINITION END */

/* INT ENABLE 0 */
#define BMI160_ANY_MOTION_X_ENABLE       0
#define BMI160_ANY_MOTION_Y_ENABLE       1
#define BMI160_ANY_MOTION_Z_ENABLE       2
#define BMI160_DOUBLE_TAP_ENABLE         4
#define BMI160_SINGLE_TAP_ENABLE         5
#define BMI160_ORIENT_ENABLE             6
#define BMI160_FLAT_ENABLE               7

/* INT ENABLE 1 */
#define BMI160_HIGH_G_X_ENABLE       0
#define BMI160_HIGH_G_Y_ENABLE       1
#define BMI160_HIGH_G_Z_ENABLE       2
#define BMI160_LOW_G_ENABLE          3
#define BMI160_DATA_RDY_ENABLE       4
#define BMI160_FIFO_FULL_ENABLE      5
#define BMI160_FIFO_WM_ENABLE        6

/* INT ENABLE 2 */
#define  BMI160_NOMOTION_X_ENABLE   0
#define  BMI160_NOMOTION_Y_ENABLE   1
#define  BMI160_NOMOTION_Z_ENABLE   2

/* FIFO definitions*/
#define FIFO_HEAD_A        0x84
#define FIFO_HEAD_G        0x88
#define FIFO_HEAD_M        0x90

#define FIFO_HEAD_G_A   0x8C
#define FIFO_HEAD_M_A   0x94
#define FIFO_HEAD_M_G   0x98

#define FIFO_HEAD_M_G_A     0x9C

#define FIFO_HEAD_INPUT_CONFIG          0x48
#define FIFO_HEAD_SENSOR_TIME           0x44
#define FIFO_HEAD_SKIP_FRAME            0x40
#define FIFO_HEAD_OVER_READ_LSB         0x80
#define FIFO_HEAD_OVER_READ_MSB         0x00
/**************************************************/
/**\name    ACCEL RANGE          */
/*************************************************/
#define BMI160_ACCEL_RANGE_2G           0X03
#define BMI160_ACCEL_RANGE_4G           0X05
#define BMI160_ACCEL_RANGE_8G           0X08
#define BMI160_ACCEL_RANGE_16G          0X0C
/**************************************************/
/**\name    ACCEL ODR          */
/*************************************************/
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED       0x00
#define BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ         0x01
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ         0x02
#define BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ         0x03
#define BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ         0x04
#define BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ         0x05
#define BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ           0x06
#define BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ           0x07
#define BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ          0x08
#define BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ          0x09
#define BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ          0x0A
#define BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ          0x0B
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ         0x0C
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED0      0x0D
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED1      0x0E
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED2      0x0F
/**************************************************/
/**\name    ACCEL BANDWIDTH PARAMETER         */
/*************************************************/
#define BMI160_ACCEL_OSR4_AVG1          0x00
#define BMI160_ACCEL_OSR2_AVG2          0x01
#define BMI160_ACCEL_NORMAL_AVG4        0x02
#define BMI160_ACCEL_CIC_AVG8           0x03
#define BMI160_ACCEL_RES_AVG16          0x04
#define BMI160_ACCEL_RES_AVG32          0x05
#define BMI160_ACCEL_RES_AVG64          0x06
#define BMI160_ACCEL_RES_AVG128         0x07
/**************************************************/
/**\name    GYRO ODR         */
/*************************************************/
#define BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED       0x00
#define BMI160_GYRO_OUTPUT_DATA_RATE_25HZ           0x06
#define BMI160_GYRO_OUTPUT_DATA_RATE_50HZ           0x07
#define BMI160_GYRO_OUTPUT_DATA_RATE_100HZ          0x08
#define BMI160_GYRO_OUTPUT_DATA_RATE_200HZ          0x09
#define BMI160_GYRO_OUTPUT_DATA_RATE_400HZ          0x0A
#define BMI160_GYRO_OUTPUT_DATA_RATE_800HZ          0x0B
#define BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ         0x0C
#define BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ         0x0D
/**************************************************/
/**\name    GYRO BANDWIDTH PARAMETER         */
/*************************************************/
#define BMI160_GYRO_OSR4_MODE       0x00
#define BMI160_GYRO_OSR2_MODE       0x01
#define BMI160_GYRO_NORMAL_MODE     0x02
#define BMI160_GYRO_CIC_MODE        0x03
/**************************************************/
/**\name    GYROSCOPE RANGE PARAMETER         */
/*************************************************/
#define BMI160_GYRO_RANGE_2000_DEG_SEC  0x00
#define BMI160_GYRO_RANGE_1000_DEG_SEC  0x01
#define BMI160_GYRO_RANGE_500_DEG_SEC   0x02
#define BMI160_GYRO_RANGE_250_DEG_SEC   0x03
#define BMI160_GYRO_RANGE_125_DEG_SEC   0x04
/**************************************************/
/**\name    MAG ODR         */
/*************************************************/
#define BMI160_MAG_OUTPUT_DATA_RATE_RESERVED       0x00
#define BMI160_MAG_OUTPUT_DATA_RATE_0_78HZ         0x01
#define BMI160_MAG_OUTPUT_DATA_RATE_1_56HZ         0x02
#define BMI160_MAG_OUTPUT_DATA_RATE_3_12HZ         0x03
#define BMI160_MAG_OUTPUT_DATA_RATE_6_25HZ         0x04
#define BMI160_MAG_OUTPUT_DATA_RATE_12_5HZ         0x05
#define BMI160_MAG_OUTPUT_DATA_RATE_25HZ           0x06
#define BMI160_MAG_OUTPUT_DATA_RATE_50HZ           0x07
#define BMI160_MAG_OUTPUT_DATA_RATE_100HZ          0x08
#define BMI160_MAG_OUTPUT_DATA_RATE_200HZ          0x09
#define BMI160_MAG_OUTPUT_DATA_RATE_400HZ          0x0A
#define BMI160_MAG_OUTPUT_DATA_RATE_800HZ          0x0B
#define BMI160_MAG_OUTPUT_DATA_RATE_1600HZ         0x0C
#define BMI160_MAG_OUTPUT_DATA_RATE_RESERVED0      0x0D
#define BMI160_MAG_OUTPUT_DATA_RATE_RESERVED1      0x0E
#define BMI160_MAG_OUTPUT_DATA_RATE_RESERVED2      0x0F

/* command register definition */
#define START_FOC_ACCEL_GYRO    0X03 /* Starts fast offset calibration for the accel and gyro*/
#define ACCEL_MODE_SUSPEND      0X10 /* Sets the PMU mode for the Accelerometer to suspend */
#define ACCEL_MODE_NORMAL       0x11 /* Sets the PMU mode for the Accelerometer to normal */
#define ACCEL_MODE_LOWPOWER     0X12 /* Sets the PMU mode for the Accelerometer Lowpower */
#define GYRO_MODE_SUSPEND       0x14 /* Sets the PMU mode for the Gyroscope to suspend */
#define GYRO_MODE_NORMAL        0x15 /* Sets the PMU mode for the Gyroscope to normal */
#define GYRO_MODE_FASTSTARTUP   0x17 /* Sets the PMU mode for the Gyroscope to fast start-up */
#define MAG_MODE_SUSPEND        0x18 /* Sets the PMU mode for the Magnetometer to suspend */
#define MAG_MODE_NORMAL         0x19 /* Sets the PMU mode for the Magnetometer to normal */
#define MAG_MODE_LOWPOWER       0x1A /* Sets the PMU mode for the Magnetometer to Lowpower */
#define FLUSH_FIFO              0xB0 /* Clears all data in the FIFO */
#define RESET_INT_ENGINE        0xB1 /* Resets the interrupt engine */
#define CLEAR_SETP_CNT          0xB2 /* step_cnt_clr Clears the step counter */
#define TRIGGER_RESET           0xB6 /* Triggers a reset */
#define ENABLE_EXT_MODE         0xC0 /* Enable the extended mode */
#define ERASE_NVM_CELL          0xC4 /* Erase NVM cell */
#define LOAD_NVM_CELL           0xC8 /* Load NVM cell */
#define RESET_ACCEL_DATA_PATH   0xF0 /* Reset acceleration data path */


/**************************************************/
/**\name     TAP DURATION         */
/*************************************************/
#define BMI160_TAP_DURN_50MS     0x00
#define BMI160_TAP_DURN_100MS    0x01
#define BMI160_TAP_DURN_150MS    0x02
#define BMI160_TAP_DURN_200MS    0x03
#define BMI160_TAP_DURN_250MS    0x04
#define BMI160_TAP_DURN_375MS    0x05
#define BMI160_TAP_DURN_500MS    0x06
#define BMI160_TAP_DURN_700MS    0x07
/**************************************************/
/**\name    TAP SHOCK         */
/*************************************************/
#define BMI160_TAP_SHOCK_50MS   0x00
#define BMI160_TAP_SHOCK_75MS   0x01
/**************************************************/
/**\name    TAP QUIET        */
/*************************************************/
#define BMI160_TAP_QUIET_30MS   0x00
#define BMI160_TAP_QUIET_20MS   0x01
/**************************************************/
/**\name    STEP DETECTION SELECTION MODES      */
/*************************************************/
#define BMI160_STEP_NORMAL_MODE         0
#define BMI160_STEP_SENSITIVE_MODE      1
#define BMI160_STEP_ROBUST_MODE         2
/**************************************************/
/**\name    STEP CONFIGURATION SELECT MODE    */
/*************************************************/
#define STEP_CONFIG_NORMAL      0X315
#define STEP_CONFIG_SENSITIVE   0X2D
#define STEP_CONFIG_ROBUST      0X71D
/**************************************************/
/**\name    BMM150 REGISTER DEFINITION */
/*************************************************/
#define BMI160_BMM150_POWE_CONTROL_REG  0x4B
#define BMI160_BMM150_POWE_MODE_REG     0x4C
#define BMI160_BMM150_DATA_REG          0x42
#define BMI160_BMM150_XY_REP            0x51
#define BMI160_BMM150_Z_REP             0x52
/**************************************************/
/**\name    SECONDARY_MAG POWER MODE SELECTION    */
/*************************************************/
#define BMI160_MAG_FORCE_MODE       0
#define BMI160_MAG_SUSPEND_MODE     1
/**************************************************/
/**\name    MAG POWER MODE SELECTION    */
/*************************************************/
#define FORCE_MODE      0
#define SUSPEND_MODE    1
#define NORMAL_MODE     2
#define MAG_SUSPEND_MODE    1
/**************************************************/
/**\name    BMI160 INTERFACE CONFIGURATION    */
/*************************************************/
#define BMI160_PRIMARY_AUTO_SECONDARY_OFF 	0x00
#define BMI160_PRIMARY_I2C_SECONDARY_OIS	0x01
#define BMI160_PRIMARY_AUTO_SECONDARY_MAG	0x02
/**************************************************/
/**\name    INTERRUPT EDGE TRIGGER ENABLE    */
/*************************************************/
#define BMI160_EDGE     0x01
#define BMI160_LEVEL    0x00
/**************************************************/
/**\name    INTERRUPT LEVEL ENABLE    */
/*************************************************/
#define BMI160_LEVEL_LOW        0x00
#define BMI160_LEVEL_HIGH       0x01
/**************************************************/
/**\name    INTERRUPT OUTPUT ENABLE    */
/*************************************************/
#define BMI160_OPEN_DRAIN   0x01
#define BMI160_PUSH_PULL    0x00

/* interrupt output enable*/
#define BMI160_INPUT    0x01
#define BMI160_OUTPUT   0x00

/**************************************************/
/**\name    INTERRUPT TAP SOURCE ENABLE    */
/*************************************************/
#define FILTER_DATA     0x00
#define UNFILTER_DATA   0x01
/**************************************************/
/**\name    SLOW MOTION/ NO MOTION SELECT   */
/*************************************************/
#define SLOW_MOTION     0x00
#define NO_MOTION       0x01
/**************************************************/
/**\name    SIGNIFICANT MOTION SELECTION   */
/*************************************************/
#define ANY_MOTION          0x00
#define SIGNIFICANT_MOTION  0x01
/**************************************************/
/**\name    LATCH DURATION   */
/*************************************************/
#define BMI160_LATCH_DUR_NONE               0x00
#define BMI160_LATCH_DUR_312_5_MICRO_SEC    0x01
#define BMI160_LATCH_DUR_625_MICRO_SEC      0x02
#define BMI160_LATCH_DUR_1_25_MILLI_SEC     0x03
#define BMI160_LATCH_DUR_2_5_MILLI_SEC      0x04
#define BMI160_LATCH_DUR_5_MILLI_SEC        0x05
#define BMI160_LATCH_DUR_10_MILLI_SEC       0x06
#define BMI160_LATCH_DUR_20_MILLI_SEC       0x07
#define BMI160_LATCH_DUR_40_MILLI_SEC       0x08
#define BMI160_LATCH_DUR_80_MILLI_SEC       0x09
#define BMI160_LATCH_DUR_160_MILLI_SEC      0x0A
#define BMI160_LATCH_DUR_320_MILLI_SEC      0x0B
#define BMI160_LATCH_DUR_640_MILLI_SEC      0x0C
#define BMI160_LATCH_DUR_1_28_SEC           0x0D
#define BMI160_LATCH_DUR_2_56_SEC           0x0E
#define BMI160_LATCHED                      0x0F
/**************************************************/
/**\name    GYRO OFFSET MASK DEFINITION   */
/*************************************************/
#define BMI160_GYRO_MANUAL_OFFSET_0_7   0x00FF
#define BMI160_GYRO_MANUAL_OFFSET_8_9   0x0300
/**************************************************/
/**\name    STEP CONFIGURATION MASK DEFINITION   */
/*************************************************/
#define BMI160_STEP_CONFIG_0_7      0x00FF
#define BMI160_STEP_CONFIG_8_10     0x0700
#define BMI160_STEP_CONFIG_11_14    0xF000

/**************************************************/
/**\name    MAG INIT DEFINITION  */
/*************************************************/
#define BMI160_COMMAND_REG_ONE      0x37
#define BMI160_COMMAND_REG_TWO      0x9A
#define BMI160_COMMAND_REG_THREE    0xC0
#define RESET_STEP_COUNTER          0xB2
/**************************************************/
/**\name    BIT SLICE GET AND SET FUNCTIONS  */
/*************************************************/
#define BMI160_GET_BITSLICE(regvar, bitname)\
        ((regvar & bitname##__MSK) >> bitname##__POS)


#define BMI160_SET_BITSLICE(regvar, bitname, val)\
        ((regvar & ~bitname##__MSK) | \
        ((val<<bitname##__POS)&bitname##__MSK))
#endif
