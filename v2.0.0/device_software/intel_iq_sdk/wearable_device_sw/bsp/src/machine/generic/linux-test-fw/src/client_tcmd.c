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

#include "os/os.h"
#include "cfw/cfw.h"
#include "cfw/cfw_debug.h"
#include "machine.h"
#ifndef __USE_BSD
#define __USE_BSD
#endif
#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cfw/cfw_internal.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "cfw/cfw_messages.h"
#include "services/test_service/test_service.h"
#include "services/gpio_service/gpio_service.h"

#include "infra/ipc_requests.h"
#include "infra/ipc_uart.h"

static service_t ** g_svc = NULL;

uint32_t interrupt_lock()
{
	return 0;
}

void interrupt_unlock(uint32_t flags)
{
}

void panic(int err)
{
	printf("\npanic(%d)\n", err);
	while (1)
		;
}

void printk(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void cfw_log(char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void cfw_msg_free(struct cfw_message * msg)
{
	free(msg);
}
uint32_t timer_hal_get_ms()
{
	printf("%s: timer_hal not implemented\n", __func__);
	return 0;
}
void timer_hal_trigger(uint32_t timeout)
{
	printf("%s: timer_hal not implemented\n", __func__);
}
void timer_hal_init()
{
	printf("%s: timer_hal not implemented\n", __func__);
}


static int count = 0;

uint16_t cfw_get_service_mgr_port_id()
{
	return 1;
}

void default_handle_message(struct cfw_message * msg, void * param)
{
	cfw_log("%s:%s for param: %s conn:%p id:%d\n", __FILE__, __func__,
			(char*) param, msg->conn, CFW_MESSAGE_ID(msg));
	cfw_msg_free(msg);
}

void client_handle_message(struct cfw_message * msg, void *param)
{
	_cfw_client_t * c = (_cfw_client_t*) param;
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_RSP: {
		cfw_open_conn_rsp_msg_t * cnf = (cfw_open_conn_rsp_msg_t *) msg;
		/** Make client handle point to server handle */
		((cfw_service_conn_t*) cnf->service_conn)->server_handle =
				cnf->svc_server_handle;
		/** Initialize service port. */
		((cfw_service_conn_t*) cnf->service_conn)->port = cnf->port;
		break;
	}
	case MSG_ID_CFW_CLOSE_SERVICE_RSP: {
		/* Free client-side conn */
	        bfree(msg->conn);
		break;
	}
	default:
		//Nothing to do
		break;
	}
	c->handle_msg(msg, c->data);
}

char bin2nib(unsigned char val)
{
	if (val < 10) {
		return val + '0';
	} else {
		return val - 10 + 'a';
	}
}

void bin2hex(char * dst, unsigned char * src, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		dst[2 * i] = bin2nib((src[i] >> 4) & 0xf);
		dst[2 * i + 1] = bin2nib((src[i]) & 0xf);
	}
}

unsigned char nib2bin(char c)
{
	if (c <= '9') {
		return c - '0';
	}
	if (c <= 'F') {
		return c - 'A' + 10;
	} else {
		return c - 'a' + 10;
	}
}

void hex2bin(unsigned char * dst, char * src, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		dst[i] = nib2bin(src[i * 2]) << 4 | nib2bin(src[i * 2 + 1]);
	}
}

struct message * message_alloc(int size, OS_ERR_TYPE *err)
{
	return (struct message *) malloc(size);
}

cfw_client_t * client;

cfw_client_t * cfw_client_init(void * queue, handle_msg_cb_t cb, void * param)
{
	_cfw_client_t * ret = (_cfw_client_t *) malloc(sizeof(*ret));
	if (!ret)
		panic(E_OS_ERR_NO_MEMORY);
	ret->client_port_id = 0;
	ret->data = param;
	ret->handle_msg = cb;
	return ret;
}

void test_fw_set_handler(handle_msg_cb_t cb, void * param)
{
	_cfw_client_t * c = (_cfw_client_t *) client;
	c->data = param;
	c->handle_msg = cb;
}

int uart_fd;

#define STATE_INIT      0
#define STATE_POLL      1
#define STATE_WAIT_OK   2
#define STATE_POLL_WAIT 3

int state = STATE_POLL;

T_QUEUE tx_queue;

int _cfw_send_message(struct cfw_message * cfw_msg)
{
	queue_send_message(tx_queue, cfw_msg, NULL);
	return E_OS_OK;
}


#define WRITE_CHUNK_SIZE 12
#define TCMD_BUF_SIZE 1024 
 
int tcmd_inject_message(struct cfw_message *cfw_msg)
{
	char tcmd_buf[TCMD_BUF_SIZE];
	struct message * msg = &cfw_msg->m;
	sprintf(tcmd_buf, "cfw inject ");
	bin2hex(tcmd_buf + 11, (unsigned char *) msg, msg->len);
	int len = msg->len < (TCMD_BUF_SIZE - 12) / 2 ? 
							msg->len : (TCMD_BUF_SIZE-12) / 2 - 1;
	tcmd_buf[11 + 2 * len] = '\n';
	tcmd_buf[12 + 2 * len] = '\0';
	int remaining = 12 + 2 * len;
	int done = 0;
	printf("write: %s\n", tcmd_buf);
	while (remaining) {
		int count = remaining > WRITE_CHUNK_SIZE ? WRITE_CHUNK_SIZE : remaining;
		int ret = write(uart_fd, tcmd_buf+done, count);
		if (ret < 0) {
			perror("wrinting tcmd");
			return -1;
		}
		remaining -= ret;
		done += ret;
		usleep(12000);
	}
	printf("Got to WAIT_OK state\n");
	state = STATE_WAIT_OK;
	return 0;
}

