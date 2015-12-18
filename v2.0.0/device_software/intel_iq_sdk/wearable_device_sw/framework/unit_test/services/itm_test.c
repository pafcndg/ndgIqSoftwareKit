/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
  *
  * The source code contained or described herein and all documents related to
  * the source code ("Material") are owned by Intel Corporation or its suppliers
  * or licensors.
  * Title to the Material remains with Intel Corporation or its suppliers and
  * licensors.
  * The Material contains trade secrets and proprietary and confidential information
  * of Intel or its suppliers and licensors. The Material is protected by worldwide
  * copyright and trade secret laws and treaty provisions.
  * No part of the Material may be used, copied, reproduced, modified, published,
  * uploaded, posted, transmitted, distributed, or disclosed in any way without
  * Intel’s prior express written permission.
  *
  * No license under any patent, copyright, trade secret or other intellectual
  * property right is granted to or conferred upon you by disclosure or delivery
  * of the Materials, either expressly, by implication, inducement, estoppel or
  * otherwise.
  *
  * Any license under such intellectual property rights must be express and
  * approved by Intel in writing
  *
  ******************************************************************************/

#include <string.h>

#include "iq/itm/itm.h"
#include "iq/itm/topic_connection.h"
#include "iq/itm/topic_application.h"
#include "util/cunit_test.h"
#include "os/os_types.h"

/* Should return != 0 if they do not match */
#define STRCMP(a,b) ((a == NULL) || strcmp((char*)a, (char*)b))

#define INTEL_UUID 0x02
#define NOTIFICATION_UUID 'N'
#define EMAIL_UUID '@'
#define PHONECALL_UUID 'A'

#define INTEL_NOTIFICATION_EMAIL 0
#define INTEL_NOTIFICATION_PCALL 1
#define NUM_TOPICS 2

enum {
	TEST_TOPIC_INIT,
	TEST_ADD_APP1,
	TEST_ADD_APP2,
	TEST_START_APP1,
	TEST_ADD_CONNECTION,
	TEST_START_APP2,
	TEST_LIST_1_LEVEL,
	TEST_LIST_INTEL_LEVEL,
	TEST_LIST_INTEL_ONLY,
	TEST_LIST_INTEL_NOTIF_ONLY,
	TEST_LIST_INTEL_NOTIF_SUBS,
	TEST_LIST_INTEL_SUBS,
	TEST_LIST_NONEXISTENT,
	TEST_SUBSCRIBE_ALL_APP2,
	TEST_UNSUBSCRIBE_ALL_APP2,
	TEST_SUBSCRIBE_NOTIF_EMAIL,
	TEST_UNSUBSCRIBE_NOTIF_EMAIL,
	TEST_SUBSCRIBE_NOTIF_PHONECALL,
	TEST_UNSUBSCRIBE_NOTIF_EMAIL2,
	TEST_PUBLISH_UC_PHONECALL,
	TEST_PUBLISH_UC_EMAIL,
	TEST_PUBLISH_BC_PHONECALL,
	TEST_PUBLISH_BC_EMAIL,
	TEST_REQUEST_PHONECALL,
	TEST_RESPONSE_PHONECALL,
	TEST_UNSUBSCRIBE_NOTIF_PHONECALL,
};

extern struct topic_application m_app1;
extern struct topic_application m_app2;

uint32_t m_test_point;
uint8_t m_con_id = 0xFF;
uint32_t m_req_id;
bool m_rx_ok;


struct lo_connection {
	// this must be the first element in order no to pull in the container_of
	struct topic_connection connection;

	// number of transported packets
	uint16_t num_pkts;
};

/* Implementation of the local loopback */
int lo_connection_list_req(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern, uint8_t depth) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_list_req_cb(con, req_id, pattern, depth);
	return 0;
}

int lo_connection_list_rsp(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_list_rsp_cb(con, req_id, status, topic_ext_list);
	return 0;
}

int lo_connection_subscribe_req(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_subscribe_req_cb(con, req_id, pattern);
	return 0;
}

int lo_connection_subscribe_rsp(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * topic_list) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_subscribe_rsp_cb(con, req_id, status, topic_list);
	return 0;
}

int lo_connection_unsubscribe_req(struct topic_connection * con, uint8_t req_id, const uint8_t * pattern) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_unsubscribe_req_cb(con, req_id, pattern);
	return 0;
}

