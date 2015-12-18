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

#ifndef __BMI160_DRV_H__
#define __BMI160_DRV_H__

 struct bmi160_sensor_drv_t {
    struct phy_sensor_t sensor;
    uint8_t bmi160_type;
};

extern struct bmi160_sensor_drv_t bmi160_accel;
extern struct bmi160_sensor_drv_t bmi160_gyro;

int bmi160_drv_generic_open(struct phy_sensor_t *sensor);
void bmi160_drv_generic_close(struct phy_sensor_t *sensor);
int bmi160_drv_generic_query_odr(struct phy_sensor_t * sensor, uint16_t odr_target, uint16_t *odr_support);
int bmi160_drv_generic_set_odr(struct phy_sensor_t *sensor, uint16_t odr_hz_x10);
int bmi160_drv_generic_enable_int_mode(struct phy_sensor_t *sensor,
                                   generic_read_data_cb callback, void *priv_data,
                                   uint16_t sample_rate_x10, bool enable);
int bmi160_drv_generic_get_fifo_len(struct phy_sensor_t *sensor);
int bmi160_drv_generic_get_property(struct phy_sensor_t * sensor, uint8_t type, void *value);
int bmi160_drv_generic_set_property(struct phy_sensor_t * sensor, uint8_t type, const void *value);
#endif
