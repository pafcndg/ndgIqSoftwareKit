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

#usage:
# reset board (to go to dfu) and launch script

from tests_library import *

def write_dummy_data(alternate, size):
	with open("/tmp/dummy","w") as fo:
		fo.write('a'*size)
	return os.system("dfu-util -a "+ str(alternate) + " -D /tmp/dummy")

def check_erased_data(alternate):
	if os.system("rm /tmp/dummy; dfu-util -a "+ str(alternate) + " -U /tmp/dummy"):
		return -1
	with open("/tmp/dummy","r") as fo:
		data = fo.read()
		for c in data:
			if c != '\xff':
				return -1
	return 0

def write_dummy_ble_data():
	os.system("rm /tmp/dummy; dfu-util -a 8 -U /tmp/dummy")
	with open("/tmp/dummy","rw+") as fo:
		fo.seek(246 * 1024)
		fo.write('a' * 1024)
	return os.system("dfu-util -a 8 -R -D /tmp/dummy")

def check_erased_ble_data():
	if os.system("rm /tmp/dummy; dfu-util -a 8 -U /tmp/dummy"):
		return -1
	with open("/tmp/dummy","r") as fo:
		fo.seek(246 * 1024)
		data = fo.read(1024)
		for c in data:
			if c != '\xff':
				return -1
	return 0

def wait_boot(ttyUSB):
	ttyUSB_read (ttyUSB, timeout=20)

def wait_user_data_erased(ttyUSB):
	t = int(time.time())
	ttyUSB_read (ttyUSB, timeout=180, terminate_word="BOOT")
	print "took",int(time.time()) - t, "sec"

def reset(ttyUSB, mode):
	ttyUSB_transfer (ttyUSB, "system reset " + str(mode) + "\n" , timeout=3)

def main():
	ret = 0
	ttyUSB = usb_manager()

	if write_dummy_data(3, 2048*3): # data
		print "factory non persistent partition dummy write fails"
		ret = -1
	if write_dummy_data(6, 2048*4): # data
		print "data partition dummy write fails"
		ret = -1

	spi_data_size = 512 * 4096 # fota
	if write_dummy_data(9, spi_data_size): # cache data
		print "spi cache data partition dummy write fails"
		ret = -1
	if write_dummy_ble_data(): # ble data
		print "ble data partition dummy write fails"
		ret = -1

	wait_boot(ttyUSB)
	reset(ttyUSB, 5)
	#wait erasing
	wait_user_data_erased(ttyUSB)
	wait_boot(ttyUSB)
	reset(ttyUSB, 1)

	if check_erased_data(6): # data
		print "data partition erase fails"
		ret = -1
	if check_erased_data(9): # cache data
		print "data partition erase fails"
		ret = -1
	if check_erased_ble_data(): # ble data
		print "ble data partition erase fails"
		ret = -1

	if ret:
		print "TEST FAILS"
	else:
		print "TEST PASSED"
	return ret


if __name__ == "__main__":
	sys.exit(main())
