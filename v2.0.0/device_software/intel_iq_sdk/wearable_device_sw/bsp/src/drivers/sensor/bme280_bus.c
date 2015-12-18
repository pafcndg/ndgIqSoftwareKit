/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential information
 * of Intel or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 *
 ******************************************************************************/

#include "drivers/serial_bus_access.h"
#include "drivers/sensor/sensor_bus_common.h"
#include "drivers/sensor/bme280_support.h"

/************************* Use Serial Bus Access API *******************/
#define BME280_REQ_NUM 2

#define BME280_BUS_ADDRESS 0x76

 struct sensor_sba_info *bme280_sba_info;

DRIVER_API_RC bme280_bus_read(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt)
{
    return bme280_bus_burst_read(reg_addr, reg_data, cnt);
}

DRIVER_API_RC bme280_bus_burst_read(uint8_t reg_addr, uint8_t * reg_data, uint32_t cnt)
{
    return sensor_bus_access(bme280_sba_info, reg_addr, reg_data, cnt, true, NULL);
}

#define BME280_BUS_WRITE_BUFFER_SIZE 4

DRIVER_API_RC bme280_bus_write(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt)
{
    uint8_t buffer[BME280_BUS_WRITE_BUFFER_SIZE];
    return sensor_bus_access(bme280_sba_info, reg_addr, reg_data, cnt, false, buffer);
}

DRIVER_API_RC bme280_config_bus(struct device *dev)
{
    struct sba_device *sba_dev = (struct sba_device *)dev;
    struct sba_master_cfg_data *sba_priv = (struct sba_master_cfg_data *)dev->parent->priv;
#ifdef CONFIG_BME280_SPI                    //on APP, bme280 is connected to ss_spi0
    bme280_sba_info = sensor_config_bus(sba_dev->addr.cs, sba_dev->dev.id, sba_priv->bus_id,
                                        SENSOR_BUS_TYPE_SPI, BME280_REQ_NUM);
#else                                       //on CRB, bme280 is connected to ss_spi0
    bme280_sba_info = sensor_config_bus(sba_dev->addr.slave_addr, sba_dev->dev.id, sba_priv->bus_id,
                                        SENSOR_BUS_TYPE_I2C, BME280_REQ_NUM);
#endif
    if(bme280_sba_info)
        return DRV_RC_OK;
    return DRV_RC_FAIL;
}


extern int bme280_sensor_register(void);
static int sba_bme280_init(struct device *dev)
{
    int ret;
    ret = bme280_sensor_register();
    ret += bme280_config_bus(dev);
    bme280_set_softreset();
    sensor_delay_ms(3);
    return ret;
}

struct driver sba_bme280_driver = {
    .init = sba_bme280_init,
    .suspend = NULL,
    .resume = NULL
};
