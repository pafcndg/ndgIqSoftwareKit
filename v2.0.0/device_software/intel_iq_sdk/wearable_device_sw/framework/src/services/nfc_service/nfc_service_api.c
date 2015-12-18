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

#include "cfw/cfw_client.h"
#include "services/nfc_service/nfc_service_api.h"

int nfc_service_enable_controller(cfw_service_conn_t * c, void * priv) {
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_NFC_ENABLE_CLF, sizeof(*msg), priv);
    cfw_send_message(msg);
    return 0;
}

int nfc_service_disable_controller(cfw_service_conn_t * c, void *priv) {
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_NFC_DISABLE_CLF, sizeof(*msg), priv);
    cfw_send_message(msg);
    return 0;
}

int nfc_service_start_rf(cfw_service_conn_t * c, void * priv) {
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_NFC_START_RF, sizeof(*msg), priv);
    cfw_send_message(msg);
    return 0;
}

int nfc_service_stop_rf(cfw_service_conn_t * c, void * priv) {
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_NFC_STOP_RF, sizeof(*msg), priv);
    cfw_send_message(msg);
    return 0;
}
