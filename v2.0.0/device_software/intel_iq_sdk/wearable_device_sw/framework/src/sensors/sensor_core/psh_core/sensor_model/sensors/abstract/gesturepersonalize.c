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
* Author(s) : Zhiqiang Liang(zhiqiang.liang@intel.com)
* Group : NBG/NTG
*/

#include "options.h"
#include "sensor_core.h"
#include "runtime.h"

#include "gesturespotting.h"
#include "recognition.h"
#include "util/compiler.h"

#define ACCEL_INDEX 0
#define GYRO_INDEX  1
#define SENSOR_NR 2
#define SAMPLE_NR 1

#define _RAWDATABUF_LEN_ (300*3)
s16 pFrameData[_RAWDATABUF_LEN_] __section(".dccm") __aligned(4);

static int gesturepersonalize_create(struct sensor *psensor)
{

    psh_log("in gesturepersonalize_create call back function in gesturepersonalize.c.\n");
    spotting_init_top(pFrameData, _RAWDATABUF_LEN_ * 2);
    return 0;
}
static int gesturepersonalize_cfg_stream(struct sensor *my,u16 *dat)
{
    return 0;
}
static int gesturepersonalize_exec(struct sensor *my,const struct sensor *reporter)
{
    s16 data[3];
    int nClassLabel = 0;
    struct gs_personalize * rec_result = my->data_buf;
    static int nFrameIdx = 0;
    int nRetSpotting = 0, ret_rec = 0;;
    short nFrameLenInBytes = 0;

    memcpy(&data[0], reporter->data_buf, sizeof(short)*3);
    //we need to keep this to calibrate some accelerometers on different board:
    // nRetSpotting = GestureSpotting_TopInterface(data[0]+235, data[1]+79, data[2]-170, pFrameData, &nFrameLenInBytes);
    nRetSpotting = GestureSpotting_TopInterface(data[0], data[1], data[2], pFrameData, &nFrameLenInBytes);

    if(nRetSpotting == 1 && nFrameLenInBytes >6*110)
    {
        nFrameIdx++;
        ret_rec = Extraction_Recognition(nFrameLenInBytes/6, pFrameData, &nClassLabel);

        psh_log("Linear SVM==========%d, RF==========%d, PVP RBF==========%d\n",(int)(ret_rec/100), (int)(((int)(ret_rec/10))%10), (int)(ret_rec%10));

        rec_result->size = sizeof(struct gs_personalize);
        rec_result->nClassLabel = (s16)nClassLabel;
        my->data_buf_size = sizeof(struct gs_personalize);

        sensor_mark_data_rdy(my);

        return 0;
    }
    else
    {
        my->data_cnt = 0;
        return 0;
    }
     return 1;
}

static int gesture_goto_idle (struct sensor *my) {
    return 0;
}

static int gesture_out_idle (struct sensor *my) {
    spotting_init_top(pFrameData, _RAWDATABUF_LEN_ * 2);
    return 0;
}

static struct sensor_funcs gesturepersonalize_funcs=
{
     .create=gesturepersonalize_create,
     .cfg_stream=gesturepersonalize_cfg_stream,
     .exec=gesturepersonalize_exec,
     .goto_idle = gesture_goto_idle,
     .out_idle = gesture_out_idle,
};
define_sensor(gesturepersonalize,struct gs_personalize,&gesturepersonalize_funcs,NULL);
