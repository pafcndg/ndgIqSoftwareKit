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

TOOLS_SECURITY ?= $(T)/tools/scripts/security/
TOOL_SIGN ?= PYTHONPATH="$(PYTHONPATH):$(T)/external/python-ecdsa/" python $(TOOLS_SECURITY)/sign.py
TOOL_GENKEY ?= $(TOOLS_SECURITY)/genkey.sh
TOOL_FACTORY_DATABASE ?= PYTHONPATH="$(PYTHONPATH):$(T)/external/python-ecdsa/" python $(TOOLS_SECURITY)/generate_factory_database.py

#############################################################
# Targets for OTA packages
#############################################################

LZG_LIB := $(T)/external/liblzg/src
LZG := $(OUT)/tools/bin/lzg
OTA_PACKAGE_TOOL ?= python $(T)/tools/scripts/build_utils/ota.py

$(OUT)/tools $(OUT)/tools/bin $(OUT)/tools/intermediates:
	$(AT)mkdir -p $@

$(LZG): $(OUT)/tools/intermediates $(OUT)/tools/bin $(LZG_LIB)/tools $(LZG_LIB)/lib
	$(MAKE) -C $(LZG_LIB) build-library OUT=$(OUT)
	$(MAKE) -C $(LZG_LIB) build-tools OUT=$(OUT)
