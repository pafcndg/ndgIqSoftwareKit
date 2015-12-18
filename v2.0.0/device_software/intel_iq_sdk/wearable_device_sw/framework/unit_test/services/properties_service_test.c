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
#include "infra/log.h"
#include "machine.h"
#include "services/service_tests.h"
#include "services/properties_service/properties_service_api.h"

static bool props_init_done = false;
static cfw_client_t * props_client;
static cfw_service_conn_t * props_service_conn = NULL;

static uint32_t read_prop = DRV_RC_TOTAL_RC_CODE;
static uint32_t write_prop = DRV_RC_TOTAL_RC_CODE;
static uint32_t add_prop = DRV_RC_TOTAL_RC_CODE;
static uint32_t read_size_1 = 0;
static uint32_t read_size_2 = 0;
static uint8_t read_prop_index = 0;
static uint8_t *read_data_1 = NULL;
static uint8_t *read_data_2 = NULL;

static void reset(void)
{
	read_prop = DRV_RC_TOTAL_RC_CODE;
	write_prop = DRV_RC_TOTAL_RC_CODE;
	add_prop = DRV_RC_TOTAL_RC_CODE;
	read_size_1 = 0;
	read_size_2 = 0;
	read_prop_index = 0;
}

static void handle_msg(struct cfw_message * msg, void * data)
{
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP:
		props_service_conn  = (cfw_service_conn_t *)((cfw_open_conn_rsp_msg_t*)msg)->service_conn;
		props_init_done = true;
		cu_print("Properties Service open\n");
		break;
	case MSG_ID_PROP_SERVICE_ADD_PROP_RSP:
		cu_print("Properties Service : MSG_ID_PROP_SERVICE_ADD_PROP_RSP\n");
		cu_print("Properties Service - Add property response: STATUS %d\n",
				((add_property_rsp_msg_t*)msg)->rsp_header.status);
		add_prop = ((add_property_rsp_msg_t*)msg)->rsp_header.status;
		break;
	case MSG_ID_PROP_SERVICE_READ_PROP_RSP:
		cu_print("Properties Service : MSG_ID_PROP_SERVICE_READ_PROP_RSP\n");
		cu_print("Properties Service - Read property response: STATUS %d\n",
				((read_property_rsp_msg_t*)msg)->rsp_header.status);
		read_prop = ((read_property_rsp_msg_t*)msg)->rsp_header.status;
		cu_print("read_prop = %u\n", read_prop);
		if (read_prop == DRV_RC_OK) {
			if (read_prop_index == 0) {
				read_size_1 = ((read_property_rsp_msg_t*)msg)->property_size;
				read_data_1 = balloc (read_size_1, NULL);
				memcpy(read_data_1, &((read_property_rsp_msg_t*)msg)->start_of_values, read_size_1);
			}
			if (read_prop_index == 1) {
				read_size_2 = ((read_property_rsp_msg_t*)msg)->property_size;
				read_data_2 = balloc (read_size_2, NULL);
				memcpy(read_data_2, &((read_property_rsp_msg_t*)msg)->start_of_values, read_size_2);
			}
			read_prop_index++;
		}
		break;
	case MSG_ID_PROP_SERVICE_WRITE_PROP_RSP:
		cu_print("Properties Service : MSG_ID_PROP_SERVICE_WRITE_PROP_RSP\n");
		cu_print("Properties Service - Write property event: STATUS %d\n",
				((write_property_rsp_msg_t*)msg)->rsp_header.status);
		write_prop = ((write_property_rsp_msg_t*)msg)->rsp_header.status;
		break;
	case MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP:
		cu_print("Properties Service : MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP\n");
		cu_print("Properties Service - Remove property response: STATUS %d\n",
				((remove_property_rsp_msg_t*)msg)->rsp_header.status);
		break;
	default:
		cu_print("default cfw handler\n");
		break;
	}
	cfw_msg_free(msg);
}

static DRIVER_API_RC test_init ()
{
	DRIVER_API_RC ret =  DRV_RC_FAIL;

	props_client = cfw_client_init(get_test_queue(), handle_msg, NULL);

	SRV_WAIT(!cfw_service_registered(PROPERTIES_SERVICE_ID), 0xFFFF);

	if (cfw_service_registered(PROPERTIES_SERVICE_ID)) {
	   cu_print("Properties Service registered, open it\n");
	   cfw_open_service_conn(props_client, PROPERTIES_SERVICE_ID, "LL storage service open");
	} else {
	   cu_print("Properties Service NOT registered\n");
	   goto out;
	}

	SRV_WAIT(!props_init_done, 0xFFFF);

	if (!props_init_done)
	   goto out;

	ret = DRV_RC_OK;
	cu_print("Properties Service initialization done\n");

out:
	CU_ASSERT("Properties Service initialization failure", ret == DRV_RC_OK);
	return ret;
}

