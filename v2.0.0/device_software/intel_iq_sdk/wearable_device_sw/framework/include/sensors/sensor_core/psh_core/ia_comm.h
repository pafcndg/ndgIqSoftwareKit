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

#ifndef _CMD_ENGINE_H_
#define _CMD_ENGINE_H_

#include "common.h"
#include "util/compiler.h"

enum cmd_id {
    CMD_READY = 128,
    CMD_PHY_TIMEOUT,
    CMD_SENSOR_DATA,
    CMD_CFG_STREAM,
    CMD_ISR_TRIGGER,
    CMD_INVALID,
};

#define IA_BIT_CFG_AS_WAKEUP_SRC ((u16)(0x1 << 0))
struct sensor_cfg_param {
    u16 sample_freq; /* HZ */
    u16 buff_delay; /* max time(ms) for data bufferring */
    u16 bit_cfg;
}__packed;

#define PSH_DBG_CMD_I2C_OP    ((u8)1)
#define PSH_DBG_CMD_SNR_QUERY ((u8)2)
struct psh_dbg_op {
    u8 cmd;
    char param[0];
}__packed;

#define IA_NOTIFY_SUSPEND ((u8)0x1)
#define IA_NOTIFY_RESUME  ((u8)0x2)
#define IA_NOTIFY_TIMESTAMP_SYNC  ((u8)0x3)
#define IA_NOTIFY_FLUSH_BATCHED   ((u8)0x4)
typedef s64 timestamp_t;
struct cmd_ia_notify_param {
    u8 id;
    char extra[0];
} __packed;

#if defined(CONFIG_TIMESTAMP)
#define SIZE_TIMESTAMP (sizeof(timestamp_t))
#else
#define SIZE_TIMESTAMP (0)
#endif

#define GET_TS(l, c)                            \
    if (SIZE_TIMESTAMP == 8) {                  \
        (l) =                           \
            ((((timestamp_t)(((u8 *)(c))[7])) << 56)    \
            | (((timestamp_t)(((u8 *)(c))[6])) << 48)   \
            | (((timestamp_t)(((u8 *)(c))[5])) << 40)   \
            | (((timestamp_t)(((u8 *)(c))[4])) << 32)   \
            | (((timestamp_t)(((u8 *)(c))[3])) << 24)   \
            | (((timestamp_t)(((u8 *)(c))[2])) << 16)   \
            | (((timestamp_t)(((u8 *)(c))[1])) << 8)    \
            | ((timestamp_t)(((u8 *)(c))[0])));     \
    } else if (SIZE_TIMESTAMP == 4) {               \
        (l) =                           \
            ((((timestamp_t)(((u8 *)(c))[3])) << 24)    \
            | (((timestamp_t)(((u8 *)(c))[2])) << 16)   \
            | (((timestamp_t)(((u8 *)(c))[1])) << 8)    \
            | ((timestamp_t)(((u8 *)(c))[0])));     \
    } else {                            \
        ASSERT(0);                      \
    }
#define PUT_TS(c, l)                            \
    if (SIZE_TIMESTAMP == 8) {                  \
        ((u8 *)(c))[7] = ((((timestamp_t)(l)) >> 56) & 0xff);   \
        ((u8 *)(c))[6] = ((((timestamp_t)(l)) >> 48) & 0xff);   \
        ((u8 *)(c))[5] = ((((timestamp_t)(l)) >> 40) & 0xff);   \
        ((u8 *)(c))[4] = ((((timestamp_t)(l)) >> 32) & 0xff);   \
        ((u8 *)(c))[3] = ((((timestamp_t)(l)) >> 24) & 0xff);   \
        ((u8 *)(c))[2] = ((((timestamp_t)(l)) >> 16) & 0xff);   \
        ((u8 *)(c))[1] = ((((timestamp_t)(l)) >> 8) & 0xff);    \
        ((u8 *)(c))[0] = (((timestamp_t)(l)) & 0xff);       \
    } else if (SIZE_TIMESTAMP == 4) {               \
        ((u8 *)(c))[3] = ((((timestamp_t)(l)) >> 24) & 0xff);   \
        ((u8 *)(c))[2] = ((((timestamp_t)(l)) >> 16) & 0xff);   \
        ((u8 *)(c))[1] = ((((timestamp_t)(l)) >> 8) & 0xff);    \
        ((u8 *)(c))[0] = (((timestamp_t)(l)) & 0xff);       \
    } else {                            \
        ASSERT(0);                      \
    }

#define SCMD_DEBUG_SET_MASK ((u16)0x1)
#define SCMD_DEBUG_GET_MASK ((u16)0x2)
struct cmd_debug_param {
    u16 sub_cmd;
    u16 mask_out;
    u16 mask_level;
} __packed;

/* bit0=0 means running only */
#define SNR_GROUP_RUN_ONLY_MASK ((bitmask_t)0x1)
#define SNR_GROUP_ALL           ((bitmask_t)-1)
#define SNR_GROUP_ALL_RUNNING   (SNR_GROUP_ALL \
            & ~SNR_GROUP_RUN_ONLY_MASK)
struct get_status_param {
    bitmask_t snr_group;
} __packed;

