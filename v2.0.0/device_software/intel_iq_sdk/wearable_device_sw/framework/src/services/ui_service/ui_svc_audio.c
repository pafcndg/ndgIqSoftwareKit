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
#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "cfw/cfw_debug.h"
#include "service_mgr.h"
#include "ui_svc.h"
#include "ui_svc_api.h"
#include "ui_svc_audio.h"
#include "audio_service_api.h"
#include "logger.h"
#include "panic_api.h"

#define UISVC_AUDIO_CLIENT "UI_AUDIO"

static cfw_client_t __maybe_unused *ui_audio_svc;
static cfw_service_conn_t *audio_client_cfw_hdl = NULL;

static message_t *ui_audio_req;
static audio_stream_t *tone_stream = NULL;
static audio_stream_t *sps_stream = NULL;
static uint8_t sps_id_stream_pending = 0;
static void *as_stream_priv;
static port_t ui_port;
static service_t *ui_svc;

/**@brief Function to broadcast to other clients LPAL events coming
 *        from the Audio service.
 *
 * @details Broadcast message is built and passed to the framework which
 *          in turn sends it to all clients consumers of the event.
 * @param[in]  msg  Event message.
 * @return   none
 */
void ui_broadcast_svc_lpal_event(message_t * msg)
{
    message_t *broadcast_msg;

    broadcast_msg = cfw_alloc_evt_msg(ui_svc,
                      MSG_ID_UI_LPAL_EVT,
                      sizeof(ui_lpal_evt_t));
    if (broadcast_msg == NULL) force_panic();

    ((ui_lpal_evt_t *) broadcast_msg)->ui_lpal_event_id =
        ((ui_lpal_evt_t *) msg)->ui_lpal_event_id;

    pr_debug(LOG_MODULE_UI_SVC,
            "Audio: broadcast LPAL EVT:0x%x from src:%d",
            ((ui_lpal_evt_t *) msg)->ui_lpal_event_id,
            msg->src);

    cfw_send_event(broadcast_msg);

    /* Message cloned by the framework. Can be deleted now. */
    cfw_msg_free(broadcast_msg);
}

/**@brief Function to broadcast to other clients ASR events coming
 *        from the Audio service.
 *
 * @details Broadcast message is built and passed to the framework which
 *          in turn sends it to all clients consumers of the event.
 * @param[in]  msg  Event message.
 * @return   none
 */
void ui_broadcast_svc_asr_event(message_t * msg)
{
    message_t *broadcast_msg;

    broadcast_msg = cfw_alloc_evt_msg(ui_svc,
                      MSG_ID_UI_ASR_EVT,
                      sizeof(ui_asr_evt_t));
    if (broadcast_msg == NULL) force_panic();

    ((ui_asr_evt_t *) broadcast_msg)->ui_asr_event_id =
        ((ui_asr_evt_t *) msg)->ui_asr_event_id;

    pr_debug(LOG_MODULE_UI_SVC,
            "Audio: broadcast ASR EVT:0x%x from src:%d",
            ((ui_asr_evt_t *) msg)->ui_asr_event_id,
            msg->src);

    cfw_send_event(broadcast_msg);

    /* Message cloned by the framework. Can be deleted now. */
    cfw_msg_free(broadcast_msg);
}

/**@brief Function to initialize tone to be played.
 *
 * @param[in]  tone_id Tone identifier.
 * @param[in]  priv Additional data. NULL if not used.
 * @return   none
 */
static void ui_tone_play_init(uint8_t tone_id, void *priv)
{
    audio_port_t port;

    tone_stream = as_stream_create("Audio Tone Resp");

    pr_debug(LOG_MODULE_UI_SVC,
        "Audio: init play tone id 0x%x stream 0x%x",
        tone_id,
        tone_stream);

    as_stream_init(audio_client_cfw_hdl,
               tone_stream,
               AUDIO_STREAM_MEDIA,
               &port);

    /* FIXME: audio port structure not finalized yet. */
    /*        To be completed here once available.    */
}

/**@brief Function to start playing tone.
 *
 * @param[in]  none.
 * @return   none
 */
static void ui_tone_play_start(void)
{
    pr_debug(LOG_MODULE_UI_SVC,
        "Audio: start play tone stream 0x%x",
        tone_stream);

    as_stream_start(audio_client_cfw_hdl, tone_stream);
}


