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

#include "cfw/cfw.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_service.h"
#include "services/services_ids.h"
#include "infra/log.h"
#include "services/nfc_service/nfc_service_api.h"
#include "services/nfc_service/nfc_service_sm.h"

void nfc_generic_rsp_callback(struct cfw_message *msg, uint8_t result);

void handle_nfc_enable_clf(struct cfw_message *msg) {
    /* setup callback function for response */
    struct nfc_msg_cb *enable_clf_cb = (struct nfc_msg_cb *) balloc(sizeof(*enable_clf_cb), NULL);
    enable_clf_cb->msg = msg;
    enable_clf_cb->msg_cb = nfc_generic_rsp_callback;

    nfc_fsm_event_post(EV_NFC_ENABLE, NULL, enable_clf_cb);
}

void handle_nfc_disable_clf(struct cfw_message *msg) {
    /* setup callback function for response */
    struct nfc_msg_cb *disable_clf_cb = (struct nfc_msg_cb *) balloc(sizeof(*disable_clf_cb), NULL);
    disable_clf_cb->msg = msg;
    disable_clf_cb->msg_cb = nfc_generic_rsp_callback;

    nfc_fsm_event_post(EV_NFC_DISABLE, NULL, disable_clf_cb);
}

void handle_nfc_start_rf(struct cfw_message *msg) {
    /* setup callback function for response */
    struct nfc_msg_cb *start_cb = (struct nfc_msg_cb *) balloc(sizeof(*start_cb), NULL);
    start_cb->msg = msg;
    start_cb->msg_cb = nfc_generic_rsp_callback;

    nfc_fsm_event_post(EV_NFC_START, NULL, start_cb);
}

void handle_nfc_stop_rf(struct cfw_message *msg) {
    /* setup callback function for response */
    struct nfc_msg_cb *stop_cb = (struct nfc_msg_cb *) balloc(sizeof(*stop_cb), NULL);
    stop_cb->msg = msg;
    stop_cb->msg_cb = nfc_generic_rsp_callback;

    nfc_fsm_event_post(EV_NFC_STOP, NULL, stop_cb);
}

void nfc_generic_rsp_callback(struct cfw_message *msg, uint8_t result)
{
    int rsp_id = 0;

    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_NFC_ENABLE_CLF:
        rsp_id = MSG_ID_NFC_ENABLE_CLF_RSP;
        break;
    case MSG_ID_NFC_DISABLE_CLF:
        rsp_id = MSG_ID_NFC_DISABLE_CLF_RSP;
        break;
    case MSG_ID_NFC_START_RF:
        rsp_id = MSG_ID_NFC_START_RF_RSP;
        break;
    case MSG_ID_NFC_STOP_RF:
        rsp_id = MSG_ID_NFC_STOP_RF_RSP;
        break;
    default:
        pr_info(LOG_MODULE_NFC, "%s: unexpected cb message id: %x", __func__, CFW_MESSAGE_ID(msg));
        break;
    }

    nfc_rsp_msg_t *resp = (nfc_rsp_msg_t *) cfw_alloc_rsp_msg(msg,
            rsp_id, sizeof(*resp));
    resp->rsp_header.status = 0;
    resp->status = result;
    cfw_send_message(resp);
}

static void handle_message(struct cfw_message * msg, void * param) {
    pr_debug(LOG_MODULE_NFC, "%s: handle_message: %d", __FILE__, CFW_MESSAGE_ID(msg));

    bool do_msg_free = 1;

    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_NFC_ENABLE_CLF:
        handle_nfc_enable_clf(msg);
        do_msg_free = 0;
        break;
    case MSG_ID_NFC_DISABLE_CLF:
        handle_nfc_disable_clf(msg);
        do_msg_free = 0;
        break;
    case MSG_ID_NFC_START_RF:
        handle_nfc_start_rf(msg);
        do_msg_free = 0;
        break;
    case MSG_ID_NFC_STOP_RF:
        handle_nfc_stop_rf(msg);
        do_msg_free = 0;
        break;
    default:
        pr_info(LOG_MODULE_NFC, "%s: unexpected message id: %x", __func__, CFW_MESSAGE_ID(msg));
        break;
    }

    if(do_msg_free)
        cfw_msg_free(msg);
}

void nfc_client_connected(conn_handle_t * instance) {
    //pr_info(LOG_MODULE_NFC, "%s:", __func__);
}

void nfc_client_disconnected(conn_handle_t * instance) {
    //pr_info(LOG_MODULE_NFC, "%s:", __func__);
}

static service_t nfc_service = {
        .service_id = NFC_SERVICE_ID,
        .client_connected = nfc_client_connected,
        .client_disconnected = nfc_client_disconnected,
};

void nfc_service_init(void * queue, int service_id) {
    nfc_service.service_id = service_id;
    cfw_register_service(queue, &nfc_service, handle_message, NULL);
    nfc_fsm_init(queue);
}