static void property_test(void)
{
	properties_storage_format_all();

	uint16_t non_persist_service_id = 1;
	uint16_t non_persist_property_id = 1;
	uint16_t persist_service_id = 2;
	uint16_t persist_property_id = 1;
	uint8_t value[] = {0xAB, 0xCD, 0xEF};
	uint8_t new_value[] = {0x11, 0x22, 0x33};
	uint8_t persisten_value[] = {0x11, 0x22, 0x33, 0x44, 0x55};
	uint16_t non_persistent_size =  sizeof(value);
	uint16_t persistent_size =  sizeof(persisten_value);

	// Add factory reset non-persistent property
	cu_print("ADD Property Non Persistent Property\n");
	properties_service_add(props_service_conn, non_persist_service_id, non_persist_property_id, false, value, non_persistent_size, NULL);

	SRV_WAIT((add_prop == DRV_RC_TOTAL_RC_CODE), 0xFFFFF);
	CU_ASSERT("Property Add failure", add_prop == DRV_RC_OK);

	cu_print("Non Persistent Property ADD done\n");
	reset();
	local_task_sleep_ms(100);

	//Read back the previously added property
	properties_service_read(props_service_conn, non_persist_service_id, non_persist_property_id, NULL);
	properties_service_read(props_service_conn, non_persist_service_id, non_persist_property_id, NULL);

	SRV_WAIT((read_prop_index != 2), 0xFFFFF);
	CU_ASSERT("Property Read failure: incorrect read size", read_size_1 == non_persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(value, read_data_1, non_persistent_size) == 0);

	CU_ASSERT("Property Read failure: incorrect read size", read_size_2 == non_persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(value, read_data_2, non_persistent_size) == 0);

	bfree(read_data_1);
	bfree(read_data_2);
	reset();
	local_task_sleep_ms(100);

	// Add factory reset persistent property
	cu_print("ADD Property Persistent Property\n");
	properties_service_add(props_service_conn, persist_service_id, persist_property_id, true, persisten_value, persistent_size, NULL);

	SRV_WAIT((add_prop == DRV_RC_TOTAL_RC_CODE), 0xFFFFF);
	CU_ASSERT("Property Add failure", add_prop == DRV_RC_OK);

	cu_print("Persistent Property ADD done\n");
	reset();
	local_task_sleep_ms(100);

	// Read back the previously added property
	properties_service_read(props_service_conn, persist_service_id, persist_property_id, NULL);
	properties_service_read(props_service_conn, persist_service_id, persist_property_id, NULL);

	SRV_WAIT((read_prop_index != 2), 0xFFFFF);
	CU_ASSERT("Property Read failure", read_prop == DRV_RC_OK);
	CU_ASSERT("Property Read failure: incorrect read size", read_size_1 == persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(persisten_value, read_data_1, persistent_size) == 0);

	CU_ASSERT("Property Read failure: incorrect read size", read_size_2 == persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(persisten_value, read_data_2, persistent_size) == 0);

	bfree(read_data_1);
	bfree(read_data_2);
	reset();
	local_task_sleep_ms(100);

	// Modify previously added property
	properties_service_write(props_service_conn, non_persist_service_id, non_persist_property_id, new_value, non_persistent_size, NULL);

	SRV_WAIT((write_prop == DRV_RC_TOTAL_RC_CODE), 0xFFFFF);
	CU_ASSERT("Property Write failure", write_prop == DRV_RC_OK);

	reset();
	local_task_sleep_ms(100);

	// Read back the the latest property value
	properties_service_read(props_service_conn, non_persist_service_id, non_persist_property_id, NULL);
	properties_service_read(props_service_conn, non_persist_service_id, non_persist_property_id, NULL);

	SRV_WAIT((read_prop_index != 2), 0xFFFFF);
	CU_ASSERT("Property Read failure: incorrect read size", read_size_1 == non_persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(new_value, read_data_1, non_persistent_size) == 0);

	CU_ASSERT("Property Read failure: incorrect read size", read_size_2 == non_persistent_size);
	CU_ASSERT("Property Read failure: incorrect read data", memcmp(new_value, read_data_2, non_persistent_size) == 0);

	bfree(read_data_1);
	bfree(read_data_2);
	reset();
}


void properties_service_test(void)
{
	cu_print("##############################################\n");
	cu_print("# Add property                               #\n");
	cu_print("# Read back the value of the property        #\n");
	cu_print("# Modify the value of the property           #\n");
	cu_print("# Read back the new value                    #\n");
	cu_print("##############################################\n");

	if (test_init() == DRV_RC_OK)
		property_test();
}
