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
# - the tests passed on tests builds
# - the board respond to a tcmd on reference_app.
# possible results :
#    FLASH FAILED  - flash failed
#    PASSED        - all tests pass
#    FAILED        - at least one test is failed
#    PANIC         - in case of panic during testing
#    ERROR         - board start failure
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

def ttyUSB_transfer(ttyUSB, write_data = None, timeout = -1, terminate_word = None, no_print = None):
    log = []

    class TimeoutAlarm(Exception):
        pass

    def timeout_handler(signum, frame):
        raise TimeoutAlarm

    if timeout > 0:
        signal(SIGALRM, timeout_handler)
        alarm(timeout)
    try:
        tty = open(ttyUSB,"r+")
        if write_data != None:
            tty.write(write_data)

        if timeout > 0:
            while 1:
                line = tty.readline()
                if no_print == None:
                    print line,
                log.append(line)
                if terminate_word != None and line.find(terminate_word) != -1:
                    break
        tty.close()
        if timeout > 0:
            alarm(0)

    except TimeoutAlarm:
        tty.close()
    return ''.join(log)

def ttyUSB_read(ttyUSB, timeout = -1, terminate_word = None, no_print = None):
    return ttyUSB_transfer(ttyUSB, write_data = None, timeout = timeout, terminate_word = terminate_word, no_print = no_print)

def flash(root_dir):
    flash_log_file = root_dir + "/out/current/flash.log"
    flash_retest_cpt = 0
    while (flash_retest_cpt < 5):
        call("make flash | tee " + flash_log_file, shell=True)
        s = open(flash_log_file).read()
        if s.find('Flash failed') != -1:
            flash_retest_cpt = flash_retest_cpt + 1
            print "flash error, switch off/on and Reflash"
            usb_manager()
        else:
            return 0

    print "Flash failed -> exit"
    return -1

def usb_manager():
    if os.path.exists("/dev/ttyRelayCard2"):
        print "USB Switcher detected, sequence switch OFF/ON enabled"
        print "SWITCH OFF"
        call("echo d > /dev/ttyRelayCard2", shell=True)
        print "Sleep 2s"
        time.sleep(2)
        print "SWITCH ON"
        call("echo n > /dev/ttyRelayCard2", shell=True)
        time.sleep(1)
    else:
        print "USB Switcher not detected"

    for i in range(16, 0, -1):
        if os.path.exists("/dev/ttyUSB" + str(i)):
            ttyUSB="/dev/ttyUSB" + str(i)
            #kill remaining process reading ttyUSB port
            call("fuser -a -k " + ttyUSB, shell=True)
            call("stty raw -echo -onlcr < " + ttyUSB, shell=True)

    return ttyUSB
