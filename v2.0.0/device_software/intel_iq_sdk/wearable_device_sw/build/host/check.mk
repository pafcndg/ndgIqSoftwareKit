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

.PHONY: _cflasher-exists _tmux-exists __user_is_root _check-jlink-as-flasher-external-tool

__user_is_root:
	@if [ `whoami` != root ]; then echo "Please run as sudoer/root" ; exit 1 ; fi

# Checks for cflasher
CFLASHER_BINARY := $(shell which cflasher)
_cflasher-exists:
	@which cflasher > /dev/null || ! echo "Please install flashing tool from http://goto.intel.com/flashtool"

# Checks for tmux
TMUX_BINARY := $(shell which tmux)
_tmux-exists:
	@which tmux > /dev/null || ! echo "Please install tmux"

# Phone Flash Tool
PHONEFLASHTOOL_MIN_VERSION="5.3.1.1"
_check-phoneflashtool_version:
	@if [ `which phoneflashtool | grep -c 'not found'` -ne 0 ]; then echo "Please install Phone FLash Tool. See http://goto.intel.com/flashtool" ; exit 1 ; fi
	# Check the version of phoneflashtool
	@python $(T)/build/host/check_pft_version.py $(PHONEFLASHTOOL_MIN_VERSION)

# JLink stuff
_check-jlink-as-flasher-external-tool:
	@if [ `grep -c jlinkDownloader $(HOME)/.flasher/.externalTools.opt` -eq 0 ] ; then echo "Wrong version of phoneflashtool installed. See http://goto.intel.com/flashtool" ; exit 1 ; fi

install-packages-required-by-emulator:
	@if [ `grep -c "Ubuntu 12.04" /etc/issue` -eq 1 ];then apt-get build-dep qemu-kvm;fi
	@if [ `grep -c "Ubuntu 14.04" /etc/issue` -eq 1 ];then apt-get install libsasl2-dev librbd-dev;fi

# udev stuff
IN_UDEV_RULES := $(T)/tools/deploy/udev
OUT_UDEV_RULES := /etc/udev/rules.d
$(OUT_UDEV_RULES)/%.rules: $(IN_UDEV_RULES)/%.rules
	@cp $< $@
_udev-rules: __user_is_root \
    $(OUT_UDEV_RULES)/99-openocd.rules \
    $(OUT_UDEV_RULES)/99-tty.rules

# Main API, implement the per-project sub-targets
one_time_setup: __user_is_root _udev-rules
	@echo Installing base build dependencies
	apt-get install python gawk git-core diffstat unzip zip texinfo gcc-multilib \
		build-essential chrpath libtool libc6:i386 doxygen graphviz tmux     \
		libc6-dev-i386 uncrustify mscgen vim-common
	@echo Installing kconfig front-ends dependencies
	apt-get install autoconf pkg-config gperf flex bison libncurses5-dev
	@echo Installing protobuf compiler dependencies
	apt-get install protobuf-compiler python-protobuf
	@$(MAKE) -s _project_one_time_setup

check_host_setup: _tmux-exists
	@$(MAKE) -s _project_check_host_setup

.PHONY: one_time_setup check_host_setup
