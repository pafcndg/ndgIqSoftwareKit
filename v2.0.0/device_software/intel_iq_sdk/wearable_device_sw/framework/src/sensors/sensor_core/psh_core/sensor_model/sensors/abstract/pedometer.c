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

static int pedometer_create(struct sensor *psensor){
    psh_info2(0,"is ok");
    return 0;
}
static int pedometer_cfg_stream(struct sensor *my,u16 *dat){
    psh_info2(0,"is ok");
    return 0;
}
static int pedometer_exec(struct sensor *my,const struct sensor *reporter)
{
/*
    struct accel_pvp_data *accel_buf = reporter->data_buf;
    struct accel_pvp_data *ped_buf = my->data_buf;
    memcpy(ped_buf,accel_buf,sizeof(*accel_buf));
    Spotting_FV_Rec_RT(accel_buf->ax, accel_buf->ay, accel_buf->az);
    sensor_mark_data_rdy(my);
    */
    return 0;
}
static struct sensor_funcs pedometer_funcs={
     .create=pedometer_create,
     .cfg_stream=pedometer_cfg_stream,
     .exec=pedometer_exec,
};
define_sensor(pedometer,struct accel_pvp_data,&pedometer_funcs,NULL);
