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

#ifndef __BMI160_SUPPORT_H__
#define __BMI160_SUPPORT_H__

#include "util/compiler.h"
#include "machine.h"
#include "os/os.h"
#include "os/os_types.h"
#include "infra/log.h"
#include "infra/time.h"
#include "drivers/data_type.h"
#include "string.h"
#include "sensors/phy_sensor_api/phy_sensor_api.h"
#include "sensors/phy_sensor_api/phy_sensor_drv_api.h"
#include "drivers/sensor/sensor_bus_common.h"
#include "drivers/sensor/bmi160_regs.h"
#include "drivers/sensor/bmm150_regs.h"

#define FIFO_FRAME              512
#define FIFO_FRAME_CNT          75
#define MAG_FIFO_FRAME_CNT      57

#define TYPE_MASK_ACCEL             0x01
#define TYPE_MASK_GYRO              0x02
#define TYPE_MASK_MAG               0x04
#define TYPE_MASK_ACCEL_GYRO        (TYPE_MASK_ACCEL|TYPE_MASK_GYRO)
#define TYPE_MASK_ACCEL_MAG         (TYPE_MASK_ACCEL|TYPE_MASK_MAG)
#define TYPE_MASK_GYRO_MAG          (TYPE_MASK_GYRO | TYPE_MASK_MAG)
#define TYPE_MASK_ACCEL_GYRO_MAG    (TYPE_MASK_ACCEL|TYPE_MASK_GYRO|TYPE_MASK_MAG)

#define ACCEL_INT_ENABLE_MASK       0x01
#define GYRO_INT_ENABLE_MASK        0x02
#define ACCEL_GYRO_INT_ENABLE_MASK  0x03
#define MAG_INT_ENABLE_MASK         0x04
#define ACCEL_INT_DISABLE_MASK      ((uint8_t)(~ACCEL_INT_ENABLE_MASK))
#define GYRO_INT_DISABLE_MASK       ((uint8_t)(~GYRO_INT_ENABLE_MASK))
#define MAG_INT_DISABLE_MASK        ((uint8_t)(~MAG_INT_ENABLE_MASK))

#define BMI160_INT1_TRIGGER_LEVEL (0<<0)
#define BMI160_INT1_ACTIVE_HIGH   (1<<1)
#define BMI160_INT1_DIR_OUTPUT    (1<<3)
#define BMI160_INT2_TRIGGER_LEVEL (0<<4)
#define BMI160_INT2_ACTIVE_HIGH   (1<<5)
#define BMI160_INT2_DIR_OUTPUT    (1<<7)

#define BMI160_ACCEL_REPORT_MG_PER_SECOND 1

#define BMI160_SENSITIVITY_ACCEL_2G_SHIFT    14
#define BMI160_SENSITIVITY_ACCEL_4G_SHIFT    13
#define BMI160_SENSITIVITY_ACCEL_8G_SHIFT    12
#define BMI160_SENSITIVITY_ACCEL_16G_SHIFT   11

#define BMI160_SENSITIVITY_GYRO_2K_NUMERATOR    61
#define BMI160_SENSITIVITY_GYRO_2K_DENOMINATOR  1

#define BMI160_SENSITIVITY_GYRO_1K_NUMERATOR    305
#define BMI160_SENSITIVITY_GYRO_1K_DENOMINATOR  10

#define BMI160_SENSITIVITY_GYRO_500_NUMERATOR   153
#define BMI160_SENSITIVITY_GYRO_500_DENOMINATOR 10

#define BMI160_SENSITIVITY_GYRO_250_NUMERATOR   76
#define BMI160_SENSITIVITY_GYRO_250_DENOMINATOR 10

#define BMI160_SENSITIVITY_GYRO_125_NUMERATOR   38
#define BMI160_SENSITIVITY_GYRO_125_DENOMINATOR 10

#define BMI160_FIFO_WATERMARK_MAX 976

