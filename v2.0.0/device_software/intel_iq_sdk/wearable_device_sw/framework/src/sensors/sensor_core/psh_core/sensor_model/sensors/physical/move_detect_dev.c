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

#include "options.h"
#include "sensor_core.h"
#include "runtime.h"

static int move_detect_dev_create(struct sensor *psensor){
    return 0;
}
static int move_detect_dev_cfg_stream(struct sensor *my,u16 *dat){
    return 0;
}
static int move_detect_dev_exec(struct sensor *my,const struct sensor *reporter){
    struct md_data *data = (struct md_data*)my->data_buf;
    if(data->state){
        psh_log("power off!!!!!!!!!!!\n");
        sensor_mark_exec_evt(my, SYS_MD_EVT_STILL);
        sensor_set_status(my, SS_INACTIVE);
    }else{
        psh_log("power on !!!!!!!!!!\n");
        sensor_mark_exec_evt(my, SYS_MD_EVT_MOVE);
        sensor_clear_status(my, SS_INACTIVE);
    }
    return 0;
}
static struct sensor_funcs move_detect_dev_funcs={
    .create=move_detect_dev_create,
    .cfg_stream=move_detect_dev_cfg_stream,
    .exec=move_detect_dev_exec,
};

struct sensor_evt_priv md_evt_priv = {
    .rpt_info = {
        .cfg_freq = 10,
        .buff_delay = 0,
    },
    .exec_evt_bitmask = SYS_MD_EVTS,
};

define_sensor(move_detect_dev,struct md_data,&move_detect_dev_funcs,&md_evt_priv);
