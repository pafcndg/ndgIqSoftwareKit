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

#include <usb/usb.h>
#include <dfu.h>

usb_device_descriptor_t dfu_device_desc = {
	.bLength = USB_DEVICE_DESCRIPTOR_SIZE,
	.bDescriptorType = 0x01,
	.bcdUSB[0] = UD_USB_2_0 & 0xff,
	.bcdUSB[1] = (UD_USB_2_0 >> 8) & 0xff,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize = 64,
	.idVendor[0] = (CONFIG_USB_DFU_VID & 0xff),
	.idVendor[1] = ((CONFIG_USB_DFU_VID >> 8) & 0xff),
	.idProduct[0] = (CONFIG_USB_DFU_PID & 0xff),
	.idProduct[1] = ((CONFIG_USB_DFU_PID >> 8) & 0xff),
	.bcdDevice[0] = 0x87,
	.bcdDevice[1] = 0x80,
	/* STRING_MANUFACTURER */
	.iManufacturer = 1,
	/* STRING_PRODUCT */
	.iProduct = 2,
	/* STRING_SERIAL */
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

struct dfu_usb_descriptor dfu_config_desc = {
	.config_descriptor = {
		.bLength = USB_CONFIG_DESCRIPTOR_SIZE,
		.bDescriptorType = 0x2,
		.wTotalLength[0] = DFU_CONF_SIZE & 0xff,
		.wTotalLength[1] = (DFU_CONF_SIZE >> 8) & 0xff,
		.bNumInterface = 1,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = UC_BUS_POWERED | UC_SELF_POWERED,
		.bMaxPower = 50,	/* max current in 2mA units */
	},
	.interface_descriptor = {
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = X86_ROM_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 4,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = X86_BOOT_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 5
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = X86_APP_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 6,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = FACT_NONPERSISTENT_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 7,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = PANIC_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 8,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = FACT_PERSISTENT_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 9,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = DATA_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 10,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = SENSOR_CORE_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 11,
		},
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = BLE_CORE_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 12,
		},
#if defined(CONFIG_SPI_FLASH)
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = SNOR_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 13,
		},
#endif
#if defined(CONFIG_CACHE_PARTITION)
		{
			.bLength = USB_INTERFACE_DESCRIPTOR_SIZE,
			.bDescriptorType = 0x4,
			.bInterfaceNumber = 0,
			.bAlternateSetting = CACHE_ALT,
			.bNumEndpoints = 0,
			.bInterfaceClass = 0xfe,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface = 14,
		},
#endif
	},
	.dfu_functional_descriptor = {
		.bLength = DFU_FUNCTIONAL_DESCRIPTOR_SIZE,
		.bDescriptorType = 0x21,
		/* SHOULD BE DFU_DOWNLOAD_CAPABLE */
		.bmAttributes =
			DFU_UPLOAD_CAPABLE | DFU_DOWNLOAD_CAPABLE | DFU_MANIFESTATION_TOLERANT,
		.wDetachTimeout[0] = 0,
		.wDetachTimeout[1] = 0,
		.wTransferSize[0] = 0,
		.wTransferSize[1] = 8,
		.bcdDFUVersion[0] = 0x10,
		.bcdDFUVersion[1] = 0x01,
	}
};

usb_string_descriptor_t dfu_strings_desc[] = {
	{
		/*String descriptor language, only one, so min size 4 bytes */
		/* 0x0409 English(US) language code used */
		.bLength = 4,
		.bDescriptorType = UDESC_STRING,
		.bString = { 0x09, 0x04 }
	},
	{
		.bLength = sizeof("Intel") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'I', 0, 'n', 0, 't', 0, 'e', 0, 'l', 0 }
	},
	{
		.bLength = sizeof("CURIECRB") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'C', 0, 'U', 0, 'R', 0, 'I', 0, 'E', 0,
			'C', 0, 'R', 0, 'B', 0}
	},
	{
		.bLength = sizeof("00.01") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { '0', 0, '0', 0, '.', 0, '0', 0, '1', 0 }
	},
	{
		.bLength = sizeof("x86_rom") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'x', 0, '8', 0, '6', 0, '_', 0, 'r', 0,
			'o', 0, 'm', 0 }
	},
	{
		.bLength = sizeof("x86_boot") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'x', 0, '8', 0, '6', 0, '_', 0, 'b', 0,
			'o', 0, 'o', 0, 't', 0 }
	},
	{
		.bLength = sizeof("x86_app") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'x', 0, '8', 0, '6', 0, '_', 0, 'a', 0,
			'p', 0, 'p', 0 }
	},
	{
		.bLength = sizeof("factory1") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'f', 0, 'a', 0, 'c', 0, 't', 0, 'o', 0,
			'r', 0,'y', 0, '1', 0}
	},
	{
		.bLength = sizeof("panic") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'p', 0, 'a', 0, 'n', 0, 'i', 0, 'c', 0 },
	},
	{
		.bLength = sizeof("factory2") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'f', 0, 'a', 0, 'c', 0, 't', 0, 'o', 0,
			'r', 0,'y', 0, '2', 0}
	},
	{
		.bLength = sizeof("data") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'd', 0, 'a', 0, 't', 0, 'a', 0 }
	},
	{
		.bLength = sizeof("sensor_core") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 's', 0, 'e', 0, 'n', 0, 's', 0, 'o', 0,
			'r', 0, '_', 0, 'c', 0, 'o', 0, 'r', 0, 'e', 0 }
	},
	{
		.bLength = sizeof("ble_core") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'b', 0, 'l', 0, 'e', 0, '_', 0, 'c', 0,
			'o', 0, 'r', 0, 'e', 0 }
	},
#if defined(CONFIG_SPI_FLASH)
	{
		.bLength = sizeof("snor") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 's', 0, 'n', 0, 'o', 0, 'r', 0 }
	},
#endif
#if defined(CONFIG_CACHE_PARTITION)
	{
		.bLength = sizeof("cache") * 2,
		.bDescriptorType = UDESC_STRING,
		.bString = { 'c', 0, 'a', 0, 'c', 0, 'h', 0, 'e', 0 }
	},
#endif
};
