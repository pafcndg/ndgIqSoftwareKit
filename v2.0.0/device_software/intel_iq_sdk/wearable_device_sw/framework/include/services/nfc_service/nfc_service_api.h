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

#ifndef __NFC_SERVICE_API_H__
#define __NFC_SERVICE_API_H__
#include "cfw/cfw.h"
#include "cfw/cfw_client.h"


#define MSG_ID_NFC_ENABLE_CLF   1
#define MSG_ID_NFC_DISABLE_CLF  2
#define MSG_ID_NFC_START_RF     3
#define MSG_ID_NFC_STOP_RF      4

#define MSG_ID_NFC_ENABLE_CLF_RSP  0x81
#define MSG_ID_NFC_DISABLE_CLF_RSP 0x82
#define MSG_ID_NFC_START_RF_RSP    0x83
#define MSG_ID_NFC_STOP_RF_RSP     0x84

#define MSG_ID_NFC_RF_EVT         0x1001

/** NFC response/event status codes. */
enum NFC_STATUS {
    NFC_STATUS_SUCCESS = 0, /**< General NFC Success code */
    NFC_STATUS_PENDING,     /**< Request received and execution started, response pending */
    NFC_STATUS_TIMEOUT,     /**< Request timed out */
    NFC_STATUS_NOT_SUPPORTED, /**< Request/feature/parameter not supported */
    NFC_STATUS_NOT_ALLOWED, /**< Request not allowed, other request pending */
    NFC_STATUS_NOT_ENABLED, /**< NFC not enabled */
    NFC_STATUS_ERROR,       /**< Generic Error */
    NFC_STATUS_WRONG_STATE, /**< Wrong state for request */
    NFC_STATUS_ERROR_PARAMETER, /**< Parameter in request is wrong */
};


void nfc_service_init(void * queue, int service_id);

int nfc_service_enable_controller(cfw_service_conn_t * service_conn, void * priv);
int nfc_service_disable_controller(cfw_service_conn_t * service_conn, void * priv);

int nfc_service_start_rf(cfw_service_conn_t * service_conn, void * priv);
int nfc_service_stop_rf(cfw_service_conn_t * service_conn, void * priv);

typedef struct nfc_rsp_msg {
    struct cfw_rsp_message rsp_header;
    uint8_t status;
} nfc_rsp_msg_t;

#endif