int lo_connection_unsubscribe_rsp(struct topic_connection * con, uint8_t req_id, uint8_t status) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_unsubscribe_rsp_cb(con, req_id, status);
	return 0;
}

int lo_connection_publish(const struct topic_application * app, struct topic_connection * con, const uint8_t * topic, const uint8_t * data, uint16_t len) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_publish_cb(con, topic, data, len);
	return 0;
}

int lo_connection_request(struct topic_connection * con, uint8_t req_id, const uint8_t * topic, const uint8_t * data, uint16_t len) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_request_cb(con, req_id, topic, data, len);
	return 0;
}

int lo_connection_response(struct topic_connection * con, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len) {
	struct lo_connection * lo = (void *)con;
	lo->num_pkts++;

	topic_connection_on_response_cb(con, req_id, status, data, len);
	return 0;
}

struct lo_connection m_connection =
{{
	lo_connection_list_req,
	lo_connection_list_rsp,
	lo_connection_subscribe_req,
	lo_connection_subscribe_rsp,
	lo_connection_unsubscribe_req,
	lo_connection_unsubscribe_rsp,
	lo_connection_publish,
	lo_connection_request,
	lo_connection_response
}, 0};

/* Implementation of the application 1 */
const uint8_t app1_topic_email[] = {INTEL_UUID, 1, NOTIFICATION_UUID, EMAIL_UUID, 0x00};
const uint8_t app1_topic_phonecall[] = {INTEL_UUID, 1, NOTIFICATION_UUID, PHONECALL_UUID, 0x00};

uint8_t const * const m_app1_topics[] = {
		[INTEL_NOTIFICATION_EMAIL] = app1_topic_email,
		[INTEL_NOTIFICATION_PCALL] = app1_topic_phonecall,
		NULL // end of list
		};

void app1_on_connect(uint8_t con_id) {
	CU_ASSERT("m_test_point == TEST_ADD_CONNECTION", m_test_point == TEST_ADD_CONNECTION);

	m_con_id = con_id;
	m_rx_ok = true;
}

void app1_on_disconnect(uint8_t con_id) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected disconnection", 0);
}

void app1_on_list_rsp(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("status == TOPIC_STATUS_OK", status == TOPIC_STATUS_OK);
	switch(m_test_point) {
	case TEST_LIST_1_LEVEL:
		// reply to the first level service (intel that has subtopics)
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02*\0\0", 4));
		break;
	case TEST_LIST_INTEL_LEVEL:
		// reply to the second level service (intel has only Notification with subtopics)
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02\x01*\0\x02\x02*\0\0", 9));
		break;
	case TEST_LIST_INTEL_ONLY:
		// reply to intel only (intel has subtopics)
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02*\0\0", 4));
		break;
	case TEST_LIST_INTEL_NOTIF_ONLY:
		// reply to intel/notification only
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02\x02N*\0\0", 6));
		break;
	case TEST_LIST_INTEL_NOTIF_SUBS:
		// reply to intel/notification subtopics
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02\x02N@\0\x02\x02NA\0\0", 11));
		break;
	case TEST_LIST_INTEL_SUBS:
		// reply to intel subtopics
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\x02\x02N@\0\x02\x02NA\0\0", 11));
		break;
	case TEST_LIST_NONEXISTENT:
		// reply to inexistant topic
		CU_ASSERT("topic_ext_list do not match", topic_ext_list && !memcmp(topic_ext_list, "\0", 1));
		break;
	default:
		CU_ASSERT("unexpected list rsp", 0);
	}
}

bool app1_on_subscribe_req(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected subscribe req", 0);
	return true;
}

void app1_on_subscribe_rsp(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_list) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("status == TOPIC_STATUS_OK", status == TOPIC_STATUS_OK);
	switch(m_test_point) {
	case TEST_SUBSCRIBE_ALL_APP2:
		// reply to intel/2/*
		CU_ASSERT("topic_list do not match", topic_list && !memcmp(topic_list, "\x02\x02N@\0\x02\x02NA\0\0", 11));
		m_rx_ok = true;
		break;
	case TEST_SUBSCRIBE_NOTIF_EMAIL:
		// reply to intel/2/email
		CU_ASSERT("topic_list do not match", topic_list && !memcmp(topic_list, "\x02\x02N@\0\0", 6));
		m_rx_ok = true;
		break;
	case TEST_SUBSCRIBE_NOTIF_PHONECALL:
		// reply to intel/2/phonecall
		CU_ASSERT("topic_list do not match", topic_list && !memcmp(topic_list, "\x02\x02NA\0\0", 6));
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected subscribe rsp", 0);
		break;
	}
}

