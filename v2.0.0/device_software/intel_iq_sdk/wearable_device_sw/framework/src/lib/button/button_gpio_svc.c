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

#include "infra/log.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "services/gpio_service/gpio_service.h"
#include "lib/button/button_gpio_svc.h"
#include "services/service_queue.h"

/* Gpio service wrapper */
static void gpio_service_handler(struct cfw_message *msg, void *param);
static void service_connection_cb(cfw_service_conn_t *conn, void *param);

/*!< gpio service connection handler */
static cfw_client_t * gpio_msg_client = NULL;

int button_gpio_svc_init(struct button *button)
{
	struct button_gpio_svc_priv *priv =
		(struct button_gpio_svc_priv*)button->priv;

	if (!gpio_msg_client) {
		/* Init gpio service wrapper */
		gpio_msg_client = cfw_client_init(
				get_service_queue(),
				gpio_service_handler, NULL);
	}

	cfw_open_service_helper(gpio_msg_client,
			priv->gpio_service_id,
			service_connection_cb, button);
	return 0;
}

void button_gpio_svc_shutdown(struct button *button)
{
	struct button_gpio_svc_priv *priv =
		(struct button_gpio_svc_priv*)button->priv;
	gpio_unlisten(
		priv->gpio_service_conn,
		priv->pin, NULL);
}

void service_connection_cb(cfw_service_conn_t * conn, void *param)
{
	struct button *button = (struct button *)param;
	struct button_gpio_svc_priv *priv =
		(struct button_gpio_svc_priv*)button->priv;

	priv->gpio_service_conn = conn;

	/* Configure GPIO in double edge with hardware debounce */
	gpio_listen(priv->gpio_service_conn, priv->pin,
			BOTH_EDGE, DEB_ON, button);
}

void gpio_service_handler(struct cfw_message *msg, void *param)
{
	struct button *button = (struct button *)msg->priv;
	struct button_gpio_svc_priv *priv =
		(struct button_gpio_svc_priv*)button->priv;

	switch (CFW_MESSAGE_ID(msg)) {
		case MSG_ID_GPIO_EVT: {
			bool state = !!(((gpio_listen_evt_msg_t*)msg)->pin_state) ^ priv->active_low;
			button_gpio_event(button, state,
					((gpio_listen_evt_msg_t*)msg)->timestamp);
			} break;

		case MSG_ID_GPIO_LISTEN_RSP: {
			int resp_stat = ((gpio_listen_rsp_msg_t *)msg)->rsp_header.status;
			if (resp_stat != 0)
				pr_error(LOG_MODULE_PWRBTN, "pwrbtn: GPIO listen failed (%d)\n",
						resp_stat);
			} break;

		default:
			break;
	}
	cfw_msg_free(msg);
}
