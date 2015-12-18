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

#include "util/compiler.h"
#include "machine.h"
#include "string.h"
#include "infra/log.h"
#include "infra/time.h"
#include "os/os.h"
#include "scss_registers.h"
#include "sensors/phy_sensor_api/phy_sensor_drv_api.h"
#include "sensors/phy_sensor_api/phy_sensor_api.h"
#include "util/compiler.h"

int get_sensor_list(phy_sensor_type_bitmap bitmap, sensor_id_t * dev_ids,
                    int size)
{
    int i, j = 0;
    int sensors_registered = get_sensors_registered();
    struct phy_sensor_t **sensors_list = get_sensors_list_prt();

    if((!dev_ids && size!=0) || (dev_ids && size<=0))
        return 0;

    if (dev_ids == NULL && size == 0) {
        for (i = 0; i < sensors_registered; i++) {
            if (sensors_list[i] && ((1 << sensors_list[i]->type) & bitmap))
                j++;
        }
        return j;   /* return how many sensors of specific types */
    }

    for (i = 0; i < sensors_registered; i++) {
        if (sensors_list[i] && ((1 << sensors_list[i]->type) & bitmap)) {
            dev_ids[j].dev_id = sensors_list[i]->dev_id;
            dev_ids[j].sensor_type = sensors_list[i]->type;
            j++;
            if(j >= size)
                break;
        }
    }

    return j;
}

sensor_t phy_sensor_open(phy_sensor_type_t sensor_type, dev_id_t dev_id)
{
    struct phy_sensor_t *sensor;
    int sensors_registered = get_sensors_registered();
    struct phy_sensor_t **sensors_list = get_sensors_list_prt();

    if (dev_id > sensors_registered || dev_id == 0 ||
        sensors_list[dev_id - 1]->type != sensor_type)
        return NULL;

    sensor = sensors_list[dev_id - 1];

    /* check if the sensor has been opened before
     * The caller cannot open a sensor which is opened.
     */
    if (sensor->refcnt) {
        return NULL;

    } else {
        if (sensor->api.open(sensor))
            return NULL;
    }

    if (sensor->api.selftest) {
        if (sensor->api.selftest(sensor))
            return NULL;
    }

    sensor->refcnt++;

    return (sensor_t) sensor;
}

void phy_sensor_close(sensor_t sensor)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return;

    phy_sensor_set_odr_value(sensor, 0);
    phy_sensor_enable_hwfifo(sensor, false, 0);
    phy_sensor_data_unregister_callback(sensor);

    phy_sensor->refcnt--;
    if (phy_sensor->refcnt == 0)
        phy_sensor->api.close(phy_sensor);
}

int phy_sensor_get_type(sensor_t sensor, phy_sensor_type_t * sensor_type)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !sensor_type || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    *sensor_type = phy_sensor->type;
    return DRV_RC_OK;
}

int phy_sensor_get_device_id(sensor_t sensor, dev_id_t * dev_id)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !dev_id || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    *dev_id = phy_sensor->dev_id;
    return DRV_RC_OK;
}

int phy_sensor_get_raw_data_len(sensor_t sensor, uint8_t *len)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !len || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;
    *len = phy_sensor->raw_data_len;
    return DRV_RC_OK;
}

int phy_sensor_get_report_mode_mask(sensor_t sensor, uint8_t *mask)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !mask || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;
    *mask = phy_sensor->report_mode_mask;
    return DRV_RC_OK;
}

int phy_sensor_enable(sensor_t sensor, bool enable)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if(phy_sensor->api.activate)
        return phy_sensor->api.activate(phy_sensor, enable);

    return DRV_RC_OK;
}

