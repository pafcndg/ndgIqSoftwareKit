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

#ifndef __SENSOR_DATA_FORMAT_H__
#define __SENSOR_DATA_FORMAT_H__

#include <stdint.h>
#include "util/compiler.h"

/**
 * @defgroup sensor_data_formats  Sensor data format
 * Sensor services and phy_drv ,sensor_core common data
 * @ingroup SS_SERVICE_API
 * @{
*/

/**
 * Sensor type definition,it mustn't be more than 32
 */
typedef enum {
    ON_BOARD_SENSOR_TYPE_START = 0,
//start of on_board
    SENSOR_PHY_TYPE_BASE = ON_BOARD_SENSOR_TYPE_START,  //start of phy sensor
    SENSOR_ACCELEROMETER,
    SENSOR_GYROSCOPE,
    SENSOR_MAGNETOMETER,
    SENSOR_BAROMETER,
    SENSOR_TEMPERATURE,
    SENSOR_HUMIDITY,
    SENSOR_RESERVED0,
    SENSOR_ABS_TYPE_BASE,      //end of phy sensor,start of abstract sensor
    SENSOR_PATTERN_MATCHING_GESTURE,
    SENSOR_ABS_TAPPING,
    SENSOR_ABS_SIMPLEGES,
    SENSOR_ABS_STEPCOUNTER,
    SENSOR_ABS_ACTIVITY,
    SENSOR_ALGO_DEMO,
    SENSOR_MOTION_DETECTOR,
    SENSOR_RESERVED00,
    SENSOR_ABS_TYPE_TOP,        //end of abstract sensor
//end of on_board
    ON_BOARD_SENSOR_TYPE_END = SENSOR_ABS_TYPE_TOP, //reduce the num of judgement position

    ANT_SENSOR_TYPE_START = ON_BOARD_SENSOR_TYPE_END,//reduce the num of judgement position
    ANT_SENSOR_TYPE_END,

    BLE_SENSOR_TYPE_START = ANT_SENSOR_TYPE_END,
    BLE_SENSOR_TYPE_END
} ss_sensor_type_t;

/**
 * Sensor type MASK definition, as the sensor_type_bit_map.
 */
#define ACCEL_TYPE_MASK     (1 << SENSOR_ACCELEROMETER)
#define GYRO_TYPE_MASK      (1 << SENSOR_GYROSCOPE)
#define MAG_TYPE_MASK       (1 << SENSOR_MAGNETOMETER)
#define BARO_TYPE_MASK      (1 << SENSOR_BAROMETER)
#define TEMP_TYPE_MASK      (1 << SENSOR_TEMPERATURE)
#define HUMI_TYPE_MASK      (1 << SENSOR_HUMIDITY)
#define PATTERN_MATCHING_TYPE_MASK       (1 << SENSOR_PATTERN_MATCHING_GESTURE)
#define TAPPING_TYPE_MASK   (1 << SENSOR_ABS_TAPPING)
#define SIMPLEGES_TYPE_MASK (1 << SENSOR_ABS_SIMPLEGES)
#define STEPCOUNTER_TYPE_MASK   (1 << SENSOR_ABS_STEPCOUNTER)
#define ACTIVITY_TYPE_MASK      (1 << SENSOR_ABS_ACTIVITY)
#define ALGO_DEMO_MASK (1 << SENSOR_ALGO_DEMO)
#define BOARD_SENSOR_MASK   (ACCEL_TYPE_MASK |      \
                             BARO_TYPE_MASK  |      \
                             TEMP_TYPE_MASK  |      \
                             HUMI_TYPE_MASK  |      \
                             MAG_TYPE_MASK   |      \
                             GYRO_TYPE_MASK  |      \
                             PATTERN_MATCHING_TYPE_MASK   |      \
                             TAPPING_TYPE_MASK|     \
                             SIMPLEGES_TYPE_MASK|   \
                             STEPCOUNTER_TYPE_MASK| \
                             ACTIVITY_TYPE_MASK|    \
                             ALGO_DEMO_MASK )
#define ANT_TYPE_MASK       (1 <<  ANT_SENSOR_TYPE_START)

#define BLE_TYPE_MASK       (1 << BLE_SENSOR_TYPE_START)

/**
 * different kinds of sensor data
 */
typedef enum {
    ACCEL_DATA = 0,
    GYRO_DATA  = 0,
    MAG_DATA = 0,
    BLE_HR_DATA = 0,
} ss_data_type_t;

/**
 * Unique data sturct transfer all kinds of sensor data
 */
