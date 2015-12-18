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

#ifndef __PHY_SENSOR_API_H__
#define __PHY_SENSOR_API_H__

#include "phy_sensor_common.h"

/**
 * @defgroup phy_sensor_api Physical Sensor API
 * Generic Physical Sensor API.
 * A user/module who wants to communicate with physical sensors should use Physical Sensor APIs.\n
 * \warning Physical Sensor API calls shoud NOT be made from ISR context
 * @ingroup phy_sensor
 * @{
 */

/**
 * Abstract physical sensor
 */
typedef void* sensor_t;
/**
 * Bitmap of physical sensor type(s)\n
 * The bit map is consistent with @ref ss_sensor_type_t
 */
typedef uint64_t phy_sensor_type_bitmap;

/*!
*  \brief   Callback function definition of data read in interrupt mode.
*
*
*  \param   sensor_data     : formated sensor data
*  \param   priv_data       : private data structure passed when registering the callback function
*
*  \return  actual raw sensor data read
*
*  User can use @ref phy_sensor_data_register_callback to register callback function.
*  When data ready interrupt occurs, the callback function will be called.
*  The callback function is called in fiber context.
*/
typedef int (*read_data_cb) (struct sensor_data * sensor_data, void *priv_data);

/*!
*  \brief   Get sensor ids of specific types of sensors.
*
*  \param   bitmap          : specify the bitmap of sensor types
*  \param   sensor_ids      : provided by caller, sensor id are returned in this array
*  \param   size            : the size of the array snesor_ids is
*
*  \return
*  return                                    |  Condition
* -------------------------------------------|------------------------------
*  # of sensor id got specified by bitmap    |  sensor_ids!=NULL && size>0
*  # of sensor in system specified by bitmap |  sensor_ids==NULL && size==0
*  0                                         |  others (illegal)
*/
int get_sensor_list(phy_sensor_type_bitmap bitmap, sensor_id_t * sensor_ids,
                    int size);