int phy_sensor_set_odr_value(sensor_t sensor, uint16_t odr_hz_x10)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;

    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if (!phy_sensor->api.set_odr)
        return DRV_RC_INVALID_OPERATION;

    if (odr_hz_x10 == phy_sensor->odr_hz_x10)
        return DRV_RC_OK;

    if (phy_sensor->api.set_odr(phy_sensor, odr_hz_x10))
        return DRV_RC_FAIL;

    phy_sensor->odr_hz_x10 = odr_hz_x10;
    return DRV_RC_OK;
}

int phy_sensor_get_odr_value(sensor_t sensor, uint16_t * odr_value)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;

    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    *odr_value = phy_sensor->odr_hz_x10;
    return DRV_RC_OK;
}

int phy_sensor_query_odr_value(sensor_t sensor, uint16_t odr_value, uint16_t *odr_support)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;
    if(!phy_sensor->api.query_odr)
        return DRV_RC_INVALID_OPERATION;
    return phy_sensor->api.query_odr(phy_sensor, odr_value, odr_support);
}


int phy_sensor_data_get_hwfifo_length(sensor_t sensor)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    int ret;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return 0;

    if (phy_sensor->hw_fifo_len)
        return phy_sensor->hw_fifo_len;

    if (phy_sensor->api.get_fifo_len) {
        ret = phy_sensor->hw_fifo_len =
            phy_sensor->api.get_fifo_len(phy_sensor);
        if (ret < 0) {
            phy_sensor->hw_fifo_len = 0;
        }
        return phy_sensor->hw_fifo_len;
    }
    return 0;
}

int phy_sensor_enable_hwfifo(sensor_t sensor, bool enable, uint16_t fifo_len)
{
    return phy_sensor_enable_hwfifo_with_buffer(sensor, enable, NULL, 0);
}


int phy_sensor_enable_hwfifo_with_buffer(sensor_t sensor, bool enable,
                                         uint8_t* buffer, uint16_t buffer_len)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    int ret;

    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if (phy_sensor->api.enable_fifo) {
        ret = phy_sensor->api.enable_fifo(phy_sensor, buffer, buffer_len, enable);
        if (ret)
            return DRV_RC_FAIL;

        if (enable)
            phy_sensor->fifo_enabled = 1;
        else
            phy_sensor->fifo_enabled = 0;

        return DRV_RC_OK;
    }

    return DRV_RC_INVALID_OPERATION;
}

static int phy_sensor_property_access(struct phy_sensor_t *phy_sensor, uint8_t property_type, void *value, bool property_set)
{
    if(property_set && phy_sensor->api.set_property){
        return phy_sensor->api.set_property(phy_sensor, property_type, (const void*)value);
    } else if (!property_set && phy_sensor->api.get_property){
        return phy_sensor->api.get_property(phy_sensor, property_type, value);
    }
    return DRV_RC_INVALID_OPERATION;
}

int phy_sensor_get_property(sensor_t sensor, uint8_t property_type, void *value)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if(property_type==SENSOR_PROP_FIFO_SHARE_BITMAP){
        phy_sensor_fifo_share_property_t *property_fifo_share = (phy_sensor_fifo_share_property_t *)value;
        property_fifo_share->bitmap = phy_sensor->fifo_share_bitmap;
    }else
        return phy_sensor_property_access(phy_sensor, property_type, value, false);

    return DRV_RC_OK;
}

int phy_sensor_set_property(sensor_t sensor, uint8_t property_type, const void *value)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    return phy_sensor_property_access(phy_sensor, property_type, (void*)value, true);
}

int phy_sensor_data_read(sensor_t sensor, struct sensor_data *sensor_data)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    int actual_len;
    uint32_t timestamp_ms;

    if (_Rarely(!sensor || !phy_sensor->refcnt || !sensor_data->data
             || (sensor_data->data_length < phy_sensor->raw_data_len)))
        return 0;

    if (_Rarely(!phy_sensor->api.read))
        return 0;

    actual_len = phy_sensor->api.read(phy_sensor, sensor_data->data,
                                      sensor_data->data_length);
    timestamp_ms = get_uptime_ms();

    if (_Rarely(actual_len != phy_sensor->raw_data_len)) {
        pr_debug(LOG_MODULE_PHY_SS, "sensor[%d] data len mismatch", phy_sensor->type);
        return 0;
    }

    sensor_data->timestamp = timestamp_ms;
    sensor_data->sensor.sensor_type = phy_sensor->type;
    sensor_data->sensor.dev_id = phy_sensor->dev_id;
    sensor_data->data_length = phy_sensor->raw_data_len;

    return actual_len;
}

