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

#include "services/ble_service/ble_service_api.h"
#include "services/ble_service/ble_service_gap_api.h"
#include "gatt_int.h"
#include "services/ble_service/ble_service_gattc_api.h"
#include "services/ble_service/ble_service_gatts_api.h"

/* declare the list of functions sorted by signature */
#define LIST_FN_SIG_NONE

#define LIST_FN_SIG_S

#define LIST_FN_SIG_P                                                                   \
        FN_SIG_P(ble_gap_get_version_req, struct ble_version_rsp *)                     \
        FN_SIG_P(ble_gap_dtm_init_req, struct ble_enable_rsp *)                         \
        FN_SIG_P(ble_gap_read_bda_req, void *)                                          \
        FN_SIG_P(ble_gap_stop_advertise_req, void *)                                    \
        FN_SIG_P(ble_gap_clr_white_list_req, void *)                                    \
        FN_SIG_P(ble_gap_stop_scan_req, void *)                                         \
        FN_SIG_P(ble_gap_sm_clear_bonds_req, struct ble_rsp *)

#define LIST_FN_SIG_S_B                                                                   \
        FN_SIG_S_B(ble_gatt_register_req, const struct ble_gatt_register *, uint8_t *, uint16_t) \
        FN_SIG_S_B(ble_gatt_send_notif_req, const struct ble_gatt_send_notif_ind_params * , uint8_t * , uint16_t) \
        FN_SIG_S_B(ble_gatt_send_ind_req, const struct ble_gatt_send_notif_ind_params * , uint8_t * , uint8_t)

#define LIST_FN_SIG_B_B_P                                                                     \
        FN_SIG_B_B_P(ble_gap_wr_adv_data_req, uint8_t *, uint8_t, uint8_t *,                  \
                uint8_t, struct ble_start_advertisement_req_msg *)                            \
        FN_SIG_B_B_P(ble_gap_wr_white_list_req, ble_addr_t * , uint8_t,                       \
                ble_gap_irk_info_t * , uint8_t, void * )                                      \
        FN_SIG_B_B_P(ble_gattc_wr_op, uint8_t *, uint8_t, uint8_t *, uint8_t, void *)

#define LIST_FN_SIG_S_P                                                                                    \
        FN_SIG_S_P(ble_gap_disconnect_req, const struct ble_gap_disconnect_req_params *, struct ble_disconnect_rsp *) \
        FN_SIG_S_P(ble_gap_sm_pairing_req, const struct ble_gap_sm_pairing_req_params *, void *)           \
        FN_SIG_S_P(ble_gap_sm_config_req, const struct ble_gap_sm_config_params *, struct ble_enable_rsp *)\
        FN_SIG_S_P(ble_gap_sm_passkey_reply_req, const struct ble_gap_sm_key_reply_req_params *, struct ble_gap_sm_key_reply_req_msg *) \
        FN_SIG_S_P(ble_gap_connect_req, const struct ble_gap_connection_params *, void *)                  \
        FN_SIG_S_P(ble_gap_start_scan_req, const struct ble_gap_scan_params *, void *)                     \
        FN_SIG_S_P(ble_gap_cancel_connect_req, const struct ble_gap_cancel_connect_params *, void *)       \
        FN_SIG_S_P(ble_gap_generic_cmd_req, const struct ble_gap_gen_cmd_params *, void *)                 \
        FN_SIG_S_P(ble_gap_set_option_req, const struct ble_gap_set_option_params *, void *)               \
        FN_SIG_S_P(ble_gap_start_advertise_req, const struct ble_gap_adv_params *,struct ble_start_advertisement_req_msg *) \
        FN_SIG_S_P(ble_gap_conn_update_req, const struct ble_gap_connect_update_params *, struct ble_conn_update_rsp*)   \
        FN_SIG_S_P(ble_gap_service_read_req, const struct ble_gap_service_read_params *, void *)                         \
        FN_SIG_S_P(ble_gap_set_enable_config_req, const struct ble_wr_config_params *, struct ble_enable_req_msg *)      \
        FN_SIG_S_P(ble_gap_set_rssi_report_req, const struct rssi_report_params *, void *)                               \
        FN_SIG_S_P(ble_gap_service_write_req, const struct ble_gap_service_write_params *, struct cfw_message *)         \
        FN_SIG_S_P(ble_gap_dbg_req, const struct debug_params * , struct ble_dbg_msg * )                                 \
        FN_SIG_S_P(ble_gattc_discover_primary_service_req, const struct ble_gattc_discover_primary_service_params *, void *)    \
        FN_SIG_S_P(ble_gattc_discover_included_services_req,  const struct ble_gattc_discover_included_service_params *, void *)\
        FN_SIG_S_P(ble_gattc_discover_char_req, const struct ble_gattc_discover_characteristic_params *, void *)                \
        FN_SIG_S_P(ble_gattc_discover_descriptor_req, const struct ble_gattc_discover_descriptor_params *, void *)              \
        FN_SIG_S_P(ble_gattc_rd_chars_req,  const struct ble_gattc_read_characteristic_params *, void *)                        \
        FN_SIG_S_P(ble_gatts_send_svc_changed_req, const struct ble_gatts_svc_changed_params * , void * )                \
        FN_SIG_S_P(ble_gatts_get_attribute_value_req, const struct ble_gatts_get_attribute_params *, void * )            \
        FN_SIG_S_P(ble_gatts_read_conn_attributes_req, const struct conn_attributes_params *, void *)

#define LIST_FN_SIG_S_B_P                                                                                                                   \
        FN_SIG_S_B_P(ble_gatts_set_attribute_value_req, const struct ble_gatts_set_attribute_params * , uint8_t * , uint8_t , void *)       \
        FN_SIG_S_B_P(ble_gatts_write_conn_attributes_req, const struct conn_attributes_params *, uint8_t * ,uint8_t ,void *)

#define LIST_FN_SIG_S_B_B_P

#endif /* RPC_FUNCTIONS_TO_BLE_CORE_H_ */
