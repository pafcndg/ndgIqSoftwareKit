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
#include "machine.h"
#include "string.h"
#include "services/properties_service/properties_service_api.h"
#include "properties_service_internal.h"

/****************************************************************************************
 *********************** SERVICE API IMPLEMENATION **************************************
 ****************************************************************************************/

int properties_service_read(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * priv)
{
	struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_PROP_SERVICE_READ_PROP_REQ,
			sizeof(read_property_req_msg_t), priv);
	read_property_req_msg_t * req = (read_property_req_msg_t *) msg;
	req->service_id = service_id;
	req->property_id = property_id;
	cfw_send_message(msg);
	return 0;
}

int properties_service_write(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * buffer, uint16_t size, void * priv)
{
	struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_PROP_SERVICE_WRITE_PROP_REQ,
				(sizeof(write_property_req_msg_t) + size), priv);
	write_property_req_msg_t * req = (write_property_req_msg_t *) msg;
	req->service_id = service_id;
	req->property_id = property_id;
	req->size = size;
	memcpy(req->value, buffer, size);
	cfw_send_message(msg);
	return 0;
}

int properties_service_add(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, bool factory_rest_persistent, void * buffer, uint16_t size, void * priv)
{
	struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_PROP_SERVICE_ADD_PROP_REQ,
				(sizeof(add_property_req_msg_t) + size), priv);
	add_property_req_msg_t * req = (add_property_req_msg_t *) msg;
	req->service_id = service_id;
	req->property_id = property_id;
	req->factory_reset_persistent = factory_rest_persistent;
	req->size = size;
	memcpy(req->value, buffer, size);
	cfw_send_message(msg);
	return 0;
}

int properties_service_remove(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * priv)
{
	struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_PROP_SERVICE_REMOVE_PROP_REQ,
				sizeof(remove_property_req_msg_t), priv);
	remove_property_req_msg_t * req = (remove_property_req_msg_t *) msg;
	req->service_id = service_id;
	req->property_id = property_id;
	cfw_send_message(msg);
	return 0;
}
