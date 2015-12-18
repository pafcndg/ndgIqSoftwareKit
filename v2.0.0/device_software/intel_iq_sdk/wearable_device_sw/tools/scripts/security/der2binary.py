#!/usr/bin/python

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

# Top level makefile, repo tool should create a link on this file at the root
# of the build environement.

# -*- coding: utf-8 -*-

"""
    Module used to convert der to binary public key
    Depends on python-ecdsa package
"""

import argparse
from ecdsa import SigningKey
from ecdsa import VerifyingKey
import sys

def main(argv):

    """
        Main procedure to sign a binary
    """


    parser = argparse.ArgumentParser(description='convert der to raw')
    parser.add_argument('-s','--secretkey_file', help='Secret key', required=True)
    parser.add_argument('-p','--publickey_file', help='Public key', required=True)
    args = parser.parse_args()

    secretkey_file = args.secretkey_file
    publickey_file = args.publickey_file


    privkey = SigningKey.from_der(open(secretkey_file).read())
    pubkey = VerifyingKey.from_der(open(publickey_file).read())

    open(secretkey_file[0:-4] + ".bin", "wb").write(privkey.to_string())
    open(publickey_file[0:-4] + ".bin", "wb").write(pubkey.to_string())

if __name__ == "__main__":
    main(sys.argv[1:])
