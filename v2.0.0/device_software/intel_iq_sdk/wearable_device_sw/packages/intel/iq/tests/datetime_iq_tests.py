#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the DateTime IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import binascii
import util
import time
from IntelCommon_pb2 import DateTime

def datetime_iq_automatic_test(device):
    print("-------------------------------------")
    print("-    DATE TIME IQ AUTOMATIC TEST    -")
    print("-------------------------------------")
    print("")
    test_datetime_iq(device)
    print("")


def test_datetime_iq(device):
    # Instantiate a datetime publish
    date = DateTime()
    date.datetime = 1000

    # Constants
    initialDate = 1000
    sleepTime   = 10
    expectedDate = initialDate + sleepTime
    tolerance = 1

    # Publish to intel/core/
    # Publish Date
    print("Set new date: 1000")
    device.itm_publish("\x49\x43\x44\x44\x00", date.SerializeToString())

    print("sleep 10 seconds")
    time.sleep(10)
    print("Send Get Time request")
    # Send request (NULL data)
    pub = device.itm_topic_req("\x49\x43\x44\x44\x00", "\x00")

    if pub == None:
        print("No topic_rsp received")
    else:
        print("topic_rsp: req_id %d status %d data %s"
              %(pub['req_id'], pub['status'], binascii.hexlify(pub['data'])))
        decodedDate = DateTime()
        decodedDate.ParseFromString(pub['data'])
        assert pub['status'] == 0
        decodedDate = DateTime()
        decodedDate.ParseFromString(pub['data'])
        print("Expected datetime = " + str(expectedDate))
        print("Received datetime = " + str(decodedDate.datetime))
        assert abs(expectedDate - decodedDate.datetime) <= tolerance

    print("TEST PASSED")

def datetime_iq_test_run_all_tests(device):
    print("---------------------------")
    print("-    DATE TIME IQ TEST    -")
    print("---------------------------")
    print("")
    test_datetime_iq(device)


if __name__ == "__main__":
    device = util.connection(["\x49\x43\x44\x44\x00"])

    datetime_iq_test_run_all_tests(device)

    device.disconnect()
