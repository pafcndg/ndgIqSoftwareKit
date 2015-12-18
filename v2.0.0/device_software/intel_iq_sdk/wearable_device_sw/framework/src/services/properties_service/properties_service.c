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

#include <string.h>

#include "infra/log.h"
#include "infra/properties_storage.h"

#include "cfw/cfw_service.h"
#include "cfw/cfw_client.h"
#include "properties_service_internal.h"
#include "services/properties_service/properties_service_api.h"

#define SERVICE_ID_PROPERTY_ID_TO_KEY(svc_id, prop_id) (((uint32_t)(svc_id)) << 16 | ((uint32_t)prop_id))

/* Handle for a Read Property request message */
static void handle_read_property(struct cfw_message *msg)
{
	read_property_req_msg_t * req = (read_property_req_msg_t *) msg;
	const uint32_t key = SERVICE_ID_PROPERTY_ID_TO_KEY(req->service_id, req->property_id);
	uint8_t tmp[PROPERTIES_STORAGE_MAX_VALUE_LEN];
	uint16_t read_len;
	properties_storage_status_t ret = properties_storage_get(key, tmp, 
	    PROPERTIES_STORAGE_MAX_VALUE_LEN, &read_len);

	read_property_rsp_msg_t *rsp = (read_property_rsp_msg_t *)
	    cfw_alloc_rsp_msg(msg, MSG_ID_PROP_SERVICE_READ_PROP_RSP,
	    sizeof(*rsp) + read_len);
	rsp->status = ret;
	rsp->property_size = read_len;
	if (ret == PROPERTIES_STORAGE_SUCCESS) {
		memcpy(&rsp->start_of_values, tmp, read_len);
	}

	switch (ret) {
	case PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR:
		rsp->rsp_header.status = DRV_RC_INVALID_OPERATION;
		break;
	case PROPERTIES_STORAGE_SUCCESS:
		rsp->rsp_header.status = DRV_RC_OK;
		break;
	default:
		rsp->rsp_header.status = DRV_RC_FAIL;
	}

	cfw_msg_free(msg);
	cfw_send_message(rsp);
}

/* Handle for an Add Property request message */
static void handle_add_property(struct cfw_message *msg)
{
	add_property_req_msg_t * req = (add_property_req_msg_t *) msg;
	const uint32_t key = SERVICE_ID_PROPERTY_ID_TO_KEY(req->service_id, req->property_id);

	uint16_t dummy1;
	bool dummy2;
	properties_storage_status_t ret = properties_storage_get_info(key, &dummy1, &dummy2);
	if (ret == PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR)
		ret = properties_storage_set(key, req->value, req->size, req->factory_reset_persistent);

	add_property_rsp_msg_t * rsp = (add_property_rsp_msg_t *)cfw_alloc_rsp_msg(msg,
		MSG_ID_PROP_SERVICE_ADD_PROP_RSP, sizeof(*rsp));

	switch (ret) {
	case PROPERTIES_STORAGE_SUCCESS:
		rsp->rsp_header.status = DRV_RC_OK;
		break;
	case PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR:
		rsp->rsp_header.status = DRV_RC_INVALID_OPERATION;
		break;
	default:
		rsp->rsp_header.status = DRV_RC_FAIL;
	}
	
	cfw_msg_free(msg);
	cfw_send_message(rsp);
}

/* Handle for a Write Property request message */
static void handle_write_property(struct cfw_message *msg)
{
	write_property_req_msg_t * req = (write_property_req_msg_t *) msg;
	const uint32_t key = SERVICE_ID_PROPERTY_ID_TO_KEY(req->service_id, req->property_id);

	uint16_t dummy1;
	bool dummy2;
	properties_storage_status_t ret = properties_storage_get_info(key, &dummy1, &dummy2);
	if (ret == PROPERTIES_STORAGE_SUCCESS)
		ret = properties_storage_set(key, req->value, req->size, false);

	write_property_rsp_msg_t * rsp = (write_property_rsp_msg_t *)cfw_alloc_rsp_msg(msg,
		MSG_ID_PROP_SERVICE_WRITE_PROP_RSP, sizeof(*rsp));

	switch (ret) {
	case PROPERTIES_STORAGE_SUCCESS:
		rsp->rsp_header.status = DRV_RC_OK;
		break;
	case PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR:
		rsp->rsp_header.status = DRV_RC_INVALID_OPERATION;
		break;
	default:
		rsp->rsp_header.status = DRV_RC_FAIL;
	}
	
	cfw_msg_free(msg);
	cfw_send_message(rsp);
}

