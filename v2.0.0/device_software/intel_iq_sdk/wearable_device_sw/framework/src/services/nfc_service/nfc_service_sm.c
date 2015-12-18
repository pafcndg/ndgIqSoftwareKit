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

#include <cfw/cfw.h>
#include <cfw/cfw_internal.h>
#include <cfw/cfw_messages.h>
#include <cfw/cfw_client.h>
#include <infra/port.h>
#include "services/services_ids.h"
#include "infra/log.h"
#include "infra/tcmd/handler.h"

#include "services/nfc_service/nfc_service_api.h"
#include "services/nfc_service/nfc_service_sm.h"

typedef enum nfc_fsm_state {
    ST_OFF,         /* controller is powered down */
    ST_CONFIG,      /* sequencer is doing the reset/init and configure scenarios */
    ST_READY,       /* configuration is done, controller is ready to start the RF loop */
    ST_ACTIVE,      /* the RF loop is active */
    ST_CLEANUP,     /* stop ongoing commands, start cleanup scenario, power off */
    ST_ERROR,       /* something's not right! */
    ST_ANY,         /* "any state" placeholder in the FSM table */
} fsm_state_t;

static const char *state_name[7] = {"OFF", "CONFIG", "READY", "ACTIVE", "CLEANUP", "ERROR", "ANY",};

static fsm_state_t state;
static uint16_t fsm_port_id;
static struct nfc_msg_cb *pending_request;

/** Internal NFC FSM event message id */
#define NFC_FSM_EVENT_MESSAGE 0xfe80

/**
 * Internal event structure.
 */
struct fsm_event_message {
    struct message m;
    int event;
    void * event_data;
    struct nfc_msg_cb *msg_cb;
};

typedef struct {
    fsm_state_t state;
    int event;
    fsm_state_t (*action)(struct fsm_event_message * evt);/* return next state */
} fsm_transition_t;

static void nfc_fsm_message_handler(struct message * msg, void * param);
static void nfc_fsm_run(struct fsm_event_message * evt);

static int set_pending_msg_cb(struct nfc_msg_cb *msg_cb);
static void process_pending_msg_cb(uint8_t status);
static void process_msg_cb(struct nfc_msg_cb *msg_cb, uint8_t status);

static fsm_state_t act_unhandled_event(struct fsm_event_message * evt);
static fsm_state_t act_config(struct fsm_event_message * evt);
static fsm_state_t act_config_done(struct fsm_event_message * evt);
static fsm_state_t act_start(struct fsm_event_message * evt);
static fsm_state_t act_start_done(struct fsm_event_message * evt);
static fsm_state_t act_stop(struct fsm_event_message * evt);
static fsm_state_t act_stop_done(struct fsm_event_message * evt);
static fsm_state_t act_cleanup(struct fsm_event_message * evt);
static fsm_state_t act_cleanup_done(struct fsm_event_message * evt);

static const fsm_transition_t fsm_table[] = {
    /* <state> , <event>, <action(event, event data)> */
    {ST_OFF,    EV_NFC_ENABLE,         act_config},       /* => CONFIG */
    {ST_CONFIG, EV_NFC_SCN_ALL_DONE,   act_config_done},  /* => READY */
    {ST_READY,  EV_NFC_START,          act_start},        /* => READY */
    {ST_READY,  EV_NFC_SCN_ALL_DONE,   act_start_done},   /* => ACTIVE */
    {ST_ACTIVE, EV_NFC_STOP,           act_stop},         /* => ACTIVE */
    {ST_ACTIVE, EV_NFC_SCN_ALL_DONE,   act_stop_done},    /* => READY */
    {ST_CLEANUP,EV_NFC_SCN_ALL_DONE,   act_cleanup_done}, /* => ST_OFF */
    {ST_ANY,    EV_NFC_DISABLE,        act_cleanup},      /* => ST_CLEANUP */

    /* catch all */
    {ST_ANY, EV_NFC_ANY,               act_unhandled_event}
};

void nfc_fsm_init(void * queue)
{
    state = ST_OFF;
    fsm_port_id = port_alloc(queue);
    pending_request = NULL;
    port_set_handler(fsm_port_id, nfc_fsm_message_handler, NULL);
}

static void nfc_fsm_message_handler(struct message * msg, void * param)
{
    struct fsm_event_message * evt = (struct fsm_event_message *) msg;
    nfc_fsm_run(evt);
    bfree(evt);
}

int nfc_fsm_event_post(int event, void * event_data, struct nfc_msg_cb *msg_cb)
{
    struct fsm_event_message * evt = (struct fsm_event_message *)
        balloc(sizeof(*evt), NULL);
    MESSAGE_ID(&evt->m) = NFC_FSM_EVENT_MESSAGE;
    MESSAGE_DST(&evt->m) = fsm_port_id;
    evt->event = event;
    evt->event_data = event_data;
    evt->msg_cb = msg_cb;
    return port_send_message(&evt->m);
}

