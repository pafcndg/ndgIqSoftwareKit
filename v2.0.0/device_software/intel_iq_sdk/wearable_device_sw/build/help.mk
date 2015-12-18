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

.PHONY: help

help:
	@echo 'Base targets:'
	@echo ' help                - show this help'
	@echo ' clean               - remove the out directory'
	@echo ' setup               - setup the build env for a given config. Use environment variables to change default config.'
	@echo ' image               - build image for current config'
	@echo ' package             - generate the package for current config in pub/'
	@echo ' one_time_setup      - performs initial setup of host machine (requires root privileges)'
	@echo
	@echo 'Environment variables:'
	@echo ' BUILDVARIANT        - one of [debug, release]'
	@echo
	@echo 'Doc & Metadata:'
	@echo ' doc                 - generate the doxygen documentation'
	@echo ' doc_package         - generate a .zip in pub/ containing the doxygen documentation'
	@echo ' doc_view            - display the generated doxygen documentation in your browser'
	@echo ' build_info          - create a json file reporting footprint of ram and flash consumption and other infos'
	@echo ' build_report        - create an HTML report for display e.g. in CI builders summary'
	@echo ' cscope              - generate cscope database'
	@echo
	@echo 'Flash:'
	@echo ' flash               - flash the device'
	@echo ' flash_help          - detailed help on flash'
	@echo ' otapackage          - generate the ota package for current config '
	@echo
	@echo 'Debug:'
	@echo ' debug_help          - detailed help on debug'
	@echo ' debug_console       - start console gdb client after calling debug_start target'
	@echo ' debug_start         - start debug environment'
	@echo ' debug_stop          - stop debug environment'
	@echo
	@echo 'Continuous Integration targets:'
	@sh -c "$(MAKE) -p _no_targets | awk -F':' '/^ci_[a-zA-Z0-9][^\$$#\/\\t=]*:([^=]|$$)/ {split(\$$1,A,/ /);for(i in A)print \" \"A[i]}' | sort"
	@echo
	@echo 'Tests and sanity checks:'
	@echo ' check_host_setup    - check that host has all necessary dependencies installed for building'
	@echo ' check_code_style    - check that source code is following coding style guidelines'
	@echo
	@echo 'Project specific targets:'
	@$(MAKE) _project_help -s
	@echo
	@echo 'Build tool options:'
	@echo ' V=1                 - verbose output (quiet by default)'
	@echo ' C=0                 - disable use of ANSI color (enabled by default)'
