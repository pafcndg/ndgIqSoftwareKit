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

#ifndef _CMD_EXPOSE_H
#define _CMD_EXPOSE_H

/**
 *sensor_name_str[] is refer to ss_sensor_type_t in the sensor_data_format.h.
 *The changes of this array need to be with the changes of ss_sensor_type_t.
 *Psh_core has no sensor_type,it just use the sensor_name to represent sensors.
 *Pensor_service [sensor_type]<---------------->Psh_core[sensor_name].
 * */
const char *sensor_name_str[] =
{
    NULL,//start of ob_board sensor
    NULL,//start of phy sensor
    ACCEL_PHY_NAME_STR, //id:1
    GYRO_PHY_NAME_STR,
    COMPASS_PHY_NAME_STR,
    NULL,
    NULL,
    NULL,//end of phy sensor,start of abs sensor
    GESTURE_PERSONALIZE,
    TAPPING_ATP,
    SIMPLEGES_ATP,
    STEPCOUNTER_NAME_STR,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,//end of on_board sensor
};

#endif
