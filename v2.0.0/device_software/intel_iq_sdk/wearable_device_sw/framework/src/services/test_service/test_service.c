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
#include "services/test_service/test_service.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_service.h"
#include "services/services_ids.h"
#include "infra/log.h"

/*******************************************************************************
 *********************** SERVICE IMPLEMENTATION ********************************
 ******************************************************************************/
void handle_test_1(struct cfw_message *msg) {
	test_1_rsp_msg_t *resp = (test_1_rsp_msg_t *) cfw_alloc_rsp_msg(msg,
			MSG_ID_TEST_1_RSP, sizeof(*resp));
	resp->rsp_header.status = 0;
	cfw_send_message(resp);

	struct cfw_message evt;
	CFW_MESSAGE_ID(&evt) = MSG_ID_TEST_1_EVT;
	CFW_MESSAGE_TYPE(&evt) = TYPE_EVT;
	CFW_MESSAGE_LEN(&evt) = sizeof(evt);
	CFW_MESSAGE_SRC(&evt) = _cfw_get_service_port(TEST_SERVICE_ID);
	cfw_send_event(&evt);
}

void handle_test_2(struct cfw_message *msg) {
	test_2_rsp_msg_t * resp = (test_2_rsp_msg_t* ) cfw_alloc_rsp_msg(msg,
			MSG_ID_TEST_2_RSP, sizeof(*resp));
	resp->rsp_header.status = 0;
	cfw_send_message(resp);
}

static void handle_message(struct cfw_message * msg, void * param) {
	pr_debug(LOG_MODULE_MAIN, "%s: handle_message: %d", __FILE__, CFW_MESSAGE_ID(msg));

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_TEST_1:
		handle_test_1(msg);
		break;
	case MSG_ID_TEST_2:
		handle_test_2(msg);
		break;
	default:
		cfw_print_default_handle_error_msg(LOG_MODULE_MAIN, CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

void test_client_connected(conn_handle_t * instance) {
	pr_info(LOG_MODULE_MAIN, "%s:", __func__);
}

void test_client_disconnected(conn_handle_t * instance) {
	pr_info(LOG_MODULE_MAIN, "%s:", __func__);
}

/****************************************************************************************
 ************************** SERVICE INITIALIZATION **************************************
 ****************************************************************************************/
static service_t test_service = {
		.service_id = TEST_SERVICE_ID,
		.client_connected = test_client_connected,
		.client_disconnected = test_client_disconnected,
};

void test_service_init(void * queue, int service_id) {
	test_service.service_id = service_id;

	cfw_register_service(queue, &test_service, handle_message, NULL);
}
