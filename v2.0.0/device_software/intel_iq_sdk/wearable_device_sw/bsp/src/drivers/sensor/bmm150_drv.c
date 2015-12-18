/*******************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmm150_support.h"
#include "drivers/sensor/bmi160_bus.h"
#include "drivers/sensor/bmi160_drv.h"
#include "drivers/sensor/bmm150_drv.h"

static int mag_open(struct phy_sensor_t *sensor)
{
    struct bmm150_sensor_drv_t *bmm150_sensor = (struct bmm150_sensor_drv_t *)sensor;

    if(bmi160_sensor_open(bmm150_sensor->bmi160_type))
        return DRV_RC_FAIL;

    return bmm150_sensor_open();
}

static void mag_close(struct phy_sensor_t *sensor)
{
    struct bmm150_sensor_drv_t *bmm150_sensor = (struct bmm150_sensor_drv_t *)sensor;
    bmi160_sensor_close(bmm150_sensor->bmi160_type);
}

static int mag_enable_fifo(struct phy_sensor_t *sensor, uint8_t *buffer, uint16_t len,
                            bool enable)
{
    if (enable)
        return bmi160_fifo_enable(BMI160_SENSOR_MAG, buffer, len, BMI160_USER_FIFO_MAG_ENABLE__MSK);
    else
        return bmi160_fifo_disable(BMI160_SENSOR_MAG, (uint8_t)(~BMI160_USER_FIFO_MAG_ENABLE__MSK));
}

static int mag_read_data(struct phy_sensor_t *sensor, uint8_t * buf, uint16_t buff_len)
{
    return bmm150_mag_read_data(buf, buff_len);
}


static int mag_read_fifo(struct phy_sensor_t *sensor, uint8_t * buf,
                           uint16_t buff_len)
{
    return bmi160_sensor_read_fifo(BMI160_SENSOR_MAG, buf, buff_len);
}

struct bmm150_sensor_drv_t bmm150_mag = {
    .sensor = {
               .type = SENSOR_MAGNETOMETER,
               .raw_data_len = sizeof(phy_mag_data_t),
               .hw_fifo_len = 1024,
               .report_mode_mask = PHY_SENSOR_REPORT_MODE_POLL_REG_MASK  |
                                   PHY_SENSOR_REPORT_MODE_INT_REG_MASK   |
                                   PHY_SENSOR_REPORT_MODE_POLL_FIFO_MASK |
                                   PHY_SENSOR_REPORT_MODE_INT_FIFO_MASK,
               .api = {
                       .open = mag_open,
                       .close = mag_close,
                       .query_odr = bmi160_drv_generic_query_odr,
                       .set_odr = bmi160_drv_generic_set_odr,
                       .enable_int_mode = bmi160_drv_generic_enable_int_mode,
                       .enable_fifo = mag_enable_fifo,
                       .fifo_read = mag_read_fifo,
                       .read = mag_read_data,
                       .get_property = bmi160_drv_generic_get_property,
                       },
               },
    .bmi160_type = BMI160_SENSOR_MAG,
};

int bmm150_sensor_register(void)
{
    return sensor_register(&bmm150_mag.sensor);
}