#define FIFO_BUFFER_OVERFLOW        ((int8_t)-12)
#define FIFO_NO_BUFFER              ((int8_t)-11)
#define FIFO_OVER_READ_RETURN       ((int8_t)-10)
#define FIFO_SENSORTIME_RETURN      ((int8_t)-9)
#define FIFO_SKIP_OVER_LEN          ((int8_t)-8)
#define FIFO_M_G_A_OVER_LEN         ((int8_t)-7)
#define FIFO_M_G_OVER_LEN           ((int8_t)-6)
#define FIFO_M_A_OVER_LEN           ((int8_t)-5)
#define FIFO_G_A_OVER_LEN           ((int8_t)-4)
#define FIFO_M_OVER_LEN             ((int8_t)-3)
#define FIFO_G_OVER_LEN             ((int8_t)-2)
#define FIFO_A_OVER_LEN             ((int8_t)-1)

#ifdef CONFIG_BMI160_ENABLE_DOUBLE_TAP
#define BMI160_ENABLE_DOUBLE_TAPPING 1
#else
#define BMI160_ENABLE_DOUBLE_TAPPING 0
#endif

#ifdef CONFIG_BMM150
#define BMI160_ENABLE_MAG 1
#else
#define BMI160_ENABLE_MAG 0
#endif

#define BMI160_MOTION_DETECTION_DEFAULT_ODR_HZ 100

#ifdef CONFIG_ARC_DRIVER_TESTS
#define BMI160_SUPPORT_FIFO_INT_DATA_REPORT 1
#else
#define BMI160_SUPPORT_FIFO_INT_DATA_REPORT 0
#endif

#define BMI160_INT_SET_0 0
#define BMI160_INT_SET_1 1
#define BMI160_INT_SET_2 2

/* sensor raw data size of sensor hardware */
#define BMI160_ACCEL_RAW_DATA_SIZE  6
#define BMI160_GYRO_RAW_DATA_SIZE   6
#define BMI160_MAG_RAW_DATA_SIZE    8

/* sensor data size output */
#define BMI160_ACCEL_FRAME_SIZE     6
#define BMI160_GYRO_FRAME_SIZE      12
#define BMI160_MAG_FRAME_SIZE       12

enum bmi160_int0_en_offset {
    BMI160_OFFSET_INT0_MIN = 0,
    BMI160_OFFSET_ANYMOTION = BMI160_OFFSET_INT0_MIN,
    BMI160_OFFSET_DOUBLE_TAP,
    BMI160_OFFSET_SINGLE_TAP,
    BMI160_OFFSET_ORIENTATION,
    BMI160_OFFSET_FLAT,
    BMI160_OFFSET_INT0_MAX
};

enum bmi160_int1_en_offset {
    BMI160_OFFSET_INT1_MIN = BMI160_OFFSET_INT0_MAX,
    BMI160_OFFSET_HIGH_G = BMI160_OFFSET_INT0_MIN,
    BMI160_OFFSET_LOW_G,
    BMI160_OFFSET_DRDY,
    BMI160_OFFSET_FIFO_FULL,
    BMI160_OFFSET_FIFO_WM,
    BMI160_OFFSET_INT1_MAX
};

enum bmi160_int2_en_offset {
    BMI160_OFFSET_INT2_MIN = BMI160_OFFSET_INT1_MAX,
    BMI160_OFFSET_NOMOTION = BMI160_OFFSET_INT2_MIN,
    BMI160_OFFSET_SETP,
    BMI160_OFFSET_INT2_MAX,
    BMI160_OFFSET_MAX = BMI160_OFFSET_INT2_MAX
};

enum bmi160_sensor_type {
    BMI160_SENSOR_ACCEL = 0,
    BMI160_SENSOR_GYRO,
#if BMI160_ENABLE_MAG
    BMI160_SENSOR_MAG,
#endif
    BMI160_SENSOR_COUNT,
    BMI160_SENSOR_MOTION = BMI160_SENSOR_COUNT,
};

