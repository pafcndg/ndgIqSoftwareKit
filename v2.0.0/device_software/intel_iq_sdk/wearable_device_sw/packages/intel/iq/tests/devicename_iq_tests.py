#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the Device Name IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import os
import util
import time
from IntelCommon_pb2 import DeviceName

def devicename_iq_automatic_test(device):
    print("------------------------------------")
    print("-    DEVICE NAME AUTOMATIC TEST    -")
    print("------------------------------------")
    print("")
    test_devicename_iq(device)
    print("")


def test_devicename_iq(device):

    current_name = device.gapdevname.read()
    print 'Current device name: ' + current_name

    deviceName = DeviceName()
    deviceName.name = 'new_Name_' + str(time.time())[5:9]
    print "set name: " + deviceName.name
    device.itm_publish("\x49\x43\x44\x4E\x00", deviceName.SerializeToString())

    time.sleep(2)

    new_name = device.gapdevname.read()
    print 'New device name: ' + new_name

    # check logs to return results
    if deviceName.name == new_name and new_name != current_name:
        print("Device Name OK")
    else:
        print("Wrong Device Name")
        assert 1==0

    print("")
    print("TEST PASSED")


def devicename_iq_test_run_all_tests(device):
    print("--------------------------")
    print("-    DEVICE NAME TEST    -")
    print("--------------------------")
    test_devicename_iq(device)


if __name__ == "__main__":
    device = util.connection(["\x49\x43\x44\x4E\x00"])

    devicename_iq_test_run_all_tests(device)

    device.disconnect()