struct set_property_param {
    u8 prop_size;
    char prop[0];
} __packed;

#define GET_PROP_MAX_SIZE (1024)
struct get_property_param {
    u8 param_size;
    char param[0];
} __packed;

#define SCMD_GET_COUNTER ((u16)0x1)
#define SCMD_CLEAR_COUNTER ((u16)0x2)
struct cmd_counter_param {
    u16 sub_cmd;
} __packed;

#define MAX_CALDATA_SIZE    ((u8)128)
struct cal_param_info {
    u8 size;
    char data[0];
} __packed;

#define CMD_CALIBRATION_SET ((u8)0x1)
#define CMD_CALIBRATION_GET ((u8)0x2)
#define CMD_CALIBRATION_START   ((u8)0x3)
#define CMD_CALIBRATION_STOP    ((u8)0x4)
#define CMD_CALIBRATION_COMPENSATION ((u8)0x5)

struct calibration_param {
    u8 sub_cmd;
    struct cal_param_info info;
} __packed;

#define CALIB_RESULT_NONE   ((u8)0)
#define CALIB_RESULT_DONE   ((u8)100)
#define CALIB_RESULT_RAW    ((u8)200)
struct resp_calibration {
    u8 calib_result;
    struct cal_param_info info;
} __packed;

struct sub_event {
    u8 sensor_id;
    u8 chan_id;
    u8 opt_id;
    u32 param1;
    u32 param2;
} __packed;

struct cmd_event_param {
    u8 num; /*sub event numbers */
    u8 op;  /* relationship AND/OR */
    struct sub_event evts[1]; /* can be N sub events */
} __packed;
#define BUF_IA_DDR_SIZE ((u16)4096 * 2)

enum resp_type {
    RESP_CMD_ACK,
    RESP_GET_TIME,
    RESP_GET_SINGLE,
    RESP_STREAMING,
    RESP_DEBUG_MSG,
    RESP_DEBUG_GET_MASK = 5,
    RESP_CAL_RESULT,
    RESP_BIST_RESULT,
    RESP_ADD_EVENT,
    RESP_CLEAR_EVENT,
    RESP_EVENT = 10,
    RESP_GET_STATUS,
    RESP_RESERVED_0, //reserved, formerly used for RESP_COMP_CAL_RESULT,
    RESP_COUNTER,
    RESP_GET_VERSION,
    RESP_TRACE_MSG = 15,
    RESP_PSH_EVT,
};

struct cmd_resp {
    u8 tran_id;
    u8 type;
    u8 sensor_id;
    u8 align_data;
    u16 data_len;
    char buf[0];
} __packed;
#define RESP_SIZE(resp) (sizeof(struct cmd_resp) \
        + resp->data_len)
#define RESP_ZMALLOC(data_len, flag) ZMALLOC(data_len \
        + sizeof(struct cmd_resp), flag)

#define PSH_EVT_ID_FLUSH_DONE ((u8)1)
struct resp_psh_evt {
    u8 evt_id;
    u8 evt_buf_len;
    char evt_buf[0];
} __packed;
#define PSH_EVT_LEN(psh_evt) (sizeof(struct resp_psh_evt) \
        + psh_evt->evt_buf_len)

struct resp_cmd_ack {
    u8 cmd_id;
    int ret;
    char extra[0];
} __packed;

#define LINK_AS_CLIENT      (0)
#define LINK_AS_MONITOR     (1)
#define LINK_AS_REPORTER    (2)
struct link_info {
    u8 sid;
    u8 ltype;
    u16 rpt_freq;
} __packed;

struct snr_info {
    u8 sid;
    u8 status;
    u16 freq;
    u16 data_cnt;
    u16 bit_cfg;
    u16 priv;
    u16 attri;

    u16 freq_max;
    char name[SNR_NAME_MAX_LEN];

    u8 health;
    u8 link_num;
    struct link_info linfo[0];
} __packed;
#define SNR_INFO_SIZE(sinfo) (sizeof(struct snr_info) \
        + sinfo->link_num * sizeof(struct link_info))
#define SNR_INFO_MAX_SIZE 256

struct resp_debug_get_mask {
    u16 mask_out;
    u16 mask_level;
} __packed;

struct resp_counter {
    u32 gpio_counter;
    u32 dma_counter;
    u32 i2c_counter;
    u32 print_counter;
} __packed;

#define VERSION_STR_MAX_SIZE ((u16)256)
struct resp_version {
    u8 str_len;
    char str[0];
} __packed;
#define RESP_VERSION_SIZE(resp_vers) (sizeof(struct resp_version) \
        + resp_vers->str_len + 1)

/* for sensor_event */
struct clear_evt_param {
    u8 evt_id;
} __packed;

struct resp_evt {
    u8 evt_id;
} __packed;

int resp_upload_data(struct cmd_resp *resp);
int resp_direct_sensor_data(void *data,uint32_t data_len,uint8_t sensor_id);

/* upload only when 1/ratio of loop buffer is still left,
 * this is used for data with lower priority or data
 * could be uploaded later
 */
int resp_upload_data_with_check(struct cmd_resp *resp,
        int ratio);
int resp_upload_rdy(void);
void psh_lbuf_flush(void);
#endif
