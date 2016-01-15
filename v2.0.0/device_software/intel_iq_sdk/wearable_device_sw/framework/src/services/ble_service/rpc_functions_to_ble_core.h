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

#ifndef RPC_FUNCTIONS_TO_BLE_CORE_H_
#define RPC_FUNCTIONS_TO_BLE_CORE_H_

#include "gap_internal.h"
#include "gatt_internal.h"

/* declare the list of functions sorted by signature */
#define LIST_FN_SIG_NONE

#define LIST_FN_SIG_S                                                                   \
        FN_SIG_S(ble_gap_start_advertise_req, struct ble_gap_adv_params *)

#define LIST_FN_SIG_P                                                                   \
        FN_SIG_P(ble_get_version_req, void *)                     \
        FN_SIG_P(ble_gap_dtm_init_req, void *)                         \
        FN_SIG_P(ble_gap_read_bda_req, void *)                                          \
        FN_SIG_P(ble_gap_stop_advertise_req, void *)                                    \
        FN_SIG_P(ble_gap_clr_white_list_req, void *)                                    \
        FN_SIG_P(ble_gap_stop_scan_req, void *)                                         \
        FN_SIG_P(ble_gap_sm_clear_bonds_req, void *)                          \
        FN_SIG_P(ble_gap_cancel_connect_req, void *)       \
        FN_SIG_P(ble_get_bonded_device_list_req, void *)

#define LIST_FN_SIG_S_B                                                                                         \
        FN_SIG_S_B(ble_gatt_register_req, const struct ble_gatt_register *, uint8_t *, uint16_t)                \
        FN_SIG_S_B(ble_gatt_send_notif_req, const struct ble_gatt_send_notif_ind_params *, uint8_t *, uint16_t) \
        FN_SIG_S_B(ble_gatt_send_ind_req, const struct ble_gatt_send_notif_ind_params *, uint8_t *, uint8_t)

#define LIST_FN_SIG_B_B_P                                                                     \
        FN_SIG_B_B_P(ble_gap_wr_white_list_req, bt_addr_le_t *, uint8_t, struct ble_gap_irk_info *, uint8_t, void * )

#define LIST_FN_SIG_S_P                                                                                    \
        FN_SIG_S_P(ble_gap_disconnect_req, const struct ble_gap_disconnect_req_params *, void *) \
        FN_SIG_S_P(ble_gap_sm_pairing_req, const struct ble_core_gap_sm_pairing_req_params *, void *)           \
        FN_SIG_S_P(ble_gap_sm_config_req, const struct ble_core_gap_sm_config_params *, void *)\
        FN_SIG_S_P(ble_gap_sm_passkey_reply_req, const struct ble_gap_sm_key_reply_req_params *, void *) \
        FN_SIG_S_P(ble_gap_connect_req, const struct ble_gap_connect_req_params *, void *)                  \
        FN_SIG_S_P(ble_gap_start_scan_req, const struct ble_gap_scan_params *, void *)                     \
        FN_SIG_S_P(ble_gap_generic_cmd_req, const struct ble_gap_gen_cmd_params *, void *)                 \
        FN_SIG_S_P(ble_gap_set_option_req, const struct ble_gap_set_option_params *, void *)               \
        FN_SIG_S_P(ble_gap_conn_update_req, const struct ble_gap_connect_update_params *, void *)   \
        FN_SIG_S_P(ble_gap_service_read_req, const struct ble_gap_service_read_params *, void *)                         \
        FN_SIG_S_P(ble_set_enable_config_req, const struct ble_wr_config_params *, void *)      \
        FN_SIG_S_P(ble_gap_set_rssi_report_req, const struct ble_core_rssi_report_params *, void *)                               \
        FN_SIG_S_P(ble_gap_service_write_req, const struct ble_gap_service_write_params *, void *)         \
        FN_SIG_S_P(ble_gap_dbg_req, const struct debug_params *, void *)                                 \
        FN_SIG_S_P(ble_gattc_discover_req, const struct ble_core_discover_params *, void *)                  \
        FN_SIG_S_P(ble_gattc_read_req,  const struct ble_gattc_read_params *, void *)     \
        FN_SIG_S_P(ble_gatts_send_svc_changed_req, const struct ble_gatts_svc_changed_params *, void * )     \
        FN_SIG_S_P(ble_gatts_get_attribute_value_req, const struct ble_gatts_get_attribute_params *, void * )

#define LIST_FN_SIG_S_B_P                                                                                                                   \
        FN_SIG_S_B_P(ble_gatts_set_attribute_value_req, const struct ble_gatts_set_attribute_params *, uint8_t *, uint8_t, void *)       \
        FN_SIG_S_B_P(ble_gattc_write_req, const struct  ble_gattc_write_params *, const uint8_t *, uint8_t, void *)

#define LIST_FN_SIG_S_B_B_P

#endif /* RPC_FUNCTIONS_TO_BLE_CORE_H_ */
