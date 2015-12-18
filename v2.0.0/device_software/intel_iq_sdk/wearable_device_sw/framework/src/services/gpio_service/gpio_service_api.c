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

#include "services/gpio_service/gpio_service.h"
#include "services/services_ids.h"
#include "cfw/cfw_client.h"

/****************************************************************************************
 *********************** SERVICE API IMPLEMENATION **************************************
 ****************************************************************************************/

int gpio_configure(cfw_service_conn_t * c, uint8_t index, uint8_t mode, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_GPIO_CONFIGURE_REQ,
            sizeof(gpio_configure_req_msg_t), priv);
    gpio_configure_req_msg_t * req = (gpio_configure_req_msg_t*) msg;
    req->mode = mode;
    req->index = index;
    cfw_send_message(msg);
    return 0;
}

int gpio_set_state(cfw_service_conn_t * c, uint8_t index, uint8_t val, void *priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_GPIO_SET_REQ,
            sizeof(gpio_set_req_msg_t), priv);
    gpio_set_req_msg_t * req = (gpio_set_req_msg_t*) msg;
    req->state = val;
    req->index = index;
    cfw_send_message(msg);
    return 0;
}

int gpio_get_state(cfw_service_conn_t * c, void *priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_GPIO_GET_REQ, sizeof(*msg), priv);
    cfw_send_message(msg);
    return 0;
}

int gpio_listen(cfw_service_conn_t * c, uint8_t pin, gpio_service_isr_mode_t mode, uint8_t debounce, void *priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_GPIO_LISTEN_REQ,
            sizeof(gpio_listen_req_msg_t), priv);
    gpio_listen_req_msg_t * req = (gpio_listen_req_msg_t*) msg;
    req->index = pin;
    req->mode = mode;
    req->debounce = debounce;
    cfw_send_message(msg);
    return 0;
}

int gpio_unlisten(cfw_service_conn_t * c, uint8_t pin, void *priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(c, MSG_ID_GPIO_UNLISTEN_REQ,
            sizeof(gpio_unlisten_req_msg_t), priv);
    gpio_unlisten_req_msg_t * req = (gpio_unlisten_req_msg_t*) msg;
    req->index = pin;
    cfw_send_message(msg);
    return 0;
}