void app1_on_unsubscribe_req(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	switch(m_test_point) {
	case TEST_UNSUBSCRIBE_ALL_APP2:
		// unsubscribe all
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL || topic_ix == INTEL_NOTIFICATION_PCALL);
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected unsubscribe req", 0);
		break;
	}
}

void app1_on_unsubscribe_rsp(uint8_t con_id, uint8_t req_id, uint8_t status) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	switch(m_test_point) {
	case TEST_UNSUBSCRIBE_ALL_APP2:
	case TEST_UNSUBSCRIBE_NOTIF_EMAIL:
		CU_ASSERT("status == TOPIC_STATUS_OK", status == TOPIC_STATUS_OK);
		m_rx_ok = true;
		break;
	case TEST_UNSUBSCRIBE_NOTIF_EMAIL2:
		CU_ASSERT("status == TOPIC_STATUS_FAIL", status == TOPIC_STATUS_FAIL);
		break;
	case TEST_UNSUBSCRIBE_NOTIF_PHONECALL:
		CU_ASSERT("status == TOPIC_STATUS_OK", status == TOPIC_STATUS_OK);
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected unsubscribe rsp", 0);
		break;
	}
}

void app1_on_publish(uint8_t con_id, const uint8_t * topic, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("topic != NULL", topic != NULL);
	CU_ASSERT("data != NULL", data != NULL);
	switch(m_test_point) {
	case TEST_PUBLISH_UC_PHONECALL:
		CU_ASSERT("topic do not match", STRCMP(topic, m_app2.topic_array[INTEL_NOTIFICATION_PCALL]) == 0);
		CU_ASSERT("data do not match", STRCMP(data, "phonecal1") == 0);
		m_rx_ok = true;
		break;
	case TEST_PUBLISH_UC_EMAIL:
		CU_ASSERT("topic do not match", STRCMP(topic, m_app2.topic_array[INTEL_NOTIFICATION_EMAIL]) == 0);
		CU_ASSERT("data do not match", STRCMP(data, "email1") == 0);
		m_rx_ok = true;
		break;
	case TEST_PUBLISH_BC_PHONECALL:
		CU_ASSERT("topic do not match", STRCMP(topic, m_app2.topic_array[INTEL_NOTIFICATION_PCALL]) == 0);
		CU_ASSERT("data do not match", STRCMP(data, "phonecal2") == 0);
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected publish", 0);
		break;
	}
}

void app1_on_tx_complete_cb(uint8_t con_id, struct on_tx_complete_params * params) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
}

void app1_on_request(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected request", 0);
}

void app1_on_response(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("status == TOPIC_STATUS_OK", status == TOPIC_STATUS_OK);
	switch(m_test_point) {
	case TEST_RESPONSE_PHONECALL:
		CU_ASSERT("data do not match", STRCMP(data, "phonecal_resp") == 0);
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected response", 0);
		break;
	}
}

void app1_on_subscribed(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL || topic_ix == INTEL_NOTIFICATION_PCALL);
}

struct topic_application m_app1 =
{
	m_app1_topics,
	app1_on_connect,
	app1_on_disconnect,
	app1_on_list_rsp,
	app1_on_subscribe_req,
	app1_on_subscribe_rsp,
	app1_on_subscribed,
	app1_on_unsubscribe_req,
	app1_on_unsubscribe_rsp,
	app1_on_publish,
	app1_on_tx_complete_cb,
	app1_on_request,
	app1_on_response
};

/* Implementation of the application 2 */
const uint8_t app2_topic_email[] = {INTEL_UUID, 2, NOTIFICATION_UUID, EMAIL_UUID, 0x00};
const uint8_t app2_topic_phonecall[] = {INTEL_UUID, 2, NOTIFICATION_UUID, PHONECALL_UUID, 0x00};
const uint8_t topic_intel[] = {INTEL_UUID, 0x00};

