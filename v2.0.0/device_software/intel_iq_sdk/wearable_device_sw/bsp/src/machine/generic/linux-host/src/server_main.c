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
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_messages.h"
#include "machine.h"

#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define __USE_BSD
#define __USE_MISC
#include <termios.h>
#include <stdio.h>
#include "infra/ipc_requests.h"
#include "infra/log.h"
#include "infra/message.h"
#include "infra/port.h"
#include "infra/ipc_uart.h"
#include "ipc_uart_host.h"
#include "host_common.h"
#include "services/test_service/test_service.h"
#include "services/ble_service/ble_service_api.h"


#define UART_PORT "/dev/ttyACM0"
#define SOCKET_NAME "/tmp/cfw"
#define MAX_CLIENT_SVC	10

/* default baudrate to use */
static speed_t baud_rate = B1000000 /*B115200*/;
static speed_t baudrate_to_termios(uint32_t baudrate);


struct _srv_client_cb {
	int state;
	int svc_id;
};

struct _srv_cb {
	T_QUEUE queue;
	/* client used for cfw client server operation */
	cfw_client_t * client;
	int ble_svc_started;
	struct _srv_client_cb client_svc_cb[MAX_CLIENT_SVC];
};

static struct _srv_cb _server_cb = {0};

extern int cfw_service_registered(int service_id);
/**
 * IPC CFW message format is the following (little endian):
 * -------------------------------------
 * |  len: 2 bytes  | chan 1 byte: sender cpu id: 1 byte |
 * -------------------------------------
 * | REQUEST_ID   | payload             |
 * -------------------------------------
 *
 * For TYPE_MESSAGE request, the payload is the message copy.
 * For TYPE_FREE is not valid (this ipc is not shared mem based)
 */
int ipc_uart_message_cback(int channel, int request, int len,
                void * p_data)
{
    int ret = 1;
    printf("%s: request: %d\n", __func__, request);

    switch (request) {
        case IPC_MSG_TYPE_MESSAGE:
                break;
        default: break;
    }
    if (ret)
        bfree(p_data);
    return ret;
}

static void start_service(struct _srv_cb *p_cb, int svc_id)
{
	printf("server_main::%s(): BLE_CORE_SERVICE_ID available:"
			" priv: 0x%p, svc_id: %d\n", __func__, p_cb,
			svc_id);
	int i;
	for (i = 0; i < MAX_CLIENT_SVC; i++)
		if ((p_cb->client_svc_cb[i].svc_id != svc_id) &&
				(p_cb->client_svc_cb[i].state == 0))
			break;
	sleep(1);
	printf("%s(): slot found: %d\n", __func__, i);

	if (i < MAX_CLIENT_SVC) {
		p_cb->client_svc_cb[i].state = 1;
		p_cb->client_svc_cb[i].svc_id = svc_id;
		cfw_open_service_conn(p_cb->client, svc_id,
				&p_cb->client_svc_cb[i]);
	} else {
		printf("server_main::%s(): NO free srv_cb slot"
				" found for svc_id: %d\n", __func__,
				svc_id);
	}
}

