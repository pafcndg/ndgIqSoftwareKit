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

import ctypes
import hashlib
import binascii

MAGIC = "$B!N"
VERSION = 0x01

class BinaryVersionHeader(ctypes.Structure):
    """Binary version header with $B!N magic, see declaration in
       infra/version.h """
    _pack_ = 1
    _fields_ = [
        # Always equal to $B!N
        ("magic", ctypes.c_char * 4),

        # Header format version
        ("version", ctypes.c_ubyte),
        ("major", ctypes.c_ubyte),
        ("minor", ctypes.c_ubyte),
        ("patch", ctypes.c_ubyte),

        # Human-friendly version string, free format (not NULL terminated)
        # Advised format is: PPPPXXXXXX-YYWWTBBBB
        #  - PPPP  : product code, e.g ATP1
        #  - XXXXXX: binary info. Usually contains information such as the
        #    binary type (bootloader, application), build variant (unit tests,
        #    debug, release), release/branch name
        #  - YY    : year last 2 digits
        #  - WW    : work week number
        #  - T     : build type, e.g. [Engineering], [W]eekly, [L]atest,
        #    [R]elease, [P]roduction, [F]actory, [C]ustom
        #  - BBBB  : build number, left padded with zeros
        # Examples:
        #  - ATP1BOOT01-1503W0234
        #  - CLRKAPP123-1502R0013
        ("version_string", ctypes.c_ubyte * 20),

        # Micro-SHA1 (first 4 bytes of the SHA1) of the binary payload excluding
        # this header. It allows to uniquely identify the exact binary used.
        ("hash", ctypes.c_ubyte * 4),

        # Position of the payload relative to the address of this structure
        ("offset", ctypes.c_int32),
        ("reserved_1", ctypes.c_ubyte * 4),

        # Size of the payload, i.e. the full binary on which the hash was
        # computed (excluding this header). The beginning of the payload
        # is assumed to start right after the last byte of this structure.
        ("size", ctypes.c_uint32),
        ("reserved_2", ctypes.c_ubyte * 4)
    ]

    def get_printable_hash(self):
        return binascii.hexlify(self.hash)

    def get_printable_version_string(self):
        return ctypes.string_at(self.version_string, 20).partition(b'\0')[0]