void tcmd_ipc_free(struct message * m)
{
	free(m);
	/* Nothing to do */
}

static char tcmd_read_buffer[TCMD_BUF_SIZE];
static int tcmd_read_ptr = 0;

static unsigned char response_message[TCMD_BUF_SIZE];

void tcmd_handle_data(int fd)
{
	char c;
	do {
		int ret = read(fd, &c, 1);
		if (ret < 0) {
			printf("read return: %d\n", ret);
			return;
		}
		tcmd_read_buffer[tcmd_read_ptr++] = c;
	} while (c != '\n');
	tcmd_read_buffer[tcmd_read_ptr] = '\0';
	printf("%s", tcmd_read_buffer);
	if (!strncmp(tcmd_read_buffer, "cfw poll ", 9)) {
		char msg_hex[TCMD_BUF_SIZE];
		int len;
		int id;
		uint32_t current;
		int ret = sscanf(tcmd_read_buffer + 9, "%d %u/%d %s", &id, &current,
				&len, msg_hex);
		if (ret > 1) {
			printf("Handle poll response : %d %u %d %s\n", id, current, len,
					msg_hex);
			if (current > TCMD_BUF_SIZE-1) {
				printf("Invalid current index : %u\n",current);
				goto exit;
			}
			hex2bin(response_message + current, msg_hex, strlen(msg_hex) / 2);
			if (current + strlen(msg_hex) / 2 == len) {
				printk("Handle message !!!\n");
				struct cfw_message * msg = (struct cfw_message *) message_alloc(
						len, NULL);
				memcpy(msg, response_message, len);
				client_handle_message(msg, client);
				//((_cfw_client_t *) client)->handle_msg(msg, ((_cfw_client_t*)client)->data);
			}
		} else if (state == STATE_POLL_WAIT) {
			if (strstr(tcmd_read_buffer, "poll")
					&& strstr(tcmd_read_buffer, "OK")) {
				state = STATE_POLL;
				printf("Got to POLL state\n");
				goto exit;
			}
		}
	} else {
		if (state == STATE_WAIT_OK) {
			if (strstr(tcmd_read_buffer, "inject")
					&& strstr(tcmd_read_buffer, "OK")) {
				printf("Got to POLL state\n");
				state = STATE_POLL;
				goto exit;
			}
		} else {
		}
		if (strstr(tcmd_read_buffer, "ACK")) {
			goto exit;
		}
	printf("%s", tcmd_read_buffer);

	}
exit:
	tcmd_read_ptr = 0;
}

int ipc_request_sync_int(int request_id, int param1, int param2, void * ptr)
{
	printf("request sync int: %d\n", request_id);
	switch (request_id) {
	case IPC_REQUEST_ALLOC_PORT:
		return 32;
	}
	return E_OS_OK;
}

int service_mgr_port_id = 1;

uint8_t port_get_cpu_id(uint16_t port)
{
	return 0;
}

void main(int argc, char** argv)
{
	T_QUEUE queue;
	void * cfw_channel;
	struct termios termios_cfg;
	char * uart_port = "/dev/ttyUSB1";

	uart_fd = open(uart_port, O_RDWR);

	printf("uart fd: %d\n", uart_fd);
	if (uart_fd < 0) {
		perror("opening tcmd channel");
		return ;
	}

	tcflush(uart_fd, TCIOFLUSH); /* to avoid reading old data */
	tcgetattr(uart_fd, &termios_cfg);
	cfmakeraw(&termios_cfg);
	termios_cfg.c_cflag |= (CRTSCTS);
	cfsetospeed(&termios_cfg, B115200);
	cfsetispeed(&termios_cfg, B115200);
	tcsetattr(uart_fd, TCSANOW, &termios_cfg);
	tcflush(uart_fd, TCIOFLUSH); /* to avoid reading old data */

	cfw_log("Server: opening uart <%s>, fd: %d\n", uart_port, uart_fd);
	fd_set fds;
	FD_ZERO(&fds);
	struct timeval tv;

	queue = queue_create(100, NULL);
	tx_queue = queue_create(100, NULL);

	client = cfw_client_init(queue, default_handle_message, "Client 1");
	test_scenario_init(client);

	while (1) {
		FD_SET(uart_fd, &fds);
		FD_SET(0, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 50000;

		int ret = select(uart_fd + 1, &fds, NULL, NULL, &tv);
		if (ret == -1) {
			perror("select()");
		} else if (ret) {
			if (FD_ISSET(0, &fds)) {
				//TODO: handle_input();
			} else if (FD_ISSET(uart_fd, &fds)) {
				tcmd_handle_data(uart_fd);
			}
		} else {
			if (state == STATE_POLL) {
				T_QUEUE_MESSAGE m;
				struct cfw_message * msg;
				queue_get_message(tx_queue, &m, OS_NO_WAIT, NULL);
				if (m!=NULL) {
					msg = (struct cfw_message*)m;
					tcmd_inject_message(msg);
				} else {
					ret = write(uart_fd, "cfw poll\n", 9);
					if (ret < 0) {
						printf("Error polling\n");
					}
					printf("Got to POLL_WAIT state\n");
					state = STATE_POLL_WAIT;
				}
			}
		}
	}
}
