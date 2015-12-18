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

#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "common.h"
#include "plat_sensor.h"
#include "sensor_bit_cfg.h"
#include "util/compiler.h"

#define __pshoption __used __aligned(1) __section(".pshinit.options")
#define __pshoption_string __pshinit
#define __pshoption_phydrv
#define __pshoption_snr __pshinit

#define define_drvops(_name, ops) \
    char __lib_##_name __pshinitdata; \
    static struct option __option_##_name __pshoption = \
    {.name = #_name, .type = OPTION_TYPE_DRVOPS, .value = (long)&ops,}


#define define_sensor(_name, _data_type, _funcs, _evt_priv) \
    char __lib_##_name __pshinitdata; \
    static void __pshinit __sensor_init_##_name(void) \
    { \
        int res; \
        struct snr_create_param *param = NULL; \
        struct snr_create_param snr; \
        while(option_get_next(#_name, &param, &param)) { \
            memcpy(&snr, param, sizeof(snr)); \
            snr.data_buf_size = sizeof(_data_type); \
            snr.funcs = _funcs; \
            snr.evt_priv = _evt_priv; \
            res = sensor_create(NULL, &snr); \
            ASSERT(!res); \
        } \
    } \
    define_initcall(__sensor_init_##_name)

#define OPTION_TYPE_BOOL    1
#define OPTION_TYPE_INT     2
#define OPTION_TYPE_STRING  3
#define OPTION_TYPE_DRVOPS  4
#define OPTION_TYPE_SNR     5

struct option {
    const char *name;
    int type;
    long    value;
};

void *__pshinit option_get_addr(char *name);
long __pshinit option_get_num(char *name, long def_value);
int __pshinit option_get(char *name, void *value);
int __pshinit option_get_next(char *name, void *cur, void *next);
int __pshinit is_option_enable(char *name);
void __pshinit options_init(void);

#endif
