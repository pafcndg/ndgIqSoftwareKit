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

static const char __option_variant_name[] __pshoption_string = "variant";
static const char __option_variant_string[] __pshoption_string = "zephyr";
static const struct option __option_variant __pshoption = {.name = __option_variant_name, .type = OPTION_TYPE_STRING, .value = (long)__option_variant_string, };

static const char __option_TRACE_ON_name[] __pshoption_string = "TRACE_ON";
static const struct option __option_TRACE_ON __pshoption = {.name = __option_TRACE_ON_name, .type = OPTION_TYPE_BOOL, .value = (long)1,};

static const char __option_VERBOSE_ON_name[] __pshoption_string = "VERBOSE_ON";
static const struct option __option_VERBOSE_ON __pshoption = {.name = __option_VERBOSE_ON_name, .type = OPTION_TYPE_BOOL, .value = (long)1,};

static struct phy_driver __phydrv_accel_phy_0 __pshoption_phydrv = { .ops_name = "accel_phy",  .sname = ACCEL_PHY_NAME_STR,  .freq_max = 200,  .fifo_len = 32,  .bit_cfg_mask = ACCEL_BIT_CFG_FS_8G | ACCEL_BIT_CFG_FS_16G | SNR_BIT_CFG_FIFO_ON, };
define_drvinit(__phydrv_accel_phy_0);

static struct phy_driver __phydrv_gyro_phy_1 __pshoption_phydrv = { .ops_name = "gyro_phy",  .sname = GYRO_PHY_NAME_STR,  .freq_max = 200,  .bit_cfg_mask = ACCEL_BIT_CFG_FS_8G | ACCEL_BIT_CFG_FS_16G | SNR_BIT_CFG_FIFO_ON, };
define_drvinit(__phydrv_gyro_phy_1);

static struct phy_driver __phydrv_motion_det_2 __pshoption_phydrv = { .ops_name = "motion_det",  .sname = MOVE_DETECT_NAME_STR,  .freq_max = FREQ_ASYNC, };
define_drvinit(__phydrv_motion_det_2);

static struct rpt_info __option_rpt_list_accel_phy_dev_0[] = {};

static const char __option_name_accel_phy_dev_0[] __pshoption_string = "accel_phy_dev";
static const struct snr_create_param __option_snr_param_accel_phy_dev_0 __pshoption_snr = { .name = ACCEL_PHY_NAME_STR,  .attri = SENSOR_ATTRI_IS_PHYSICAL,  .freq_max = 100,  .rpt_list = __option_rpt_list_accel_phy_dev_0,  .rpt_num = sizeof(__option_rpt_list_accel_phy_dev_0)/sizeof(__option_rpt_list_accel_phy_dev_0[0]), };
static const struct option __option_accel_phy_dev_0 __pshoption = {.name = __option_name_accel_phy_dev_0, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_accel_phy_dev_0, };

static struct rpt_info __option_rpt_list_gyro_phy_dev_1[] = {};

static const char __option_name_gyro_phy_dev_1[] __pshoption_string = "gyro_phy_dev";
static const struct snr_create_param __option_snr_param_gyro_phy_dev_1 __pshoption_snr = { .name = GYRO_PHY_NAME_STR,  .attri = SENSOR_ATTRI_IS_PHYSICAL,  .freq_max = 100,  .rpt_list = __option_rpt_list_gyro_phy_dev_1,  .rpt_num = sizeof(__option_rpt_list_gyro_phy_dev_1)/sizeof(__option_rpt_list_gyro_phy_dev_1[0]), };
static const struct option __option_gyro_phy_dev_1 __pshoption = {.name = __option_name_gyro_phy_dev_1, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_gyro_phy_dev_1, };

static struct rpt_info __option_rpt_list_move_detect_dev_2[] = {};

static const char __option_name_move_detect_dev_2[] __pshoption_string = "move_detect_dev";
static const struct snr_create_param __option_snr_param_move_detect_dev_2 __pshoption_snr = { .name = MOVE_DETECT_NAME_STR,  .attri = SENSOR_ATTRI_IS_PHYSICAL,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_move_detect_dev_2,  .rpt_num = sizeof(__option_rpt_list_move_detect_dev_2)/sizeof(__option_rpt_list_move_detect_dev_2[0]), };
static const struct option __option_move_detect_dev_2 __pshoption = {.name = __option_name_move_detect_dev_2, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_move_detect_dev_2, };