int phy_sensor_fifo_read(sensor_t sensor, uint8_t *buffer, uint16_t buff_len)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return 0;

    if(phy_sensor->api.fifo_read)
        return phy_sensor->api.fifo_read(phy_sensor, buffer, buff_len);

    return 0;
}

static int generic_callback(uint8_t * buffer, uint16_t buff_len,
                            void *priv_data)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)priv_data;
    uint32_t timestamp_ms = get_uptime_ms();
    __aligned(4) uint8_t sensor_data_bytes[32];
    struct sensor_data *sensor_data = (struct sensor_data *)sensor_data_bytes;
    uint16_t interval_ms = 0;
    uint8_t cnt;
    uint16_t pos = 0;

    if (_Rarely(!phy_sensor || !buff_len))
        return DRV_RC_INVALID_CONFIG;

    if (phy_sensor->odr_hz_x10) /* if the phy_sensor->odr_hz_x10 is 0, the sensor is on-change type, interval_ms is 0 */
        interval_ms = 10000 / phy_sensor->odr_hz_x10;

    for (cnt = buff_len / phy_sensor->raw_data_len; cnt > 0; cnt--) {
        sensor_data->timestamp = timestamp_ms - (cnt - 1) * interval_ms;
        sensor_data->sensor.sensor_type = phy_sensor->type;
        sensor_data->sensor.dev_id = phy_sensor->dev_id;
        sensor_data->data_length = phy_sensor->raw_data_len;
        memcpy(sensor_data->data, buffer+pos, sensor_data->data_length);
        pos += sensor_data->data_length;
        if (_Usually(phy_sensor->read_callback))
            phy_sensor->read_callback(sensor_data, phy_sensor->cb_priv_data);
    }

    return DRV_RC_OK;
}

int phy_sensor_data_register_callback(sensor_t sensor, read_data_cb callback,
                                      void *priv_data, uint16_t sampling_rate)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt || !callback))
        return DRV_RC_INVALID_CONFIG;

    phy_sensor->read_callback = callback;
    phy_sensor->cb_priv_data = priv_data;

    /* if the driver not implement int(*enable_int)(...),
     * that means the sensor not support report data in interrupt mode
     */
    if (callback && phy_sensor->api.enable_int_mode) {
        return phy_sensor->api.enable_int_mode(phy_sensor, generic_callback,
                                               (void *)phy_sensor,
                                               sampling_rate, true);
    }

    return DRV_RC_INVALID_OPERATION;
}

int phy_sensor_data_unregister_callback(sensor_t sensor)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    int ret;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if (!phy_sensor->read_callback)
        return DRV_RC_INVALID_CONFIG;

    ret = DRV_RC_INVALID_OPERATION;
    if (phy_sensor->api.enable_int_mode) {
        ret = phy_sensor->api.enable_int_mode(phy_sensor, NULL, NULL, 0,
                                        false);
    }

    phy_sensor->read_callback = NULL;
    phy_sensor->cb_priv_data = NULL;

    return ret;
}

int phy_sensor_selftest(sensor_t sensor)
{
    struct phy_sensor_t *phy_sensor = (struct phy_sensor_t *)sensor;
    if (_Rarely(!sensor || !phy_sensor->refcnt))
        return DRV_RC_INVALID_CONFIG;

    if (phy_sensor->api.selftest)
        return phy_sensor->api.selftest(phy_sensor);

    return DRV_RC_INVALID_OPERATION;
}
