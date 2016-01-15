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

otapackage: package ota_binaries | pub
	$(AT)cd $(OUT)/ota/pub/ ; \
	zip -r $(PUB)/$(PROJECT)/$(PROJECT)-$(BOARD)-$(BUILDVARIANT)-ota-$(BUILD_TAG).zip package.ota.signed.bin

ota_binaries: image \
	$(OUT)/ota/pub/arc.bin \
	$(OUT)/ota/pub/quark.signed.bin \
	$(OUT)/ota/pub/ble_core/image.bin
	$(OTA_PACKAGE_TOOL) -o $(OUT)/ota/pub/package.ota.bin \
	-p package -m 0 -a 0 \
	-i $(addprefix $(OUT)/ota/intermediates/, $(addsuffix .lzg, $(notdir $(filter %.bin %.part, $^))))
	$(AT) $(TOOL_SIGN) -f 1 -i $(OUT)/ota/pub/package.ota.bin -o $(OUT)/ota/pub/package.ota.signed.bin -s $(OTA_SIGNING_KEY)
	echo 'Ota binaries generated in $(OUT)/ota/pub'

$(OUT)/ota/pub/%: $(LZG) | $(OUT)/ota $(OUT)/ota/pub $(OUT)/ota/intermediates
	$(AT)$(LZG) -9 $(OUT)/firmware/$* $(OUT)/ota/intermediates/$(notdir $*).lzg
	$(OTA_PACKAGE_TOOL) \
		-o $(OUT)/ota/pub/$(addprefix package., $(addsuffix .ota.bin, $(basename $(notdir $*)))) \
		-p package -m 0 -a 0 \
		-i $(OUT)/ota/intermediates/$(notdir $*).lzg

$(OUT)/ota $(OUT)/ota/pub $(OUT)/ota/intermediates:
	$(AT)mkdir -p $@