static void srv_client_handle_msg(struct cfw_message * msg, void * data)
{
	cfw_dump_message(msg);
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP:
	{
		cfw_open_conn_rsp_msg_t *resp = (cfw_open_conn_rsp_msg_t *)msg;
		printf("server_main:%s(): MSG_ID_CFW_OPEN_SERVICE_RSP::port_id: %d, cpu_id: %d, "
				"service_conn: 0x%p, priv 0x%p\n", __func__,
				resp->port, resp->cpu_id, resp->service_conn,
				msg->priv);
		break;
	}
	case MSG_ID_CFW_REGISTER_SVC_AVAIL_EVT_RSP:
		printf("server_main::%s(): MSG_ID_CFW_REGISTER_SVC_AVAIL_EVT_RSP, priv:"
				" 0x%p\n",
				__func__, msg->priv);
		break;
	case MSG_ID_CFW_SVC_AVAIL_EVT: {
		cfw_svc_available_evt_msg_t * evt =
				(cfw_svc_available_evt_msg_t *)msg;
		if ((evt->service_id == BLE_CORE_SERVICE_ID) ||
				(0 != cfw_service_registered(
						BLE_CORE_SERVICE_ID))) {
			_server_cb.ble_svc_started = 1;
			printf("\n%s(): BLE_CORE_SERVICE_ID registered!\n", __func__);
#ifdef CONFIG_BLE_CORE_TEST
			extern void test_ble_service_init(void);

			test_ble_service_init();
#endif
		}
		break;
		}
	default:
		pr_info(LOG_MODULE_MAIN, "server_main::%s(): Unhandled Msg: 0x%x",
				__func__, CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

#ifdef CONFIG_BLE_CORE_TEST
extern void test_ble_service_update_battery_level(uint8_t level);
static uint8_t battery_level = 90;
#endif

/**
 * \brief wait for a client to connect.
 *
 * \param socket_name the socket file name to listen to.
 *
 * \return the file descriptor of the client socket.
 */
int wait_client(char * socket_name, char *uart_port)
{
    struct sockaddr_un addr;
    int fd,cl,ufd,flags;
    bool display_msg_check = false;
    struct termios termios_cfg;

    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_name, sizeof(addr.sun_path)-1);

    unlink(socket_name);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(-1);
    }

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	while (1) {
		if (!display_msg_check) {
			printf
			    ("UART/socket not ready. Waiting for connection...\n");
			display_msg_check = true;
		}
		if ((cl = accept(fd, NULL, NULL)) != -1) {
			fcntl(fd, F_SETFL, flags);
			pr_info(LOG_MODULE_MAIN, "Server: accepting client socket cl: %d", cl);
			return cl;
		}

		if ((ufd = open(uart_port, O_RDWR)) != -1) {
			fcntl(fd, F_SETFL, flags);
			tcflush(ufd, TCIOFLUSH); /* to avoid reading old data */
			tcgetattr(ufd, &termios_cfg);
			cfmakeraw(&termios_cfg);
			termios_cfg.c_cflag |= (CRTSCTS);
			//termios_cfg.c_cflag &= ~(CRTSCTS);
			cfsetospeed(&termios_cfg, baud_rate);
			cfsetispeed(&termios_cfg, baud_rate);
			tcsetattr(ufd, TCSANOW,
					&termios_cfg);
			tcflush(ufd, TCIOFLUSH); /* to avoid reading old data */

			pr_info(LOG_MODULE_MAIN, "Server: opening uart <%s>, fd: %d", uart_port, ufd);
			return ufd;
		}
	}
	return -1;
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

void main(int argc, char** argv)
{
    int loops = 0;
    void * cfw_channel;

    if (argc > 1) {
	    baud_rate = baudrate_to_termios(strtol(argv[1], NULL, 0));
    } else {
	    baud_rate = baudrate_to_termios(0); /* forcing 115200 */
    }
    /* Set the remote CPU handlers */
    set_cpu_message_sender(CPU_ID_BLE, linux_ipc_send_message);
    set_cpu_message_sender(CPU_ID_ARC, linux_ipc_send_message);
    set_cpu_message_sender(CPU_ID_HOST, linux_ipc_send_message);
    set_cpu_free_handler(CPU_ID_BLE, linux_ipc_free);
    set_cpu_free_handler(CPU_ID_ARC, linux_ipc_free);
    set_cpu_free_handler(CPU_ID_HOST, linux_ipc_free);
    set_cpu_id(CPU_ID_QUARK);


    int uart_fd = wait_client(SOCKET_NAME, UART_PORT);

    printf("Client connected on %d\n", uart_fd);
    uart_ipc_init(uart_fd);
    fd_set fds;
    FD_ZERO(&fds);
    struct timeval tv;
    _server_cb.queue = queue_create(10, NULL);
    cfw_service_mgr_init(_server_cb.queue);

    /* We use the main task queue to support most services */
    set_service_queue(_server_cb.queue);

    _server_cb.client = (cfw_client_t *)cfw_client_init(_server_cb.queue, srv_client_handle_msg,
		    &_server_cb);
    /* get informed on new services */
    cfw_register_svc_available(_server_cb.client, BLE_CORE_SERVICE_ID, &_server_cb);

    /* some test services */
    test_service_init(_server_cb.queue, TEST_SERVICE_ID);

    /* ble service */
    ble_cfw_service_init(_server_cb.queue);

    cfw_channel = ipc_uart_channel_open(0, ipc_uart_message_cback);
    linux_ipc_set_channel(cfw_channel);

    while(1) {
        FD_SET(uart_fd, &fds);
        FD_SET(0, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(uart_fd + 1, &fds, NULL, NULL, &tv);
        if (ret == -1) {
            perror("select()");
        } else if (ret) {
            if (FD_ISSET(0, &fds)) {
            } else if(FD_ISSET(uart_fd, &fds)) {
                uart_ipc_handle_data(uart_fd);
            }
        } else {
            printf(".");
#ifdef CONFIG_BLE_CORE_TEST
            test_ble_service_update_battery_level(battery_level--);
            if (battery_level == 10)
        	    battery_level = 100;
#endif
        }
        cfw_process(_server_cb.queue);
    }
}

static speed_t baudrate_to_termios(uint32_t baudrate)
{
	pr_info(LOG_MODULE_MAIN, "%s(): baudrate: %d", __func__, baudrate);

	switch(baudrate) {
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	case 2000000:
		return B2000000;
	case 2500000:
		return B2500000;
	case 3000000:
		return B3000000;
	case 3500000:
		return B3500000;
	case 4000000:
		return B4000000;
	default:
		pr_info(LOG_MODULE_MAIN, "%s(): WARNING: %d does not match any termios "
				"baudrate!\nUsing default: 115200", __func__, baudrate);
		break;
	}
	return B115200;
}