/*!
*  \brief   Open a sensor device with specified dev id.\n
*           A sensor must be opened first before using any other APIs begin with phy_sensor_xxx
*
*  \param   sensor_type     : specify the sensor type
*  \param   dev_id          : specify the dev_id of sensor
*
*  \return  The sensor pointer of abstract physical sensor\n
*           User should check the return value is not NULL before using it.
*/
sensor_t phy_sensor_open(phy_sensor_type_t sensor_type, dev_id_t dev_id);
/*!
*  \brief   Close a sensor device.
*
*  \param   sensor          : specify the sensor, which is retrieved by phy_sensor_open()
*
*/
void phy_sensor_close(sensor_t sensor);
/*!
*  \brief   Get the type of sensor specified.
*
*  \param   sensor          : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   sensor_type     : allocated by user, returns sensor type
*
*  \return  0 if success, error otherwise.\n
*/
int phy_sensor_get_type(sensor_t sensor, phy_sensor_type_t * sensor_type);
/*!
*  \brief   Get the device id of sensor specified.
*
*  \param   sensor          : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   dev_id     : allocated by user, returns sensor device id
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_get_device_id(sensor_t sensor, dev_id_t * dev_id);
/*!
*  \brief   Enable/Disable a sensor.
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   enable     : TRUE-->enable; FALSE-->disable
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_enable(sensor_t sensor, bool enable);
/*!
*  \brief   Set the Output Data Rate of Sensor
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   odr_value  : sensor Output Data Rates; unit is (Hz/10)\n
*                        If target output data rate is 100HZ, 1000 should be used.\n
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_set_odr_value(sensor_t sensor, uint16_t odr_value);
/*!
*  \brief   Get the Output Data Rate of Sensor
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   odr_value  : sensor Output Data Rates; unit is (Hz/10)\n
*                        If *odr_value is 1000, it means 100HZ
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_get_odr_value(sensor_t sensor, uint16_t * odr_value);
/*!
*  \brief   Query the Output Data Rate of Sensor
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   odr_value  : sensor Output Data Rate user want to set; unit is (Hz/10)\n
*                        If *odr_value is 1000, it means 100HZ
*  \param   odr_support: sensor Output Data Rate sensor support; unit is (Hz/10)\n
*                        If *odr_value is 1000, it means 100HZ
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_query_odr_value(sensor_t sensor, uint16_t odr_value, uint16_t *odr_support);
/*!
*  \brief   Get the length of raw sensor data
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   len        : raw data length of sensor in byte\n
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_get_raw_data_len(sensor_t sensor, uint8_t *len);
/*!
*  \brief   Get report mode of Sensor
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   mask       : report_mode_mask see @ref phy_sensor_report_mode\n
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_get_report_mode_mask(sensor_t sensor, uint8_t *mask);
/*!
*  \brief   Get hardware FIFO length
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*
*  \return  fifo length\n
*  \note    If hardware fifo is not supported by sensor, 0 returned\n
*           Hardware FIFO may be shared by multiple physical sensors\n
*/
int phy_sensor_data_get_hwfifo_length(sensor_t sensor);
/*!
*  \brief   Enable/Disable hardware FIFO
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   enable     : TRUE-->enable; FALSE-->disable
*  \param   fifo_len   : this is not used.
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_enable_hwfifo(sensor_t sensor, bool enable, uint16_t fifo_len);
/*!
*  \brief   Enable/Disable hardware FIFO
*
*  \param   sensor     : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   enable     : TRUE-->enable; FALSE-->disable
*  \param   buffer     : buffer used when read fifo data
*  \param   buffer_len : buffer length.
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_enable_hwfifo_with_buffer(sensor_t sensor, bool enable,uint8_t* buffer, uint16_t buffer_len);
/*!
*  \brief   Get the value of specific proper_type for sensor
*
*  \param   sensor            : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   property_type     : the type of property, see @ref phy_sensor_property_type_t
*  \param   value             : pointer to value of the property
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_get_property(sensor_t sensor, uint8_t property_type,
                            void *value);
/*!
*  \brief   Set the value of specific proper_type for sensor
*
*  \param   sensor            : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   property_type     : the type of property, see @ref phy_sensor_property_type_t
*  \param   value             : pointer to value of the property
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_set_property(sensor_t sensor, uint8_t property_type,
                            const void *value);
/*!
*  \brief   Read sensor register data in polling mode.\n
*           One sensor frame is read if succeed
*
*  \param   sensor        : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   sensor_data   : allocated by user, used to store the sensor data\n
*                           user should init the filed 'data_length' of the data struct to actaul length of data buffer\n
*                           the filed 'data_length' will be modifed to actual length of raw sensor data
*
*  \return  actual length of raw sensor data in byte.\n
*/
int phy_sensor_data_read(sensor_t sensor, struct sensor_data *sensor_data);
/*!
*  \brief   Read sensor fifo data in polling mode.\n
*           There can be multiple sensor frames read
*
*  \param   sensor        : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   buffer        : data buffer to read sensor data, it's allocated by caller\n
*                           raw sensor data is stored in the buffer.\n
*                           If fifo is enabled with valid buffer, then this buffer should\n
*                           be NULL. If fifo is enabled without a valid buffer, then you must\n
*                           provide a valid buffer in this API.
*  \param   buff_len      : length of data buffer
*
*  \return  actual length of raw sensor data in byte.\n
*/
int phy_sensor_fifo_read(sensor_t sensor, uint8_t *buffer, uint16_t buff_len);
/*!
*  \brief   register a callback function to read sensor data in interrupt mode.
*
*  \param   sensor        : specify the sensor, which is retrieved by phy_sensor_open()
*  \param   callback      : used by user to read sensor data in interrupt mode\n
*                           when data need to report triggerd by interrupt, the callback will be called.\n
*                           see definition of callback function \ref read_data_cb.
*  \param   priv_data     : private data passed by user
*  \param   sampling_rate : sensor Output Data Rates; unit is (Hz/10)\n
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_data_register_callback(sensor_t sensor, read_data_cb callback,
                                      void *priv_data, uint16_t sampling_rate);
/*!
*  \brief   unregister a callback function to read sensor data in interrupt mode.
*           After this function is called, user need to get sensor data in polling mode.
*
*  \param   sensor        : specify the sensor, which is retrieved by phy_sensor_open()
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_data_unregister_callback(sensor_t sensor);

/*!
*  \brief   Sensor selftest
*
*  \param   sensor        : specify the sensor, which is retrieved by phy_sensor_open()
*
*  \return  0 if success, error otherwise, see @ref DRIVER_API_RC.\n
*/
int phy_sensor_selftest(sensor_t sensor);
/** @} */

#endif
