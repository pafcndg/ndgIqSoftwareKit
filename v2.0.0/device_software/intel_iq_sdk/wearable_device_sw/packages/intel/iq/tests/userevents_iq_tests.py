#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the User Events IQ
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

import util
import binascii
from IntelUserEvents_pb2 import UserEvent


def userevents_iq_automatic_test(device):
    print("------------------------------------------")
    print("-    USER EVENT IQ AUTOMATIC TEST   -")
    print("------------------------------------------")
    print("")
    print("USER EVENT IQ CAN'T BE AUTOMATICALLY TESTED")
    print("")
    print("TEST PASSED")


def userevents_iq_test(device):

    print("Press Power Button")
    print("- Short Press, expected type: 0")
    print("- Double Press, expected type: 1")
    print("- Long Press, expected type: 3")
    print("OR Tap")
    print("- Double Tap, expected type: 0")
    print("- Triple Tap, expected type: 1")
    pub = device.itm_poll(expect="publish", timeout = 30)
    if pub == None:
        print("No publish received")
    else:
        print("Publish on topic " + binascii.hexlify(pub['topic']))
        userevent = UserEvent()
        userevent.ParseFromString(pub['data'])
        if userevent.HasField('button_event'):
            print ("Button Event Type: " + str(userevent.button_event.type))
        elif userevent.HasField('tapping_event'):
            print ("Tapping Event Type: " + str(userevent.tapping_event.type))
        else:
            print ("No Button or Tapping event")

def userevents_iq_test_run_all_tests(device):
    print("---------------------------")
    print("-    USER EVENT IQ TEST   -")
    print("---------------------------")
    device.itm_subscribe_req("\x49\x43\x45\x55\x00\x00")
    userevents_iq_test(device)


if __name__ == "__main__":
    device = util.connection(None)

    userevents_iq_test_run_all_tests(device)

    device.disconnect()
