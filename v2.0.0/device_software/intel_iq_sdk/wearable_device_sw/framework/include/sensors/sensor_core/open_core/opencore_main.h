/****************************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************************/

#include <nanokernel.h>
#include "opencore_common.h"

#define STACKSIZE 900
#define COMMIT_DATA_MAX_LENGTH 60
#define VALID_FRAME_LENGTH      6
#define RAW_FRAME_LENGTH        7

typedef enum{
	ALGO_OPEN = 0,
	ALGO_CLOSE,
	ALGO_SUBSCRIBE,
	ALGO_UNSUBSCRIBE,
	ALGO_GET_PROPERTY,
	ALGO_SET_PROPERTY,
	ALGO_CALIBRATION_START,
	ALGO_CALIBRATION_PROCESS,
	ALGO_CALIBRATION_STOP,
	ALGO_CALIBRATION_SET,
	ALGO_CTL_TOP,
}core_sensor_ctl_t;

extern struct pm_wakelock opencore_main_wl;
extern struct pm_wakelock opencore_cali_wl;

extern list_head_t feed_list;

extern list_head_t exposed_sensor_list;

extern list_head_t phy_sensor_list_int;
extern list_head_t phy_sensor_list_poll;
extern list_head_t phy_sensor_poll_active_list;
extern list_head_t phy_sensor_poll_active_array[PHY_TYPE_KEY_LENGTH * PHY_ID_KEY_LENGTH];

extern int motion_detect_callback(struct sensor_data* sensor_data, void* priv_data);
void RefleshSensorCore(void);
void SensorCoreInit(void);
