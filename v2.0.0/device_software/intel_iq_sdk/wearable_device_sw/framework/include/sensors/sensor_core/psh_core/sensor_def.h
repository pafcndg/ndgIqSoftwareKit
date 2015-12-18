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

#ifndef _SENSOR_DEF_H
#define _SENSOR_DEF_H
/*
   This file defines configure package and output data format
   of any type of sensor
*/
#include "common.h"
#include "util/compiler.h"

#define BIST_NAME_STR "BIST"
#define ACCEL_PHY_NAME_STR "ACCEL"
#define TERMINAL_PHY_NAME_STR "TCPHY"
#define COMPASS_PHY_NAME_STR "COMPS"
#define GYRO_PHY_NAME_STR "GYRO"
#define BARO_PHY_NAME_STR "BARO"
#define NDOF_NAME_STR "9DOF"
#define PEDOMETER_NAME_STR "PEDOM"
#define STEPCOUNTER_NAME_STR "SCOUN"
#define PHY_ACTIVITY_NAME_STR "PHYAC"
#define MOVE_DETECT_NAME_STR "MOVDT"
#define SIGNIFICANT_MOTION_NAME_STR "SIGMT"
#define SIMPLE_TAPPING_NAME_STR "STAP"
#define GESTURE_PERSONALIZE "GSUD"
#define TAPPING_ATP "TAPA"
#define SIMPLEGES_ATP "SGESA"

enum sensor_type {
    PHY_SENSOR_SID_BASE = 0,
#ifdef CONFIG_SENSORS_NUM_EXPAND
    PHY_SENSOR_SID_TOP = 7,
#else
    PHY_SENSOR_SID_TOP = 6,
#endif

    ABS_SENSOR_SID_BASE = 100,

    PORT_SENSOR_SID_BASE = 200,
    CS_PORT,    /* port for streaming configuration and uploading */
#ifdef CONFIG_PSH_CORE_GS_PORT
    GS_PORT,    /* port for get_single configuration and uploading */
#endif
    EVT_PORT,   /* port for event configuration and delivery */
    PORT_SENSOR_SID_TOP,

    STUB_FOR_VTHREAD_NONE = 255,
};
#define PSH_ITSELF      (PHY_SENSOR_SID_BASE) /* means PSH itself */
#define PHY_SENSOR_NUM  (PHY_SENSOR_SID_TOP - PHY_SENSOR_SID_BASE - 1)
#define PORT_SENSOR_NUM (PORT_SENSOR_SID_TOP - PORT_SENSOR_SID_BASE - 1)
#ifdef CONFIG_SENSORS_NUM_EXPAND
#define ABS_SENSOR_NUM  (sizeof(bitmask_t) * 3 - PHY_SENSOR_NUM - PORT_SENSOR_NUM - 1)
#else
#define ABS_SENSOR_NUM  (sizeof(bitmask_t) * 2- PHY_SENSOR_NUM - PORT_SENSOR_NUM - 2)
#endif
#define ABS_SENSOR_SID_TOP  (ABS_SENSOR_SID_BASE + ABS_SENSOR_NUM + 1)

#define ABS_SENSOR_INDEX(x) ((x > ABS_SENSOR_SID_BASE && x < ABS_SENSOR_SID_TOP) \
        ? (x - ABS_SENSOR_SID_BASE - 1) : 0)
#define PHY_SENSOR_INDEX(x)    ((x > PHY_SENSOR_SID_BASE && x < PHY_SENSOR_SID_TOP) \
        ? (x - PHY_SENSOR_SID_BASE - 1) : 0)
#define PORT_SENSOR_INDEX(x) ((x > PORT_SENSOR_SID_BASE && x < PORT_SENSOR_SID_TOP) \
        ? (x - PORT_SENSOR_SID_BASE - 1) : 0)

/*Total number of ports*/
#define SENSOR_PORTS_NUM (PORT_SENSOR_NUM * MAX_NUM_HOSTS)

#define GS_DATA_MAX_SIZE (300*6*2)


