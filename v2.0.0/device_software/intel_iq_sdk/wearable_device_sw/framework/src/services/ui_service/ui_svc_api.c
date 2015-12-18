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

#include <stdint.h>
#include <string.h>
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "services/ui_service/ui_svc.h"
#include "services/ui_service/ui_svc_api.h"
#ifdef CONFIG_HAS_AUDIO
#include "services/ui_service/ui_svc_audio.h"
#endif
#include "infra/log.h"

#define PANIC_FAIL_1 1
#define PANIC_FAIL_2 2
#define PANIC_FAIL_3 3


int8_t ui_get_available_features(cfw_service_conn_t * service_conn, void *priv)
{
    struct cfw_message *msg;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    msg = cfw_alloc_message_for_service(service_conn,
                        MSG_ID_UI_GET_FEAT_REQ,
                        sizeof(struct cfw_message), priv);
    if (msg == NULL) panic(PANIC_FAIL_2);

    cfw_send_message(msg);

    return UI_SUCCESS;
}


int8_t ui_get_enabled_events(cfw_service_conn_t * service_conn, void *priv)
{
    struct cfw_message *msg;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    msg = cfw_alloc_message_for_service(service_conn,
                        MSG_ID_UI_GET_EVT_REQ,
                        sizeof(struct cfw_message), priv);
    if (msg == NULL) panic(PANIC_FAIL_2);

    cfw_send_message(msg);

    return UI_SUCCESS;
}


int8_t ui_set_enabled_events(cfw_service_conn_t * service_conn, uint32_t mask,
              uint32_t enable, void *priv)
{
    struct cfw_message *msg;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    msg = cfw_alloc_message_for_service(service_conn,
                        MSG_ID_UI_SET_EVT_REQ,
                        sizeof(ui_set_events_req_t), priv);
    if (msg == NULL) panic(PANIC_FAIL_2);

    /* Check if this is a known event. */
    if ((ui_get_events_list() & mask) == mask) {
        ((ui_set_events_req_t *) msg)->mask = mask;
        ((ui_set_events_req_t *) msg)->enable = enable;
        cfw_send_message(msg);
    } else {
        pr_debug(LOG_MODULE_UI_SVC,
            "Error: Cannot set enabled events. Unknown mask 0x%08x",
            mask);
        return UI_INVALID_PARAM;
    }

    return UI_SUCCESS;
}


int8_t ui_play_led_pattern(cfw_service_conn_t * service_conn, uint8_t led_id,
            enum led_type type, led_s * pattern, void *priv)
{
    ui_play_led_pattern_req_t * req;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    req = (ui_play_led_pattern_req_t *)cfw_alloc_message_for_service(service_conn,
                          MSG_ID_UI_LED_REQ,
                          sizeof(ui_play_led_pattern_req_t),
                          priv);
    if (req == NULL) panic(PANIC_FAIL_2);

    req->led_id = led_id;
    req->type = type;

    if (pattern) {
        memcpy(&req->pattern, pattern, sizeof(req->pattern));
    }

    cfw_send_message(&req->header);

    return UI_SUCCESS;
}


int8_t ui_play_vibr_pattern(cfw_service_conn_t * service_conn,
            vibration_type type, vibration_u * pattern, void *priv)
{
    ui_play_vibr_pattern_req_t * req;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    req = (ui_play_vibr_pattern_req_t *)cfw_alloc_message_for_service(service_conn,
                          MSG_ID_UI_VIBR_REQ,
                          sizeof(ui_play_vibr_pattern_req_t),
                          priv);
    if (req == NULL) panic(PANIC_FAIL_2);

    req->type = type;
    if (pattern) {
        memcpy(&req->pattern, pattern, sizeof(req->pattern));
    }

    cfw_send_message(&req->header);

    return UI_SUCCESS;
}


#ifdef CONFIG_HAS_AUDIO
int8_t ui_audio_response(cfw_service_conn_t * service_conn,
            uint32_t audio_resp_id, void * priv)
{
    struct cfw_message * msg;
    uint32_t resp_id = audio_resp_id;

    if (service_conn == NULL) panic(PANIC_FAIL_1);

    /* Domain id in response indicates if this is a LPAL or ASR one. */
    /*   Byte #3    | Byte #2 | Byte #1           | Byte #0          */
    /*   future use | Tone id | Domain id  4 bits | Sentence id      */
    /*              |         | Request id 4 bits |                  */
    resp_id = resp_id >> 8;

    if (!resp_id) {
        pr_debug(LOG_MODULE_UI_SVC,
            "Error: Missing domain identifier in response id 0x%08x",
            audio_resp_id);
        return UI_INVALID_PARAM;
    }

    if ((resp_id & RESP_ID_MASK_DOMAIN) == AUDIO_LPAL_DOMAIN) {
        msg = cfw_alloc_message_for_service(service_conn,
                              MSG_ID_UI_LPAL_REQ,
                              sizeof(ui_lpal_evt_rsp_t),
                              priv);
        if (msg == NULL) panic(PANIC_FAIL_2);

        ((ui_lpal_evt_rsp_t *)msg)->ui_lpal_event_rsp_id = audio_resp_id;

        pr_debug(LOG_MODULE_UI_SVC,
            "MSG_ID_UI_LPAL_REQ: response id 0x%08x",
            audio_resp_id);
    } else {
        /* All domain > AUDIO_LPAL_DOMAIN are ASR. */
        msg = cfw_alloc_message_for_service(service_conn,
                              MSG_ID_UI_ASR_REQ,
                              sizeof(ui_asr_evt_rsp_t),
                              priv);
        if (msg == NULL) panic(PANIC_FAIL_3);

        ((ui_asr_evt_rsp_t *)msg)->ui_asr_event_rsp_id = audio_resp_id;

        pr_debug(LOG_MODULE_UI_SVC,
            "MSG_ID_UI_ASR_REQ: response id 0x%08x",
            audio_resp_id);
    }

    cfw_send_message(msg);

    return UI_SUCCESS;
}
#endif
