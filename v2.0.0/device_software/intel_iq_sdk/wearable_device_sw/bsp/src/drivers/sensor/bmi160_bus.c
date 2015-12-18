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

#include "drivers/serial_bus_access.h"
#include "drivers/sensor/sensor_bus_common.h"
#include "drivers/sensor/bmi160_support.h"
#include "drivers/sensor/bmi160_bus.h"
#include "drivers/sensor/bmi160_drv.h"
#include "drivers/sensor/bmm150_drv.h"

/************************* Use Serial Bus Access API *******************/
#define BMI160_REQ_NUM 4
struct sensor_sba_info *bmi160_sba_info;

DRIVER_API_RC bmi160_bus_read(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt)
{
    return sensor_bus_access(bmi160_sba_info, reg_addr, reg_data, cnt, true, NULL);
}

DRIVER_API_RC bmi160_bus_burst_read(uint8_t reg_addr, uint8_t * reg_data, uint32_t cnt)
{
    return sensor_bus_access(bmi160_sba_info, reg_addr, reg_data, cnt, true, NULL);
}

#define BMI160_BUS_WRITE_BUFFER_SIZE 4

DRIVER_API_RC bmi160_bus_write(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt)
{
    uint8_t buffer[BMI160_BUS_WRITE_BUFFER_SIZE];
    return sensor_bus_access(bmi160_sba_info, reg_addr, reg_data, cnt, false, buffer);
}

DRIVER_API_RC bmi160_config_bus(struct device *dev)
{
    struct sba_device *sba_dev = (struct sba_device *)dev;
    struct sba_master_cfg_data *sba_priv = (struct sba_master_cfg_data *)dev->parent->priv;
#ifdef CONFIG_BMI160_SPI
    bmi160_sba_info = sensor_config_bus(sba_dev->addr.cs, sba_dev->dev.id, sba_priv->bus_id,
                                        SENSOR_BUS_TYPE_SPI, BMI160_REQ_NUM);
#else
    bmi160_sba_info = sensor_config_bus(sba_dev->addr.slave_addr, sba_dev->dev.id, sba_priv->bus_id,
                                        SENSOR_BUS_TYPE_I2C, BMI160_REQ_NUM);
#endif
    if(bmi160_sba_info)
        return DRV_RC_OK;
    return DRV_RC_FAIL;
}
extern int bmi160_sensor_register(void);
static int sba_bmi160_init(struct device *dev)
{
    int ret;
    ret = bmi160_sensor_register();
    bmi160_accel.sensor.fifo_share_bitmap = (1<<bmi160_gyro.sensor.dev_id);
    bmi160_gyro.sensor.fifo_share_bitmap  = (1<<bmi160_accel.sensor.dev_id);
#if BMI160_ENABLE_MAG
    ret += bmm150_sensor_register();
    bmi160_accel.sensor.fifo_share_bitmap |= (1<<bmm150_mag.sensor.dev_id);
    bmi160_gyro.sensor.fifo_share_bitmap  |= (1<<bmm150_mag.sensor.dev_id);

    bmm150_mag.sensor.fifo_share_bitmap |=(1<<bmi160_accel.sensor.dev_id);
    bmm150_mag.sensor.fifo_share_bitmap |= (1<<bmi160_gyro.sensor.dev_id);
#endif
    ret += bmi160_config_bus(dev);

#ifdef CONFIG_BMI160_SPI
    uint8_t dummy = 0;
    /* Perform a dummy read from 0x7f to switch to spi interface */
    bmi160_read_reg(0x7F,&dummy);
#endif
    /* Perform a softreset */
    bmi160_set_command_register(TRIGGER_RESET);
    return ret;
}

#ifdef CONFIG_BMI160_SPI
struct driver spi_bmi160_driver = {
    .init = sba_bmi160_init,
    .suspend = NULL,
    .resume = NULL
};
#else
struct driver i2c_bmi160_driver = {
    .init = sba_bmi160_init,
    .suspend = NULL,
    .resume = NULL
};
#endif
