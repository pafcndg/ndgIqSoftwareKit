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

#ifndef __BMM150_SUPPORT_H__
#define __BMM150_SUPPORT_H__

#define BMM150_DEVICE_ID    (0x32)

#define BMM150_DO_INIT         1

#define BMI160_BMM150_READ  1
#define BMI160_BMM150_WRITE 0

#define BMI160_PULL_UP_DATA             (0x30)

#define BMM150_POWER_ON          0x01
#define BMM150_POWER_OFF         0x00

#define BMM150_DO_SOFT_RESET     0x01

#define BMM150_POWER_MODE_NORMAL 0x00
#define BMM150_POWER_MODE_FORCED 0x01
#define BMM150_POWER_MODE_SLEEP  0x03

#define BMM150_NO_SELFTEST       0x00

#define BMM150_PRESETMODE_LOWPOWER      1
#define BMM150_PRESETMODE_REGULAR       2
#define BMM150_PRESETMODE_HIGHACCURACY  3
#define BMM150_PRESETMODE_ENHANCED      4

#define BMM150_COMPENSATE_X 0
#define BMM150_COMPENSATE_Y 1

#define BMM150_MAG_READ_BURST_1         0x00
#define BMM150_MAG_READ_BURST_2         0x01
#define BMM150_MAG_READ_BURST_6         0x02
#define BMM150_MAG_READ_BURST_8         0x03

#define BMI150_SOFTRESET_MASK           0x42

#define BMI150_USE_0_001uT              0

/*!
 * @brief Structure bmm150 mag trim data
 */
struct trim_data_t {
    int8_t dig_x1;      /**<BMM150 trim x1 data*/
    int8_t dig_y1;      /**<BMM150 trim y1 data*/

    int8_t dig_x2;      /**<BMM150 trim x2 data*/
    int8_t dig_y2;      /**<BMM150 trim y2 data*/

    uint16_t dig_z1;    /**<BMM150 trim z1 data*/
    int16_t dig_z2;     /**<BMM150 trim z2 data*/
    int16_t dig_z3;     /**<BMM150 trim z3 data*/
    int16_t dig_z4;     /**<BMM150 trim z4 data*/

    uint8_t dig_xy1;    /**<BMM150 trim xy1 data*/
    int8_t dig_xy2;     /**<BMM150 trim xy2 data*/

    uint16_t dig_xyz1;  /**<BMM150 trim xyz1 data*/
};

DRIVER_API_RC bmm150_bus_access_manual(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt, uint8_t read);

int32_t bmm150_mag_compensate_Z(int16_t mag_z, uint16_t mag_r);

int32_t bmm150_mag_compensate_X_Y(int16_t value_s16, uint16_t mag_r, uint8_t type);

static inline DRIVER_API_RC bmm150_read_reg(uint8_t reg_addr, uint8_t *reg_data)
{
    return bmm150_bus_access_manual(reg_addr, reg_data, 1, BMI160_BMM150_READ);
}

static inline DRIVER_API_RC bmm150_write_reg(uint8_t reg_addr, uint8_t *reg_data)
{
    return bmm150_bus_access_manual(reg_addr, reg_data, 1, BMI160_BMM150_WRITE);
}

static void inline bmm150_8bytes_to_xyzr(uint8_t* bytes, struct bmm150_mag_xyzr_t* data)
{
        /* Data X */
    bytes[0] = BMI160_GET_BITSLICE(bytes[0], BMI160_USER_DATA_MAG_X_LSB);
    data->x = (int16_t)
              ((((int32_t)((int8_t)bytes[1]))<< 5) | (bytes[0]));
    /* Data Y */
    bytes[2] = BMI160_GET_BITSLICE(bytes[2], BMI160_USER_DATA_MAG_Y_LSB);
    data->y = (int16_t)
              ((((int32_t)((int8_t)bytes[3]))<< 5) | (bytes[2]));
    /* Data Z */
    bytes[4] = BMI160_GET_BITSLICE(bytes[4], BMI160_USER_DATA_MAG_Z_LSB);
    data->z = (int16_t)
              ((((int32_t)((int8_t)bytes[5]))<< 7) | (bytes[4]));
    /* Data R */
    bytes[6] = BMI160_GET_BITSLICE(bytes[6], BMI160_USER_DATA_MAG_R_LSB);
    data->r = (int16_t)
              ((((int32_t)((int8_t)bytes[7]))<< 6) | (bytes[6]));
}

static inline void bmm150_xyzr_to_s32xyz(struct bmm150_mag_xyzr_t *mag_xyzr, struct bmi160_s32_xyz_t *mag_comp_xyz)
{
    /* Compensation for X axis */
    mag_comp_xyz->x = bmm150_mag_compensate_X_Y(mag_xyzr->x, mag_xyzr->r, BMM150_COMPENSATE_X);

    /* Compensation for Y axis */
    mag_comp_xyz->y = bmm150_mag_compensate_X_Y(mag_xyzr->y, mag_xyzr->r, BMM150_COMPENSATE_Y);

    /* Compensation for Z axis */
    mag_comp_xyz->z = bmm150_mag_compensate_Z(mag_xyzr->z, mag_xyzr->r);

#if BMI150_USE_0_001uT
    /* convert 16 LSB/uT to 0.001uT */
    mag_comp_xyz->x = (mag_comp_xyz->x*125/2);
    mag_comp_xyz->y = (mag_comp_xyz->y*125/2);
    mag_comp_xyz->z = (mag_comp_xyz->z*125/2);
#endif
}

int bmm150_mag_read_data(uint8_t * buf, uint16_t buff_len);
DRIVER_API_RC bmm150_resume(uint8_t is_init);
DRIVER_API_RC bmm150_suspend(void);
DRIVER_API_RC bmm150_sensor_open(void);

#endif
