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

//record the latest reported activity state
static u32 lastest_activity;

static int activity_create(struct sensor *psensor)
{
    lastest_activity = 0;
    return 0;
}

static int activity_cfg_stream(struct sensor *my,u16 *dat)
{
    lastest_activity = 0;
    return 0;
}

static int activity_exec(struct sensor *my,const struct sensor *reporter)
{
    struct stepcounter_result *in = (struct stepcounter_result *)(reporter->data_buf);
    struct activity_result *out = (struct activity_result *)(my->data_buf);

    //any change in activity(remove redundant report from stepcounter) ?
    if(in->activity != lastest_activity){
        out->type = in->activity;
        sensor_mark_data_rdy(my);
        lastest_activity = in->activity;
    }

    return 0;
}

static int activity_goto_idle (struct sensor *my) {
    return 0;
}

static struct sensor_funcs activity_funcs={
    .create=activity_create,
    .cfg_stream=activity_cfg_stream,
    .exec=activity_exec,
    .goto_idle = activity_goto_idle,
};
define_sensor(activity,struct activity_result,&activity_funcs,NULL);

