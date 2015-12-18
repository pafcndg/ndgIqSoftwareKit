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
#include "atp_act.h"
#include "util/compiler.h"

static struct stepcounter_result latest_res = {0};

static int stepcounter_create(struct sensor *psensor)
{
    alg_steps_activity_init();
    latest_res.steps = 0;
    latest_res.activity = 0;
    return 0;
}

static int stepcounter_cfg_stream(struct sensor *my,u16 *dat)
{
    latest_res.steps = 0;
    latest_res.activity = 0;
    return 0;
}
static int stepcounter_goto_idle (struct sensor *my) {
    return 0;
}

static int stepcounter_exec(struct sensor *my,const struct sensor *reporter){
    int16_t data[3];
    uint32_t steps_num;
    int16_t activity_type;
    struct stepcounter_result *my_data = my->data_buf;

    memcpy(&data[0], reporter->data_buf, sizeof(int16_t)*3);

    alg_steps_activity_run(data);

    steps_num = alg_steps_report();
    activity_type = alg_activity_report();

    //any change in result? (to remove redundant report or report on change)
    if(latest_res.steps != steps_num ||
            latest_res.activity != activity_type){

        my_data->steps = steps_num;
        my_data->activity = activity_type;
        sensor_mark_data_rdy(my);

        latest_res.steps = steps_num;
        latest_res.activity = activity_type;
    }

    if(!alg_steps_is_body_iq_idle())
        sensor_set_es(my, ES_SELF_PREVENT_IDLE);

    return 0;
}

struct stepcounter_property {
    u8 type;
    int value;
}__packed;

static int stepcounter_set_prop(struct sensor *my,
                                    const struct sensor *cli,
                                    const void *prop, u8 prop_size)
{

    const struct stepcounter_property *prop_set = prop;

    if (unlikely(prop_size != sizeof(*prop_set)))
        return E_PARAM;

    switch(prop_set->type){
    case 1:
        //enable/disable step counter
        if(prop_set->value == 0)
            alg_steps_disable();
        else
            alg_steps_enable();
        break;
    case 2:
        //reset steps counter
        alg_steps_reset();
        break;
    default:
        break;
    }
    return 0;
}

static struct sensor_funcs stepcounter_funcs={
	 .create=stepcounter_create,
	 .cfg_stream=stepcounter_cfg_stream,
	 .exec=stepcounter_exec,
	 .set_property  = &stepcounter_set_prop,
     .goto_idle = stepcounter_goto_idle,
};
define_sensor(stepcounter,struct stepcounter_result,&stepcounter_funcs,NULL);
