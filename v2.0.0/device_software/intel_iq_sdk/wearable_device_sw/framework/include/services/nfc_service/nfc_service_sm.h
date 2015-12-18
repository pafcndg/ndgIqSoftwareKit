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

#ifndef __NFC_SERVICE_SM_H__
#define __NFC_SERVICE_SM_H__

enum {
    /* Service events */
    EV_NFC_ENABLE,
    EV_NFC_DISABLE,
    EV_NFC_START,
    EV_NFC_STOP,
    /* Driver events */
    EV_NFC_RX_IRQ,
    /* Command sequencer events */
    EV_NFC_SCN_START,
    EV_NFC_SCN_DONE,
    EV_NFC_SCN_ALL_DONE,
    EV_NFC_SCN_FAIL,
    EV_NFC_SCN_TIMER,
    /* NFC stack events */
    EV_NFC_NDLC_ACK,
    EV_NFC_NCI_OK,
    EV_NFC_HCI_OK,
    /* "any event" placeholder for the FSM table */
    EV_NFC_ANY,
    EV_NFC_MAX_EVENT
} nfc_fsm_events;

struct nfc_msg_cb {
    struct cfw_message *msg;
    void (*msg_cb)(struct cfw_message *msg, uint8_t result);
};

/**
 * Initialize the NFC FSM.
 *
 * @param queue the queue on which the FSM event handler should run
 *
 * @return 0 if success
 */
void nfc_fsm_init(void * queue);

/**
 * Post an event to the NFC FSM queue.
 *
 * @param event the event id
 * @param event_data the data associated with the event when applicable.
 */
int nfc_fsm_event_post(int event, void * event_data, struct nfc_msg_cb *msg_cb);

#endif /* __NFC_SERVICE_SM_H__ */
