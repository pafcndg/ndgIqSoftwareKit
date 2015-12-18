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

#include "misc/__assert.h"
#include "infra/log.h"
#include "sensors/phy_sensor_api/phy_sensor_drv_api.h"

static struct phy_sensor_t *sensors_list[MAX_PHY_SENSOR_NUM] = { NULL };

static int sensors_registered = 0;

int sensor_register(struct phy_sensor_t *sensor)
{
    struct phy_sensor_api_t *api;

    if (sensor == NULL)
        return DRV_RC_INVALID_CONFIG;

    if (sensor->dev_id > 0)
        return DRV_RC_CONTROLLER_IN_USE;

    api = &sensor->api;
    /* check if all mandatory APIs implemented */
    __ASSERT(api->open!=NULL, "open not implemented");
    __ASSERT(api->close!=NULL, "close not implemented");

    /* add the sensor to sensors_list */
    sensors_list[sensors_registered] = sensor;
    sensor->dev_id = ++sensors_registered;
    return DRV_RC_OK;
}

int get_sensors_registered(void)
{
    return sensors_registered;
}

struct phy_sensor_t** get_sensors_list_prt(void)
{
    return sensors_list;
}
