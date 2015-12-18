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

BOARD        ?= crb

ifeq ($(filter ctb crb orb, $(BOARD)),)
$(error The reference application can only run on the Curie Test/Reference Board and ORB)
endif
BUILDVARIANT ?= release

PROJECT      ?= curie_reference

QUARK_DEFCONFIG ?= $(PROJECT_PATH)/quark/defconfig_$(BOARD)
ARC_DEFCONFIG ?= $(PROJECT_PATH)/arc/defconfig_$(BOARD)
BLE_CORE_DEFCONFIG ?= $(PROJECT_PATH)/ble_core/defconfig_$(BOARD)

# Version of the reference app. We use Intel IQ SDK version number for the
# reference application
include ../../build/wearable_device_sw_version.mk
VERSION_MAJOR  := $(WEARABLE_DSW_VERSION_MAJOR)
VERSION_MINOR  := $(WEARABLE_DSW_VERSION_MINOR)
VERSION_PATCH  := $(WEARABLE_DSW_VERSION_PATCH)

include ../curie_common/build/curie_common_targets.mk