uint8_t const * const m_app2_topics[] = {
		[INTEL_NOTIFICATION_EMAIL] = app2_topic_email,
		[INTEL_NOTIFICATION_PCALL] = app2_topic_phonecall,
		NULL // end of list
		};

void app2_on_connect(uint8_t con_id) {
	CU_ASSERT("m_test_point == TEST_START_APP2", m_test_point == TEST_START_APP2);
	CU_ASSERT("m_con_id == con_id", m_con_id == con_id);
	m_rx_ok = true;
}

void app2_on_disconnect(uint8_t con_id) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected disconnection", 0);
}

void app2_on_list_rsp(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_ext_list) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected list response", 0);
}

bool app2_on_subscribe_req(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	switch(m_test_point) {
	case TEST_SUBSCRIBE_ALL_APP2:
		// subscribe all
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL || topic_ix == INTEL_NOTIFICATION_PCALL);
		break;
	case TEST_SUBSCRIBE_NOTIF_EMAIL:
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL);
		break;
	case TEST_SUBSCRIBE_NOTIF_PHONECALL:
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_PCALL);
		break;
	default:
		CU_ASSERT("unexpected subscribe request", 0);
		break;
	}
	return true;
}

void app2_on_subscribe_rsp(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * topic_list) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected subscribe response", 0);
}

void app2_on_unsubscribe_req(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	switch(m_test_point) {
	case TEST_UNSUBSCRIBE_ALL_APP2:
		// unsubscribe all
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL || topic_ix == INTEL_NOTIFICATION_PCALL);
		break;
	case TEST_UNSUBSCRIBE_NOTIF_EMAIL:
		// unsubscribe all
		CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL);
		break;
	case TEST_UNSUBSCRIBE_NOTIF_PHONECALL:
		break;
	default:
		CU_ASSERT("unexpected unsubscribe request", 0);
		break;
	}
}

void app2_on_unsubscribe_rsp(uint8_t con_id, uint8_t req_id, uint8_t status) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("unexpected unsubscribe response", 0);
}

void app2_on_publish(uint8_t con_id, const uint8_t * topic, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);

	// publish are dispatched to all applications because the subscriptions results are
	// not stored locally, but this is implementation dependent, so it is not verified
}

void app2_on_tx_complete_cb(uint8_t con_id, struct on_tx_complete_params * params) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
}

void app2_on_request(uint8_t con_id, uint8_t req_id, uint8_t topic_ix, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	switch(m_test_point) {
	case TEST_REQUEST_PHONECALL:
		m_req_id = req_id;
		m_rx_ok = true;
		break;
	default:
		CU_ASSERT("unexpected request", 0);
		break;
	}
}

void app2_on_response(uint8_t con_id, uint8_t req_id, uint8_t status, const uint8_t * data, uint16_t len) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
}

void app2_on_subscribed(uint8_t con_id, uint8_t topic_ix) {
	CU_ASSERT("con_id == m_con_id", con_id == m_con_id);
	CU_ASSERT("topic_ix do not match", topic_ix == INTEL_NOTIFICATION_EMAIL || topic_ix == INTEL_NOTIFICATION_PCALL);
}

struct topic_application m_app2 =
{
	m_app2_topics,
	app2_on_connect,
	app2_on_disconnect,
	app2_on_list_rsp,
	app2_on_subscribe_req,
	app2_on_subscribe_rsp,
	app2_on_subscribed,
	app2_on_unsubscribe_req,
	app2_on_unsubscribe_rsp,
	app2_on_publish,
	app2_on_tx_complete_cb,
	app2_on_request,
	app2_on_response
};

#define TEST_POINT(x) do {m_test_point = x; m_rx_ok = false;} while (0)

/**@brief ITM unit tests.
 *
 * @details Unit tests of the ITM
 */
