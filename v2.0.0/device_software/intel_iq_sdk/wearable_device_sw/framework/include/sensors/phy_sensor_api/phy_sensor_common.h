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

#ifndef __PHY_SENSOR_COMMON__H
#define __PHY_SENSOR_COMMON__H

#include <stdbool.h>
#include "drivers/data_type.h"
#include "services/sensor_service/sensor_data_format.h"
#include "util/compiler.h"

#define MAX_PHY_SENSOR_NUM 16

/**
 * @defgroup phy_sensor Physical Sensor Layer
 * The Physical sensor layer is an abstract layer to access physical sensor.
 * It hides the HW differences of various sensor hardware.\n
 * There are two parts in this layer:\n
 * Physical sensor API: Provide function calls to access physical sensors.\n
 * Physical sensor driver interface: Defined interfaces physical sensor drivers need to implement.\n
 * See @ref SS_SERVICE for detail
 * @msc
 *  "Sensor Core","Physical sensor API","Physical sensor driver";
 *  "Sensor Core"=>"Physical sensor API" [label="call physical sensor API"];
 *  "Physical sensor API"=>"Physical sensor driver" [label="call\nphyscial sensor driver\ninterface"];
 * @endmsc
 * @ingroup SS_SERVICE
 * @{
 */

/**
 * Physical sensor type.
 * The definition is identical to @ref ss_sensor_type_t.
 */
typedef uint8_t phy_sensor_type_t;
/**
 * Physical sensor device id.
 * A ID will be assigned to one sensor registered in physical sensor layer
 */
typedef uint8_t dev_id_t;
/**
 * Physical sensor id.
 */
typedef struct sensor_id{
    phy_sensor_type_t sensor_type;
    dev_id_t dev_id;
} sensor_id_t;
/**
 * Raw sensor data for accelerometer, uint is mg/s
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} phy_accel_data_t;
/**
 * Raw sensor data for gyroscope, uint is millideg/s
 */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} phy_gyro_data_t;
/**
 * Raw sensor data for magnetometer, unit is 16LSB/uT
 */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} phy_mag_data_t;
/**
 * Sensor event defined in Physical sensor layer
 */
enum phy_sensor_event {
    PHY_SENSOR_EVENT_STEP = 0,
    PHY_SENSOR_EVENT_SIGNIFICANT_MOTION,
    PHY_SENSOR_EVENT_ANY_MOTION,
    PHY_SENSOR_EVENT_NO_MOTION,
    PHY_SENSOR_EVENT_DOUBLE_TAP,
    PHY_SENSOR_EVENT_SINGLE_TAP,
    PHY_SENSOR_EVENT_ORIENT,
    PHY_SENSOR_EVENT_FLAT,
    PHY_SENSOR_EVENT_LOW_G,
    PHY_SENSOR_EVENT_HIGH_G,
    PHY_SENSOR_EVENT_WM,
    PHY_SENSOR_EVENT_COUNT
};

/**
 * Sensor report mode defined in Physical sensor layer
 */
enum phy_sensor_report_mode {
    /** Sensor supports polling sensor data from register.
     *  User can get one sample by calling @ref phy_sensor_data_read.\n
     *  Sensor driver needs to implement @ref phy_sensor_api_t.read
     */
    PHY_SENSOR_REPORT_MODE_POLL_REG = 0,
    /** Sensor supports report sensor data from register in interrupt mode.
     *  User needs to use @ref phy_sensor_data_register_callback to register callback function,
     *  one sample is reported via callback function each time.\n
     *  Sensor driver needs to implement @ref phy_sensor_api_t.enable_int_mode
     *  for reporting sensor data from registers
     */
    PHY_SENSOR_REPORT_MODE_INT_REG,
    /** Sensor supports polling sensor data from hardware fifo.
     *  User can get one or more samples via @ref phy_sensor_fifo_read
     *  if any sensor data in fifo.\n
     *  Sensor driver needs to implement @ref phy_sensor_api_t.enable_fifo and @ref phy_sensor_api_t.fifo_read.
     */
    PHY_SENSOR_REPORT_MODE_POLL_FIFO,
    /** Sensor supports report sensor data from fifo in interrupt mode (by watermark).
     *  User needs to use @ref phy_sensor_data_register_callback to register callback function,
     *  one or more sample is reported via callback function each time.\n
     *  Sensor driver needs to implement @ref phy_sensor_api_t.enable_fifo and @ref phy_sensor_api_t.enable_int_mode for
     *  reporting sensor data from fifo.
     *  @note if a sensor supports both PHY_SENSOR_REPORT_MODE_INT_REG and PHY_SENSOR_REPORT_MODE_INT_FIFO,
     *  and a callback function is registered, when fifo is enabled, data will be reported in PHY_SENSOR_REPORT_MODE_INT_FIFO mode,
     *  when fifo is disabled, data will be reported in PHY_SENSOR_REPORT_MODE_INT_REG mode.
     */
    PHY_SENSOR_REPORT_MODE_INT_FIFO,
    PHY_SENSOR_REPORT_MODE_COUNT
};

#define PHY_SENSOR_REPORT_MODE_POLL_REG_MASK  (1<<PHY_SENSOR_REPORT_MODE_POLL_REG)
#define PHY_SENSOR_REPORT_MODE_INT_REG_MASK   (1<<PHY_SENSOR_REPORT_MODE_INT_REG)
#define PHY_SENSOR_REPORT_MODE_POLL_FIFO_MASK (1<<PHY_SENSOR_REPORT_MODE_POLL_FIFO)
#define PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK  (1<<PHY_SENSOR_REPORT_MODE_INT_FIFO)

