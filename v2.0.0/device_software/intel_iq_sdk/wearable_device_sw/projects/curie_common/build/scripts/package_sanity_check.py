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

# Flash the latest build and analyze the console output to check if:
# - the flash succeeded
# - the boot succeeded without panic
# - the board respond to a tcmd on reference_app.
# possible results :
#    FLASH FAILED  - flash failed
#    PASSED        - tcmd passed
#    PANIC         - in case of panic during testing
#    HANGED        - board does not respond anymore

import os
import sys
import json
import subprocess
import argparse
import fcntl
import threading
import time
from signal import alarm, signal, SIGALRM, SIGKILL
from subprocess import Popen, PIPE, call, check_output
from tests_library import *

def testVersion(ttyUSB):
    s = ttyUSB_transfer (ttyUSB, "version get\n" , timeout=2)

    # check logs to return results
    if s.find('Panic') != -1:
        return "PANIC"
    elif s.find('Micro-sha1') != -1:
        return "PASSED"

    return "HANGED"

def testPanic(ttyUSB, root_dir):
    s = ttyUSB_transfer (ttyUSB, "debug panic 4\n" , timeout=15, terminate_word="BOOT", no_print=True)
    if s.find('PANIC') == -1 or s.find('BOOT') == -1:
        print s
        print "debug panic command failed"
        return "FAILED"
    print s.replace("PANIC","Panic")
    # panic is written on nvm by bootloader, reset board again to upload panic partition
    s = ttyUSB_read (ttyUSB, timeout=20)
    s = ttyUSB_transfer (ttyUSB, "system reset 1\n" , timeout=3)

    # dump panic
    print "Dump panic"
    dump_file = "/tmp/panic_dump_file"
    os.system("rm "+ dump_file)
    if os.system("dfu-util.pft -a 4 -R -U " + dump_file ):
        print "upload panic partition failed"
        return "FAILED"

    # parse it
    s = check_output(['python', root_dir + '/wearable_device_sw/tools/scripts/panic/DecodePanic.py', root_dir + '/out/current/firmware', dump_file])
    print s
    if s.count("magic = 0x21636e50") != 2 :
        print "PANIC not found in nvm"
        return "FAILED"

    return "PASSED"

def campaign(root_dir):
    result = "PASSED"
    ttyUSB = usb_manager()
    # erase panic partition
    time.sleep(1)
    os.system("truncate -s 4096 /tmp/erase; dfu-util.pft -a 4 -D /tmp/erase -R")

    # wait for boot
    ttyUSB_read (ttyUSB, timeout=25)

    # test if version cmd is responding
    result = testVersion(ttyUSB)
    if result != "PASSED":
        return result

    result = testPanic(ttyUSB, root_dir)

    return result


def main():
    parser = argparse.ArgumentParser(description="Flash the latest build and "
        "analyze the console output to check if:"
        "FLASH FAILED  - flash failed"
        "PASSED        - tcmd passed"
        "PANIC         - in case of panic during testing"
        "HANGED        - board does not respond anymore")
    parser.add_argument('root_dir', help="the top level directory (containing out/current after a build)")
    args = parser.parse_args()

    flash_result = flash(args.root_dir)
    # stop tests if the flash failed
    if(flash_result != 0):
        result = "FLASH_FAILED"
    else:
        result = campaign(args.root_dir)
    print "reference app result: " + result


    if (result != "PASSED"):
        return -1
    return 0

if __name__ == "__main__":
    sys.exit(main())
