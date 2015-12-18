#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the Notifications IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import sys
import util
import fota_flash_cancel_req_during_flashing_rob
import fota_flash_start_req_during_flashing_rob
import fota_flash_first_req_is_bad_start_rob
import fota_flash_first_req_is_cancel_rob
import fota_flash_first_req_is_continue_rob
import fota_flash_recovery_iq

from IntelFirmwareUpdate_pb2 import FirmwareUpdate
from IntelFirmwareUpdate_pb2 import FirmwareUpdateState


def fota_iq_test(device, firmware_path):

	device_1 = fota_flash_cancel_req_during_flashing_rob.cancel_req_during_flashing(device, firmware_path)
	device_2 = fota_flash_start_req_during_flashing_rob.start_req_during_flashing(device_1, firmware_path)
	device_3 = fota_flash_first_req_is_bad_start_rob.bad_start_request(device_2, firmware_path)
	device_4 = fota_flash_first_req_is_cancel_rob.first_req_cancel(device_3, firmware_path)
	device_5 = fota_flash_first_req_is_continue_rob.first_req_continue(device_4, firmware_path)
	device_6 = fota_flash_recovery_iq.nominal_fota_flash(device, firmware_path)
	print("")
	return device_6


def fota_iq_automatic_test(device, firmware_path):
	print("--------------------------------")
	print("-    FOTA IQ AUTOMATIC TEST    -")
	print("--------------------------------")
	device_1 = fota_iq_test(device, firmware_path)
	print("")
	return device_1


def fota_iq_test_run_all_tests(device, firmware_path):
	device_1 = fota_iq_test(device, firmware_path)
	return device_1

if __name__ == "__main__":

	device = util.connection([], "fota")

	fota_iq_test_run_all_tests(device, sys.argv[2])

	if(device.disconnected == False):
		device.disconnect()