void itm_test(void)
{
	int app1, app2;
	uint8_t iti[16];
	const uint8_t *ita[5];

	int err_code;

	cu_print("ITM service unit tests\n");

	cu_print("1 - Unit testing the helper functions\n");

	cu_print("1.1 - topic_index\n");
	CU_ASSERT("topic_index(m_app1_topics, &m_app1_topics[0]) == 0", topic_index(m_app1_topics, &m_app1_topics[0]) == 0);
	CU_ASSERT("topic_index(m_app1_topics, &m_app1_topics[1]) == 1", topic_index(m_app1_topics, &m_app1_topics[1]) == 1);
	CU_ASSERT("topic_index(m_app1_topics, &m_app1_topics[2]) == 2", topic_index(m_app1_topics, &m_app1_topics[2]) == 2);

	cu_print("1.2 - topic_size\n");
	CU_ASSERT("topic_size(m_app1_topics[0]) == 5", topic_size(m_app1_topics[0]) == 5);
	CU_ASSERT("topic_size(m_app1_topics[1]) == 5", topic_size(m_app1_topics[1]) == 5);
	CU_ASSERT("topic_size(topic_intel) == 2", topic_size(topic_intel) == 2);

	cu_print("1.2.1 - topic_list_size\n");
	CU_ASSERT("topic_list_size(NULL) == 0", topic_list_size(NULL) == 0);
	CU_ASSERT("topic_list_size(empty topic) == 1", topic_list_size((uint8_t*)"") == 1);
	CU_ASSERT("topic_list_size(one topic) == 5", topic_list_size((uint8_t*)"\x01\x02\x03\x00") == 5);
	CU_ASSERT("topic_list_size(two topic) == 11", topic_list_size((uint8_t*)"\x01\x02\x03\x00\x04\x05\x06\x07\x08\x00") == 11);

	cu_print("1.3 - topic_depth\n");
	CU_ASSERT("topic_depth(m_app1_topics[0]) == 4", topic_depth(m_app1_topics[0]) == 4);
	CU_ASSERT("topic_depth(m_app1_topics[1]) == 4", topic_depth(m_app1_topics[1]) == 4);
	CU_ASSERT("topic_depth(topic_intel) == 1", topic_depth(topic_intel) == 1);

	cu_print("1.4 - topic_find\n");
	CU_ASSERT("topic_find(m_app1_topics, app1_topic_email) == 0", topic_find(m_app1_topics, app1_topic_email) == 0);
	CU_ASSERT("topic_find(m_app1_topics, app1_topic_phonecall) == 1", topic_find(m_app1_topics, app1_topic_phonecall) == 1);
	CU_ASSERT("topic_find(m_app1_topics, topic_intel) == 0xFF", topic_find(m_app1_topics, topic_intel) == 0xFF);

	cu_print("1.5 - topic_match\n");
	CU_ASSERT("topic_match((uint8_t*)\"\", (uint8_t*)\"\") == true", topic_match((uint8_t*)"", (uint8_t*)"") == true);
	CU_ASSERT("topic_match((uint8_t*)\"\", (uint8_t*)\"a\") == false", topic_match((uint8_t*)"", (uint8_t*)"a") == false);
	CU_ASSERT("topic_match((uint8_t*)\"a\", (uint8_t*)\"\") == false", topic_match((uint8_t*)"a", (uint8_t*)"") == false);
	CU_ASSERT("topic_match((uint8_t*)\"a\", (uint8_t*)\"a\") == true", topic_match((uint8_t*)"a", (uint8_t*)"a") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abcd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"abcd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abc\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"abc") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abc\", (uint8_t*)\"abcd\") == false", topic_match((uint8_t*)"abc", (uint8_t*)"abcd") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abc+\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"abc+") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"ab+d\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"ab+d") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"a+cd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"a+cd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"+bcd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"+bcd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"+bc\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"+bc") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"+b\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"+b") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"+\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"+") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abc\", (uint8_t*)\"+\") == false", topic_match((uint8_t*)"abc", (uint8_t*)"+") == false);
	CU_ASSERT("topic_match((uint8_t*)\"ab\", (uint8_t*)\"+\") == false", topic_match((uint8_t*)"ab", (uint8_t*)"+") == false);
	CU_ASSERT("topic_match((uint8_t*)\"a\", (uint8_t*)\"+\") == true", topic_match((uint8_t*)"a", (uint8_t*)"+") == true);
	CU_ASSERT("topic_match((uint8_t*)\"a\", (uint8_t*)\"*\") == true", topic_match((uint8_t*)"a", (uint8_t*)"*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"ab\", (uint8_t*)\"*\") == true", topic_match((uint8_t*)"ab", (uint8_t*)"*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abc\", (uint8_t*)\"*\") == true", topic_match((uint8_t*)"abc", (uint8_t*)"*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"**\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"**") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"a*\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"a*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"ab*\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"ab*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abc*\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"abc*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abcd*\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"abcd*") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"abc*d\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"abc*d") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"ab*d\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"ab*d") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"a*d\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"a*d") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*abcd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*abcd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*bcd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*bcd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*cd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*cd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*d\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*d") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*a\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"*a") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*bc\") == false", topic_match((uint8_t*)"abcd", (uint8_t*)"*bc") == false);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"*abcd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"*abcd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcd\", (uint8_t*)\"ab*cd\") == true", topic_match((uint8_t*)"abcd", (uint8_t*)"ab*cd") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcdebcde\", (uint8_t*)\"a*cde\") == true", topic_match((uint8_t*)"abcdebcde", (uint8_t*)"a*cde") == true);
	CU_ASSERT("topic_match((uint8_t*)\"abcdeb\", (uint8_t*)\"a*cde\") == false", topic_match((uint8_t*)"abcdeb", (uint8_t*)"a*cde") == false);

	cu_print("2 - Unit testing the core functions\n");

	cu_print("2.1 - initialization\n");
	// initialize the connection base class
	TEST_POINT(TEST_TOPIC_INIT);

	// add the applications
	TEST_POINT(TEST_ADD_APP1);
	err_code = itm_add_application(&m_app1);
	CU_ASSERT("err_code invalid", err_code >= 0);
	app1 = err_code;
	TEST_POINT(TEST_ADD_APP2);
	err_code = itm_add_application(&m_app2);
	CU_ASSERT("err_code invalid", err_code >= 0);
	app2 = err_code;
	err_code = itm_add_application(&m_app2);
	CU_ASSERT("err_code valid and should not", err_code == E_OS_ERR_NOT_ALLOWED);

	// start the first application
	TEST_POINT(TEST_START_APP1);
	err_code = itm_start_application(app1);
	CU_ASSERT("err_code invalid", err_code == 0);
	err_code = itm_start_application(2);
	CU_ASSERT("err_code valid and should not", err_code == E_OS_ERR);

	// send a command before there is a connection
	iti[0] = 0;
	err_code = itm_list_req(app1, 0, iti, 1);
	CU_ASSERT("err_code invalid", err_code == E_OS_ERR);

	// add the connection
	TEST_POINT(TEST_ADD_CONNECTION);
	topic_connection_on_connect_cb(&m_connection.connection);
	CU_ASSERT("m_con_id != 0xFF", m_con_id != 0xFF);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	// send a command before application is started
	iti[0] = 0;
	err_code = itm_list_req(app2, 0, iti, 1);
	CU_ASSERT("err_code invalid", err_code == E_OS_ERR_NOT_ALLOWED);

	// start the second application
	TEST_POINT(TEST_START_APP2);
	err_code = itm_start_application(app2);
	CU_ASSERT("err_code invalid", err_code == 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.2 - topics discovery\n");
	cu_print("2.2.1 - Discovering all first level topics\n");
	TEST_POINT(TEST_LIST_1_LEVEL);
	iti[0] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 1);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.2 - Discovering INTEL subtopics\n");
	TEST_POINT(TEST_LIST_INTEL_LEVEL);
	iti[0] = INTEL_UUID;
	iti[1] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 2);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.3 - Discovering INTEL only\n");
	TEST_POINT(TEST_LIST_INTEL_ONLY);
	iti[0] = INTEL_UUID;
	iti[1] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 1);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.4 - Discovering INTEL/Notification only\n");
	TEST_POINT(TEST_LIST_INTEL_NOTIF_ONLY);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 3);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.5 - Discovering INTEL/Notification subtopics\n");
	TEST_POINT(TEST_LIST_INTEL_NOTIF_SUBS);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 4);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.6 - Discovering INTEL subtopics\n");
	TEST_POINT(TEST_LIST_INTEL_SUBS);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 4);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.2.7 - Discovering inexistent topic\n");
	TEST_POINT(TEST_LIST_NONEXISTENT);
	iti[0] = NOTIFICATION_UUID;
	iti[1] = 0;
	err_code = itm_list_req(app1, m_con_id, iti, 10);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.3 - topics subscription\n");
	cu_print("2.3.1 - Subscribe to all app2 topics\n");
	TEST_POINT(TEST_SUBSCRIBE_ALL_APP2);
	err_code = itm_subscribe_req(app1, m_con_id, m_app2_topics);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.3.2 - Unsubscribe from all app2 topics\n");
	TEST_POINT(TEST_UNSUBSCRIBE_ALL_APP2);
	err_code = itm_unsubscribe_req(app1, m_con_id, m_app2_topics);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.3.3 - Subscribe to intel/app2/notification/email topic\n");
	TEST_POINT(TEST_SUBSCRIBE_NOTIF_EMAIL);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = EMAIL_UUID;
	iti[4] = 0;
	ita[0] = iti;
	ita[1] = NULL;
	err_code = itm_subscribe_req(app1, m_con_id, ita);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.3.4 - Unsubscribe from intel/app2/notification/email topic\n");
	TEST_POINT(TEST_UNSUBSCRIBE_NOTIF_EMAIL);
	err_code = itm_unsubscribe_req(app1, m_con_id, ita);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.3.5 - Subscribe to intel/app2/notification/phonecall topic\n");
	TEST_POINT(TEST_SUBSCRIBE_NOTIF_PHONECALL);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = PHONECALL_UUID;
	iti[4] = 0;
	ita[0] = iti;
	ita[1] = NULL;
	err_code = itm_subscribe_req(app1, m_con_id, ita);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.3.6 - Unsubscribe from intel/app2/notification/email topic\n");
	TEST_POINT(TEST_UNSUBSCRIBE_NOTIF_EMAIL2);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = EMAIL_UUID;
	iti[4] = 0;
	ita[0] = iti;
	ita[1] = NULL;
	err_code = itm_unsubscribe_req(app1, m_con_id, ita);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.4 - topics publish\n");
	cu_print("2.4.1 - Unicast data on phonecall\n");
	TEST_POINT(TEST_PUBLISH_UC_PHONECALL);
	err_code = itm_publish(app2, m_con_id, INTEL_NOTIFICATION_PCALL, (uint8_t*)"phonecal1", 10);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.4.2 - Unicast data on email\n");
	TEST_POINT(TEST_PUBLISH_UC_EMAIL);
	err_code = itm_publish(app2, m_con_id, INTEL_NOTIFICATION_EMAIL, (uint8_t*)"email1", 7);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.4.3 - Broadcast data on phonecall\n");
	TEST_POINT(TEST_PUBLISH_BC_PHONECALL);
	err_code = itm_publish(app2, 0xFF, INTEL_NOTIFICATION_PCALL, (uint8_t*)"phonecal2", 10);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.4.3 - Broadcast data on email\n");
	TEST_POINT(TEST_PUBLISH_BC_EMAIL);
	err_code = itm_publish(app2, 0xFF, INTEL_NOTIFICATION_EMAIL, (uint8_t*)"email2", 10);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == false", m_rx_ok == false);

	cu_print("2.5 - topics request\n");
	cu_print("2.5.1 - Request on phonecall\n");
	TEST_POINT(TEST_REQUEST_PHONECALL);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = PHONECALL_UUID;
	iti[4] = 0;
	err_code = itm_request(app1, m_con_id, iti, (uint8_t*)"phonecal2", 10);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.5.2 - Response on phonecall\n");
	TEST_POINT(TEST_RESPONSE_PHONECALL);
	err_code = itm_response(m_con_id, m_req_id, TOPIC_STATUS_OK, (uint8_t*)"phonecal_resp", 14);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("2.6 - close test\n");
	TEST_POINT(TEST_UNSUBSCRIBE_NOTIF_PHONECALL);
	iti[0] = INTEL_UUID;
	iti[1] = 2;
	iti[2] = NOTIFICATION_UUID;
	iti[3] = PHONECALL_UUID;
	iti[4] = 0;
	ita[0] = iti;
	ita[1] = NULL;
	err_code = itm_unsubscribe_req(app1, m_con_id, ita);
	CU_ASSERT("err_code invalid", err_code >= 0);
	CU_ASSERT("m_rx_ok == true", m_rx_ok == true);

	cu_print("End of ITM unit tests\n");

}
