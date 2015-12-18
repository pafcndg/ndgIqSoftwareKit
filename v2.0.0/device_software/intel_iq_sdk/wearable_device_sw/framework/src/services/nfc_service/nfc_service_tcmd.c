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
#include <string.h>

#include "infra/log.h"
#include "infra/tcmd/handler.h"

#include "services/nfc_service/nfc_service_api.h"
#include "services/nfc_service/nfc_service_sm.h"
#include "services/services_ids.h"
#include "service_queue.h"
#include "cfw/cproxy.h"

static cfw_service_conn_t *nfc_service_conn;

static void _tcmd_nfc_handle_msg(struct cfw_message *msg, void *data)
{
    switch (CFW_MESSAGE_ID(msg)) {
        case MSG_ID_NFC_ENABLE_CLF_RSP:
            pr_info(LOG_MODULE_MAIN, "TCMD: NFC Enable response: %d!", ((nfc_rsp_msg_t*)msg)->status);
        break;

        case MSG_ID_NFC_DISABLE_CLF_RSP:
            pr_info(LOG_MODULE_MAIN, "TCMD: NFC Disable response: %d!", ((nfc_rsp_msg_t*)msg)->status);
        break;

        case MSG_ID_NFC_START_RF_RSP:
            pr_info(LOG_MODULE_MAIN, "TCMD: NFC Start response!");
        break;

        case MSG_ID_NFC_STOP_RF_RSP:
            pr_info(LOG_MODULE_MAIN, "TCMD: NFC Stop response!");
        break;

        default:
            pr_info(LOG_MODULE_MAIN, "Other NFC RSP/EVT!");
        break;
    }

    cproxy_disconnect(nfc_service_conn);
    cfw_msg_free(msg);
}

void nfc_svc_tcmd(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    if(argc < 3)
        goto show_help;

    nfc_service_conn = cproxy_connect(NFC_SERVICE_ID, _tcmd_nfc_handle_msg, NULL);
    if(nfc_service_conn == NULL){
        TCMD_RSP_ERROR(ctx, "Cannot connect to NFC service");
        return;
    }

    if (!strncmp("enable", argv[2], 6) ) {
        nfc_service_enable_controller(nfc_service_conn, NULL);
    } else if (!strncmp("disable", argv[2], 7)) {
        nfc_service_disable_controller(nfc_service_conn, NULL);
    } else if (!strncmp("start", argv[2], 5)) {
        nfc_service_start_rf(nfc_service_conn, NULL);
    } else if (!strncmp("stop", argv[2], 4)) {
        nfc_service_stop_rf(nfc_service_conn, NULL);
    } else {
        goto show_help;
    }

    TCMD_RSP_FINAL(ctx, NULL);
    return;

show_help:
    TCMD_RSP_ERROR(ctx, "nfc svc <enable|disable|start|stop>");
return;
}
DECLARE_TEST_COMMAND_ENG(nfc, svc, nfc_svc_tcmd);

static void nfc_fsm_tcmd(int argc, char ** argv, struct tcmd_handler_ctx *ctx)
{
    if(argc < 3)
        goto show_help;

    pr_info(LOG_MODULE_MAIN, "fsm event: %s", argv[2]);
    if (!strncmp("enable", argv[2], 6) ) {
        nfc_fsm_event_post(EV_NFC_ENABLE, NULL, NULL);
    } else if (!strncmp("disable", argv[2], 7)) {
        nfc_fsm_event_post(EV_NFC_DISABLE, NULL, NULL);
    } else if (!strncmp("start", argv[2], 5)) {
        nfc_fsm_event_post(EV_NFC_START, NULL, NULL);
    } else if (!strncmp("stop", argv[2], 4)) {
        nfc_fsm_event_post(EV_NFC_STOP, NULL, NULL);
    } else if (!strncmp("scn_all_done", argv[2], 12)) {
        nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
    }if (!strncmp("fsm_auto_test", argv[2], 9)) {
        nfc_fsm_event_post(EV_NFC_ENABLE, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_START, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_STOP, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_DISABLE, NULL, NULL);
        nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
    }

    TCMD_RSP_FINAL(ctx, NULL);
    return;

show_help:
    TCMD_RSP_ERROR(ctx, "nfc fsm <enable|disable|start|stop|scn_all_done|fsm_auto_test>");
return;
}
DECLARE_TEST_COMMAND(nfc, fsm, nfc_fsm_tcmd);