static void nfc_fsm_run(struct fsm_event_message * evt)
{
    uint8_t i;
    uint8_t previous_state;

    for (i = 0; i < (sizeof(fsm_table) / sizeof(fsm_table[0])); ++i) {
        if (fsm_table[i].state == state || fsm_table[i].state == ST_ANY) {
            if ((fsm_table[i].event == evt->event) || fsm_table[i].event == EV_NFC_ANY) {

                previous_state = state;

                if (fsm_table[i].action != NULL){
                    state = (fsm_table[i].action)(evt);
                }

                if (previous_state != state) {
                    pr_info(LOG_MODULE_NFC, "state: %s -> %s", state_name[previous_state], state_name[state]);
                }

                break; /* we found the matching state/event */
            }
        }
    }
}

void process_msg_cb(struct nfc_msg_cb *msg_cb, uint8_t status)
{
    if(msg_cb){
        if(msg_cb->msg_cb && msg_cb->msg){
            msg_cb->msg_cb(msg_cb->msg, status);
            cfw_msg_free(msg_cb->msg);
            msg_cb->msg = NULL;
            msg_cb->msg_cb = NULL;
            bfree(msg_cb);
            msg_cb = NULL;
        }
    }
}

int set_pending_msg_cb(struct nfc_msg_cb *msg_cb)
{
    if(msg_cb != NULL && pending_request == NULL){ /* set pending req/callback */
        pending_request = msg_cb;
        return 0;
    } else {
        process_msg_cb(msg_cb, NFC_STATUS_NOT_ALLOWED);
        return 1;
    }
}

void process_pending_msg_cb(uint8_t status)
{
    if(pending_request != NULL){
        process_msg_cb(pending_request, status);
        pending_request = NULL;
    }
}

fsm_state_t act_unhandled_event(struct fsm_event_message * evt)
{
    pr_info(LOG_MODULE_NFC, "Unhandled NFC FSM state: %s, event: %d, event_data: %d", state_name[state], evt->event, evt->event_data);
    if(evt->msg_cb != NULL){
        pr_warning(LOG_MODULE_NFC, "Unhandled callback function: %x. Response WRONG_STATE.", evt->msg_cb);
        process_msg_cb(evt->msg_cb, NFC_STATUS_WRONG_STATE);
    }
    return state;
}

fsm_state_t act_config(struct fsm_event_message * evt)
{
    if(set_pending_msg_cb(evt->msg_cb)){
        pr_warning(LOG_MODULE_NFC, "Command not allowed - another command is pending.");
        return state;
    }

    pr_info(LOG_MODULE_NFC, "NFC power up and configure!");

    /* power up chip */
    /* start scenarios for init/reset/configure */

    /* simulate a scenario done!*/
    nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);

    return ST_CONFIG;
}

fsm_state_t act_config_done(struct fsm_event_message * evt)
{
    pr_info(LOG_MODULE_NFC, "NFC config done!");
    process_pending_msg_cb(NFC_STATUS_SUCCESS);
    return ST_READY;
}

fsm_state_t act_start(struct fsm_event_message * evt)
{
    if(set_pending_msg_cb(evt->msg_cb)){
        pr_warning(LOG_MODULE_NFC, "another command is pending!");
        return state;
    }

    pr_info(LOG_MODULE_NFC, "NFC start!");
    /* start scenario for starting RF loop */

    /* simulate a scenario done!*/
    nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);

    return ST_READY;
}

fsm_state_t act_start_done(struct fsm_event_message * evt)
{
    pr_info(LOG_MODULE_NFC, "NFC RF active!");
    {
        struct cfw_message evt;
        CFW_MESSAGE_ID(&evt) = MSG_ID_NFC_RF_EVT;
        CFW_MESSAGE_TYPE(&evt) = TYPE_EVT;
        CFW_MESSAGE_LEN(&evt) = sizeof(evt);
        CFW_MESSAGE_SRC(&evt) = _cfw_get_service_port(NFC_SERVICE_ID);
        cfw_send_event(&evt);
    }
    process_pending_msg_cb(NFC_STATUS_SUCCESS);
    return ST_ACTIVE;
}

fsm_state_t act_stop(struct fsm_event_message * evt)
{
    if(set_pending_msg_cb(evt->msg_cb)){
        pr_warning(LOG_MODULE_NFC, "another command is pending!");
        return state;
    }

    pr_info(LOG_MODULE_NFC, "NFC stop!");

    /* simulate a scenario done!*/
    nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);

    return ST_ACTIVE;
}

fsm_state_t act_stop_done(struct fsm_event_message * evt)
{
    pr_info(LOG_MODULE_NFC, "NFC RF stopped!");
    process_pending_msg_cb(NFC_STATUS_SUCCESS);
    return ST_READY;
}

fsm_state_t act_cleanup(struct fsm_event_message * evt)
{
    /* report error to any pending command */
    process_pending_msg_cb(NFC_STATUS_ERROR);

    /* set the disable command's callback */
    set_pending_msg_cb(evt->msg_cb);

    pr_info(LOG_MODULE_NFC, "NFC start cleanup!");

    /* simulate a scenario done!*/
    nfc_fsm_event_post(EV_NFC_SCN_ALL_DONE, NULL, NULL);
    return ST_CLEANUP;
}

fsm_state_t act_cleanup_done(struct fsm_event_message * evt)
{
    pr_info(LOG_MODULE_NFC, "NFC power down!");
    process_pending_msg_cb(NFC_STATUS_SUCCESS);
    return ST_OFF;
}
