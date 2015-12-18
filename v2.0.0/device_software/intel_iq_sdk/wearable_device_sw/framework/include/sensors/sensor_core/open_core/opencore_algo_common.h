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

#include "stdbool.h"
#include "stddef.h"
#include "sensors/phy_sensor_api/phy_sensor_common.h"
#include "sensors/phy_sensor_api/phy_sensor_api.h"
#include "string.h"
#include "os/os.h"
#include "infra/log.h"
#include "misc/printk.h"
#include "util/list.h"
#include "ipc_ia.h"
#include "ipc_comm.h"
#include "services/sensor_service/sensor_svc_api.h"

#define DIRECT_RAW (1<<1)

#define CMD_SUSPEND_SC    100
#define CMD_RESUME_SC     101
#define CMD_DOUBLE_TAP_SC 102
#define CMD_CALIBRATION_PROCESS_RESULT 103
#define CMD_RAWDATA_REG_INT_SC	104
#define CMD_RAWDATA_FIFO_INT_SC	105

#define DEFAULT_ID       (uint8_t)(~0)
#define DEFAULT_VALUE    (int16_t)(~0)

#define PHY_TYPE_KEY_LENGTH  3
#define PHY_ID_KEY_LENGTH   2

typedef enum{
	BASIC_ALGO_GESTURE = 1,
	BASIC_ALGO_STEPCOUNTER,
	BASIC_ALGO_TAPPING,
	BASIC_ALGO_SIMPLEGES,
	BASIC_ALGO_RAWDATA,
	BASIC_ALGO_DEMO,
}basic_algo_type_t;

typedef struct{
	list_t link;
	ss_sensor_type_t type;
	uint8_t id;
	uint32_t depend_flag;
	uint8_t ready_flag;
	void *rpt_data_buf;
	uint16_t rpt_data_buf_len;
	uint16_t stat_flag;
	uint8_t data_frame_count;
}exposed_sensor_t;

typedef struct{
	ss_sensor_type_t type;
	uint8_t id;

	int32_t range_max;
	int32_t range_min;

	int freq;
	int rt;

	void* delay_buffer;
	uint16_t delay_buffer_repo;
	uint16_t delay_data_count;
	uint16_t put_idx;
	uint16_t get_idx;

	int16_t raw_data_offset;
}sensor_data_demand_t;

struct feed_general_t;
typedef struct{
	int (*exec)(void**, struct feed_general_t*);
	int (*init)(struct feed_general_t*);
	int (*deinit)(struct feed_general_t*);
	int (*reset)(struct feed_general_t*);
	int (*goto_idle)(struct feed_general_t*);
	int (*out_idle)(struct feed_general_t*);
	int (*get_property)(struct feed_general_t*, uint8_t exposed_type,
					uint8_t exposed_id, uint8_t property_type, void* ptr_return);
	int (*set_property)(struct feed_general_t*, uint8_t exposed_type,
					uint8_t exposed_id, uint8_t param_length, void* ptr_param);
}feed_control_api_t;

typedef struct feed_general_t{
	list_t link;
	sensor_data_demand_t* demand;
	int8_t demand_length;
	uint32_t report_flag;
	uint16_t stat_flag;
	uint16_t mark_flag;
	feed_control_api_t ctl_api;
	void *param;
	uint8_t idle_hold_flag;
	uint8_t rf_cnt;
	basic_algo_type_t type;
}feed_general_t;

typedef struct{
	list_t int_link;
	list_t poll_link;
	list_t poll_active_link;
	list_t poll_active_array_link;
	list_t fifo_share_link;

	phy_sensor_type_t type;
	dev_id_t id;
	sensor_t ptr;

	int32_t range_max;
	int32_t range_min;
	uint8_t sensor_data_frame_size;
	uint32_t fifo_share_bitmap;

	uint16_t stat_flag;

	uint8_t attri_mask;
	uint8_t fifo_use_flag;
	uint8_t need_poll;
	uint8_t fifo_share_done_flag;
	uint8_t idle_ref;
	uint16_t fifo_length;

	uint16_t freq;
	uint32_t npp;
	uint32_t pi;
	uint32_t pi_used_balloc;

	void* clb_data_buffer;
	void* feed_data_buffer;
	void* buffer;
	int16_t buffer_length;
	int8_t raw_sensor_data_count;
}sensor_handle_t;

