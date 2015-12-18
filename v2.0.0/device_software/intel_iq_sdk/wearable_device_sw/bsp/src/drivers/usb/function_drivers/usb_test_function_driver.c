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

#include <stddef.h>
#include <stdint.h>
#include "usb.h"
#include "usb_driver_interface.h"

/**
 * \file
 *	\brief USB interface test code.
 *
 *	This file implements a simple usb profile that will read and write
 *	to 2 bulk enpoints.
 *
 */
static usb_device_descriptor_t usb_test_device_desc = {
	.bLength = USB_DEVICE_DESCRIPTOR_SIZE,
	.bDescriptorType = 0x01,
	.bcdUSB[0] = 0x10,
	.bcdUSB[1] = 0x01,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize = 64,
	.idVendor[0] = 0x87,
	.idVendor[1] = 0x80,
	.idProduct[0] = 0x99,
	.idProduct[1] = 0x0A,
	.bcdDevice[0] = 0x87,
	.bcdDevice[1] = 0x80,
	.iManufacturer = 1,/* STRING_MANUFACTURER */
	.iProduct = 2,	 /* STRING_PRODUCT */
	.iSerialNumber = 3,/* STRING_SERIAL */
	.bNumConfigurations = 1,
};

struct custom_config_descriptor {
	usb_config_descriptor_t config_descriptor;
	usb_interface_descriptor_t interface_descriptor;
	usb_endpoint_descriptor_t endpoints[2];
};

static struct custom_config_descriptor usb_test_config_desc = {
	.config_descriptor = {
		.bLength = USB_CONFIG_DESCRIPTOR_SIZE,
		.bDescriptorType = 0x2,
		.wTotalLength[0] = 32,
		.wTotalLength[1] = 0,
		.bNumInterface = 1,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = UC_BUS_POWERED | UC_SELF_POWERED,
		.bMaxPower = 50,	/* max current in 2mA units */
	},
	.interface_descriptor = {
		.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
		.bDescriptorType = 0x4,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = 0xff,
		.bInterfaceSubClass = 1,
		.bInterfaceProtocol = 1,	/*Protocol */
		.iInterface = 0,
	},
	.endpoints = {
		{
			.bLength = USB_ENDPOINT_DESCRIPTOR_SIZE,
			.bDescriptorType = UDESC_ENDPOINT,
			.bEndpointAddress = UE_DIR_IN + 1,
			.bmAttributes = UE_BULK,
			.wMaxPacketSize[0] = 0,
			.wMaxPacketSize[1] = 1,
			.bInterval = 0,
		},
		{
			.bLength = USB_ENDPOINT_DESCRIPTOR_SIZE,
			.bDescriptorType = UDESC_ENDPOINT,
			.bEndpointAddress = UE_DIR_OUT + 1,
			.bmAttributes = UE_BULK,
			.wMaxPacketSize[0] = 64,
			.wMaxPacketSize[1] = 0,
			.bInterval = 0,
		}
	},
};


static usb_string_descriptor_t usb_test_string_desc[] = {
      {
	/*String descriptor language, only one, so min size 4 bytes */
	/* 0x0409 English(US) language code used */
       .bLength = 4,	/*Descriptor size */
       .bDescriptorType = UDESC_STRING,	/*Descriptor type */
       .bString = {{0x09, 0x04}}
       },
      {
	/*Manufacturer String Descriptor "Intel" */
       .bLength = 0x0C,
       .bDescriptorType = UDESC_STRING,
       .bString =
	       {{'I', 0}, {'n', 0}, {'t', 0}, {'e', 0}, {'l', 0}}
       },
      {
	/*Product String Descriptor "ATP-Ref1.0" */
       .bLength = 0x16,
       .bDescriptorType = UDESC_STRING,
       .bString =
       {{'A', 0}, {'T', 0}, {'P', 0}, {'-', 0}, {'R', 0}, {'e', 0},
	       {'f', 0}, {'1', 0}, {'.', 0}, {'0', 0}}
       },
      {
	/*Serial Number String Descriptor "00.01" */
       .bLength = 0x0C,
       .bDescriptorType = UDESC_STRING,
       .bString =
       {{'0', 0}, {'0', 0}, {'.', 0}, {'0', 0}, {'1', 0}}
      },
};

static int usb_test_class_handle_req(usb_device_request_t *pSetup, uint32_t * piLen,
		   uint8_t ** ppbData)
{
	*piLen = 0;
	return 0;
}

static char buffer[256];
static char out_buf[] = "The USB test function driver is alive!";

static void usb_test_ep_complete(int ep_address, void *priv, int status, int actual)
{
	if (ep_address == 0x81) {
		usb_ep_write(0x81, (uint8_t*)out_buf, sizeof(out_buf), NULL);
	} else if (ep_address == 0x1) {
		usb_ep_read(0x1, (uint8_t*)buffer, 64, NULL);
	}
}

static void usb_test_class_start()
{
	usb_ep_write(0x81, (uint8_t*)out_buf, 6, NULL);
	usb_ep_read(0x1, (uint8_t*)buffer, 64, NULL);
}

static void usb_test_event_handler(struct usb_event * event)
{
	switch(event->event) {
		case USB_EVENT_SET_CONFIG:
			usb_test_class_start();
			break;
		case USB_EVENT_RESET:
			break;
		case USB_EVENT_DISCONNECT:
			break;
	}
}

#define EP0_BUFFER_SIZE 128
static uint8_t ep0_buffer[EP0_BUFFER_SIZE];

struct usb_interface_init_data usb_test_init_data = {
	.ep_complete = usb_test_ep_complete,
	.class_handler = usb_test_class_handle_req,
	.usb_evt_cb = usb_test_event_handler,
	.ep0_buffer = &ep0_buffer[0],
	.ep0_buffer_size = EP0_BUFFER_SIZE,
	.dev_desc = &usb_test_device_desc,
	.conf_desc = (usb_config_descriptor_t*)&usb_test_config_desc,
	.conf_desc_size = sizeof(usb_test_config_desc),
	.strings_desc = usb_test_string_desc,
	.num_strings = sizeof(usb_test_string_desc) /
		sizeof(usb_string_descriptor_t),
	.eps = usb_test_config_desc.endpoints,
	.num_eps = 2,
};

void usb_test_class_init(void *priv)
{
	usb_interface_init(&usb_test_init_data);
}
