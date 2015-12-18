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

T=$(PWD)

include $(T)/build/verbosity.mk

KBUILD_OUT=$(OUT)/host-test-fw

all: dirs $(KBUILD_OUT)/tcmd_main_test

apis.so: built-in-api.a

CFLAGS = -m32 -g -g3
CFLAGS +=  -I $(T)/framework/include/
CFLAGS +=  -I $(T)/bsp/include/
CFLAGS +=  -I $(T)/bsp/include/machine/soc/intel/quark_se/
CFLAGS +=  -I $(T)/bsp/include/machine/generic/linux-host/
CFLAGS +=  -I $(T)/devices/quark_se/quark/include
CFLAGS +=  -I $(T)/devices/quark_se/common/include

$(KBUILD_OUT) :
	mkdir -p $(KBUILD_OUT)

$(KBUILD_OUT)/.config : $(KBUILD_OUT) $(T)/bsp/src/machine/generic/linux-test-fw/defconfig
	$(AT) cp $(T)/bsp/src/machine/generic/linux-test-fw/defconfig $(KBUILD_OUT)/.config

$(KBUILD_OUT)/built-in.a: $(KBUILD_OUT)/.config
	$(AT)$(MAKE) -C $(T) -f $(T)/build/Makefile.build \
	SRC=. \
	OUT=$(KBUILD_OUT) \
	CFLAGS="$(CFLAGS)" \
	KCONFIG=$(KBUILD_OUT)/.config

dirs:
	$(AT)mkdir -p $(KBUILD_OUT)/bsp/src/machine/generic/linux-test-fw/src/
	$(AT)mkdir -p $(KBUILD_OUT)/bsp/src/infra/
	$(AT)mkdir -p $(KBUILD_OUT)/bsp/src/util
	$(AT)mkdir -p $(KBUILD_OUT)/bsp/src/os/linux/
	$(AT)mkdir -p $(KBUILD_OUT)/bsp/src/machine/generic/linux-host/src/

$(KBUILD_OUT)/%.o : $(T)/%.c
	@echo $(ANSI_RED)"[hCC]"$(ANSI_OFF) $@
	$(AT)gcc $(CFLAGS) -c -o $@ $<

$(KBUILD_OUT)/tcmd_main_test: $(OBJS) $(OUT)/host-test-fw/built-in.a
	$(AT)gcc -m32 -lc $(OBJS) $(OUT)/host-test-fw/built-in.a -o $(KBUILD_OUT)/tcmd_main_test
