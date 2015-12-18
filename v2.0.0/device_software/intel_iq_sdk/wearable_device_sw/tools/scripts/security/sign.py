#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
    Module used to sign a binary using ECDSA and SHA256
    Depends on python-ecdsa package
"""

import ctypes
import argparse
from ecdsa import SigningKey
from hashlib import sha256
import sys

class SigHeader(ctypes.Structure):

    """
        Signature header with $SIG magic
    """
    _fields_ = [
        ("magic", ctypes.c_char * 4),
        ("version", ctypes.c_byte),
        ("key_index", ctypes.c_byte),
        ("reserved0", ctypes.c_int16),
        ("size", ctypes.c_int32),
        ("signature", ctypes.c_ubyte * 64),
        ("reserved1", ctypes.c_ubyte * 52),
    ]

def main(argv):

    """
        Main procedure to sign a binary
    """


    parser = argparse.ArgumentParser(description='tool to sign binaries using ECDSA')
    parser.add_argument('-s','--privkey_file', help='Private key', required=True)
    parser.add_argument('-i','--input_file', help='File to sign', required=True)
    parser.add_argument('-o','--output_file', help='Signed output', required=True)
    parser.add_argument('-f','--ota_package', help='OTA package', required=True, default=0)
    args = parser.parse_args()

    privkey_file = args.privkey_file
    input_file = args.input_file
    output_file = args.output_file
    ota_package = int(args.ota_package)


    privkey = SigningKey.from_der(open(privkey_file).read())

    fd = open(input_file, "rb")
    if ota_package == 0:
        fd.seek(0x80)
    message = bytearray(fd.read())
    fd.close()

    # align on 4 bytes
    if len(message) % 4 != 0:
        message += bytearray.fromhex("ff" * (4 - (len(message) % 4)))

    signature = bytearray(privkey.sign(message, hashfunc=sha256))
    assert len(signature) == 64

    sigheader = SigHeader("$SIG", 0x01, 0x00, 0x00, len(message))
    sigheader.signature[0:64] = signature[0:64]
    sigheader.reserved1[0:52] = [0] * 52
    arr = bytearray(sigheader)
    assert len(arr) == 128
    arr[0x80:0x80+len(message)] = message

    sigfile = open(output_file, "wb")
    sigfile.write(arr)
    sigfile.close()

if __name__ == "__main__":
    main(sys.argv[1:])
