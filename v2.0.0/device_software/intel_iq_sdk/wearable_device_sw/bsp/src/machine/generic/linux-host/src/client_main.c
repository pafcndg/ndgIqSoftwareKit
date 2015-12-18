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
#include "os/os.h"
#include "cfw/cfw_debug.h"
#include "machine.h"

#include <stdarg.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cfw/cfw_internal.h"
#include "cfw/cfw_messages.h"
#include "services/test_service/test_service.h"
#include "services/services_ids.h"
#include "host_common.h"
#include "infra/ipc_requests.h"
#include "infra/ipc_uart.h"
#include "infra/log.h"
#include "infra/message.h"
#include "ipc_uart_host.h"

static service_t ** g_svc = NULL;

int ipc_uart_message_cback(int channel, int request, int len, void * p_data)
{
    int ret = 1;

    printf("Got IPC message: len: %d\n", len);
    switch (request) {
    case IPC_MSG_TYPE_MESSAGE:
            break;
    default: break;
    }
    if (ret)
        bfree(p_data);
    return ret;
}

int connect_server(char * socket_name)
{
    struct sockaddr_un addr;
    char buf[100];
    int fd,rc;

    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_name, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }

    return fd;
}

void cfw_process(T_QUEUE queue)
{
    struct cfw_message * message;
    T_QUEUE_MESSAGE m;

    queue_get_message(queue, &m, OS_WAIT_FOREVER, NULL);
    message = (struct cfw_message *) m;
    if ( message != NULL ) {
        pr_info(LOG_MODULE_MAIN, "Got message: %p", message);
        port_process_message(&message->m);
    }
}

cfw_service_conn_t * test_service_conn = NULL;


void my_handle_message(struct cfw_message * msg, void * param)
{
    pr_info(LOG_MODULE_MAIN, "%s:%s for param: %s conn:%p", __FILE__, __func__, (char*)param, msg->conn);
    cfw_dump_message(msg);

    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP: {
        pr_info(LOG_MODULE_MAIN, "%s:%s for conn: %s", __FILE__, __func__, (char*)msg->priv);
        cfw_open_conn_rsp_msg_t * cnf = (cfw_open_conn_rsp_msg_t*)msg;
        int events[1] = {MSG_ID_TEST_1_EVT};
        test_service_conn = cnf->service_conn;
        cfw_register_events(test_service_conn, events, 1, msg->priv);
        break;
    }

    case MSG_ID_CFW_REGISTER_EVT_RSP:
        {
            if (!strcmp(msg->priv, "Conn2")) {
                test_service_test_1(test_service_conn, "Coucou");
            }
            break;
        }

    case MSG_ID_TEST_1_RSP:
        {
            pr_info(LOG_MODULE_MAIN, "got MSG_ID_TEST_1_RSP Priv: %s", (char*)msg->priv);
            test_service_test_2(test_service_conn, "Testing 2");
            break;
        }

    case MSG_ID_TEST_2_RSP:
        {
            pr_info(LOG_MODULE_MAIN, "got MSG_ID_TEST_2_RSP Priv: %s", (char*)msg->priv);
            test_service_test_1(test_service_conn, "Testing 1");
            break;
        }
    default:
        {
            pr_info(LOG_MODULE_MAIN, "%s: Unhandled message: %d", __func__, CFW_MESSAGE_ID(msg));
            break;
        }
    }
    cfw_msg_free(msg);
}

void main(int argc, char** argv)
{
    T_QUEUE queue;
    void * cfw_channel;

    /* Set the remote CPU handlers */
    set_cpu_message_sender(CPU_ID_ARC, linux_ipc_send_message);
    set_cpu_message_sender(CPU_ID_QUARK, linux_ipc_send_message);
    set_cpu_free_handler(CPU_ID_ARC, linux_ipc_free);
    set_cpu_free_handler(CPU_ID_QUARK, linux_ipc_free);
    set_cpu_id(CPU_ID_HOST);

    int uart_fd = connect_server("/tmp/cfw");
    printf("Client connected to server: %d\n", uart_fd);

    uart_ipc_init(uart_fd);
    fd_set fds;
    FD_ZERO(&fds);
    struct timeval tv;

    queue = queue_create(10, NULL);
	void * ports = port_alloc_port_table(20);

	port_set_port_id(FRAMEWORK_SERVICE_ID);
	port_set_cpu_id(FRAMEWORK_SERVICE_ID, CPU_ID_QUARK);

    _cfw_init_proxy(queue, ports, g_svc, 1);

    cfw_channel = ipc_uart_channel_open(0, ipc_uart_message_cback);
    linux_ipc_set_channel(cfw_channel);

    cfw_client_t * c = cfw_client_init(queue, my_handle_message, "Client 1");

    cfw_open_service_conn(c, TEST_SERVICE_ID, "Conn1");
    cfw_open_service_conn(c, TEST_SERVICE_ID, "Conn2");

    while(1) {
        FD_SET(uart_fd, &fds);
        FD_SET(0, &fds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(uart_fd + 1, &fds, NULL, NULL, &tv);
        if (ret == -1) {
            perror("select()");
        } else if (ret) {
            if (FD_ISSET(0, &fds)) {
                //TODO: handle_input();
            } else if(FD_ISSET(uart_fd, &fds)) {
                uart_ipc_handle_data(uart_fd);
            }
        } else {
            /* printf("timeout"); */
        }
        cfw_process(queue);
    }
}
