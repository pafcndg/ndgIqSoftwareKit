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

# Custom methods for Quark SE build_info generation

import os
import sys
import json
import build_info

# BLE Core
ble_core_total_flash = 262144   # BLE Core chip has 256KiB Flash
ble_core_total_ram   = 16384    # BLE Core chip has 16KiB RAM

# BLE Core Softdevice
ble_core_softdevice_flash = 131072  # Softdevice already occupies 128KiB
ble_core_softdevice_ram   = 9216    # Softdevice use 9KiB of RAM

# BLE Core app
ble_core_app_flash = ble_core_total_flash - ble_core_softdevice_flash
ble_core_app_ram   = ble_core_total_ram - ble_core_softdevice_ram

def load_partition_sizes(build_dir):
    sizes = {}
    with open(build_dir + "/build_info_data.h") as myfile:
            lines = {}
            lines = (line.rstrip() for line in myfile) # All lines including the blank ones
            lines = (line for line in lines if line) # Non-blank lines#
            for l in lines:
                if l[0] != '#':
                    name, var = l.partition("=")[::2]
                    sizes[name.strip()] = eval(var)
    return sizes

def get_custom_info(build_dir):
    """ Compute various info for each sub-targets (quark, arc and ble_core).
        This code is the only project-specific bit used by most scripts.
    """
    sizes = load_partition_sizes(build_dir)
    build_setup = json.loads(open(build_dir + "/build_setup.json", "r").read())
    buildvariant = build_setup['BUILDVARIANT']

    quark_file = 'firmware/quark'
    quark_binheader = build_info.get_binary_version_header_from_binfile(os.path.join(build_dir, quark_file+'.bin'))

    arc_file = 'firmware/arc'
    arc_binheader = build_info.get_binary_version_header_from_binfile(os.path.join(build_dir, arc_file+'.bin'))

    ble_core_file = 'firmware/ble_core/image'
    ble_core_binheader = build_info.get_binary_version_header_from_binfile(os.path.join(build_dir, ble_core_file+'.bin'))

    bootloader_file = 'firmware/bootloader_quark'
    bootloader_binheader = build_info.get_binary_version_header_from_binfile(os.path.join(build_dir, bootloader_file+'.bin'))

    ble_core_fp = ""
    try:
        ble_core_fp = build_info.get_footprint_from_bin_and_statfile(os.path.join(build_dir, ble_core_file+'.bin'),
                os.path.join(build_dir, ble_core_file+'.stat'), ble_core_total_flash, ble_core_app_ram)
    except:
        pass

    return {
        "quark": {
            "footprint": build_info.get_footprint_from_bin_and_statfile(os.path.join(build_dir, quark_file+'.bin'),
                os.path.join(build_dir, quark_file+'.stat'), sizes['_QUARK_FLASH_SIZE'], sizes['_QUARK_RAM_SIZE']),
            "hash": quark_binheader.get_printable_hash(),
            "version_string": quark_binheader.get_printable_version_string(),
            "bin_file": quark_file+'.bin',
            "elf_file": quark_file+'.elf'
        },
        "arc": {
            "footprint": build_info.get_footprint_from_bin_and_statfile(os.path.join(build_dir, arc_file+'.bin'),
                os.path.join(build_dir, arc_file+'.stat'), sizes['_ARC_FLASH_SIZE'], sizes['_ARC_RAM_SIZE']),
            "hash": arc_binheader.get_printable_hash(),
            "version_string": arc_binheader.get_printable_version_string(),
            "bin_file": arc_file+'.bin',
            "elf_file": arc_file+'.elf'
        },
        "ble_core": {
            "footprint": ble_core_fp,
            "hash":  ble_core_binheader.get_printable_hash(),
            "version_string": ble_core_binheader.get_printable_version_string(),
            "bin_file": ble_core_file+'.bin',
            "elf_file": ble_core_file+'.elf'
        },
        "bootloader": {
            "footprint": build_info.get_footprint_from_bin_and_statfile(os.path.join(build_dir, bootloader_file+'.bin'),
                os.path.join(build_dir, bootloader_file+'.stat'), sizes['_BOOTLOADER_FLASH_SIZE'], sizes['_QUARK_RAM_SIZE']),
            "hash": bootloader_binheader.get_printable_hash(),
            "version_string": bootloader_binheader.get_printable_version_string(),
            "bin_file": bootloader_file+'.bin',
            "elf_file": bootloader_file+'.elf'
        }
    }