static inline int is_phy_sensor(enum sensor_type type)
{
    return ((type > PHY_SENSOR_SID_BASE) && (type < PHY_SENSOR_SID_TOP));
}

static inline int is_abs_sensor(enum sensor_type type)
{
    return ((type > ABS_SENSOR_SID_BASE) && (type < ABS_SENSOR_SID_TOP));
}

static inline int is_port_sensor(enum sensor_type type)
{
    return ((type > PORT_SENSOR_SID_BASE) && (type < PORT_SENSOR_SID_TOP));
}

#define GS_DATA_MAX_SIZE (300*6*2)

#define FREQ_ASYNC          ((u16)(-1))
#define FREQ_FORWARD_RPT    ((u16)(-2)) /* bypass freq reqest to reporter */

/* Physical sensor data defs */
struct accel_pvp_data {
    short ax;
    short ay;
    short az;
}__packed;

struct compass_phy_data {
    s16 cx;
    s16 cy;
    s16 cz;
}__packed;

struct baro_phy_data {
    s32 p;
}__packed;

struct als_phy_data {
    u16 lux;
}__packed;
#define ALS_PHY_DATA_INVALID ((u8)(-1))

struct ps_phy_data {
    u16 near;
}__packed;
#define PS_PHY_DATA_INVALID ((u8)(-1))

/* Abstract sensor cfg/data defs */
struct comp_ag_data {
    s16 ag_x;
    s16 ag_y;
    s16 ag_z;
}__packed;

struct ndof_data {
    s32 m[9];
}__packed;

struct ndofag_data {
    s32 m[9];
}__packed;

struct ndofam_data {
    s32 m[9];
}__packed;
#define ORIENT_UNKNOWN      0
#define LANDSCAPE_UP        1
#define LANDSCAPE_DOWN      2
#define PORTRAIT_UP         3
#define PORTRAIT_DOWN       4

#define FACE_UNKNOWN        0
#define FACE_UP             1
#define FACE_DOWN           2

#define TC_VAL_NUM          5

struct tc_data {
    s16 orientation;
    s16 face;
}__packed;

struct gs_data {
    u16 size;
    s16 sample[GS_DATA_MAX_SIZE / 2];
}__packed;

struct gspx_data {
    s16 proximity;
    u16 size;
    s16 sample[GS_DATA_MAX_SIZE / 2];
}__packed;

#define ACT_MAX_REPORT 64

struct phy_activity_data {
    s16 len;
    s16 values[ACT_MAX_REPORT];
}__packed;

#define BIST_RET_OK     ((u8)0)
#define BIST_RET_ERR_SAMPLE ((u8)1) /* couldn't sample properly */
#define BIST_RET_ERR_HW     ((u8)2) /* sesnor HW is wrong */
#define BIST_RET_NOSUPPORT  ((u8)3) /* no such sesnor HW on board */
struct bist_data {
    u8 result[PHY_SENSOR_NUM + 1];
}__packed;

struct gesture_flick_data {
    s16 flick;
}__packed;

struct gesture_eartouch_data {
    s16 eartouch;
}__packed;

struct shaking_data {
    s16 shaking;
}__packed;

struct directional_shaking_data {
    s16 shaking;
}__packed;

struct gesture_tilt_data {
    s16 tilt;
}__packed;

struct gesture_snap_data {
    s16 snap;
}__packed;

struct jump_instant_activity {
        s32 typeclass;
}__packed;

struct instant_activity_data {
        s32 typeclass;
}__packed;

struct wpd_pedo_data {
        s32 internal_steps;
        s32 internal_status;
        s32 internal_typeclass;
}__packed;

struct step_counter {
    s32 steps;
}__packed;

struct step_detector {
    s32 detector;
}__packed;

struct step_turn {
    s32 turn;
}__packed;

struct stap_data {
    s16 stap;
}__packed;

struct dtwgs_data {
    s16 gsnum;
    s32 score;
}__packed;

struct device_position_data {
    s16 pos;
}__packed;

struct lift_data {
    s16 look;
    s16 vertical;
}__packed;

