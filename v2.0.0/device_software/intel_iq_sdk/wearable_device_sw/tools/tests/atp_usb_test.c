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
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 *****************************************************************************
 * Compile with:
 * gcc atp_usb_test.c `pkg-config --libs --cflags libusb-1.0` -o atp_usb_test
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

struct usb_bus *busses;

char buf[256];
/* This buffer shall contain exactly the same string as "out_buf" of
 * bsp/src/drivers/usb/function_drivers/usb_test_function_driver.c */
char received[] = "The USB test function driver is alive!";

int main(int argc, char **argv) {
	int i;
	for (i=0;i<256;i++) buf[i] = i;

	libusb_init(NULL);
	libusb_device_handle* devh = libusb_open_device_with_vid_pid(NULL, 0x8087, 0x0a99);
	assert(devh != NULL);
	libusb_claim_interface(devh, 0);
	int count = 5000;

	while(count--) {
		int actual = 0;
		int ret;
		ret = libusb_bulk_transfer(devh, 0x81, buf, 128, &actual, 1000);
		assert(ret == 0);
		assert(actual == sizeof(received));
		assert(strncmp(received, buf, sizeof(received)) == 0);
		actual = 0;
		ret = libusb_bulk_transfer(devh, 0x1, buf, 64, &actual, 1000);
		assert(ret == 0);
		assert(actual == 64);
	}

	printf("TEST PASSED!\n");
	libusb_close(devh);
	libusb_exit(NULL);
	return 0;
}

