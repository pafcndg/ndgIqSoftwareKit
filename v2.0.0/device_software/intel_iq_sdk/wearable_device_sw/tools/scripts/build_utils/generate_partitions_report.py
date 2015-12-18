#!/usr/bin/env python2

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

import os
import sys
import argparse
import subprocess
import json
import operator
import re

def process_memory_mapping(build_dir):
    flash_chips_list = []
    partitions_list = []
    partitions_dict = {}
    with open(os.path.join(build_dir, "partitions_info.h")) as myfile:
        lines = {}
        lines = (line.rstrip() for line in myfile) # All lines including the blank ones
        lines = (line for line in lines if line) # Non-blank lines#
        for l in lines:
            if l.startswith('partition='):
                partition = extract_struct_fields(l)
                # if its a valid partition, add it to the list AND DICTIONARY
                if partition != {}:
                    partitions_list.append(partition)
                    partitions_dict[partition['name']]=partition
            elif l.startswith('flash_chip='):
                flash_chip = extract_struct_fields(l)
                # if its a valid chip, add it to the list
                if flash_chip != {}:
                    flash_chips_list.append(flash_chip)
            else:
                # don't know how to parse this line: skip
                continue
    # Processing after all extraction
    # Flash chip total size
    for flash_chip in flash_chips_list:
        flash_chip['size'] = flash_chip['nb_blocks'] * flash_chip['block_size']
    # Partition number of blocks and size
    for partition in partitions_list:
        partition['nb_blocks'] = (partition['end_block'] - partition['start_block'] + 1)
        partition['size'] = partition['nb_blocks'] * flash_chips_list[partition['flash_id']]['block_size']
    # return lists
    return flash_chips_list, partitions_list, partitions_dict

def extract_struct_fields(line):
    structure = {}
    # Search for patterns:  'word': value,
    filter = re.compile('\'[a-zA-Z_]*\': .*?[,\}]')
    array = filter.findall(line);
    # Extract all the fields and add to structure
    for keyval in array:
        name, var = re.split(':|,|}', keyval)[:2:]
        name = name.replace("'", "").strip()
        structure[name] = eval(var)
    return structure

def size_to_KiB(size_bytes):
    return "%0.2f KiB" % (size_bytes / 1024.0)

def size_to_percent(used_bytes, total_bytes):
    return "%0.2f %%" % (100.0 * used_bytes / total_bytes)


def generate_partitions_section(build_dir, link_quark, link_arc, link_bootloader, link_ble_core):

    # Extract partitions information
    flash_list, partitions_list, partitions_dict = process_memory_mapping(build_dir)

    # Extract partition usage
    quark_file = 'firmware/quark'
    quark_used_flash = os.path.getsize(os.path.join(build_dir, quark_file+'.bin'))
    partitions_dict['QUARK_FLASH']['used'] = quark_used_flash
    if link_quark != "":
        partitions_dict['QUARK_FLASH']['url'] = link_quark

    arc_file = 'firmware/arc'
    arc_used_flash = os.path.getsize(os.path.join(build_dir, arc_file+'.bin'))
    partitions_dict['ARC_FLASH']['used'] = arc_used_flash
    if link_arc != "":
        partitions_dict['ARC_FLASH']['url'] = link_arc

    bootloader_file = 'firmware/bootloader_quark'
    bootloader_used_flash = os.path.getsize(os.path.join(build_dir, bootloader_file+'.bin'))
    partitions_dict['BOOTLOADER_FLASH']['used'] = bootloader_used_flash
    if link_bootloader != "":
        partitions_dict['BOOTLOADER_FLASH']['url'] = link_bootloader

    ble_core_file = 'firmware/ble_core/image'
    ble_core_used_flash = os.path.getsize(os.path.join(build_dir, ble_core_file+'.bin'))
    partitions_dict['BLE_APP']['used'] = ble_core_used_flash
    if link_ble_core != "":
        partitions_dict['BLE_APP']['url'] = link_ble_core

    def _display_chips_mapping(flash_list, partitions_list):
        out = ""
        for chip in flash_list:
            out += "\n    <h2>%s</h2>\n" % chip['nice_name']
            out += _display_memory_mapping_list(partitions_list, chip)
        return out

    def _display_memory_mapping_list(partitions_list, chip):
        chip_id = chip['flash_id']

        out = "      <table>"
        out += "<tr><th>Memory chip</th><th>Blocks</th><th>Partition</th>"
        out += "<th>Nb blocks</th><th>Size</th><th>Used</th><th>Usage</th></tr>"
        # Get number of partitions on this flash
        nb_partitions=0
        for partition in partitions_list:
            if partition['flash_id'] == chip_id:
                nb_partitions = nb_partitions + 1
        # 1 row per partition
        nb_rows=nb_partitions
        # We write the left column for the flash_id (and empty line above)
        out += "<tr><td rowspan=%d>%s<br/>Flash %d<br/>Block size: %d</td>" % (nb_rows+1, chip['nice_name'], chip_id, chip['block_size'])
        out += "<td></td><td></td><td></td><td></td><td></td><td></td></tr>"
        for partition in partitions_list:
            if partition['flash_id'] == chip_id:
                out += _display_memory_mapping_row(partition)
        # End of mapping table for this chip_id
        out +="</table>"
        return out

    def _display_memory_mapping_row(partition):
        out = "<tr>"
        # Start of partition row
        out += "<td class='value'>%d<br>%d</td>"  % (partition['start_block'], partition['end_block'])
        try:
            # has a valid URL to display
            out += "<td class='text'><a href=\"%s\">%s</a></td>" % (partition['url'], partition['name'])
        except:
            # no valid URL to display
            out += "<td class='text'>%s</td>" % partition['name']
        out += "<td class='value'>%d</td>" % partition['nb_blocks']
        out += "<td class='value'>%s</td>" % size_to_KiB(partition['size'])
        # if partition has a 'used' field, show it
        try:
            out += "<td class='value'>%s</td>" % size_to_KiB(partition['used'])
            out += "<td class='value'>%s</td>" % size_to_percent(partition['used'], partition['size'])
        except:
            out += "<td class='value'>/</td>"
            out += "<td class='value'>/</td>"
        # End of partition row
        out += "</tr>"
        return out

    return _display_chips_mapping(flash_list, partitions_list)
