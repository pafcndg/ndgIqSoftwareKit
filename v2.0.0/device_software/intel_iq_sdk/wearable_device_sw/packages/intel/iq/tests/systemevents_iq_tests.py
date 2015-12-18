#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the System Events IQ
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
import os
import time
from IntelSystemEvents_pb2 import SystemEvent

def system_event_iq_test(device):

    sysevent_found = True
    device.itm_subscribe_req("\x49\x43\x45\x53\x00\x00")
    # retrieve all publish (boot and crash events)
    while (sysevent_found == True):
        sysevent_found = False
        pub = device.itm_poll(expect="publish")
        if pub == None:
            print("No publish received")
        else:
            print("")
            print("Publish on topic " + binascii.hexlify(pub['topic']))
            sysevent = SystemEvent()
            sysevent.ParseFromString(pub['data'])
            if sysevent.HasField('boot_event'):
                print("## Boot Event ##")
                print("Boot MAIN, expected type: 0")
                print("Boot OTA, expected type: 1")
                print("Boot FACTORY_RESET, expected type: 2")
                print("Received Boot Event Type: " + str(sysevent.boot_event.type))
                print("Timestamp: " + str(sysevent.timestamp.datetime))
                sysevent_found = True
            if sysevent.HasField('crash_event'):
                print("## Crash Event ##")
                if sysevent.crash_event.HasField('quark_panic'):
                    print("")
                    print("QUARK_PANIC")
                    print("Timestamp: " + str(sysevent.crash_event.quark_panic.timestamp))
                    print("eip: " + str(hex(sysevent.crash_event.quark_panic.eip)))
                    print("type: " + str(sysevent.crash_event.quark_panic.type))
                    print("error: " + str(hex(sysevent.crash_event.quark_panic.error)))
                    sysevent_found = True
                if sysevent.crash_event.HasField('arc_panic'):
                    print("")
                    print("ARC_PANIC")
                    print("Timestamp: " + str(sysevent.crash_event.arc_panic.timestamp))
                    print("eret: " + str(hex(sysevent.crash_event.arc_panic.eret)))
                    print("ecr: " + str(hex(sysevent.crash_event.arc_panic.ecr)))
                    print("efa: " + str(hex(sysevent.crash_event.arc_panic.efa)))
                    sysevent_found = True
                print("")
            if sysevent.HasField('shutdown_event'):
                print("## Shutdown Event ##")
                print("USER SHUTDOWN,             Expected Type: 0")
                print("CRITICAL_BATTERY_SHUTDOWN, Expected Type: 1")
                print("Received Shutdown Event Type: " + str(sysevent.shutdown_event.shutdown_type))
                print("Timestamp: " + str(sysevent.timestamp.datetime))
                assert sysevent.shutdown_event.shutdown_type == 0
                sysevent_found = True

    # Shutdown Test
    print("")
    print("Press Power Button (Long Press: between 1 and 6 seconds) to shutdown board")
    pub = device.itm_poll(expect="publish", timeout=30)
    if pub == None:
        print("No publish received")
    else:
        print("Publish on topic " + binascii.hexlify(pub['topic']))
        sysevent = SystemEvent()
        sysevent.ParseFromString(pub['data'])
        if sysevent.HasField('shutdown_event'):
            print("## Shutdown Event ##")
            print("USER SHUTDOWN,             Expected Type: 0")
            print("CRITICAL_BATTERY_SHUTDOWN, Expected Type: 1")
            print("Received Shutdown Event Type: " + str(sysevent.shutdown_event.shutdown_type))
            print("Timestamp: " + str(sysevent.timestamp.datetime))
            assert sysevent.shutdown_event.shutdown_type == 0
        else:
            print("No Shutdown event")

def system_event_iq_automatic_test(device):
    print("----------------------------------------")
    print("-    SYSTEM EVENT IQ AUTOMATIC TEST    -")
    print("----------------------------------------")
    print("")
    print("SYSTEM EVENT IQ CAN'T BE AUTOMATICALLY TESTED")
    print("")
    print("TEST PASSED")


def system_event_iq_run_all_test(device):
    print("------------------------------")
    print("-    SYSTEM EVENT IQ TEST    -")
    print("------------------------------")
    print("")
    system_event_iq_test(device)


if __name__ == "__main__":

    device = util.connection(["\x49\x43\x45\x53\x00\x00"])

    system_event_iq_test(device)

    device.disconnect()
