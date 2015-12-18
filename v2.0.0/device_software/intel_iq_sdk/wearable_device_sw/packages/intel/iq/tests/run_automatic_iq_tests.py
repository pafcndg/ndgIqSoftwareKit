#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for running non regression tests for IQs
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

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
    device_1 = fota_iq_tests.fota_iq_automatic_test(device_0, sys.argv[2])
    device_1.disconnect()

    # Remove arg 2 from parameter and connect again for other test
    sys.argv.pop()
    device_2 = util.connection(["\x49\x42\x41\x00","\x49\x43\x44\x44\x00","\x49\x43\x44\x46\x00","\x49\x43\x44\x4E\x00","\x49\x43\x4E\x00"])

    systemevents_iq_tests.system_event_iq_automatic_test(device_2)
    devicename_iq_tests.devicename_iq_automatic_test(device_2)
    notif_iq_tests.notif_iq_automatic_test(device_2)
    userevents_iq_tests.userevents_iq_automatic_test(device_2)
    datetime_iq_tests.datetime_iq_automatic_test(device_2)
    body_iq_tests.body_iq_automatic_test(device_2)
    factoryreset_iq_tests.factoryreset_iq_automatic_test(device_2)
