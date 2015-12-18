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

import binascii
import ctypes
import sys

import curie_factory_data

base_addr = "0xffffe000"

def add_custom_arguments(parser):
    parser.add_argument('--bt_mac_address_type', help="enter BT MAC address type: static or public", required=True)
    parser.add_argument('--bt_mac_address', help="enter BT MAC address in human readable format. eg: aa:8e:24:33:ee:36", required=True)
    parser.add_argument('--ble_name', help="enter BLE name string", required=True)

def fill_factory_bin(fdata, args):
    # We explicitely set all fields to zero since this is for the Curie module
    # This will be set per-project
    fdata.hardware_info.hardware_name = 0x00
    fdata.hardware_info.hardware_type = 0x00
    fdata.hardware_info.hardware_revision = 0x00
    fdata.hardware_info.hardware_variant[0] = 0x00
    fdata.hardware_info.hardware_variant[1] = 0x00
    fdata.hardware_info.hardware_variant[2] = 0x00


    curie_fdata = curie_factory_data.CurieProjectData()
    # Curie MAC address type
    ############
    if args.bt_mac_address_type.lower() == "public":
        bt_mac_address_type = (ctypes.c_ubyte).from_buffer_copy(binascii.unhexlify("00"))
    elif args.bt_mac_address_type.lower() == "static":
        bt_mac_address_type = (ctypes.c_ubyte).from_buffer_copy(binascii.unhexlify("01"))
    else:
        print "Invalid bt_mac_address_type"
        sys.exit()

    print "MAC address type: " + args.bt_mac_address_type.lower()
    curie_fdata.bt_mac_address_type = bt_mac_address_type

    # Curie MAC address
    ############
    bt_mac_address = binascii.unhexlify(args.bt_mac_address.replace(':',''))
    assert len(bt_mac_address) == 6
    print "BT MAC address     : " + args.bt_mac_address
    curie_fdata.bt_mac_address = (ctypes.c_ubyte * 6).from_buffer_copy(bt_mac_address)

    ble_name = args.ble_name.lower()
    assert len(ble_name) <= 19
    print "BLE name: " + ble_name
    curie_fdata.ble_name = (ctypes.c_ubyte * 20).from_buffer_copy(ble_name.ljust(20, '\0'))

    # Curie : pad with 0xFF to fit project_data size
    ############
    curie_fdata_padded = bytearray(curie_fdata).ljust(len(fdata.project_data), binascii.unhexlify("FF"))
    fdata.project_data = type(fdata.project_data).from_buffer_copy(curie_fdata_padded)

def post_factory_data_hook(fdata, args):
    # Creates the OpenOCD flash script
    flash_script = """### Generated script, please do not edit
### Flash provisining data in OTP
load_image factory_data.bin %s
verify_image factory_data.bin %s
""" % (base_addr, base_addr)
    f = open('factory_data.cmd', 'w')
    f.write(flash_script)
    f.close()

    # Re-check the first bit in the final .bin, just in case
    f = open('factory_data.bin', 'rb')
    d = bytearray(f.read())
    assert d[0] == 0xFF, "Warning, factory data will be definitely locked"
