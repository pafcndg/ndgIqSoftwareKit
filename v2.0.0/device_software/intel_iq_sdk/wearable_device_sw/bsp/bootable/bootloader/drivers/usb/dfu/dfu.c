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

#include <dfu.h>
#include <usb/usb_driver_interface.h>
#include <printk.h>
#include <bootlogic.h>
#include <utils.h>

#ifndef NULL
#define NULL 0
#endif

/*Class specific request*/
#define DFU_DETACH          0x00
#define DFU_DNLOAD          0x01
#define DFU_UPLOAD          0x02
#define DFU_GETSTATUS       0x03
#define DFU_CLRSTATUS       0x04
#define DFU_GETSTATE        0x05
#define DFU_ABORT           0x06

static struct dfu_ops dfu_ops = {
	.alternate = -1,
	.state = dfuIDLE,
	.status = statusOK,
	.data = NULL,
	.len = 0,
	.device_request = NULL,
	.erase = NULL,
	.write = NULL,
	.read = NULL
};

void do_dfu_download(usb_device_request_t *setup_packet, uint32_t *len)
{
	pr_debug("%s\n", __func__);

	if (0 != UGETW(setup_packet->wLength)) {
		/* download has started */
		dfu_ops.state = dfuDNBUSY;
		*len = UGETW(setup_packet->wLength);
		dfu_ops.len = *len;
		dfu_set_alternate(&dfu_ops);

		if (dfu_ops.write)
			dfu_ops.write(&dfu_ops);
	} else {
		/* Download complete */
		dfu_download_cleanup(&dfu_ops);
		dfu_ops.alternate = -1;
	}

	*len = 0;
}

void do_dfu_upload(usb_device_request_t *setup_packet, uint32_t *len)
{
	pr_debug("%s\n", __func__);

	if (dfu_ops.state != dfuIDLE && dfu_ops.state != dfuUPLOAD_IDLE){
		return;
	}

	if (0 == UGETW(setup_packet->wLength)) {
		/* Upload complete */
		dfu_ops.state = dfuIDLE;
		dfu_ops.alternate = -1;
		*len = 0;
		return;
	}

	/* upload has started */
	dfu_ops.state = dfuUPLOAD_IDLE;
	*len = UGETW(setup_packet->wLength);
	dfu_ops.len = *len;
	dfu_set_alternate(&dfu_ops);

	if (dfu_ops.read) {
		dfu_ops.read(&dfu_ops);
		return;
	} else {
		dfu_ops.state = dfuERROR;
		dfu_ops.status = errTARGET;
		*len = 0;
	}

}

void do_dfu_abort(usb_device_request_t *setup_packet, uint32_t *len)
{
	pr_debug("%s\n", __func__);
	dfu_ops.state = dfuIDLE;
}

void do_dfu_detach(usb_device_request_t *setup_packet, uint32_t *len)
{
	pr_debug("%s\n", __func__);
	dfu_ops.state = dfuIDLE;
	*len = 0;
	/*
	 * call dfu_reset to catch USB reset
	 * if do_dfu_reset returns, we boot
	 */
	do_dfu_reset();
}

volatile int dfu_reset = 0;
void do_dfu_reset(void)
{
	pr_debug("%s\n", __func__);
	dfu_reset = 1;
}

extern uint32_t dfu_busy;
int ClassHandleReq(usb_device_request_t *setup_packet, uint32_t *len,
		   uint8_t **data)
{
	unsigned char *dfu_buffer = *data;
	dfu_ops.data = *data;
	dfu_ops.len = *len;
	dfu_ops.device_request = setup_packet;

	switch (setup_packet->bRequest) {
	case UR_SET_INTERFACE:
		dfu_ops.alternate = UGETW(setup_packet->wValue);
		dfu_buffer[0] = dfu_ops.alternate;
		*len = 0;
		break;
	case DFU_GETSTATUS:
		pr_debug("DFU_GETSTATUS: %x\r\n", dfu_ops.state);
		dfu_buffer[0] = dfu_ops.status;
		dfu_buffer[1] = 1;
		dfu_buffer[2] = 0;
		dfu_buffer[3] = 0;
		dfu_buffer[4] = dfu_ops.state;	/* state: idle */
		dfu_buffer[5] = 0;	/* status string */
		*len = 6;

		if (dfu_ops.state == dfuDNBUSY)
			dfu_ops.state = dfuDNLOAD_IDLE;
		break;

	case DFU_GETSTATE:
		pr_debug("DFU_GETSTATE\r\n");
		(*data)[0] = dfu_ops.state;
		*len = 1;
		break;

	case DFU_ABORT:
		do_dfu_abort(setup_packet, len);
		break;

	case DFU_CLRSTATUS:
		pr_debug("DFU_CLRSTATUS\r\n");
		dfu_ops.state = dfuIDLE;
		dfu_ops.status = statusOK;
		break;

	case DFU_DNLOAD:
		/* a dfu transfer has started */
		dfu_busy = 1;
		do_dfu_download(setup_packet, len);
		break;

	case DFU_UPLOAD:
		/* a dfu transfer has started */
		dfu_busy = 1;
		do_dfu_upload(setup_packet, len);
		*len = dfu_ops.len;
		break;

	case DFU_DETACH:
		do_dfu_detach(setup_packet, len);
			break;
	default:
		pr_debug("UNKNOWN: %d\r\n", setup_packet->bRequest);
		return -1;
	}

	return 0;
}

#define EP0_BUFFER_SIZE 2048
uint8_t ep0_buffer[EP0_BUFFER_SIZE];

struct usb_interface_init_data dfu_init_data = {
	.ep_complete = NULL,
	.class_handler = ClassHandleReq,
	.usb_evt_cb = NULL,
	.ep0_buffer = &ep0_buffer[0],
	.ep0_buffer_size = EP0_BUFFER_SIZE,
	.dev_desc = &dfu_device_desc,
	.conf_desc = (usb_config_descriptor_t*)&dfu_config_desc,
	.conf_desc_size = sizeof(dfu_config_desc),
	.strings_desc = dfu_strings_desc,
	.num_strings = sizeof(dfu_strings_desc) /
		sizeof(usb_string_descriptor_t),
	.eps = NULL,
	.num_eps = 0,
};

void dfu_class_init()
{
	usb_interface_init(&dfu_init_data);
}
