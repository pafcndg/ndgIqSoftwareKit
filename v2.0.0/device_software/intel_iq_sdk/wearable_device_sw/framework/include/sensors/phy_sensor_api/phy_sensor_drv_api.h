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

#ifndef __PHY_SENSOR_DRV_API_H__
#define __PHY_SENSOR_DRV_API_H__

#include "phy_sensor_common.h"

/**
 * @defgroup phy_sensor_drv_api Physical Sensor Driver Interface
 * Physical Sensor Driver Interface.
 * A physical sensor driver needs to implement physical sensor driver interfaces
 * to integrate in Physical sensor layer
 * @ingroup phy_sensor
 * @{
 */

struct phy_sensor_t;

/**
 * Callback function type used by physical sensor api
 * to enable data report in interrupt mode.
 *
 * @param buffer data buffer to pass raw sensor data.
 * @param buff_len length of sensor data in buffer (in byte)
 * @param priv_data private data passed when @ref phy_sensor_api_t.enable_int_mode is called
 *
 * @return 0 if success, error otherwise.
 *
 * The callback function is registered via @ref phy_sensor_api_t.enable_int_mode
 */
typedef int (*generic_read_data_cb) (uint8_t * buffer, uint16_t buff_len,
                                     void *priv_data);
/**
 * APIs for physical sensor driver
 */
struct phy_sensor_api_t {
    /** Mandatory for a physical sensor driver
     *  The interface will be called by @ref phy_sensor_open
     */
    int (*open) (struct phy_sensor_t * sensor);
    /** Mandatory for a physical sensor driver
     *  The interface will be called by @ref phy_sensor_close
     */
    void (*close) (struct phy_sensor_t * sensor);
    /** The interface will be called by @ref phy_sensor_enable*/
    int (*activate) (struct phy_sensor_t * sensor, bool enable);
    /** The interface will be called by @ref phy_sensor_set_odr_value*/
    int (*set_odr) (struct phy_sensor_t * sensor, uint16_t odr_hz);
    /** The interface will be called by @ref phy_sensor_query_odr_value*/
    int (*query_odr)(struct phy_sensor_t * sensor, uint16_t odr_target, uint16_t *odr_support);
    /** The interface will be called by @ref phy_sensor_data_read.\n
     *  The interface is used to read one sensor frame in polling mode.
     */
    int (*read) (struct phy_sensor_t * sensor, uint8_t * buffer,
                 uint16_t buff_len);
    /** The interface will be called by @ref phy_sensor_fifo_read.\n
     *  The interface is used to read sensor frames in hardware fifo.
     */
    int (*fifo_read) (struct phy_sensor_t * sensor, uint8_t * buffer,
                 uint16_t buff_len);
    /** Tell the driver to report data in interrupt mode
     *  The interface will be called by
     *  @ref phy_sensor_data_register_callback or
     *  @ref phy_sensor_data_unregister_callback
     */
    int (*enable_int_mode) (struct phy_sensor_t * sensor,
                            generic_read_data_cb callback, void *priv_data,
                            uint16_t sample_rate, bool enable);
    /** The interface will be called by @ref phy_sensor_data_get_hwfifo_length */
    int (*get_fifo_len) (struct phy_sensor_t * sensor);
    /** The interface will be called by @ref phy_sensor_enable_hwfifo */
    int (*enable_fifo) (struct phy_sensor_t * sensor, uint8_t *buffer, uint16_t len, bool enable);
    /** The interface will be called by @ref phy_sensor_get_property */
    int (*get_property) (struct phy_sensor_t * sensor, uint8_t type,
                         void *value);
    /** The interface will be called by @ref phy_sensor_set_property */
    int (*set_property) (struct phy_sensor_t * sensor, uint8_t type,
                         const void *value);
    /** The interface will be called by @ref phy_sensor_selftest */
    int (*selftest) (struct phy_sensor_t * sensor);
};

/**
 * Data structure for physical sensor
 */
struct phy_sensor_t {
    uint32_t version;
    /** valid dev_id start from 1, if dev_id is 0, the sensor is not registered.\n
     *  this field is initialized automatically by physical sensor layer
     */
    dev_id_t dev_id;
    /** driver author need to init this field */
    phy_sensor_type_t type;
    /** If sensor support hardware fifo, driver author need to init this field */
    uint16_t hw_fifo_len;
    /** If the fifo of this sensor is shared with others,
     *  driver author need to init this field after all shared sensors registered,
     *  before opening the sensor; if sensor has dedicated fifo or no fifo , ignore this field */
    uint32_t fifo_share_bitmap;
    /** raw sensor data length in byte, driver author need to init this field */
    uint8_t raw_data_len;
    /** indicate the data report mode the sensor supports, driver author need to init this field,
     *  see @ref phy_sensor_report_mode*/
    uint8_t report_mode_mask;
    /** APIs driver author need to implement */
    struct phy_sensor_api_t api;

    /** internal usage by physical sensor layer */
    uint8_t refcnt;
    /** internal usage by physical sensor layer */
    uint8_t fifo_enabled;
    /** internal usage by physical sensor layer */
    uint16_t odr_hz_x10;
    /** internal usage by physical sensor layer */
    int (*read_callback) (struct sensor_data * sensor_data, void *priv_data);
    /** internal usage by physical sensor layer */
    void *cb_priv_data;
};

/**
 * Register one sensor in physical sensor system
 */
int sensor_register(struct phy_sensor_t *sensor);

/**
 * Return the number of sensors registered in physical sensor system
 */
int get_sensors_registered(void);

/**
 * Get the pointer of sensor list
 */
struct phy_sensor_t** get_sensors_list_prt(void);

/** @} */
#endif
