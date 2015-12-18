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

# This file describes development flashing interface for all programs and
# projects
#
# No rules shall be modified/added/integrated without PUPDR review/approval

.PHONY: flash __flash_json_file

OUT_FLASH ?= $(OUT)/firmware
# FIXME: this is project dependent, shall contain project path
PROJECT_CONFIG_DIR ?= $(T)/tools/config
IN_FLASH ?= $(PROJECT_CONFIG_DIR)/flash

FLASH_DL_ENV ?= local
FLASH_DL_FILTER ?= ci.props.build_type=engineering
FLASH_BOARD ?= $(BOARD)
FLASH_VARIANT ?= $(BUILDVARIANT)
FLASH_CLI_OPTS ?=-f $(OUT_FLASH)/flash.json -c $(FLASH_CONFIG) $(if $(VERBOSE),--log-level 5,)

$(OUT_FLASH)/%.json: $(IN_FLASH)/flashtool/%.json $(OUT_FLASH)
	$(AT)cp $< $@


# FIXME: need to find a solution for external users and customers
# flashtool can be delivered but need a redirection URL mechanism
# or flashtool has to be added to the repo

__flash_json_file: \
	$(OUT_FLASH)/auto.json \
	$(OUT_FLASH)/crash.json \
	$(OUT_FLASH)/dump.json \
	$(OUT_FLASH)/factory.json \
	$(OUT_FLASH)/flash.json \
	$(OUT_FLASH)/recover.json \
	$(OUT_FLASH)/test.json

__flash_cli_opts:
	@echo $(FLASH_CLI_OPTS)

$(OUT_FLASH)/generate_factory_bin_oem.py:
	$(AT)cp $(T)/tools/factory/generate_factory_bin_oem.py $@

$(OUT_FLASH)/generate_factory_bin_product.py:
	$(AT)cp $(T)/tools/factory/generate_factory_bin_product.py $@

$(OUT_FLASH)/factory_data.py:
	$(AT)cp $(T)/tools/factory/factory_data.py $@

#ifeq ($(FLASH_DL_ENV), remote)
##cflasher --artifact --art-env 'NDG - $(PROJECT)' --search \
#--property '$(FLASH_DL_FILTER)&ci.props.owner=$(USER_WINDOWS)'\
#--use-latest-published --board $(FLASH_BOARD) \
#--buildvariant $(FLASH_VARIANT)--flashfile-type flash
#@echo $(FLASH_DL_ENV) not supported yet
#endif

flash_rom_bootloader:
	$(AT)$(MAKE) flash_custom FLASH_CLI_OPTS='-f $(OUT_FLASH)/flash.json -c jtag_x86_rom+bootloader $(if $(VERBOSE),--log-level 5,)'

flash_dfu:
	$(AT)$(MAKE) flash_custom FLASH_CLI_OPTS='-f $(OUT_FLASH)/flash.json -c usb_full $(if $(VERBOSE),--log-level 5,)'

flash_persistent:
	$(MAKE) flash_custom FLASH_CLI_OPTS='-f $(OUT_FLASH)/flash.json -c usb_blank_persistent $(if $(VERBOSE),--log-level 5,)'

flash_custom: _check-phoneflashtool_version __flash_json_file __flash_cli_opts _cflasher-exists _project_flash debug_stop
	$(AT)$(CFLASHER_BINARY) $(FLASH_CLI_OPTS) 2>&1 | tee -a $(OUT)/flash.log

flash: flash_files
ifndef FLASH_CONFIG
	$(AT)$(MAKE) flash_rom_bootloader
	$(AT)$(MAKE) flash_dfu
else
	$(AT)$(MAKE) flash_custom
endif


flash_files: __flash_json_file $(OUT_FLASH)/generate_factory_bin_oem.py $(OUT_FLASH)/generate_factory_bin_product.py $(OUT_FLASH)/factory_data.py
	@echo
	@echo $(ANSI_CYAN)"Preparing flash images"$(ANSI_OFF)
	$(AT)$(MAKE) _project_flash_files
	@echo $(ANSI_CYAN)"Done images"$(ANSI_OFF)

flash_help:
	@echo FLASH_DL_ENV = local  remote
	@echo FLASH_DL_FILTER ?= ci.props.build_type=engineering
	@echo FLASH_VARIANT ?= release  debug  factory  production
	@echo FLASH_CONFIG ?= sw variant
	@echo http://goto.intel.com/flashtool
