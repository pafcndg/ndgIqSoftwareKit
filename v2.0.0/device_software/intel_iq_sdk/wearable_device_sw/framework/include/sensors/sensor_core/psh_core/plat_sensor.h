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

#ifndef _PLAT_SENSORS_
#define _PLAT_SENSORS_

#include "common.h"
#include "axis_map.h"

/* sensors' physical interface */
struct phy_cfg_param {
    u16 freq;
    u16 bit_cfg;
};

struct phy_driver;
typedef int (*phy_init_f)(struct phy_driver *phy_drv);
typedef int (*phy_config_f)(struct phy_driver *phy_drv, struct phy_cfg_param *cfg);
typedef u16 (*freq_query_f)(struct phy_driver *phy_drv, u16 freq_to);
typedef int (*phy_read_data_f)(struct phy_driver *phy_drv, void *data_buf, u8 data_size);
typedef int (*phy_self_test_f)(struct phy_driver *phy_drv);
typedef int (*phy_set_property_f)(struct phy_driver *phy_drv, const void *prop, u8 type);
typedef int (*phy_get_property_f)(struct phy_driver *phy_drv,
        const void *param, u8 param_size,
        void *prop_buf, u16 *prop_buf_size);

struct phy_sensor_funcs {
    phy_init_f init;
    phy_config_f config;
    /* called before call cfg_stream, query the actual frequency
       will be set by driver */
    freq_query_f freq_query;
    phy_read_data_f read_data;
    phy_self_test_f self_test;
    phy_set_property_f set_property;
    phy_get_property_f get_property;
};

struct phy_driver {
    void *priv_data;
    struct phy_sensor_funcs funcs;
    char *ops_name;
    struct i2c_adapter *adap;
    int bus;
    unsigned int pin;
    unsigned int int_mode;
    char *gendrv_xml;
    u8 addr;
    u8 id;
    u8 sid;
    u8 fifo_len;
    u16 freq_max;
    u16 bit_cfg_mask;
    u16 status;
    u32 who_am_i;
    struct axis_map axis_map;
    char sname[SNR_NAME_MAX_LEN];
};

/* driver status */
#define DS_DATA_OVERFLOW  ((u16)(0x1 << 0))
#define DS_DATA_UNDERFLOW ((u16)(0x1 << 1))
#define DS_GAIN_HIGHEST   ((u16)(0x1 << 2))
#define DS_GAIN_LOWEST    ((u16)(0x1 << 3))

#define DS_FIFO_OVERRUN   ((u16)(0x1 << 4))

static inline void drv_set_status(
        struct phy_driver *drv, u16 status)
{
    drv->status |= status;
}

static inline void drv_clear_status(
        struct phy_driver *drv, u16 status)
{
    drv->status &= ~status;
}

static inline int drv_test_status(
        const struct phy_driver *drv, u16 status)
{
    return (drv->status & status);
}

static inline u16 drv_test_clear_status(
        struct phy_driver *drv, u16 status)
{
    u16 res = drv_test_status(drv, status);

    if (res)
        drv_clear_status(drv, status);
    return res;
}

struct phy_driver *ps_get_phy_driver(const char *sname);
int register_phy_driver(struct phy_driver *drv);

#endif

