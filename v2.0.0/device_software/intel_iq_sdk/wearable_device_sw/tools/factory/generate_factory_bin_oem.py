#!/usr/bin/python
# -*- coding: utf-8 -*-

# INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors.
# Title to the Material remains with Intel Corporation or its suppliers and
# licensors.
# The Material contains trade secrets and proprietary and confidential information
# of Intel or its suppliers and licensors. The Material is protected by worldwide
# copyright and trade secret laws and treaty provisions.
# No part of the Material may be used, copied, reproduced, modified, published,
# uploaded, posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise.
#
# Any license under such intellectual property rights must be express and
# approved by Intel in writing

# This script must be included in .zip packages delivered to factory
# It will be run by phone flash tool at flash time to:
# - generate the factory image containing provisioning info just before flashing
# - generate the OpenOCD/JLink script necessary to flash this .bin
# - fill the private provisioning data base

# This file is project-independent, but project-specific bits can be hooked
# by adding a custom_factory.py script next to this one

import os
import sys
import json
import argparse
import sqlite3
import ctypes
import uuid
import binascii

import factory_data

# This one should include project-specific hooks
import custom_factory_data_oem

def main():
    parser = argparse.ArgumentParser(description="Generate a binary file containing"
        "various provisioning info such as serial number or public keys. This script"
        "is meant to be used at the factory")
    parser.add_argument('--database', help="the full path to the tinySQL database containing pre-generated"
        "values for signing keys or UUIDs", required=True)
    parser.add_argument('--factory_sn', help="the factory serial number, up to 32 ASCII characters", required=True)
    parser.add_argument('--hardware_id', help="the hardware ID, binary number in hexa representation, up to 32 bytes. If smaller then 32, it is padded with 0s.", required=True)
    parser.add_argument('--flash_files_dir', help="the directory where flash files are located", required=True)
    custom_factory_data_oem.add_custom_arguments(parser)
    args = parser.parse_args()

    # Open the SQL database and find next free row, i.e. the one without serial number set
    print args.database
    con = sqlite3.connect(args.database)
    cur = con.cursor()
    cur.execute('SELECT id,uuid FROM device_keys WHERE factory_serialno IS NULL')
    row = cur.fetchone()
    if row == None:
        print "No more free rows where found in the data base"
        print "Please provision new UUID by adding rows in the DB and try again"
        exit(-1)
    id = row[0]
    print "Found a free row at id=%d in database" % id
    print "UUID : " + str(uuid.UUID(bytes=row[1]))

    # Create the instance containing all info put at the proper address
    fdata = factory_data.OEMData()
    factory_data.init_oem_base_fields(fdata)
    assert ctypes.string_at(fdata.magic, 4) == factory_data.MAGIC
    assert fdata.version == factory_data.VERSION

    fdata.uuid = (ctypes.c_ubyte * 16).from_buffer_copy(row[1])

    # Fill the .bin with generic stuff
    assert len(args.factory_sn)<=32
    if len(args.factory_sn) < 32:
        args.factory_sn += '\0' * (33-len(args.factory_sn))
    fdata.factory_sn = (ctypes.c_ubyte * 32).from_buffer_copy(bytearray(args.factory_sn))
    print "S/N  : " + args.factory_sn

    assert len(args.hardware_id)<=64
    if len(args.hardware_id) < 64:
        args.hardware_id = '0' * (64-len(args.hardware_id)) + args.hardware_id
    assert len(args.hardware_id) == 64
    hwid = bytearray(binascii.unhexlify(args.hardware_id))
    fdata.hardware_id = (ctypes.c_ubyte * 32).from_buffer_copy(hwid)
    print "HWID : " + binascii.hexlify(fdata.hardware_id)

    # Fill the .bin with project-specific stuff
    custom_factory_data_oem.fill_factory_bin(fdata, args)

    # Save the .bin file
    arr = bytearray(fdata)
    assert len(arr) == 512, "ERROR: oem data struct size error"
    out_file = open("factory_data.bin", "wb")
    out_file.write(arr)
    out_file.close()

    # Post-build hook, use it to generate flashing script or other files
    custom_factory_data_oem.post_factory_data_hook(fdata, args)

    # Set the serial number for the current row in the data base and save it
    cur.execute('UPDATE device_keys SET factory_serialno = ? WHERE id = ?', (args.factory_sn, id))
    con.commit()

if __name__ == "__main__":
    sys.exit(main())