static struct rpt_info __option_rpt_list_gesturepersonalize_3[] = {{ .sname = ACCEL_PHY_NAME_STR,  .buff_delay = 1000,  .cfg_freq = 100, }, };

static const char __option_name_gesturepersonalize_3[] __pshoption_string = "gesturepersonalize";
static const struct snr_create_param __option_snr_param_gesturepersonalize_3 __pshoption_snr = { .name = GESTURE_PERSONALIZE,  .attri = SENSOR_ATTRI_AUTO_IDLE | SENSOR_ATTRI_ACTIVE_IDLE,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_gesturepersonalize_3,  .rpt_num = sizeof(__option_rpt_list_gesturepersonalize_3)/sizeof(__option_rpt_list_gesturepersonalize_3[0]), };
static const struct option __option_gesturepersonalize_3 __pshoption = {.name = __option_name_gesturepersonalize_3, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_gesturepersonalize_3, };

static struct rpt_info __option_rpt_list_stepcounter_4[] = {{ .sname = ACCEL_PHY_NAME_STR,  .buff_delay = 1000,  .cfg_freq = 33, }, };

static const char __option_name_stepcounter_4[] __pshoption_string = "stepcounter";
static const struct snr_create_param __option_snr_param_stepcounter_4 __pshoption_snr = { .name = STEPCOUNTER_NAME_STR,  .attri = SENSOR_ATTRI_AUTO_IDLE | SENSOR_ATTRI_ACTIVE_IDLE,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_stepcounter_4,  .rpt_num = sizeof(__option_rpt_list_stepcounter_4)/sizeof(__option_rpt_list_stepcounter_4[0]), };
static const struct option __option_stepcounter_4 __pshoption = {.name = __option_name_stepcounter_4, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_stepcounter_4, };

static struct rpt_info __option_rpt_list_activity_5[] = {{ .sname = STEPCOUNTER_NAME_STR,  .buff_delay = 1000,  .cfg_freq = 33, }, };

static const char __option_name_activity_5[] __pshoption_string = "activity";
static const struct snr_create_param __option_snr_param_activity_5 __pshoption_snr = { .name = PHY_ACTIVITY_NAME_STR,  .attri = SENSOR_ATTRI_AUTO_IDLE | SENSOR_ATTRI_ACTIVE_IDLE,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_activity_5,  .rpt_num = sizeof(__option_rpt_list_activity_5)/sizeof(__option_rpt_list_activity_5[0]), };
static const struct option __option_activity_5 __pshoption = {.name = __option_name_activity_5, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_activity_5, };

static struct rpt_info __option_rpt_list_tapping_6[] = {{ .sname = ACCEL_PHY_NAME_STR,  .buff_delay = 1000,  .cfg_freq = 100, }, };

static const char __option_name_tapping_6[] __pshoption_string = "tapping";
static const struct snr_create_param __option_snr_param_tapping_6 __pshoption_snr = { .name = TAPPING_ATP,  .attri = SENSOR_ATTRI_AUTO_IDLE | SENSOR_ATTRI_ACTIVE_IDLE,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_tapping_6,  .rpt_num = sizeof(__option_rpt_list_tapping_6)/sizeof(__option_rpt_list_tapping_6[0]), };
static const struct option __option_tapping_6 __pshoption = {.name = __option_name_tapping_6, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_tapping_6, };

static struct rpt_info __option_rpt_list_simpleges_7[] = {{ .sname = ACCEL_PHY_NAME_STR,  .buff_delay = 1000,  .cfg_freq = 20, }, };

static const char __option_name_simpleges_7[] __pshoption_string = "simpleges";
static const struct snr_create_param __option_snr_param_simpleges_7 __pshoption_snr = { .name = SIMPLEGES_ATP,  .attri = SENSOR_ATTRI_AUTO_IDLE | SENSOR_ATTRI_ACTIVE_IDLE,  .freq_max = FREQ_ASYNC,  .rpt_list = __option_rpt_list_simpleges_7,  .rpt_num = sizeof(__option_rpt_list_simpleges_7)/sizeof(__option_rpt_list_simpleges_7[0]), };
static const struct option __option_simpleges_7 __pshoption = {.name = __option_name_simpleges_7, .type = OPTION_TYPE_SNR, .value = (long)&__option_snr_param_simpleges_7, };
