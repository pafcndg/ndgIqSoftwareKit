#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the FactoryReset IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import binascii
import os
import time
import util
from IntelDevice_pb2 import FactoryReset

def factoryreset_iq_automatic_test(device):
    print("--------------------------------------")
    print("-    FACTORY RESET AUTOMATIC TEST    -")
    print("--------------------------------------")
    print("")
    test_factory_reset(device)
    print("")


def test_factory_reset(device):

    print("factory_reset will be sent and we check for the device to disconnect")

    factoryReset = FactoryReset()
    factoryReset.factory_reset = 1;
    print("set factory_reset: " + str(factoryReset.factory_reset))
    device.itm_publish("\x49\x43\x44\x46\x00", factoryReset.SerializeToString())

    device.wait_disc("Board has not reboot", 10)
    if(device.disconnected == True):
        print "Board has reboot"

    print "TEST PASSED"

def factoryreset_iq_test_run_all_tests(device):
    print("----------------------------")
    print("-    FACTORY RESET TEST    -")
    print("----------------------------")
    test_factory_reset(device)


if __name__ == "__main__":
    device = util.connection(["\x49\x43\x44\x4E\x00"])

    factoryreset_iq_test_run_all_tests(device)

    if(device.disconnected == False):
		device.disconnect()
