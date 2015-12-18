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
#include "cfw/cfw_client.h"
#include "cfw/cfw_messages.h"
#include "test_fw.h"
#include "machine.h"
#include "services/gpio_service/gpio_service.h"

cfw_service_conn_t * gpio_service_conn = NULL;

static int count = 0;

cfw_client_t * client = NULL;

void test_handle_message(struct cfw_message * msg, void * param)
{
	cfw_log("%s:%s for param: %s conn:%p id:%d\n", __FILE__, __func__,
			(char*) param, msg->conn, CFW_MESSAGE_ID(msg));
	//cfw_dump_message(msg);

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_REGISTER_SVC_AVAIL_EVT_RSP:
		break;
	case MSG_ID_CFW_SVC_AVAIL_EVT: {
		int svc_id = ((cfw_svc_available_evt_msg_t *)msg)->service_id;
		if (svc_id == SOC_GPIO_SERVICE_ID) {
			cfw_open_service_conn(client, SOC_GPIO_SERVICE_ID, "Conn1");
		}
		break;
	}
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		cfw_log("%s:%s for conn: %s\n", __FILE__, __func__,
				(char*) CFW_MESSAGE_PRIV(msg));
		cfw_open_conn_rsp_msg_t * cnf = (cfw_open_conn_rsp_msg_t*) msg;
		gpio_service_conn = cnf->service_conn;
		gpio_configure(gpio_service_conn, 26, 1, NULL);
		break;
	}
	case MSG_ID_GPIO_CONFIGURE_RSP:
		gpio_set_state(gpio_service_conn, 26, 1, NULL);
		break;

	case MSG_ID_GPIO_SET_RSP: {
		int val;
		void * next;
		if (!((int) CFW_MESSAGE_PRIV(msg))) {
			val = 0;
			next = (void*) 1;
		} else {
			val = 1;
			next = (void*) 0;
		}
		printf("Got response: %d %d \n", val, (int) next);
		if (count ++ > 100) {
			cfw_close_service_conn(gpio_service_conn, NULL);
		} else {
			gpio_set_state(gpio_service_conn, 26, val, next);
		}
	}
		break;

	case MSG_ID_CFW_CLOSE_SERVICE_RSP: {
		cfw_close_conn_rsp_msg_t * resp = (cfw_close_conn_rsp_msg_t*) msg;
		cfw_log("%s: res: %d\n", __func__, resp->rsp_header.status);
		exit(0);
	}
	break;

	default: {
		cfw_log("%s: Unhandled message: %d\n", __func__, CFW_MESSAGE_ID(msg));
		break;
	}
	}
	cfw_msg_free(msg);

}


void test_scenario_init(cfw_client_t * cfw_client)
{
	client = cfw_client;

	test_fw_set_handler(test_handle_message, NULL);

	cfw_register_svc_available(client, SOC_GPIO_SERVICE_ID, NULL);
}