/**@brief Function to initialize speech sequence to be played.
 *
 * @param[in]  sequence_id Speech sequence identifier.
 * @param[in]  priv Additional data. NULL if not used.
 * @return   none
 */
static void ui_sps_play_init(uint8_t sequence_id, void *priv)
{
    audio_port_t port;

    sps_stream = as_stream_create("Audio UI Sps resp");

    pr_debug(LOG_MODULE_UI_SVC,
        "Audio: init play sps id 0x%x stream 0x%x",
        sequence_id,
        sps_stream);

    as_stream_init(audio_client_cfw_hdl,
               sps_stream,
               AUDIO_STREAM_VOICE,
               &port);

    /* FIXME: audio port structure not finalized yet. */
    /*        To be completed here once available.    */
}

/**@brief Function to start playing speech sequence.
 *
 * @param[in]  none.
 * @return   none
 */
static void ui_sps_play_start(void)
{
    pr_debug(LOG_MODULE_UI_SVC,
        "Audio: start play sps stream 0x%x",
        sps_stream);

    as_stream_start(audio_client_cfw_hdl, sps_stream);
}

/**@brief Function to handle terminated audio streams.
 *
 * @details In this function played stream is freed. A new stream is
 *          initialized if necessary. Once all streams have been
 *          played, response message is built and sent.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void ui_handle_as_stream_end(message_t * msg)
{
    rsp_message_t *resp;
    uint16_t resp_id;

    audio_stream_mgr_evt_msg *evt = (audio_stream_mgr_evt_msg *) msg;

    /* Need to filter MSG_ID_AS_STREAM_XX messages, as the UI SVC is not */
    /* the only one listening to them.                                   */
    if ((evt->sHandle != tone_stream) && (evt->sHandle != sps_stream))
        return;

    pr_debug(LOG_MODULE_UI_SVC,
        "Audio stream 0x%08x ended - reason %d",
        evt->sHandle,
        evt->sHandle->reason);

    if (tone_stream) {
        as_stream_destroy(tone_stream);
        tone_stream = NULL;
    }

    /* Check if more data to play. */
    if (sps_stream) {
        /* Play done. */
        as_stream_destroy(sps_stream);
        sps_stream = NULL;
        sps_id_stream_pending = 0;
    } else if (sps_id_stream_pending) {
        /* Play speech sequence. */
        ui_sps_play_init(sps_id_stream_pending, as_stream_priv);
    }

    if (!tone_stream && !sps_stream) {
        /* Request completed. */
        if (ui_audio_req->id == MSG_ID_UI_LPAL_REQ)
            resp_id = MSG_ID_UI_LPAL_RSP;
        else
            resp_id = MSG_ID_UI_ASR_RSP;

        resp = cfw_alloc_rsp_msg(ui_audio_req, resp_id, sizeof(*resp));
        if (resp == NULL) force_panic();

        resp->status = evt->sHandle->reason;
        cfw_send_message(resp);

        cfw_msg_free(ui_audio_req);
        ui_audio_req = NULL;
    }
}

/**@brief Function to handle audio requests to play tone and/or sentence.
 *
 * @param[in]  msg  Request message.
 * @return   none
 */
void ui_handle_audio_req(message_t * msg)
{
    uint32_t audio_data;
    uint8_t sps_id;
    uint8_t tone_id;

    ui_audio_req = cfw_clone_message(msg);
    if (ui_audio_req == NULL) force_panic();

    /* Tone and sequence identifiers are coded as follows:      */
    /*   Byte #3    | Byte #2 | Byte #1           | Byte #0     */
    /*   future use | Tone id | Domain id  4 bits | Sentence id */
    /*              |         | Request id 4 bits |             */

    if (msg->id == MSG_ID_UI_LPAL_REQ) {
        audio_data = ((ui_lpal_evt_rsp_t *) msg)->ui_lpal_event_rsp_id;
    } else if (MSG_ID_UI_ASR_REQ) {
        audio_data = ((ui_asr_evt_rsp_t *) msg)->ui_asr_event_rsp_id;
    }

    sps_id = audio_data & RESP_ID_MASK_SENTENCE;
    sps_id_stream_pending = sps_id;

    audio_data = audio_data >> 16;
    tone_id = audio_data & RESP_ID_MASK_TONE;

    if (tone_id)
        /* Play tone first. */
        ui_tone_play_init(tone_id, msg->priv);
    else if (sps_id)
        /* Play speech sequence. */
        ui_sps_play_init(sps_id, msg->priv);
    else
        /* Empty request. */
        pr_error(LOG_MODULE_UI_SVC,
            "No tone or speech sentence in request");
}

