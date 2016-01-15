#!/usr/bin/env python

# Copyright (c) 2015, Intel Corporation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# to launch:
# make otapackage
# python dfu_ota_stress.py ~/atlaspeak/
#

import argparse
import os
import sys
import time
from tests_library import *

def dfu_flash(ttyUSB, ota_package):
	print "download",ota_package
	ret =  os.system("dfu-util -a 10 -R -D " + ota_package)
	if ret:
		reset(ttyUSB, 1)
		time.sleep(4)
		ret = os.system("dfu-util -a 10 -R -D " + ota_package)
	return ret

def testVersion(ttyUSB):
	s = ttyUSB_transfer (ttyUSB, "version get\n" , timeout=2)

# check logs to return results
	if s.find('Panic') != -1:
		return "PANIC"
	elif s.find('Micro-sha1') != -1:
		return "PASSED"

	return "HANGED"

def wait_ota_installed(ttyUSB):
	t = int(time.time())
	ttyUSB_read (ttyUSB, timeout=180, terminate_word="BOOT")
	print "took",int(time.time()) - t, "sec"

def wait_boot(ttyUSB):
	ttyUSB_read (ttyUSB, timeout=20)

def reset(ttyUSB, mode):
	ttyUSB_transfer (ttyUSB, "system reset " + str(mode) + "\n" , timeout=3)

def verify(firmware_path):

	binaries = {}
	binaries["quark"] = {"src":firmware_path + "quark.signed.bin", "alt":2 , "verify": "/tmp/quark.dump"}
	binaries["arc"] = {"src":firmware_path + "arc.bin", "alt":7 , "verify": "/tmp/arc.dump"}
	binaries["ble"] = {"src":firmware_path + "ble_core/image.bin","alt":8 , "verify": "/tmp/ble.dump"}

	ret = 0
	for key in binaries.keys():
		binary = binaries[key]
		os.system("rm "+ binary["verify"]);
		if os.system("dfu-util -a " + str(binary["alt"]) + " -U " + binary["verify"] ):
			ret = -1
		size = os.stat(binary["src"]).st_size
		print "verify", binary
		if key == "ble": # erase softdevice 1k data partition
			print "erase softdevice 1k data partition"
			with open(binary["verify"],"rw+") as fo:
				fo.seek(0xc00)
				fo.write('\0'*1024)

		if size <= 0 or os.system("cmp -n "+ str(size) + " " + binary["verify"] + " " + binary["src"]):
			ret = -1

	return ret

# usage ex: python dfu_ota_stress.py ~/atlaspeak/out/current/ota/pub/package.ota.signed.bin
def main():
	parser = argparse.ArgumentParser(description="Flash ota package throgth dfu, reboot"
		", decompress and flash ota package on flash."
		"check flash integrity")
	parser.add_argument('ota_bin', help="ota package binary")
	parser.add_argument('firmware_dir', help="directory containing firmware used for ota packaging", nargs='?', default='')
	args = parser.parse_args()
	ota_package = args.ota_bin
	if args.firmware_dir == '':
		firmware_path = os.path.dirname(args.ota_bin) + "/../../firmware/"
	else:
		firmware_path = args.firmware_dir

	ttyUSB = usb_manager()

	# copy ota package on spi flash
	if dfu_flash(ttyUSB, ota_package):
		print "ota package download fail"
		return -1
	wait_boot(ttyUSB)
	reset(ttyUSB, 6)
	print "wait until OTA package is installed"
	wait_ota_installed(ttyUSB)
	print "OTA install finished"

	wait_boot(ttyUSB)
	ret = testVersion(ttyUSB)
	if ret != "PASSED":
		print "ota installation and boot fails"
		return -2
	reset(ttyUSB, 1)

	if verify(firmware_path):
		print "firmware verify fail"
		return -3

	print "OTA TEST PASSED"
	return 0

if __name__ == "__main__":
	sys.exit(main())