enum bmi160_power_status {
    BMI160_POWER_SUSPEND = 0,
    BMI160_POWER_NORMAL,
    BMI160_POWER_LOWPOWER,          /* Accel & Mag only */
    BMI160_POWER_FAST_STARTUP,      /* Gyro only */
};


typedef void (*convert_sensor_data_fun)(uint8_t *buf);
typedef int (*sensor_read_data_fun)(uint8_t * buf, uint16_t buff_len);

/*!
 * @brief Structure containing gyro xyz data
 */
struct bmi160_gyro_t {
    int32_t x;/**<gyro X  data*/
    int32_t y;/**<gyro Y  data*/
    int32_t z;/**<gyro Z  data*/
};
/*!
 * @brief Structure containing accel xyz data
 */
struct bmi160_accel_t {
    int16_t x;/**<accel X  data*/
    int16_t y;/**<accel Y  data*/
    int16_t z;/**<accel Z  data*/
};

struct bmm150_mag_xyzr_t {
    int16_t x;/**< BMM150 X raw data*/
    int16_t y;/**< BMM150 Y raw data*/
    int16_t z;/**<BMM150 Z raw data*/
    uint16_t r;/**<BMM150 R raw data*/
};

struct bmi160_s16_xyz_t {
    int16_t x;/**<accel X  data*/
    int16_t y;/**<accel Y  data*/
    int16_t z;/**<accel Z  data*/
};

struct bmi160_s32_xyz_t {
    int32_t x;/**<mag X  data*/
    int32_t y;/**<mag Y  data*/
    int32_t z;/**<mag Z  data*/
};

typedef int (*read_data) (uint8_t * buffer, uint16_t buff_len, void *priv_data);

/*!
 * @brief Structure containing bmi160 runtime information
 */
struct bmi160_rt_t {
    uint8_t chip_id;
    read_data read_callbacks[BMI160_SENSOR_COUNT];
    void *cb_priv_data[BMI160_SENSOR_COUNT];
    read_data motion_callback;
    void *motion_cb_data;
    phy_sesnor_event_cb_t wm_callback[BMI160_SENSOR_COUNT];
    void *wm_cb_priv_data[BMI160_SENSOR_COUNT];
    uint8_t watermark_trigger_cnt[BMI160_SENSOR_COUNT];     /** the number of sample frames to trigger watermark interrupt */
    uint8_t sensor_opened;
    uint8_t power_status[BMI160_SENSOR_COUNT];
    uint8_t motion_state;
    uint8_t accel_config;
    uint8_t int_en[BMI160_OFFSET_MAX];
    uint8_t fifo_en[BMI160_SENSOR_COUNT];
    uint8_t *fifo_ubuffer[BMI160_SENSOR_COUNT];         /** buffers provided by user for fifo data read */
    uint16_t fifo_ubuffer_len[BMI160_SENSOR_COUNT];
    uint16_t fifo_ubuffer_ptr[BMI160_SENSOR_COUNT];
    uint8_t sensor_enabled[BMI160_SENSOR_COUNT];
    uint16_t sensor_odr[BMI160_SENSOR_COUNT];
    uint32_t range_native[BMI160_SENSOR_COUNT];
    uint8_t mag_manual_enable;
    uint8_t undersampling_avg;  //actual avg number = 2^undersampling_avg
    uint8_t sba_suspend;
    convert_sensor_data_fun convert_data_funs[BMI160_SENSOR_COUNT];
    sensor_read_data_fun reg_data_read_funs[BMI160_SENSOR_COUNT];
    /* function pointers for external sensor */
    uint8_t (*parse_mag_sensor_data)(uint8_t *out_buf, uint8_t *out_index, uint16_t *fifo_index);
    DRIVER_API_RC (*change_mag_powermode)(uint8_t powermode);
};

