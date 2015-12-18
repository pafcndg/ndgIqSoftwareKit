#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for running all IQ tests
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import time
import util
import body_iq_tests
import devicename_iq_tests
import notif_iq_tests
import userevents_iq_tests
import datetime_iq_tests
import factoryreset_iq_tests
import fota_iq_tests
import systemevents_iq_tests

if __name__ == "__main__":
    import sys

    #special case for fota which need two parameters
    device_0 = util.connection([], "fota")
    device_1 = fota_iq_tests.fota_iq_test_run_all_tests(device_0, sys.argv[2])
    if (device_1.disconnected == False):
       device_1.disconnect()

    # Remove arg 2 from parameter and connect again for other test with topic id
    sys.argv.pop()
    device_2 = util.connection(["\x49\x42\x41\x00","\x49\x43\x44\x44\x00","\x49\x43\x44\x46\x00","\x49\x43\x44\x4E\x00","\x49\x43\x4E\x00"])

    datetime_iq_tests.datetime_iq_test_run_all_tests(device_2)
    devicename_iq_tests.devicename_iq_test_run_all_tests(device_2)
    notif_iq_tests.notif_iq_test_run_all_tests(device_2)
    userevents_iq_tests.userevents_iq_test_run_all_tests(device_2)
    device_3 = body_iq_tests.body_iq_test_run_all_tests(device_2)
    if (device_3.disconnected == False):
        device_3.disconnect()

    # The device is deconnected, connect with a new topic id
    device_4 = util.connection(["\x49\x43\x45\x53\x00\x00"])
    systemevents_iq_tests.system_event_iq_run_all_test(device_4)
    if (device_4.disconnected == False):
        device_4.disconnect()
    # Wait board wake up again
    print "\nPress the power button to wake up the board...."
    time.sleep(25)
    # The device is deconnected, connect with a new topic id
    device_5 = util.connection(["\x49\x43\x44\x4E\x00"])
    factoryreset_iq_tests.factoryreset_iq_test_run_all_tests(device_5)