struct gravity_data {
    s32 x;
    s32 y;
    s32 z;
}__packed;

struct orientation_data {
    s32 azimuth;
    s32 pitch;
    s32 roll;
}__packed;

struct linear_accel_data {
    s32 x;
    s32 y;
    s32 z;
}__packed;

struct rotation_vector_data {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
}__packed;

struct game_rotation_vector_data {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
}__packed;

struct geomagnetic_rotation_vector_data {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
}__packed;

struct pedometer_data {
    s32 steps;
    s16 mode;
}__packed;

#define MD_STATE_UNKNOWN 0
#define MD_STATE_MOVE 1
#define MD_STATE_SLIGHT 2
#define MD_STATE_STILL 3

struct md_data {
    s16 state;
}__packed;

struct sm_data {
    s16 state;
}__packed;

struct pz_data {
    s16 deltX;
    s16 deltY;      /* pz deltX and deltY: 0.01deg/s */
}__packed;

struct mag_heading_data {
    s32 heading;
}__packed;

struct comp_cal_data {
}__packed;

struct gyro_cal_data {
}__packed;

struct lpe_phy_data {
    u32 lpe_msg;
}__packed;

struct dual_panel_mode_data {
    u16 mode;
    u16 angle;
} __packed;

#define PDR_MAX_REPORT 64

struct pdr_sample {
    s32 x;      /* position x, unit is cm */
    s32 y;      /* position y, unit is cm */
    s32 fl; /* floor level, unit is 1 */
    s32 heading;    /* heading angle, unit is 0.01 deg */
    s32 step;   /* step counts in PDR, unit is 1 step */
    s32 distance;   /* total PDR distance, unit is cm */
    s32 confidence;/* heading confidence, the smaller the better */
} __packed;

struct pdr_data {
    s16 size;
    struct pdr_sample sample[PDR_MAX_REPORT];
} __packed;

struct pickup_data {
    s16 pickup;
} __packed;

struct tilt_detector_data {
    s16 tiltd;
} __packed;

/* sensor property settings */
struct bist_property {
    u8 tran_id;
    u8 cmd_id;
}__packed;

struct comp_ag_property {
    u8 type;
    u32 freq_base;
}__packed;

#define ACCEL_IDLE_WITH_FIFO_ON  ((u8)0x1 << 0)
#define ACCEL_IDLE_WITH_FIFO_OFF ((u8)0x1 << 1)
struct accel_phy_property {
    u8 type;
}__packed;

struct cs_port_property {
    u8 type;
    u16 buff_delay; /* ms */
    u16 ia_bit_cfg;
}__packed;

struct lpe_phy_property {
    u8 type;
    u32 ms_delay;
}__packed;

#define BUFF_DELAY_MAX         ((u16)-1)
#define BUFF_DELAY_FORWARD_RPT ((u16)-2)

#define OMS_RSI_ASYNC ((u32)-1)
struct rpt_info {
    u16 cfg_freq;
    u16 bit_cfg;
    u16 buff_delay; /* ms */

    /* only internal used by sensor core */
    /* Reporter's Sample Interval */
    u32 oms_rsi;
    /* Reporter's Next Sample Time Point */
    u32 oms_nstp;
    /* data cnt left for buff dealy */
    u32 buff_cnt;
/* #define RPT_FREQ_DEBUG 1 */
#if defined(CONFIG_RPT_FREQ_DEBUG)
    u32 data_cnt;
    u32 oms_start;
    u32 oms_prev;
    u32 nsi_max;
    u32 nsi_min;
    u32 to_time;
#endif

    u8 sid;
    char sname[SNR_NAME_MAX_LEN];
}__packed;

struct sensor_funcs;
struct sensor_evt_priv;
struct snr_create_param {
    u16 data_buf_size;
    u16 attri;
    u16 freq_max;
    struct sensor_funcs *funcs;
    struct sensor_evt_priv *evt_priv;
    char name[SNR_NAME_MAX_LEN];
    u8 rpt_num;
    struct rpt_info *rpt_list;
}__packed;

#endif
