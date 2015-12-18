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

/*
   APIs exposed from sensor_core
 */

#ifndef _SENSOR_CORE_H_
#define _SENSOR_CORE_H_

#include "common.h"

#define define_simple_phy_sensor(_name, data_type) \
    static void __pshinit __phy_sensor_init_##_name(void) \
    { \
        int res; \
        struct snr_create_param param = { \
            .data_buf_size = sizeof(data_type), \
            .attri = SENSOR_ATTRI_IS_PHYSICAL, \
            .funcs = NULL, \
            .name = #_name, \
        }; \
        res = sensor_create(NULL, &param); \
        ASSERT(!res); \
    } \
    define_initcall(__phy_sensor_init_##_name)

void sensor_core_init(void);
void sc_reset(void);

int sc_cfg_stream(u8 sensor_id, const void *cfg_buf);
int sc_stop_stream(u8 sensor_id);

#ifdef CONFIG_PSH_CORE_GS_PORT
int sensor_get_single(u8 sensor_id);
#endif

int sensor_calibration(u8 sid, const void *param);

/* no thread-safety protection, could only called in cmd_engine */
int sensor_clear_evt(u8 evt_id);

/* no thread-safety protection, could only called in cmd_engine */
int sensor_parse_evt(void *param_buf, u32 size);

int phy_sensor_report(u8 sid);

int sc_get_snr_info(u8 tran_id, u8 sid, bitmask_t snr_group);

int sc_phy_sensor_trigger(void);

const char *sc_get_sensor_name(u8 sid);

int sc_run_bist(u8 tran_id, u8 trigger_cmd_id);

int sc_set_property(u8 sid, const void *prop, u8 prop_size);
int sc_get_property(u8 sid, const void *param, u8 param_size,
        void *prop_buf, u16 *prop_buf_size);

int sc_lpe_trigger(void);

int sc_handle_ia_notify(const void *param);

int sensor_core_create(void);
#endif
