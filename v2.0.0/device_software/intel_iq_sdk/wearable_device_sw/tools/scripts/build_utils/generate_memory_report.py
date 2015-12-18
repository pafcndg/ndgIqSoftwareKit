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

# Return a dict containing symbol_name: path/to/file/where/it/originates
# for all symbols from the .elf file. Optionnaly strips the path according
# to the passed sub-path
def load_symbols_and_paths(elf_file, path_to_strip = None):
    symbols_paths = {}
    nm_out = subprocess.check_output(["nm", elf_file, "-S", "-l", "--size-sort", "--radix=d"])
    for line in nm_out.split('\n'):
        fields = line.replace('\t', ' ').split(' ')
        # Get rid of trailing empty field
        if len(fields) == 1 and fields[0] == '':
            continue
        assert len(fields)>=4
        if len(fields)<5:
            path = ":/" + fields[3]
        else:
            path = fields[4].split(':')[0]
        if path_to_strip != None:
            if path_to_strip in path:
                path = path.replace(path_to_strip, "") + '/' + fields[3]
            else:
                path = ":/" + fields[3]
        symbols_paths[fields[3]] = path
    return symbols_paths


# Return a JavaScript snippet necessary for drawing a treemap diagram.
# To display the diagram, the HTML code needs to add a div with an ID
# equal to the div_name argument
def create_java_script_for_table(table, div_name):
    out = """<script type="text/javascript">
google.load("visualization", "1", {packages:["treemap"]});
google.setOnLoadCallback(drawChart);
function drawChart() {
    var data = google.visualization.arrayToDataTable(["""

    for t in table:
        out += "[" + t[0] + ", " + t[1] + ", " + t[2] + ", " + t[3] + "],\n"

    out += """]);

    tree = new google.visualization.TreeMap(document.getElementById('"""+ div_name +"""'));

    tree.draw(data, {
        minHighlightColor: '#8c6bb1',
        midHighlightColor: '#9ebcda',
        maxHighlightColor: '#edf8fb',
        minColor: '#009688',
        midColor: '#f7f7f7',
        maxColor: '#ee8100',
        headerHeight: 20,
        fontColor: 'black',
        showScale: true,
        generateTooltip: showSize,
        title: 'Footprint Details (Left click to dig in the elf file, right click to go up)',
      });

    function showSize(row, size, value) {
        return '<div style="background:#fd9; padding:10px; border-style:solid">' +
           '<span style="font-family:Courier"><b> ' + data.getValue(row, 0) + ': ' + size + ' Bytes</b> </div>';
    }

}</script>\n"""
    return out



