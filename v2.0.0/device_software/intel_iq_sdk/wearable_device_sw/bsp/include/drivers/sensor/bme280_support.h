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

#ifndef __BME280_SUPPORT_H__
#define __BME280_SUPPORT_H__

#include "util/compiler.h"
#include "machine.h"
#include "os/os.h"
#include "os/os_types.h"
#include "infra/log.h"
#include "drivers/data_type.h"
#include "sensors/phy_sensor_api/phy_sensor_drv_api.h"
#include "drivers/sensor/bme280.h"
#include "drivers/sensor/sensor_bus_common.h"
#include "drivers/sensor/bme280_bus.h"
#include "misc/printk.h"

enum bme280_sensor_type {
    BME280_SENSOR_TEMP = 0,
    BME280_SENSOR_PRESS,
    BME280_SENSOR_HUMID,
    BME280_SENSOR_COUNT,
};

DRIVER_API_RC bme280_sensor_open(struct phy_sensor_t *sensor);
DRIVER_API_RC bme280_set_workmode(struct phy_sensor_t *sensor);
struct bme280_t* bme280_get_ptr(void);

#endif