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

THIS_DIR    := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
T           ?= $(abspath $(THIS_DIR)/../../..)
OUT         := $(abspath $(T)/../out/current/quark_se/bootupdater)
TOOLS_PATH  ?= $(T)/tools
BINARY_VERSION_HEADER ?= $(TOOLS_PATH)/scripts/build_utils/add_binary_version_header.py
BOOTUPDATER_LDS ?= $(T)/bsp/bootable/bootupdater/quark.lds

ifndef VERSION_MAJOR
$(error "VERSION_MAJOR not defined")
endif
ifndef VERSION_MINOR
$(error "VERSION_MINOR not defined")
endif
ifndef VERSION_PATCH
$(error "VERSION_PATCH not defined")
endif
ifndef VERSION_STRING_SUFFIX
$(error "VERSION_STRING_SUFFIX not defined")
endif

ARCH=x86
include $(T)/build/Makefile.toolchain

KCONFIG_FILE ?= $(OUT)/.config
include $(KCONFIG_FILE)

ifeq ($(BUILDVARIANT),release)
CFLAGS += -DNDEBUG=1
endif

CFLAGS += -g
CFLAGS += -I$(PROJECT_INCLUDES)
CFLAGS += -I$(T)/bsp/include
CFLAGS += -I$(T)/bsp/bootable/bootupdater/include
CFLAGS += -D__CPU_QUARK__
CFLAGS += -nostartfiles
CFLAGS += -ffreestanding -nostartfiles -fno-exceptions
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -fdata-sections -ffunction-sections

.PHONY: FORCE


$(OUT)/bootupdater.lds: $(BOOTUPDATER_LDS) | $(OUT)
	@echo $(ANSI_RED)"[bPP]"$(ANSI_OFF) $@
	$(AT)$(CC) $(CFLAGS) -E -P -o $@ -ansi -D__ASSEMBLY__  -x assembler-with-cpp -P $< -I$(PROJECT_INCLUDES)

$(OUT)/bootupdater.elf: $(OUT)/bootupdater.lds $(OUT)/kbuild/built-in.a
	@echo $(ANSI_RED)"[bLD]"$(ANSI_OFF) $@
	$(AT)$(LD) -o $@ \
		-T $(OUT)/bootupdater.lds \
		--whole-archive $(OUT)/kbuild/built-in.a --no-whole-archive \
		-Map $(OUT)/bootupdater.map

$(OUT)/bootupdater.bin: $(OUT)/bootupdater.elf
	@echo $(ANSI_RED)"[bX]"$(ANSI_OFF) $@
	$(AT)$(OBJCOPY) -O binary $< $@
	$(AT)$(T)/tools/scripts/build_utils/add_binary_version_header.py \
		--major $(VERSION_MAJOR) \
		--minor $(VERSION_MINOR) \
		--patch $(VERSION_PATCH) \
		--version_string ATP1LAK000-$(VERSION_STRING_SUFFIX) $@ $(DEV_NULL)
	@echo $(ANSI_CYAN)"Done boot updater"$(ANSI_OFF)

.PHONY: bootupdater
bootupdater: $(OUT)/bootupdater.bin | $(OUT)

image: bootupdater

all: image

# This included file defines base targets like doc or $(OUT)/built-in.a
include $(T)/build/base.mk