/**@brief Function to handle messages from framework.
 *
 * @details In this function are handled messages of
 *          communications with the Audio service.
 * @param[in]  msg   Message from framework.
 * @param[in]  param Additional data. NULL if not used.
 * @return   none.
 */
void ui_audio_svc_handle_message(message_t * msg, void *param)
{
    int client_events[4];
    cfw_open_conn_rsp_msg_t *cnf = NULL;
    audio_stream_mgr_evt_msg *evt = NULL;

    pr_debug(LOG_MODULE_UI_SVC,
        "Audio: received msg: %d src: %d dst: %d",
        msg->id,
        msg->src,
        msg->dst);

    /* Dump message content. */
    cfw_dump_message(msg);

    ui_port = msg->dst;

    switch (msg->id) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:{
        cnf = (cfw_open_conn_rsp_msg_t *) msg;
        if (cnf == NULL) force_panic();

        audio_client_cfw_hdl = cnf->service_conn;
        if (audio_client_cfw_hdl == NULL) force_panic();

        client_events[0] = MSG_ID_AS_LPAL_EVT;
        client_events[1] = MSG_ID_AS_ASR_EVT;
        client_events[2] = MSG_ID_AS_STREAM_STOPPED_EVT;
        client_events[3] = MSG_ID_AS_STREAM_TERMINATED_EVT;

        /* Events registration. */
        cfw_register_events(audio_client_cfw_hdl,
                    client_events, 4, msg->priv);

        pr_info(LOG_MODULE_UI_SVC,
            "Audio: Audio service id %lu opened",
            UI_SVC_ID);
        }
        break;
    case MSG_ID_CFW_CLOSE_SERVICEi_RSP:
        pr_info(LOG_MODULE_UI_SVC,
            "Audio: Audio service has been closed");
        break;
    case MSG_ID_CFW_REGISTER_EVT_RSP:
        pr_debug(LOG_MODULE_UI_SVC,
            "Audio: Events registered for Audio service");
        break;
    case MSG_ID_AS_LPAL_EVT:
        ui_broadcast_svc_lpal_event(msg);
        break;
    case MSG_ID_AS_ASR_EVT:
        ui_broadcast_svc_asr_event(msg);
        break;
    case MSG_ID_AS_STREAM_STOPPED_EVT:
        /* Need to filter MSG_ID_AS_STREAM_XX messages, as the UI SVC is */
        /* not the only one listening to them.                           */
        /* If not tone or speech sequence initialized, msg is ignored.   */
        evt = (audio_stream_mgr_evt_msg *)msg;

        if ((evt->sHandle == tone_stream) || (evt->sHandle == sps_stream)) {
            pr_debug(LOG_MODULE_UI_SVC,
                "Audio: stream 0x%08x stopped, reason %d",
                evt->sHandle,
                evt->sHandle->reason);

            if (evt->sHandle->reason == AUDIO_STREAM_STATE_INIT_SUCCESS) {
                if (tone_stream)
                    ui_tone_play_start();
                else if (sps_stream)
                    ui_sps_play_start();
            }
        }
        break;
    case MSG_ID_AS_STREAM_TERMINATED_EVT:
        ui_handle_as_stream_end(msg);
        break;
    default:
        break;
    }

    cfw_msg_free(msg);
}

/**@brief Open connection with the Audio service.
 *
 * @details The function opens the audio service and provides message
 *          handler function pointer.
 * @param[in] queue Queue of messages.
 * @param[in] service Pointer to ui service.
 * @return   none.
 */
void ui_audio_svc_open(void *queue, service_t *service)
{
    ui_svc = service;

    ui_audio_svc = cfw_client_init(queue,
                     ui_audio_svc_handle_message,
                     UISVC_AUDIO_CLIENT);
    if (ui_audio_svc == NULL) force_panic();

    cfw_open_service_conn(ui_audio_svc, AUDIO_SVC_ID, "UI SVC Audio");
}