def generate_target_memory_section(out_current_dir, rel_path, target_file, source_dir, features_json):
    features_path_data = None
    try:
        features_path_data = json.loads(open(features_json, 'r').read())
    except:
        pass

    bin_file_stripped = os.path.join(rel_path, target_file+'.bin')
    elf_file_stripped = os.path.join(rel_path, target_file+'.elf')
    bin_file_abs = os.path.join(out_current_dir, bin_file_stripped)
    elf_file_abs = os.path.join(out_current_dir, elf_file_stripped)

     # First deal with size on flash. These are the symbols flagged as LOAD in objdump output
    size_out = subprocess.check_output(["objdump", "-hw", elf_file_abs])
    loaded_section_total = 0
    loaded_section_names = []
    loaded_section_names_sizes = {}
    ram_section_total = 0
    ram_section_names = []
    ram_section_names_sizes = {}
    for line in size_out.split('\n'):
        if "LOAD" in line:
            loaded_section_total = loaded_section_total + int(line.split()[2], 16)
            loaded_section_names.append(line.split()[1])
            loaded_section_names_sizes[line.split()[1]] = int(line.split()[2], 16)
        if "ALLOC" in line and "READONLY" not in line and "CODE" not in line:
            ram_section_total = ram_section_total + int(line.split()[2], 16)
            ram_section_names.append(line.split()[1])
            ram_section_names_sizes[line.split()[1]] = int(line.split()[2], 16)

    # Actual .bin size, which doesn't not always match section sizes
    bin_size = os.stat(bin_file_abs).st_size

    # Get the path associated to each symbol
    symbols_paths = load_symbols_and_paths(elf_file_abs, source_dir)

    # A set of helper function for building a simple tree with a path-like
    # hierarchy.
    def _insert_one_elem(tree, path, size):
        splitted_path = path.split('/')
        cur = None
        for p in splitted_path:
            if cur == None:
                cur = p
            else:
                cur = cur + '/' + p
            if cur in tree:
                tree[cur] += size
            else:
                tree[cur] = size

    def _parent_for_node(e):
        parent = "root" if len(e.split('/')) == 1 else e.rsplit('/', 1)[0]
        if e == "root":
            parent = None
        return parent

    def _childs_for_node(tree, node):
        res = []
        for e in tree:
            if _parent_for_node(e) == node:
                res += [e]
        return res

    def _siblings_for_node(tree, node):
        return _childs_for_node(tree, _parent_for_node(node))

    def _max_sibling_size(tree, node):
        siblings = _siblings_for_node(tree, node)
        return max([tree[e] for e in siblings])


    # Extract the list of symbols a second time but this time using the objdump tool
    # which provides more info as nm
    symbols_out = subprocess.check_output(["objdump", "-tw", elf_file_abs])
    flash_symbols_total = 0
    data_nodes = {}
    data_nodes['root'] = 0

    ram_symbols_total = 0
    ram_nodes = {}
    ram_nodes['root'] = 0
    for l in symbols_out.split('\n'):
        line = l[0:9] + "......." + l[16:]
        fields = line.replace('\t', ' ').split(' ')
        # Get rid of trailing empty field
        if len(fields) != 5:
            continue
        size = int(fields[3], 16)
        if fields[2] in loaded_section_names and size != 0:
            flash_symbols_total += size
            _insert_one_elem(data_nodes, symbols_paths[fields[4]], size)
        if fields[2] in ram_section_names and size != 0:
            ram_symbols_total += size
            _insert_one_elem(ram_nodes, symbols_paths[fields[4]], size)

    def _init_features_list_results(features_list):
        for feature in features_list:
            _init_feature_results(feature)

    def _init_feature_results(feature):
        feature["size"] = 0
        # recursive through children
        for child in feature["children"]:
            _init_feature_results(child)

    def _check_all_symbols(symbols_struct, features_list):
        out = ""
        sorted_nodes = sorted(symbols_struct.items(), key=operator.itemgetter(0))
        named_symbol_filter = re.compile('.*\.[a-zA-Z]+/.*')
        out_symbols_filter = re.compile('^:/')
        for symbpath in sorted_nodes:
            matched = 0
            # The files and folders (not matching regex) are discarded
            # like: folder folder/file.ext
            is_symbol=named_symbol_filter.match(symbpath[0])
            is_generated=out_symbols_filter.match(symbpath[0])
            if is_symbol == None and is_generated == None:
                continue
            # The symbols inside a file are kept: folder/file.ext/symbol
            # and unrecognized paths too (":/")
            for feature in features_list:
                matched = matched + _does_symbol_matches_feature(symbpath[0], symbpath[1], feature)
            if matched is 0:
                out += "UNCATEGORIZED: %s %d<br/>" % (symbpath[0], symbpath[1])
        return out

    def _does_symbol_matches_feature(symbol, size, feature):
        matched = 0
        # check each include-filter in feature
        for inc_path in feature["folders"]:
            # filter out if the include-filter is not in the symbol string
            if inc_path not in symbol:
                continue
            # if the symbol match the include-filter, check against exclude-filter
            is_excluded = 0
            for exc_path in feature["excludes"]:
                if exc_path in symbol:
                    is_excluded = 1
                    break
            if is_excluded == 0:
                matched = 1
                feature["size"] = feature["size"] + size
                # it can only be matched once per feature (add size once)
                break
        # check children independently of this feature's result
        for child in feature["children"]:
            child_matched = _does_symbol_matches_feature(symbol, size, child)
            matched = matched + child_matched
        return matched

    def _display_features_table(features_list, flash_symbols_total, loaded_section_total):
        out = "<table>"
        # Core features are the first top-level item
        is_core = 1
        for feature in features_list:
            out += _display_feature_row(feature, 0, is_core)
            is_core = 0
        out += "<tr>"
        out += "<td class=\"feature level0\">All symbols</td>"
        out += "<td class=\"feature_size level0\">%.1f KiB</td>" % (flash_symbols_total / 1024.0)
        out += "</tr>"
        out += "<tr>"
        out += "<td class=\"feature level0\">Unnamed symbols</td>"
        out += "<td class=\"feature_size level0\">%.1f KiB</td>" % ((loaded_section_total-flash_symbols_total) / 1024.0)
        out += "</tr>"
        out += "<tr>"
        out += "<td class=\"feature level0\">Total</td>"
        out += "<td class=\"feature_size level0\">%.1f KiB</td>" % (loaded_section_total / 1024.0)
        out += "</tr>"
        out += "</table>"
        return out

    def _display_feature_row(feature, level, is_core):
        out = ""
        # early return if the feature is empty
        if len(feature["children"]) == 0 and feature["size"] == 0:
            return out
        # feature is not empty below this point
        out = "<tr>"
        out += "<td class=\"feature level%d\">%s</td>" % (level, feature["name"])
        out += "<td class=\"feature_size level%d\">" % level
        if feature["size"] > 0:
            if is_core or level is not 1:
                # Core feature: no "+" sign in size
                # Also for children of Extra Features
                out += "%.1f KiB" % (feature["size"] / 1024.0)
            else:
                # Extra feature: "+" sign in level-1 item
                out += "%+.1f KiB" % (feature["size"] / 1024.0)
        out += "</td></tr>"
        for child in feature["children"]:
            out += _display_feature_row(child, level+1, is_core)
        return out


    # Generate a table for elements on flash, used for the flash usage treemap
    table = []
    table += [[ "'Node'", "'Parent'", "'Size'", "'Color'" ]]
    table += [[ "'data without symbols (strings, etc..)'", "'root'", '%d' % (loaded_section_total-flash_symbols_total), '-100' ]]
    table += [[ "'.bin overhead (section alignment etc..)'", "'root'", '%d' % (bin_size-loaded_section_total), '-80' ]]
    for e in data_nodes:
        parent = _parent_for_node(e)
        table += [[ "'" + e + "'", ("'" + parent + "'" if parent != None else "null"), "%d" % data_nodes[e], "%d" % (data_nodes[e]%100-50) ]]
    suffix = create_java_script_for_table(table, "chart_div_"+target_file)

    # Generate a table for elements on RAM, used for the RAM usage treemap
    table = []
    table += [[ "'Node'", "'Parent'", "'Size'", "'Color'" ]]
    for e in ram_nodes:
        parent = _parent_for_node(e)
        table += [[ "'" + e + "'", ("'" + parent + "'" if parent != None else "null"), "%d" % ram_nodes[e], "%d" % (ram_nodes[e]%100-50) ]]
    suffix += create_java_script_for_table(table, "chart_div_ram_"+target_file)


    # Create a simplified tree keeping only the most important contributors
    # This is used for the pie diagram summary
    min_parent_size = bin_size/25
    min_sibling_size = bin_size/35
    tmp = {}
    for e in data_nodes:
        if _parent_for_node(e) == None:
            continue
        if data_nodes[_parent_for_node(e)] < min_parent_size:
            continue
        if _max_sibling_size(data_nodes, e) < min_sibling_size:
            continue
        tmp[e] = data_nodes[e]

    # Keep only final nodes
    tmp2 = {}
    for e in tmp:
        if len(_childs_for_node(tmp, e)) == 0:
            tmp2[e] = tmp[e]

    # Group nodes too small in an "other" section
    filtered_data_nodes = {}
    for e in tmp2:
        if tmp[e] < min_sibling_size:
            k = _parent_for_node(e) + "/(other)"
            if k in filtered_data_nodes:
                filtered_data_nodes[k] += tmp[e]
            else:
                filtered_data_nodes[k] = tmp[e]
        else:
            filtered_data_nodes[e] = tmp[e]

    def _parent_level_3_at_most(node):
        e = _parent_for_node(node)
        while e.count('/')>2:
            e = _parent_for_node(e)
        return e

    # Try to use the same color for stuff coming from the same directory at level 3
    colors = ["#{0:6x}".format(abs(hash(_parent_level_3_at_most(e))))[:7] for e in sorted(filtered_data_nodes.keys())]
    color_str = "["
    for c in colors:
        color_str += "{color: '" + c + "'},"
    color_str += "]"

    table = []
    for e in sorted(filtered_data_nodes.keys()):
        table += [[e, filtered_data_nodes[e]]]

    suffix += """<script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChartPie);
      function drawChartPie() {
        var data = google.visualization.arrayToDataTable(["""
    suffix += "['Node', 'Size (Bytes)'],\n"
    for e in table:
        suffix += "['" + e[0] + "', %d],\n" % e[1]
    suffix += "])\n"
    suffix += """var options = {
          title: 'Flash Footprint Summary',
          pieHole: 0.5,
          slices: %s
        };

        var chart = new google.visualization.PieChart(document.getElementById('piechart_3d_%s'));
        chart.draw(data, options);
      }
    </script>\n""" % (color_str, target_file)

    out = """
    <h2>Size on flash</h2>
      <h3>Sections</h3>
        <p>Actual bin file size: %s Bytes composed of:</p>
        <ul>
          <li>Sections:
          <ul>""" % bin_size

    for i in loaded_section_names_sizes:
        out += """
            <li>%s: %d Bytes</li>""" % (i, loaded_section_names_sizes[i])

    out += """
            <li><b>total</b>: %d Bytes</li>""" % loaded_section_total
    out += """
          </ul>
        <li>.bin overhead (section alignment etc..): %d Bytes</li>
    </ul>
    <p>Data without symbols (strings, etc..) size: %d Bytes</p>""" % (bin_size-loaded_section_total, loaded_section_total-flash_symbols_total)

    if features_path_data != None:
        out += """
        <h3>Features</h3>
          """
        _init_features_list_results(features_path_data["features"])
        out += _check_all_symbols(data_nodes, features_path_data["features"])
        out += _display_features_table(features_path_data["features"], flash_symbols_total, loaded_section_total)
        out += """
          <p>The rules/paths for each category is available in the file <b>tools/scripts/build_utils/features_list.json</b>.</p>"""

    out += """
      <h3>Charts</h3>
        <div id="piechart_3d_%s" style="width: 900px; height: 500px;"></div>
        <div id="chart_div_%s" style="width: 1024px; height: 768px;"></div>""" \
        % (target_file, target_file)

    out += """
    <h2>Size on RAM</h2>
      <p>Total RAM usage: %d Bytes, from the following sections:</p>
      <ul>""" % ram_section_total

    for i in ram_section_names_sizes:
        out += """
        <li>%s: %d Bytes</li>""" % (i, ram_section_names_sizes[i])
    out += """
      </ul>
      <div id="chart_div_ram_%s" style="width: 1024px; height: 768px;"></div>
""" % target_file
    return out, suffix