/**
 * Sensor event data
 */
typedef struct {
    uint8_t event_type;
} phy_event_data_t;
/**
 * Formated sensor data
 */
struct sensor_data {
    struct sensor_id sensor;
    uint8_t data_length;    /**< length of raw sensor data */
    uint32_t timestamp;     /**< timestamp in millisecond */
    uint8_t data[] __aligned(4);     /**< sensor raw data */
} __aligned(4);

typedef struct {
    uint8_t event_type;
    void *data;
} phy_sensor_event_t;

/**
 * Sensor property types defined in Physical sensor layer
 */
typedef enum {
    /** Sensing range(read only),
     *  Sensing range of sensor is determined when compile time.
     *  Unit for SENSOR_ACCELEROMETER: g\n
     *  Unit for SENSOR_GYROSCOPE: degree/s\n
     *  Unit for SENSOR_MAGNETOMETER: uT\n
     *  Unit for SENSOR_BAROMETER: Pa\n
     *  Unit for SENSOR_TEMPERATURE: degree Celsius\n
     *  Unit for SENSOR_HUMIDITY: %%RH
     *  @note Although this property can be supported by driver optionally,
     *  sensor drivers (like accelerometer) should handle this property
     *  if sensing range information is useful to application.
     */
    SENSOR_PROP_SENSING_RANGE = 0,
    /** Sensing resolution(read only),
     *  Sensing resolution of sensor is determined when compile time.
     *  @note Although this property can be supported by driver optionally,
     *  sensor drivers should handle this property
     *  if sensing range information is useful to application.
     */
    SENSOR_PROP_SENSING_RESOLUTION,
    /** Bitmap indicates which sensors the hardware fifo of the sensor shares,
     *  the bitmap is set according to @ref phy_sensor_t.dev_id
     */
    SENSOR_PROP_FIFO_SHARE_BITMAP,
    /** FIFO watermark
     */
    SENSOR_PROP_FIFO_WATERMARK,
    /** Driver specific property type */
    SENSOR_PROP_DRIVER_SPECIFIC,
    SENSOR_PROP_COUNT
} phy_sensor_property_type_t;

/** sensing range property */
typedef struct {
    int32_t low;    /**< min value of sensing range */
    int32_t high;   /**< max value of sensing range */
}phy_sensor_range_property_t;

/** sensing resolution property */
typedef struct {
    uint32_t value;    /**< value of sensing resolution */
}phy_sensor_resolution_property_t;

/** Indicates which sensors the hardware fifo of the sensor shares */
typedef struct {
    uint32_t bitmap;
}phy_sensor_fifo_share_property_t;

typedef void (*phy_sesnor_event_cb_t) (phy_sensor_event_t *event, void *priv_data);
/** sensor fifo watermark property */
typedef struct {
    uint32_t count;                 /**< number of samples to trigger watermark */
    phy_sesnor_event_cb_t callback; /**< callback function to report the watermark event */
    void *priv_data;
}phy_sensor_watermark_property_t;

/* Sensor specific data structures */
/* BMI160 specific data structures */
/**
 * BMI160 specific property types
 */
typedef enum {
    BMI160_SUBPROP_AVG = 0,
    BMI160_SUBPROP_COUNT
} bmi160_property_sub_type_t;
/**
 * BMI160 avg property
 */
typedef struct {
    /** should be BMI160_SUBPROP_AVG */
    uint8_t sub_type;
    /** the actual average number of cycles = 2^avg_value */
    uint8_t avg_value;
} bmi160_avg_property_t;
/**
 * BME280 specific property types
 */
typedef enum {
    BME280_SUBPROP_OSR = 0,
    BME280_SUBPROP_TSB,
    BME280_SUBPROP_FILTER,
    BME280_SUBPROP_COUNT
} bme280_property_sub_type_t;
/**
 * BME280 osr property
 */
typedef struct {
    /** should be BME280_SUBPROP_OSR */
    uint8_t sub_type;
    /** 0 --> skipped
     *  1 --> 1
     *  2 --> 2
     *  3 --> 4
     *  4 --> 8
     *  5 --> 16
     */
    uint8_t value;
} bme280_osr_property_t;
/**
 * BME280 time_standby property
 */
typedef struct {
    /** should be BME280_SUBPROP_TSB */
    uint8_t sub_type;
    /** time standby
     *  0 --> 0.5  ms
     *  6 --> 10   ms
     *  7 --> 20   ms
     *  1 --> 62.5 ms
     *  2 --> 125  ms
     *  3 --> 250  ms
     *  4 --> 500  ms
     *  5 --> 1000 ms
     */
    uint8_t value;
} bme280_tsb_property_t;

/**
 * BME280 filter property
 */
typedef struct {
    /** should be BME280_SUBPROP_FILTER */
    uint8_t sub_type;
    /** filter coeff
     *  0 --> filter off
     *  1 --> filter coeff is 2
     *  2 --> filter coeff is 4
     *  3 --> filter coeff is 8
     *  4 --> filter coeff is 16
     */
    uint8_t value;
} bme280_filter_property_t;

/** @} */
#endif