/* Handle for a Remove Property request message. */
static void handle_remove_property(struct cfw_message *msg)
{
	remove_property_req_msg_t* req = (remove_property_req_msg_t*)msg;
	const uint32_t key = SERVICE_ID_PROPERTY_ID_TO_KEY(req->service_id, req->property_id);
	properties_storage_status_t ret = properties_storage_delete(key);

	remove_property_rsp_msg_t *rsp = (remove_property_rsp_msg_t *) cfw_alloc_rsp_msg(msg,
			MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP, sizeof(*rsp));

	switch (ret) {
	case PROPERTIES_STORAGE_SUCCESS:
		rsp->rsp_header.status = DRV_RC_OK;
		break;
	case PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR:
		rsp->rsp_header.status = DRV_RC_INVALID_OPERATION;
		break;
	default:
		rsp->rsp_header.status = DRV_RC_FAIL;
	}

	cfw_msg_free(msg);
	cfw_send_message(rsp);
}


static void handle_request(struct cfw_message * msg, void * param)
{
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_PROP_SERVICE_READ_PROP_REQ:
		handle_read_property(msg);
		break;
	case MSG_ID_PROP_SERVICE_REMOVE_PROP_REQ:
		handle_remove_property(msg);
		break;
	case MSG_ID_PROP_SERVICE_WRITE_PROP_REQ:
		handle_write_property(msg);
		break;
	case MSG_ID_PROP_SERVICE_ADD_PROP_REQ:
		handle_add_property(msg);
		break;
	default:
		cfw_print_default_handle_error_msg(LOG_MODULE_MAIN, CFW_MESSAGE_ID(msg));
		break;
	}
}

static void properties_client_connected(conn_handle_t * instance)
{
}

static void properties_client_disconnected(conn_handle_t * instance)
{
}

static service_t properties_service = {
	.service_id = PROPERTIES_SERVICE_ID,
	.client_connected = properties_client_connected,
	.client_disconnected = properties_client_disconnected,
};

void property_service_init(void * queue)
{
	properties_storage_init();
	cfw_register_service(queue, &properties_service, handle_request, NULL);
}

#ifdef CONFIG_TCMD_SERVICES_QUARK_SE_PROPERTIES

#include "infra/tcmd/handler.h"

static void tcmd_property_read(int argc,
                               char *argv[],
                               struct tcmd_handler_ctx *ctx)
{
	char *pend;
	uint16_t svc_id, prop_id;

	if (argc != 4) {
		goto error_msg;
	}

	svc_id = strtoul(argv[2], &pend, 10);
	if (*pend != 0) {
		goto error_msg;
	}

	prop_id = strtoul(argv[3], &pend, 10);
	if (*pend != 0) {
		goto error_msg;
	}

	char buf[PROPERTIES_STORAGE_MAX_VALUE_LEN];
	uint16_t readlen;
	int i;
	properties_storage_status_t ret = properties_storage_get(SERVICE_ID_PROPERTY_ID_TO_KEY(svc_id, prop_id), buf, PROPERTIES_STORAGE_MAX_VALUE_LEN, &readlen);

	char tmp[25];
	char* p = tmp;
	if (ret == PROPERTIES_STORAGE_SUCCESS)
	{
		for (i = 0; i < readlen; i++) {
			snprintf(p, 25, "%02x ", buf[i]);
			if ((i+1) % 8 == 0) {
				/* Output every eight bytes */
				TCMD_RSP_PROVISIONAL(ctx, tmp);
				p = tmp;
			} else {
				p += 3;
			}
		}
		if (i % 8 != 0) {
			/* Output remaining bytes */
			TCMD_RSP_FINAL(ctx, tmp);
		} else {
			TCMD_RSP_FINAL(ctx, NULL);
		}
	} else {
		snprintf(tmp, 25, "status: %u", ret);
		TCMD_RSP_ERROR(ctx, tmp);
	}

	return;
error_msg:
	TCMD_RSP_ERROR(ctx, TCMD_ERROR_MSG_INV_ARG);

}
DECLARE_TEST_COMMAND_ENG(property, read, tcmd_property_read);

#endif
