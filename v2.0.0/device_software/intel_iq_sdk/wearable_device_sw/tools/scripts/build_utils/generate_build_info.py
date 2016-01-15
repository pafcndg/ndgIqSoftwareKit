#!/usr/bin/env python

# Copyright (C) 2015, Intel Corporation
# All rights reserved.

# Generate a JSON file with useful info on the build like memory footprints size
# The JSON section for the given build directory is dumped to stdout
# The data is also merged in the given merge_out_file allowing to combine the
# build info for several targets in a single file.

import os
import sys
import json
import fcntl
import argparse

import custom_build_info

def main():
    parser = argparse.ArgumentParser(description="Generate a JSON file with useful"
        " info on the build like memory footprints size. The resulting JSON is "
        "merged with existing JSON data in merge_out_file and is also printed to"
        "screen.")
    parser.add_argument('build_dir', help="the directory where to look for build artifacts")
    parser.add_argument('merge_out_file', help="a file in which to merge the resulting JSON data")
    args = parser.parse_args()

    custom_info = custom_build_info.get_custom_info(args.build_dir)
    print json.dumps(custom_info, indent=4)

    build_setup = json.loads(open(args.build_dir + "/build_setup.json", "r").read())
    board = build_setup['BOARD']
    buildvariant = build_setup['BUILDVARIANT']
    project = build_setup['PROJECT']
    build_tag = build_setup['BUILD_TAG']
    download_dir_url = build_setup['DOWNLOAD_DIR_URL']

    # Merge the generated JSON into merge_out_file if it exists
    fd = os.open(args.merge_out_file, os.O_RDWR | os.O_CREAT)
    of = os.fdopen(fd, "r+")
    # Ensure that no other process is writing to this file at the same time
    # i.e.(blocks until other process is done)
    fcntl.lockf(of, fcntl.LOCK_EX)
    data = of.read()
    previousJson = {}
    if len(data)!=0:
        previousJson = json.loads(data)

    previousJson['project'] = project
    previousJson['build_tag'] = build_tag
    previousJson['download_dir_url'] = download_dir_url
    previousJson['manifest_file'] = 'manifest-'+build_tag+'.xml'

    # Append data specific to this variant build
    if 'variants' not in previousJson:
        previousJson['variants']={}
    variant = project+'-'+board+'-'+buildvariant
    if variant not in previousJson['variants']:
        previousJson['variants'][variant]={}
    previousJson['variants'][variant]['board'] = board
    previousJson['variants'][variant]['buildvariant'] = buildvariant
    previousJson['variants'][variant]['package_file'] = '/'+project+'/'+variant+'-'+build_tag+'.zip'
    previousJson['variants'][variant]['targets'] = custom_info

    # Append stuff for Phone Flash Tool
    if 'hardwares' not in previousJson:
        previousJson['hardwares']={}
    if board not in previousJson['hardwares']:
        previousJson['hardwares'][board] = {'variants': {}, 'hidden_to_end_user': False}
    previousJson['hardwares'][board]['variants'][project+'-'+buildvariant] = {'hardware_family': project, 'flashfiles': {'flash': ['/'+project+'/'+project+'-'+board+'-'+buildvariant+'-'+build_tag+'.zip:flash.json']}}

    of.seek(0, 0)
    of.write(json.dumps(previousJson, sort_keys=True, indent=4))

if __name__ == "__main__":
    sys.exit(main())