typedef struct {
    uint8_t sensor_type;               /*!< Its type is sensor_type_t                   */
    uint8_t subscription_type;         /*!< Defined for a specific sensor_type          */
    uint32_t timestamp;                /*!< Record the time when this data is generated */
    uint8_t data_length;               /*!< The data payload size                       */
    uint8_t data[0];                   /*!< The first address of  data                  */
} ss_sensor_data_header_t;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/*
 * Phy and abs sensor priv data struct
 */

/*
 * @brief: The gesture pri data,abs sensor.
 * @param: nClassLabel:the recognition result
 * 1: up down
 * 2: left right
 * 3: down up
 * 4: right left
 * 5: counter clockwise
 * 6: clockwise
 * 7: alpha
 * 8: tick
 */
struct gs_personalize
{
    u16 size;           /*!< reserved */
    s16 nClassLabel;    /*!< recognition result of user defined gestures */
    s16 reserved;
}__packed;

/*
 * @brief: The tapping pri data, abs sensor.
 * @param:  tapping_cnt
 *          0  no tap
 *          1  not used
 *          2  double tap
 *          3  triple tap
 */
struct tapping_result
{
    s16 tapping_cnt;        /*!< the number of consecutive tapping detected,see table above */
}__packed;

/*
 *@brief: the simleges pri data, abs sensor.
 *@param: type
 * 1:shake
 * 2:flick in negative x direction
 * 3:flick in positive x direction
 * 4:flick in negative y direction
 * 5:flick in positive y direction
 * 6:flick in negative z direction
 * 7:flick in positive z direction
 */
struct simpleges_result
{
    s16 type;
}__packed;

/*
 * @breif: The stepcounter pri data, abs sensor.
 * @param:  activity
 *          0  no activity
 *          1  walking
 *          2  running
 *          3  biking
 *          4  sleeping
 */
struct stepcounter_result
{
    u32 steps;      /*!< the cumulative step count */
    u32 activity;   /*!< the activity type currently detected */
}__packed;

/*
 * @breif: The activity pri data, abs sensor.
 * @param: type
 *          0  no activity
 *          1  walking
 *          2  running
 *          3  biking
 *          4  sleeping
 */
struct activity_result
{
    u32 type;       /*!< the activity type currently detected */
}__packed;


/**************************
the struct below is definition of the report event data struct of basis algo
, which must be defined by user accordding to the basis algo result;
***************************/
struct demo_algo_result{
    int type;
    short ax;   /*!< Acceleration for X-axis, unit:mg */
    short ay;   /*!< Acceleration for Y-axis, unit:mg */
    short az;   /*!< Acceleration for Z-axis, unit:mg */
    int gx;   /*!< Angular velocity in rotation along X-axis, unit:m_degree/s */
    int gy;   /*!< Angular velocity in rotation along Y-axis, unit:m_degree/s */
    int gz;   /*!< Angular velocity in rotation along Z-axis, uint:m_degree/s */
}__packed;

/*
 * The accel pri data, phy sensor.
 */
struct accel_phy_data {
    short ax;   /*!< Acceleration for X-axis, unit:mg */
    short ay;   /*!< Acceleration for Y-axis, unit:mg */
    short az;   /*!< Acceleration for Z-axis, unit:mg */
}__packed;

/*
 * The gyro pri data, phy sensor.
 */
struct gyro_phy_data {
    int gx;   /*!< Angular velocity in rotation along X-axis, unit:m_degree/s */
    int gy;   /*!< Angular velocity in rotation along Y-axis, unit:m_degree/s */
    int gz;   /*!< Angular velocity in rotation along Z-axis, uint:m_degree/s */
}__packed;

/*
 * The magnetic pri data, phy sensor.
 */
struct mag_phy_data {
    int mx;   /*!< X-axis magnetic data, unit:16LSB/uT */
    int my;   /*!< Y-axis magnetic data, unit:16LSB/uT */
    int mz;   /*!< Z-axis magnetic data, unit:16LSB/uT */
}__packed;

/*
 * The temperature pri data, phy sensor.
 */
typedef struct temp_phy_data {
    int32_t value;  /*!< temperature data, unit:DegC/100 */
}__packed phy_temp_data_t;

/*
 * The humidity pri data, phy sensor.
 */
typedef struct humi_phy_data {
    uint32_t value; /*!< humidity data, unit:1/1024 %RH */
}__packed phy_humi_data_t;

/*
 * The barometer pri data, phy sensor.
 */
typedef struct baro_phy_data{
    uint32_t value; /*!< barometer data, unit:Pa */
}__packed phy_baro_data_t;
/** @} */

#endif/*__SENSOR_DATA_FORMAT_H__*/
