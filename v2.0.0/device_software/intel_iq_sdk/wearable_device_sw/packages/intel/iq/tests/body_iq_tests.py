#!/usr/bin/python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Script for testing the Body IQ
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
from IntelBodyIQ_pb2 import Activity
from IntelBodyIQ_pb2 import ActivitySettings


status = ['STARTED', 'ONGOING', 'FINISHED']
activity = ['WALKING', 'RUNNING', 'BIKING']
device = None

def body_iq_automatic_test(device):
    print("--------------------------------")
    print("-    BODY IQ AUTOMATIC TEST    -")
    print("--------------------------------")
    print("")
    print("BODY IQ CAN'T BE AUTOMATICALLY TESTED")
    print("")
    print("TEST PASSED")


def body_iq_test_activity_start_report_request_and_stop(device):
    print("-------------------------------------")
    print("-   ACTIVITY START AND STOP TEST    -")
    print("-------------------------------------")
    print("")
    print("Start an activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that an activity started report has been received
    assert status[DecodedActivity.activity_status] == 'STARTED'

    print("")
    print("ACTIVITY STARTED: OK")
    print("")

    # Send request (NULL data)
    pub = device.itm_topic_req("\x49\x42\x41\x00", "\x00")

    assert pub != None
    print("Report request sended")

    print("topic_rsp: req_id %d status %d data %s"
          %(pub['req_id'], pub['status'], binascii.hexlify(pub['data'])))
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that the activity is an ongoing one
    assert status[DecodedActivity.activity_status] == 'ONGOING'

    print("")
    print("INTERMEDIATE ACTIVITY REPORT: OK")

    print("")
    print("Stop your activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that an activity finished report has been received
    assert status[DecodedActivity.activity_status] == 'FINISHED'

    print("")
    print("ACTIVITY FINISHED: OK")

    print("")
    print("TESTS PASSED")


def body_iq_test_activity_start_activity_reports_and_stop(device):
    print("------------------------------------------------")
    print("- ACTIVITY START PERIODIC REPORT AND STOP TEST -")
    print("------------------------------------------------")
    print("")
    print("Start an activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that an activity started report has been received
    assert status[DecodedActivity.activity_status] == 'STARTED'

    print("")
    print("ACTIVITY STARTED: OK")
    print("")

    print("")
    print("Continue your activity during periodic reports time (set by default at 5 minutes)")
    print("")

    pub = device.itm_poll(expect="publish", timeout=350)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that the activity is an ongoing one
    assert status[DecodedActivity.activity_status] == 'ONGOING'

    print("")
    print("PERIODIC ACTIVITY REPORT: OK")

    print("")
    print("Stop your activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that an activity finished report has been received
    assert status[DecodedActivity.activity_status] == 'FINISHED'

    print("")
    print("ACTIVITY FINISHED: OK")

    print("")
    print("TESTS PASSED")


def body_iq_test_device_connect_after_activity(device):
    print("--------------------------------------")
    print("- DEVICE CONNECT AFTER ACTIVITY TEST -")
    print("--------------------------------------")
    device.disconnect()
    print("")
    print("Start an activity")
    print("")

    time.sleep(10)

    print("Stop your activity")
    print("")

    time.sleep(15)

    device = util.connection(["\x49\x42\x41\x00"])
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])

    # Verify that an activity finished report has been received
    assert status[DecodedActivity.activity_status] == 'FINISHED'

    print("")
    print("TESTS PASSED")
    return device


def body_iq_test_device_connect_during_activity(device):
    print("---------------------------------------")
    print("- DEVICE CONNECT DURING ACTIVITY TEST -")
    print("---------------------------------------")
    device.disconnect()
    print("")
    print("Start an activity")
    print("")

    time.sleep(10)

    device = util.connection(["\x49\x42\x41\x00"])
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")

    pub = device.itm_poll(expect="publish", timeout=10)
    assert pub == None

    print("")
    print("Stop your activity")
    print("")

    # Wait for activity to be finished and published to not pollute following tests
    time.sleep(10)

    print("")
    print("TESTS PASSED")
    return device

def body_iq_test_granularity_setting(device):
    device.disconnect()
    print("---------------------------------------")
    print("-      SET NEW GRANULARITY TEST       -")
    print("---------------------------------------")
    device = util.connection(["\x49\x42\x41\x53\x00"])
    activitySettings = ActivitySettings()
    activitySettings.activity_report_granularity = 50
    device.itm_topic_req("\x49\x42\x41\x53\x00", activitySettings.SerializeToString())

    print("")
    print("TESTS PASSED")
    return device


def body_iq_test_flash_storage_activity(device):
    print("-------------------------------")
    print("- FLASH STORAGE ACTIVITY TEST -")
    print("-------------------------------")
    device.disconnect()
    print("")
    print("Do 2 activities (6 steps by activity is enough)")
    print("")

    time.sleep(45)

    device = util.connection(["\x49\x42\x41\x00"])
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")

    nb_publish = 0
    while (nb_publish < 2):
        pub = device.itm_poll(expect="publish", timeout=30)
        assert pub != None
        DecodedActivity = Activity()
        DecodedActivity.ParseFromString(pub['data'])
        step_count = 0
        if DecodedActivity.HasField('walking_activity'):
            step_count = DecodedActivity.walking_activity.step_count
        elif DecodedActivity.HasField('running_activity'):
            step_count = DecodedActivity.running_activity.step_count
        print("Activity " + activity[DecodedActivity.type] + "\n" +
              " begin " + str(DecodedActivity.timestamp_begin) +
              " end " + str(DecodedActivity.timestamp_end) +
              " step count " + str(step_count) + "\n" +
              " status " + status[DecodedActivity.activity_status])
        nb_publish = nb_publish + 1
        # Verify that the 3 first activities finished reports has been received
        assert status[DecodedActivity.activity_status] == 'FINISHED'

    time.sleep(10)
    print("")
    print("TESTS PASSED")
    return device


def body_iq_test_activity_classification(device):
    device.disconnect()
    print("---------------------------------------")
    print("-    ACTIVITY CLASSIFICATION TEST     -")
    print("---------------------------------------")
    device = util.connection(["\x49\x42\x41\x00","\x49\x42\x41\x53\x00"])
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")
    activitySettings = ActivitySettings();

    # Disable activity classification
    activitySettings.activity_classification_enable_disable = False
    device.itm_topic_req("\x49\x42\x41\x53\x00", activitySettings.SerializeToString())

    print("")
    print("Start an activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub == None

    print("")
    print("Stop your activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub == None

    print("")
    print("Do activity(ies) during periodic reports time (set by default at 5 minutes)")
    print("")

    pub = device.itm_poll(expect="publish", timeout=350)
    assert pub == None

    # Enable activity classification
    activitySettings.activity_classification_enable_disable = True
    device.itm_topic_req("\x49\x42\x41\x53\x00", activitySettings.SerializeToString())

    print("")
    print("Start an activity")
    print("")

    pub = device.itm_poll(expect="publish", timeout=30)
    assert pub != None
    DecodedActivity = Activity()
    DecodedActivity.ParseFromString(pub['data'])
    step_count = 0
    if DecodedActivity.HasField('walking_activity'):
        step_count = DecodedActivity.walking_activity.step_count
    elif DecodedActivity.HasField('running_activity'):
        step_count = DecodedActivity.running_activity.step_count
    print("Activity " + activity[DecodedActivity.type] + "\n" +
          " begin " + str(DecodedActivity.timestamp_begin) +
          " end " + str(DecodedActivity.timestamp_end) +
          " step count " + str(step_count) + "\n" +
          " status " + status[DecodedActivity.activity_status])
    # Verify that the activity started report has been received
    assert status[DecodedActivity.activity_status] == 'STARTED'

    print("")
    print("Stop your activity")
    print("")

    time.sleep(10)
    print("")
    print("TESTS PASSED")
    return device

def body_iq_test_clear_activities(device):
    print("-------------------------")
    print("- CLEAR ACTIVITIES TEST -")
    print("-------------------------")
    device.disconnect()
    print("")
    print("Do 2 activities (6 steps by activity is enough)")
    print("")

    time.sleep(45)

    device = util.connection(["\x49\x42\x41\x53\x00", "\x49\x42\x41\x00"])
    print "Cleaning all stored activities...."
    notif = ActivitySettings()
    notif.activity_discard_stored_data = 1
    # Publish to intel/body/activity/setting and Wait response
    pub = device.itm_topic_req("\x49\x42\x41\x53\x00", notif.SerializeToString())
    if pub == None:
        print("No topic_rsp received")
    else:
        print("topic_rsp: status %s"%(str(pub['status'])))
        assert pub['status'] == 0

    # Try to get activities
    print "Trying to get activities..."
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")
    # return code is none when subscribe is OK
    assert rl == None
    pub = device.itm_poll(expect="publish", timeout=30)
    # No publish sent by the device => return code is none
    assert pub == None

    print("")
    print("TESTS PASSED")
    return device


def body_iq_test_run_all_tests(device):
    print("----------------------")
    print("-    BODY IQ TEST    -")
    print("----------------------")
    rl = device.itm_subscribe_req("\x49\x42\x41\x00\x00")
    #run manual test
    body_iq_test_activity_start_report_request_and_stop(device)
    body_iq_test_activity_start_activity_reports_and_stop(device)
    device_2 = body_iq_test_device_connect_after_activity(device)
    device_3 = body_iq_test_granularity_setting(device_2)
    device_4 = body_iq_test_device_connect_during_activity(device_3)
    device_5 = body_iq_test_flash_storage_activity(device_4)
    device_6 = body_iq_test_activity_classification(device_5)
    device_7 = body_iq_test_clear_activities(device_6)
    return device_7


if __name__ == "__main__":
    device = util.connection(["\x49\x42\x41\x00"])

    print "Cleaning all stored activities...."
    notif = ActivitySettings()
    notif.activity_discard_stored_data = 1
    # Publish to intel/body/activity/setting and Wait response
    pub = device.itm_topic_req("\x49\x42\x41\x53\x00", notif.SerializeToString())
    if pub == None:
        print("No topic_rsp received")
    else:
        print("topic_rsp: status %s"%(str(pub['status'])))
        assert pub['status'] == 0

    device_3 = body_iq_test_run_all_tests(device)

    device_3.disconnect()
