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

/**
 * \file CFW.c main test file
 *
 * This file contains a main test code.
 */

#include <stdio.h>
#include <stdlib.h>
#include "cfw/cfw.h"
#include "services/test_service/test_service.h"
#include "services/services_ids.h"
#include "cfw/cfw_debug.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "machine.h"
#include "infra/log.h"

cfw_service_conn_t * test_service_conn = NULL;

#ifdef TRACK_ALLOCS
extern int alloc_count;
#endif

void my_handle_message(struct cfw_message * msg, void * param) {
	pr_info(LOG_MODULE_MAIN, "%s:%s for param: %s conn:%p", __FILE__, __func__, (char*)param, msg->conn);
	cfw_dump_message(msg);

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		pr_info(LOG_MODULE_MAIN, "%s:%s for conn: %s", __FILE__, __func__, (char*)msg->priv);
		cfw_open_conn_rsp_msg_t * cnf = (cfw_open_conn_rsp_msg_t*)msg;
		int events[1] = {MSG_ID_TEST_1_EVT};
		test_service_conn = cnf->service_conn;
		cfw_register_events(test_service_conn, events, 1, msg->priv);
	}
	break;

	case MSG_ID_CFW_REGISTER_EVT_RSP:
		if (!strcmp(msg->priv, "Conn2")) {
			test_service_test_1(test_service_conn, "Coucou");
		}
		break;

	case MSG_ID_TEST_1_RSP: {
		pr_info(LOG_MODULE_MAIN, "got MSG_ID_TEST_1_RSP Priv: %s", (char*)msg->priv);
		test_service_test_2(test_service_conn, "Testing 2");
		break;
	}
	case MSG_ID_TEST_2_RSP: {
		pr_info(LOG_MODULE_MAIN, "got MSG_ID_TEST_2_RSP Priv: %s", (char*)msg->priv);
		test_service_test_1(test_service_conn, "Testing 1");
		break;
	}
	}
	cfw_msg_free(msg);

}

extern void _cfw_loop(void * );

void * queue;

struct elem {
    list_t list;
    int value;
};

list_head_t test_list = {NULL, NULL};

struct elem elements[] = {{NULL, 1}, {NULL, 2}, {NULL, 3}};

void test_cb(void *e, void *p)
{
    struct elem *elem = (struct elem *)e;
    printf("%d->%d[%p] ", elem->value, elem->list.next ? ((struct elem *)elem->list.next)->value : -1, elem->list.next);
}

int test_cb_del(void *e, void *p)
{
    struct elem *elem = (struct elem *)e;
    int to_remove = *(int*)p;
    printf("element: %d toremove: %d\n", elem->value, to_remove);
    return elem->value == to_remove;
}

void sanity_test_list() {
    list_add(&test_list, &elements[0].list);
    list_add(&test_list, &elements[1].list);
    list_add(&test_list, &elements[2].list);

    list_foreach(&test_list, test_cb, NULL);
    printf("\n");
    int elem_to_remove = 2;

    list_foreach_del(&test_list, test_cb_del, (void*)&elem_to_remove);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    list_add(&test_list, &elements[1].list);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    list_foreach_del(&test_list, test_cb_del, (void*)&elem_to_remove);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    list_add(&test_list, &elements[1].list);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    elem_to_remove = 1;

    list_foreach_del(&test_list, test_cb_del, (void*)&elem_to_remove);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    elem_to_remove = 3;

    list_foreach_del(&test_list, test_cb_del, (void*)&elem_to_remove);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

    elem_to_remove = 2;

    list_foreach_del(&test_list, test_cb_del, (void*)&elem_to_remove);
    list_foreach(&test_list, test_cb, NULL);
    printf("\n");

}

void test_timers(void);

int main(void) {
	queue = queue_create(10, NULL);
	cfw_service_mgr_init(queue);

	sanity_test_list();
	test_timers();

	test_service_init(queue, TEST_SERVICE_ID);

	pr_info(LOG_MODULE_MAIN, "End of init....");
	pr_info(LOG_MODULE_MAIN, "Start of real stuff...\n");

	cfw_client_t *c = cfw_client_init(queue, my_handle_message, "Client 1");
	//cfw_client_t *c2 = cfw_client_init(queue, my_handle_message, "Client 2");

	cfw_open_service_conn(c, TEST_SERVICE_ID, "Conn1");
	cfw_open_service_conn(c, TEST_SERVICE_ID, "Conn2");

	_cfw_loop(queue);
	return EXIT_SUCCESS;
}
