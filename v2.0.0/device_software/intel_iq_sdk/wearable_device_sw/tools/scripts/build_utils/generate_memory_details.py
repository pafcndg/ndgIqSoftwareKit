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

import generate_partitions_report
import generate_memory_report


def generate_output_page(sections_list):
    out = create_hmtl_head(sections_list)
    out += create_page_content(sections_list)
    return out

def create_hmtl_head(sections_list):
    out = """<!DOCTYPE html>
<html>
<head>
  <title>Memory report</title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />"""
    out += insert_html_head_style();
    out += insert_html_head_javascript(sections_list);
    out += """
</head>
"""
    return out


def insert_html_head_style():
    out = """
  <!-- CSS Styling -->
  <style>
    tr:nth-child(even) { background-color: #FFFFFF; }
    tr:nth-child(odd)  { background-color: #E3EDFF; }
    td.value           { text-align: right; padding-left: 2em; padding-right:0.2em;}
    td.text            { text-align: left;  padding-left: 0.2em;}
    .feature { text-align: left; }
    .level0  { padding-left: 20px; font-size: 105%; font-weight: bold; }
    .level1  { padding-left: 50px; font-size: 100%;}
    .level2  { padding-left: 80px; font-size: 90%;}
    .feature_size { text-align: right; }
    .nav_target {
      padding-top: 2em;
    }
    .nav {
      position: fixed;
      top: 0;
    }
    .nav ul {
      list-style: none;
      background-color: #444;
      text-align: center;
      padding: 0;
      margin: 0;
    }
    .nav li {
      font-family: sans-serif;
      font-size: 1.2em;
      display: inline-block;
      width: 130px;
      line-height: 50px;
      border-bottom: none;
    }
    .nav a {
      display: block;
      text-decoration: none;
      color: #fff;
    }
    .nav a:hover {
      background-color: #03c;
    }
    .nav a.active {
      background-color: #fff;
      color: #444;
    }
  </style>
"""
    return out

def insert_html_head_javascript(sections_list):
    out = """
  <!-- Javascripts -->
  <script type="text/javascript" src="https://www.google.com/jsapi"></script>
  <script type="text/javascript">
  function show_or_hide(id, active) {
     var e = document.getElementById(id);
     if(id == active)
        e.style.display = 'block';
     else
        e.style.display = 'none';
  }

  function set_active(active) {
"""
    for section in sections_list:
        out += "      show_or_hide('div_%s', active);\n" % section['short']
    out += """
  }

  function default_selection() {
    /*set_active('div_map');*/
  }

  </script>
"""
    return out


def create_page_content(sections_list):
    out = """
<!-- Page content -->
<body onload="default_selection()">
  <header>
"""
    # Generate navigation menu
    out += """
    <div class="nav">
      <ul id="menu">
"""
    for section in sections_list:
        out += """        <li><a id="link_%s" href="#" onclick="set_active('div_%s');">%s</a></li>\n""" \
                 % (section['short'], section['short'], section['nice'])
    out += """      </ul>
    </div>
  </header>
"""
    # Generate sections content
    for section in sections_list:
        out += """
<div id="div_%s">
  <h1 class="nav_target" id="mem_%s">%s</h1>
%s
</div>
""" % (section['short'], section['short'], section['nice'], section['content'])
    # Generate appendix
    for section in sections_list:
        if section['suffix'] != "":
            out += "%s\n" % section['suffix']
    out += """
</body>
</html>
"""
    return out

def main():
    parser = argparse.ArgumentParser(description="""Output an HTML report with
    the analyze of where the size on flash and on RAM of the ELF file is spent""")
    parser.add_argument('build_dir', help="the directory where to look for build artifacts")
    parser.add_argument('source_dir', help="the directory where to look for the source")
    parser.add_argument('features_list', help="the file that lists all the features and their folders")
    parser.add_argument('--ble_core', help="the BLE core is built and not pre-built", default="0")
    args = parser.parse_args()

    # Generate sections and their content
    sections_list = []

    content = generate_partitions_report.generate_partitions_section(args.build_dir, "", "", "", "")
    sections_list.append({'short':'map', 'nice':'Mapping', 'content':content, 'suffix':''})


    content, suffix = generate_memory_report.generate_target_memory_section( \
                args.build_dir, 'firmware/', 'quark', \
                args.source_dir, \
                args.features_list)
    sections_list.append({'short':'qrk', 'nice':'Quark', 'content':content, 'suffix':suffix})


    content, suffix = generate_memory_report.generate_target_memory_section( \
                args.build_dir, 'firmware/', 'arc', \
                args.source_dir, \
                args.features_list)
    sections_list.append({'short':'arc', 'nice':'ARC', 'content':content, 'suffix':suffix})


    content, suffix = generate_memory_report.generate_target_memory_section( \
                args.build_dir, 'firmware/', 'bootloader_quark', \
                args.source_dir, \
                '')
    sections_list.append({'short':'bootloader', 'nice':'Bootloader', 'content':content, 'suffix':suffix})

    if args.ble_core != "0":
      content, suffix = generate_memory_report.generate_target_memory_section( \
                  args.build_dir, 'firmware/ble_core/', 'quark_se_ble_core', \
                  args.source_dir, \
                  '')
      sections_list.append({'short':'ble', 'nice':'BLE core', 'content':content, 'suffix':suffix})

    # Generate HTML report
    out = generate_output_page(sections_list)
    print out


if __name__ == "__main__":
    sys.exit(main())

