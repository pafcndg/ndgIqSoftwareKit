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

# Flash the latest build and analyze the console output to determine if the test
# passed or not
#    FLASH FAILED  - flash failed
#    PASSED        - all tests pass
#    FAILED        - at least one test is failed
#    PANIC         - in case of panic during testing
#    ERROR         - board start failure
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

def main():
    parser = argparse.ArgumentParser(description="Flash the latest build and "
        "analyze the console output to determine if the test passed or not"
        "FLASH FAILED  - flash failed"
        "PASSED        - all tests pass"
        "FAILED        - at least one test is failed"
        "PANIC         - in case of panic during testing"
        "ERROR         - board re-start failure")
    parser.add_argument('root_dir', help="the top level directory (containing out/current after a build)")
    parser.add_argument('result_file', help="path to .json file containing test result for current build variant")
    args = parser.parse_args()

    with open(args.root_dir + "/out/current/build_setup.json") as data_file:
        build_setup_data = json.load(data_file)

    flash_result = flash(args.root_dir)
    # stop tests if the flash failed
    if(flash_result == 0):
        ttyUSB = usb_manager()
        s = ttyUSB_read (ttyUSB, timeout=150, terminate_word="Tests execution complete")

        # check logs to return test results
        if s.find('TEST') != -1:
            if s.find('Panic') != -1:
                result = "PANIC"
            elif s.find('TEST FAILED') != -1:
                result = "FAILED"
            else:
                if s.find('Tests execution complete') != -1:
                    result = "PASSED"
                else:
                    result = "HANGED"
        else:
            result = "ERROR"
    else:
        result = "FLASH_FAILED"

    print build_setup_data["PROJECT"] + " test result: " + result

    report = os.open(args.result_file, os.O_RDWR | os.O_CREAT)
    of = os.fdopen(report, "r+")
    # Ensure that no other process is writing to this file at the same time
    # i.e.(blocks until other process is done)
    fcntl.lockf(of, fcntl.LOCK_EX)
    oldJson = of.read()
    if len(oldJson)!=0:
        reportJson = json.loads(oldJson)
    else:
        reportJson = []
    reportJson.append({'PROJECT': build_setup_data["PROJECT"], 'BUILD_TAG': build_setup_data["BUILD_TAG"], 'TEST_RESULT': result})
    of.seek(0, 0)
    of.write(json.dumps(reportJson, sort_keys=True, indent=4, separators=(',', ': ')))

if __name__ == "__main__":
    sys.exit(main())