extern uint8_t bmi160_fifo_data[FIFO_FRAME];
extern struct bmi160_accel_t bmi160_accel_fifo[FIFO_FRAME_CNT];
extern struct bmi160_gyro_t bmi160_gyro_fifo[FIFO_FRAME_CNT];
extern uint8_t bmi160_accel_index;
extern uint8_t bmi160_gyro_index;
#if BMI160_ENABLE_MAG
extern struct bmi160_s32_xyz_t bmi160_mag_fifo[MAG_FIFO_FRAME_CNT];
extern uint8_t bmi160_mag_index;
#endif

#if BMI160_ENABLE_DOUBLE_TAPPING
extern uint32_t bmi160_any_motion_timestamp;
extern uint8_t bmi160_wait_double_tap;
#else
extern uint8_t bmi160_any_motion_detected;
#endif

extern uint8_t bmi160_need_update_wm_interrupt;

DRIVER_API_RC bmi160_set_if_mode(uint8_t v_if_mode_u8);
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
DRIVER_API_RC bmi160_set_command_register(uint8_t v_command_reg_u8);
/*!
 *  @brief This API write page enable from the register 0x7F bit 7
 *
 *
 *
 *  @param v_page_enable_u8: The value of page enable
 *  value   |  page
 * ---------|-----------
 *   0      |  DISABLE
 *   1      |  ENABLE
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_set_paging_enable(uint8_t page_enable);
DRIVER_API_RC bmi160_set_target_page(uint8_t target_page);
DRIVER_API_RC bmi160_set_mag_manual_enable(uint8_t enable, uint8_t burst);
/*!
 *  @brief This API reads the  of the sensor
 *  form the register 0x23 and 0x24 bit 0 to 7 and 0 to 2
 *  @brief this byte counter is updated each time a complete frame
 *  was read or writtern
 *
 *
 *  @param v_fifo_length_u32 : The value of fifo byte counter
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_fifo_length(uint32_t *v_fifo_length_u32);
/*!
 *  @brief This API flush the hw fifo
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_flush_fifo(void);
/*!
 *  @brief This API register callback function
 *  for interrupt mode to report sensor data
 *
 *
 *  @param callback: the call back function
 *  @param priv_data: argument of the callback function
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_register_read_cb(read_data callback, void *priv_data, uint8_t sensor_type);
/*!
 *  @brief This API unregister callback function
 *
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_unregister_read_cb(uint8_t sensor_type);
/*!
 *  @brief This API register callback function
 *  for motion detection event
 *
 *
 *  @param callback: the call back function
 *  @param priv_data: argument of the callback function
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_register_motion_cb(read_data callback, void *priv_data);
/*!
 *  @brief This API unregister callback function
 *  for motion detection event
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_unregister_motion_cb(void);
/*!
 *  @brief This API set the output data rate of sensor
 *
 *
 *  @param odr_hz_x10: output data rate, unit is HZ/10
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_sensor_set_rate(uint16_t odr_hz_x10, uint8_t sensor_type);
/*!
 *  @brief This API query the output data rate of sensor
 *
 *
 *  @param odr_target:  target output data rate, unit is HZ/10
 *  @param odr_support: pointer of support output data rate, unit is HZ/10
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_sensor_query_rate(uint16_t odr_target, uint16_t *odr_support, uint8_t sensor_type);
/*!
 *  @brief This API change the power mode of sensor specific
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *  @param accel_pm: new power mode for accelerometer
 *   value    |    Description
 *  ----------|-------------------------------
 *    0       | BMI160_POWER_SUSPEND
 *    1       | BMI160_POWER_NORMAL
 *    2       | BMI160_POWER_LOWPOWER
 *    3       | BMI160_POWER_FAST_STARTUP
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi16_set_user_sensor_config(uint8_t type, uint8_t powermode, uint8_t change_config, uint8_t config);

DRIVER_API_RC bmi160_change_sensor_powermode(uint8_t sensor_type, uint8_t powermode);
/*!
 *  @brief This API open a sensor
 *
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_sensor_open(uint8_t sensor_type);
/*!
 *  @brief This API close a sensor
 *
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_sensor_close(uint8_t sensor_type);
/*!
 *  @brief This API enable hardware fifo for specific sensor
 *
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *  @param fifo_en_mask: mask to enable fifo for sensor_type
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_fifo_enable(uint8_t sensor_type, uint8_t *buffer,
                                 uint16_t buffer_len, uint8_t fifo_en_mask);
/*!
 *  @brief This API enable hardware fifo for specific sensor
 *
 *
 *  @param sensor_type:
 *   value    |    type
 *  ----------|-------------------------------
 *    0       | BMI160_SENSOR_ACCEL
 *    1       | BMI160_SENSOR_GYRO
 *    2       | BMI160_SENSOR_MAG
 *  @param fifo_disable_mask: mask to disable fifo for sensor_type
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
*/
DRIVER_API_RC bmi160_fifo_disable(uint8_t sensor_type, uint8_t fifo_disable_mask);

