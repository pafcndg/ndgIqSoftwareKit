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

import util
import binascii
import time
import os
import re
import sys
from IntelFirmwareUpdate_pb2 import FirmwareUpdate
from IntelFirmwareUpdate_pb2 import FirmwareUpdateState


def first_req_cancel(device, firmware_path):

	#get firmware id
	quark_id = device.disfwrev.read()
	print "QUARK ID : %s"%quark_id

	# Set binary file, the binary test file shall contain only at least QUARK firmware
	firmware_path = sys.argv[2]
	try:
		print "Firmware file: " + firmware_path
		stat = os.stat(firmware_path)
		file_len = stat.st_size
	except:
		print '\nERROR: file - ' + firmware_path + '- not found'
		exit()

	try:
		# Search the pattern ATP1LAK000 in the binary file
		fd = open(firmware_path, "rb")
		s = fd.read(file_len)
		fd.close()
		firmware_id_to_flash = re.search('ATP1LAK000-[0-9A-Fa-f]{9}', s).group()
	except:
		print '\nERROR: binary test file: ' + firmware_path + ' does not contain QUARK firmware'
		exit()

	#Perform a state request before starting
	print ("----------------------------")
	print ("------  CHECK DEVICE  ------")
	print ("-----------------------------")
	# Publish to intel/core/FirmwareUpdate/State and Wait response
	pub = device.itm_topic_req("\x49\x43\x46\x53\x00", "\x00")
	if pub == None:
		print("No topic_rsp received")
	else:
		print("topic_rsp: req_id %s status %s"%(str(pub['req_id']), str(pub['status'])))
		decodedResp = FirmwareUpdateState()
		decodedResp.ParseFromString(pub['data'])
		print("response value = " + str(decodedResp.firmware_update_state))
		assert pub['status'] == 0
		if decodedResp.firmware_update_state != FirmwareUpdateState.NONE:
			#Perform a cancel request
			print ("--------------------------------------------------")
			print ("------  DEVICE NOT READY => CANCEL REQUEST  ------")
			print ("--------------------------------------------------")
			# Prepare a Firmware Update CANCEL request
			notif.firmware_update_cmd = FirmwareUpdate.CANCEL

			# Publish to intel/core/Firmware and Wait response
			pub = device.itm_topic_req("\x49\x43\x46\x00", notif.SerializeToString())
			if pub == None:
				print("No topic_rsp received")
			else:
				print("topic_rsp: status %s"%(str(pub['status'])))
				assert pub['status'] == 0
		else:
			print ("---------------------------------------------")
			print ("------  DEVICE READY => START REQUEST  ------")
			print ("---------------------------------------------")

	# Instantiate a Firmware Update request
	notif = FirmwareUpdate()
	notif.firmware_total_length = file_len
	# BLE max message size is 132 bytes, the BLE message contain header + topic + req_id + protobuf serialization
	# protobuf serialization contain : command (start, cancel continue) + firmware size + data + size
	# After some test max data size allowed is 114
	data_max_size = 114
	notif.offset = 0

	#Perform a cancel request as a 1st req
	print ("------------------------------------")
	print ("------  1st RESQUEST: CANCEL  ------")
	print ("------------------------------------")
	# Prepare a CANCEL request
	notif.firmware_update_cmd = FirmwareUpdate.CANCEL

	# Publish to intel/core/Firmware and Wait response
	pub = device.itm_topic_req("\x49\x43\x46\x00", notif.SerializeToString())
	if pub == None:
		print("No topic_rsp received")
	else:
		print("topic_rsp: status %s"%(str(pub['status'])))
		assert pub['status'] == 3

	print ("-----------------------------------")
	print ("------      CHECK DEVICE:    ------")
	print ("------  NONE STATE EXPECTED  ------")
	print ("-----------------------------------")
	# Publish to intel/core/FirmwareUpdate/State and Wait response
	pub = device.itm_topic_req("\x49\x43\x46\x53\x00", "\x00")
	if pub == None:
		print("No topic_rsp received")
	else:
		print("topic_rsp: req_id %s status %s"%(str(pub['req_id']), str(pub['status'])))
		decodedResp = FirmwareUpdateState()
		decodedResp.ParseFromString(pub['data'])
		print("Response value = %d (expected: %d)"%(decodedResp.firmware_update_state, FirmwareUpdateState.NONE))
		assert pub['status'] == 0
		assert decodedResp.firmware_update_state == FirmwareUpdateState.NONE


	print ("-----------------------------")
	print ("------  START REQUEST  ------")
	print ("-----------------------------")
	notif.firmware_update_cmd = FirmwareUpdate.START
	print "Firmware size: %d"%file_len
	# Publish to intel/core/FirmwareUpdate and Wait response
	pub = device.itm_topic_req("\x49\x43\x46\x00", notif.SerializeToString())
	if pub == None:
		print("No topic_rsp received")
	else:
		print("topic_rsp: status %s"%(str(pub['status'])))
		assert pub['status'] == 0

	#Wait recovery reboot
	print 'Waiting RECOVERY reboot.....'
	time.sleep(20)

	# connect to device
	device = util.connection([], "fota")

	print ("-------------------------------------")
	print ("------      STATE REQUEST:     ------")
	print ("------  STARTED RESP EXPECTED  ------")
	print ("-------------------------------------")
	# Publish to intel/core/FirmwareUpdate/State and Wait response
	pub = device.itm_topic_req("\x49\x43\x46\x53\x00", "\x00")
	if pub == None:
		print("No topic_rsp received")
	else:
		print("topic_rsp: req_id %s status %s"%(str(pub['req_id']), str(pub['status'])))
		decodedResp = FirmwareUpdateState()
		decodedResp.ParseFromString(pub['data'])
		print("Response value = %d (expected: %d)"%(decodedResp.firmware_update_state, FirmwareUpdateState.STARTED))
		assert pub['status'] == 0
		assert decodedResp.firmware_update_state == FirmwareUpdateState.STARTED

	print ("-------------------------------------")
	print ("------    CONTINUE REQUEST     ------")
	print ("-------------------------------------")
	# Prepare a Firmware Update CONTINUE request
	notif.firmware_update_cmd = FirmwareUpdate.CONTINUE

	fd = open(firmware_path, "rb")
	time_before = time.time()
	i = 0
	while 1:
		i = i + 1
		if i%50 == 0:
			print str(i) + ' requests executed'

		notif.data = fd.read(data_max_size)
		# Publish to intel/core/Firmware and Wait response
		pub = device.itm_topic_req("\x49\x43\x46\x00", notif.SerializeToString())
		if pub == None:
			print("No topic_rsp received")
			print("topic_rsp: status %d"%(pub['status']))
		else:
			assert pub['status'] == 0

		notif.offset += len(notif.data)

		if len(notif.data) < data_max_size:
			break

	fd.close()

	print "Time to perform " + str(i) + " transfers and flash " + str(file_len) + " bytes : " + str(time.time() - time_before).split('.')[0] + "s"

	#Wait ota reboot
	print "Waiting OTA reboot....(110 s)"
	time.sleep(110)

	# connect to device
	device = util.connection([], "fota")
	quark_id_new = device.disfwrev.read()

	print "New quark id: " + quark_id_new + " --- Old quark id: " + quark_id
	if quark_id_new == quark_id:
		print "Same firmware id --- binary file was ignored by bootloader"
	else:
		print "Firmware id different --- binary file was accepted by bootloader"

	return device

if __name__ == "__main__":

	# connect to device
	device = util.connection([], "fota")

	first_req_cancel(device, sys.argv[2])

	device.disconnect()