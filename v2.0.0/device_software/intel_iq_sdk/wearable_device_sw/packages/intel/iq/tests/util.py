#!/usr/bin/env python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Module defining library test functions
#
# Requires the bluepy helper to be compiled:
#     make -C /path/to/bluepy/bluepy
# To find out your BD address, you can use for instance the
# "nRF Master Control Panel" application.
# To distinguish between devices, use signal strength or set
# a specific BLE name using the "ble set_name" test command
#

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



# import btle_curie from a known location relative to the local script
import imp
import os
import inspect
current_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
btle_curie_path = '../../curie-ble/tools/scripts/bluepy/btle_curie.py'
try:
    btle_curie = imp.load_source('btle_curie', current_dir + '/' + btle_curie_path)
except:
    print '\nERROR: \nCan not find the file: ' + current_dir + btle_curie_path
    exit()


def connection(topics, test_type="none"):
    import sys
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('host', action='store',
                        help='BD address of BT device')
    parser.add_argument('-i', '--device', action='store', help='HCI device (hci0 etc..)', default="hci0")
    parser.add_argument('-p', '--public', action='store_true',
                        help='Indicate BD address is public (default is random)')
    parser.add_argument('-v','--verbose', action='store_true',
                        help='Increase output verbosity')

    if test_type != "none":
        parser.add_argument('binary_file', action='store',
                            help='Firmware to flash')

    arg = parser.parse_args(sys.argv[1:])

    btle_curie.btle.Debugging = arg.verbose

    # connect to device
    device = btle_curie.curie_device(arg)

    device.setSecurityLevel("medium")

    # discover
    device.discover()

    ispp_mtu = device.ispp_setup(1000)

    if topics != None:
        my_app = btle_curie.DefaultTopicApplication(topics)

        # add some topics
        device.itm_add_topic_app(my_app)

    device.itm_connect()

    return device
