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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cfw/cfw.h"
#include "os/os.h"
#include "machine.h"
#include "infra/ipc_uart.h"
#include "infra/ipc_requests.h"
#include "ipc_uart_host.h"

#define IPC_MAX_CHANNEL         32
#define IPC_FRAME_MAX_LEN       1024

static void dump(unsigned char * data, int len);

static struct ipc_uart_channels channels[IPC_MAX_CHANNEL];

static int ipc_fd = -1;

void * ipc_uart_channel_open(int channel_id, int (*cb)(int, int, int, void *))
{
    struct ipc_uart_channels * chan;

    if (channel_id > IPC_MAX_CHANNEL - 1)
        return NULL;

    chan = &channels[channel_id];

    if (chan->state != IPC_CHANNEL_STATE_CLOSED)
        return NULL;

    chan->state = IPC_CHANNEL_STATE_OPEN;
    chan->cb = cb;

    return chan;
}


void uart_ipc_send(void * handle, int len, void * p_data)
{
    struct ipc_uart_channels * chan = (struct ipc_uart_channels *)handle;
    int ret;
    int writen = 0;
    uint8_t buffer[1024];
    uint8_t *p_buf;
    struct ipc_uart_header *ipc_uart_tx_hdr = (struct ipc_uart_header *)buffer;

    ipc_uart_tx_hdr->len = len;
    ipc_uart_tx_hdr->channel = chan->index;
    ipc_uart_tx_hdr->src_cpu_id = get_cpu_id();

    memcpy(buffer + sizeof(struct ipc_uart_header), p_data, len);

    len += sizeof(struct ipc_uart_header);
    p_buf = buffer;
    do {
        ret = write(ipc_fd, p_buf, len - writen);
        if (ret <= 0) {
            perror("write()");
       } else {
            writen += ret;
            p_buf += ret;
        }
    } while (writen < len);

    printf("Wrote message len: %d\n", len);
}

static struct ipc_uart_header ipc_header;
static uint8_t * ipc_frame = NULL;

static uint16_t read_bytes = 0;

#define STATE_READ_HEADER       1
#define STATE_READ_PAYLOAD      2

static int state = STATE_READ_HEADER;

static void uart_ipc_push_frame(uint16_t len, uint8_t * p_data)
{
    int channel = ipc_header.channel;
    int cpu_id = ipc_header.src_cpu_id;

    printf("Got ipc frame: len: %d, channel %d\nDUMP"
                    " header:\n", len, channel);
    if (channels[channel].cb != NULL) {
        channels[channel].cb(channel, IPC_MSG_TYPE_MESSAGE, len, p_data);
    }
}

void uart_ipc_init(int fd)
{
    int i;
    ipc_fd = fd;
    for (i=0; i < IPC_MAX_CHANNEL;i++) {
        channels[i].state = IPC_CHANNEL_STATE_CLOSED;
        channels[i].cb = NULL;
        channels[i].index = i;
    }

}

void uart_ipc_handle_data(int fd)
{
    int ret;

    if (state == STATE_READ_HEADER) {
        ret = read(fd, (uint8_t *)&ipc_header + read_bytes, sizeof(ipc_header)
                   - read_bytes);
        printf("ipc_uart: read header. ret: %d\n", ret);
        if (ret <= 0) {
            perror("uart_ipc: read()\n");
        } else {
            read_bytes += ret;
        }
        if (read_bytes == sizeof(ipc_header)) {
            state = STATE_READ_PAYLOAD;
            printf("uart ipc: payload state: header: 0x%x\n",
                            *(uint32_t *)&ipc_header);
            read_bytes = 0;
            ipc_frame = (uint8_t *)balloc(ipc_header.len, NULL);
        }
    }
    if (state == STATE_READ_PAYLOAD) {
        ret = read(fd, ipc_frame + read_bytes, ipc_header.len - read_bytes);
        if (ret <= 0) {
            perror("read()");
        } else {
            read_bytes += ret;
        }
        if (read_bytes == ipc_header.len) {
            state = STATE_READ_HEADER;
            printf("ipc_uart: frame received, read_bytes: %d, header state\n", read_bytes);
            uart_ipc_push_frame(read_bytes, ipc_frame);
            read_bytes = 0;
            ipc_frame = NULL;
        }
    }
}

int ipc_request_sync_int(int request_id, int param1, int param2, void * ptr)
{
    return 0;
}

/* data shall point to the first byte of an IPC frame! */
static void dump(unsigned char * data, int len)
{
    int i;
    struct ipc_uart_header *header = (struct ipc_uart_header *)data;

    printf("ipc-dump: len 0x%x ch: 0x%x src cpu_id: 0x%x::", header->len,
		    header->channel, header->src_cpu_id);
    for (i = sizeof(struct ipc_uart_header); i<len; i++) {
        if ((i%8) == 0)
            printf("\n");
        printf("%02x ", data[i]);
    }
    printf("\n");
}

void rpc_transmit_cb(uint8_t * p_buf, uint16_t length) {
}

uint8_t * rpc_alloc_cb(uint16_t length) {
	return malloc(length);
}
