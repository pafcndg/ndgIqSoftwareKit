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

#ifndef RPC_FUNCTIONS_TO_QUARK_H_
#define RPC_FUNCTIONS_TO_QUARK_H_

#include "gap_internal.h"
#include "gatt_internal.h"
#include "gap_internal.h"

/* declare the list of functions sorted by signature */
#define LIST_FN_SIG_NONE \
        FN_SIG_NONE(on_nble_up)

#define LIST_FN_SIG_S                                                                              \
        FN_SIG_S(on_ble_get_version_rsp, const struct ble_version_response *)                      \
        FN_SIG_S(on_ble_gap_connect_evt, const struct ble_gap_connect_evt *)                       \
        FN_SIG_S(on_ble_gap_disconnect_evt, const struct ble_gap_disconnect_evt *)                 \
        FN_SIG_S(on_ble_gap_conn_update_evt, const struct ble_gap_conn_update_evt *)               \
        FN_SIG_S(on_ble_gap_sm_status_evt, const struct ble_gap_sm_status_evt *)                   \
        FN_SIG_S(on_ble_gap_sm_passkey_display_evt, const struct ble_gap_sm_passkey_disp_evt *)    \
        FN_SIG_S(on_ble_gap_sm_passkey_req_evt, const struct ble_gap_sm_passkey_req_evt *)         \
        FN_SIG_S(on_ble_gap_to_evt, const struct ble_gap_timout_evt *)                             \
        FN_SIG_S(on_ble_gap_rssi_evt, const struct ble_gap_rssi_evt *)                             \
        FN_SIG_S(on_ble_gap_service_read_rsp, const struct ble_core_response *)                         \
        FN_SIG_S(on_ble_gap_read_bda_rsp, const struct ble_service_read_bda_response *)            \
        FN_SIG_S(on_ble_gap_disconnect_rsp, const struct ble_core_response *)                           \
        FN_SIG_S(on_ble_gap_sm_pairing_rsp, const struct ble_core_response *)                           \
        FN_SIG_S(on_ble_gap_sm_config_rsp, struct ble_gap_sm_config_rsp *)                         \
        FN_SIG_S(on_ble_gap_clr_white_list_rsp, const struct ble_core_response *)                       \
        FN_SIG_S(on_ble_gap_sm_passkey_reply_rsp, const struct ble_core_response *)                     \
        FN_SIG_S(on_ble_gap_connect_rsp, const struct ble_core_response *)                              \
        FN_SIG_S(on_ble_gap_start_scan_rsp, const struct ble_core_response *)                           \
        FN_SIG_S(on_ble_gap_stop_scan_rsp, const struct ble_core_response *)                            \
        FN_SIG_S(on_ble_gap_cancel_connect_rsp, const struct ble_core_response *)                       \
        FN_SIG_S(on_ble_gap_set_option_rsp, const struct ble_core_response *)                           \
        FN_SIG_S(on_ble_gap_generic_cmd_rsp, const struct ble_core_response *)                          \
        FN_SIG_S(on_ble_gap_conn_update_rsp, const struct ble_core_response *)                          \
        FN_SIG_S(on_ble_gap_sm_clear_bonds_rsp, const struct ble_core_response *)                       \
        FN_SIG_S(on_ble_gap_service_write_rsp, const struct ble_service_write_response *)          \
        FN_SIG_S(on_ble_set_enable_config_rsp, const struct ble_core_response *)                        \
        FN_SIG_S(on_ble_gap_set_rssi_report_rsp, const struct ble_core_response *)                      \
        FN_SIG_S(on_ble_gap_wr_white_list_rsp, const struct ble_core_response *)                        \
        FN_SIG_S(on_ble_gap_dbg_rsp, const struct debug_response *)                                \
        FN_SIG_S(on_ble_gatts_send_svc_changed_rsp, const struct ble_core_response *)                   \
        FN_SIG_S(on_ble_gatts_set_attribute_value_rsp, const struct ble_gatts_attribute_response *)\
        FN_SIG_S(on_ble_gatts_send_notif_ind_rsp, const struct ble_gatt_notif_ind_rsp *)           \
        FN_SIG_S(on_ble_get_bonded_device_list_rsp, const struct ble_get_bonded_device_list_rsp*)  \
        FN_SIG_S(on_ble_gap_start_advertise_rsp, const struct ble_core_response *)                      \
        FN_SIG_S(on_ble_gap_stop_advertise_rsp, const struct ble_core_response *)

#define LIST_FN_SIG_P                                                                             \
        FN_SIG_P(on_ble_gap_dtm_init_rsp, void *)

#define LIST_FN_SIG_S_B                                                                           \
        FN_SIG_S_B(ble_log, const struct ble_log_s *,                                             \
                char *, uint8_t)                                                                  \
        FN_SIG_S_B(on_ble_gattc_value_evt, const struct ble_gattc_value_evt *,                    \
                uint8_t *, uint8_t)                                                               \
        FN_SIG_S_B(on_ble_gatts_write_evt, const struct ble_gatt_wr_evt *, const uint8_t *, uint8_t) \
        FN_SIG_S_B(on_ble_gatts_get_attribute_value_rsp, const struct ble_gatts_attribute_response *,\
                   uint8_t *, uint8_t)                                                               \
        FN_SIG_S_B(on_ble_gatt_register_rsp, const struct ble_gatt_register *, const struct ble_gatt_attr_idx_entry *, uint8_t)   \
        FN_SIG_S_B(on_ble_gattc_discover_rsp, const struct ble_gattc_disc_rsp *, const uint8_t *, uint8_t)  \

#define LIST_FN_SIG_B_B_P

#define LIST_FN_SIG_S_P                                                                           \
        FN_SIG_S_P(on_ble_gattc_write_rsp, const struct ble_gattc_write_rsp *, void*)

#define LIST_FN_SIG_S_B_P                                                                                  \
        FN_SIG_S_B_P(on_ble_gattc_read_rsp, const struct ble_gattc_read_rsp *, uint8_t *, uint8_t, void*)  \

#define LIST_FN_SIG_S_B_B_P

#endif /* RPC_FUNCTIONS_TO_QUARK_H_ */
