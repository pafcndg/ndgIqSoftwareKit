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
#include "tapping_atp.h"

static int tapping_create(struct sensor *psensor)
{
    tapping_atp_initial();
    return 0;
}
static int tapping_cfg_stream(struct sensor *my,u16 *dat)
{
    return 0;
}

static int tapping_exec(struct sensor *my,const struct sensor *reporter)
{
    int16_t data[3];
    struct tapping_result *my_data = my->data_buf;
    TAPATPResult tRes;

    memcpy(&data[0], reporter->data_buf, sizeof(int16_t)*3);

    tRes = tapping_atp_process(data);
    if(tRes != TAPATP_NO_TAPPING){
        my_data->tapping_cnt = (s16)tRes;
        sensor_mark_data_rdy(my);
    }

    return 0;
}

static int tapping_goto_idle (struct sensor *my) {
    return 0;
}

static int tapping_out_idle(struct sensor *my)
{
    tapping_atp_initial();
    return 0;
}

static struct sensor_funcs tapping_funcs={
        .create=tapping_create,
        .cfg_stream=tapping_cfg_stream,
        .exec=tapping_exec,
        .goto_idle = tapping_goto_idle,
        .out_idle = tapping_out_idle
};
define_sensor(tapping,struct tapping_result,&tapping_funcs,NULL);