DRIVER_API_RC bmi160_update_fifo_watermark(void);
/*!
 *  @brief This API read register sensor data for accel
 *
 *
 *  @param buf: buffer to store sensor data
 *  @buff_len: length of buffer
 *
 *
 *  @return actual length of sensor data read
 *
 *
*/
int bmi160_accle_read_data(uint8_t * buf, uint16_t buff_len);
/*!
 *  @brief This API read fifo sensor data for accel
 *
 *
 *  @param buf: buffer to store sensor data
 *  @buff_len: length of buffer
 *
 *
 *  @return actual length of sensor data read
 *
 *
*/
//int bmi160_accel_read_fifo(uint8_t * buf, uint16_t buff_len);
 int bmi160_sensor_read_fifo(uint8_t sensor_type, uint8_t *buf, uint16_t buff_len);
/*!
 *  @brief This API read register sensor data for gyro
 *
 *
 *  @param buf: buffer to store sensor data
 *  @buff_len: length of buffer
 *
 *
 *  @return actual length of sensor data read
 *
 *
*/
int bmi160_gyro_read_data(uint8_t * buf, uint16_t buff_len);

DRIVER_API_RC bmm150_mag_compensate_xyz(struct bmi160_s32_xyz_t *mag_comp_xyz);
/*!
 *  @brief This function used for reading the
 *  fifo data of header mode
 *
 *  @param fifo_data: buffer to store data
 *  @param size : The value of read length
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
 */
DRIVER_API_RC bmi160_get_fifo_data(uint8_t *fifo_data, uint32_t size);
/*!
 *  @brief This function used for reading the
 *  fifo data of  header mode
 *
 *  @param v_fifo_length_u32 : The value of FIFO length
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval 1 -> Error
 *
 *
 */
DRIVER_API_RC bmi160_read_fifo_header_data(uint16_t fifo_read_len);

DRIVER_API_RC bmi160_int_enable(uint8_t int_set, uint8_t int_offset, uint8_t enable_mask);
DRIVER_API_RC bmi160_int_disable(uint8_t int_set, uint8_t int_offset, uint8_t disable_mask);

DRIVER_API_RC bmi160_change_accel_sensing_range(uint8_t range);
DRIVER_API_RC bmi160_change_gyro_sensing_range(uint16_t range);
/*!
 *  @brief This API convert accel raw data
 *
 *
 *  @param data: the sensor data to convert
 *  the result is write back to data
 *
 *
*/
void bmi160_convert_accel_data(uint8_t *buff);
/*!
 *  @brief This API convert accel raw data
 *
 *
 *  @param data: the sensor data to convert
 *  the result is write back to data
 *
 *
*/
void bmi160_convert_gyro_data(uint8_t *buff);

#define bmi160_delay_ms sensor_delay_ms

struct bmi160_rt_t *bmi160_get_ptr(void);


#define DEBUG_BMI160 0
#if DEBUG_BMI160
#include "misc/printk.h"
#endif

#endif
