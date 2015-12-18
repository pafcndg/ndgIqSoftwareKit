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
T           := $(abspath $(THIS_DIR)/../../..)
OUT         := $(abspath $(T)/../out/current)
PUB         := $(abspath $(T)/../pub)
ZEPHYR_BASE := $(T)/external/zephyr
PROJECT_PATH ?= $(CURDIR)

REF_BUILD_INFO ?= "pub/reference_build_info.json"

# Parallelism is handled in sub-makefiles which contain the CPU intensive tasks
.NOTPARALLEL:

.DEFAULT_GOAL := help

$(OUT)/firmware:
	$(AT)mkdir -p $@

ifneq ("$(wildcard $(T)/prebuilt/ble_core/image.bin)","")
USE_PREBUILT_BLE_CORE := yes
endif


# Adds the doc and doc_package targets
PROJECT_LOGO := ""
PROJECT_NAME := "Curie SDK"
EXTRA_DOC_INPUT := $(T)/projects/curie_common/doc $(T)/packages/intel/connectivity $(T)/packages/intel/iq
EXTRA_IMAGES_INPUT := $(T)/projects/curie_common/doc/images
EXTRA_CHECK_STYLE_INPUT := bootstrap

BOOT_SIGNING_KEY ?= $(T)/build/security/keys/boot_private.der
OTA_SIGNING_KEY ?= $(T)/build/security/keys/ota_private.der

# Bootloader defconfig
BOOTLOADER_ROOT ?= $(T)/bsp/bootable/bootloader
BOOTLOADER_DEFCONFIG ?= $(BOOTLOADER_ROOT)/board/intel/configs/$(BOARD)_defconfig

# Bootupdater defconfig
BOOTUPDATER_DEFCONFIG ?= $(T)/projects/curie_common/bootupdater/defconfig_$(BOARD)

# Cos defconfig
COS_DEFCONFIG ?= $(T)/projects/curie_common/cos/defconfig_$(BOARD)

include $(T)/build/common_targets.mk

_project_help:

_project_setup:
	@echo "Header Version String : ATP1XXXXXX-$(VERSION_STRING_SUFFIX)"
	@echo "Configure build with the following config files:"
	@echo "  QUARK_DEFCONFIG  : $(QUARK_DEFCONFIG)"
	@echo "  ARC_DEFCONFIG       : $(ARC_DEFCONFIG)"
	@echo "  BLE_CORE_DEFCONFIG    : $(BLE_CORE_DEFCONFIG)"
	@echo
	$(AT)mkdir -p $(OUT)/quark_se/quark
	$(AT)mkdir -p $(OUT)/quark_se/arc
	$(AT)mkdir -p $(OUT)/quark_se/ble_core
	$(AT)mkdir -p $(OUT)/quark_se/bootupdater
	$(AT)mkdir -p $(OUT)/quark_se/cos
	@echo $(ANSI_CYAN)"Generating build configs"$(ANSI_OFF)
	$(AT)$(MAKE) quark_defconfig DEFCONFIG=$(QUARK_DEFCONFIG)
	$(AT)$(MAKE) arc_defconfig DEFCONFIG=$(ARC_DEFCONFIG)
	$(AT)$(MAKE) ble_core_defconfig DEFCONFIG=$(BLE_CORE_DEFCONFIG)
	$(AT)$(MAKE) bootupdater_defconfig \
		DEFCONFIG=$(BOOTUPDATER_DEFCONFIG) \
		PROJECT_PATH=$(T)/bsp/bootable/bootupdater/
	$(AT)$(MAKE) cos_defconfig \
		DEFCONFIG=$(COS_DEFCONFIG) \
		PROJECT_PATH=$(T)/bsp/bootable/cos/

_project_one_time_setup:
	$(AT)$(MAKE) install-packages-required-by-emulator

_project_check_host_setup: _cflasher-exists _check-jlink-as-flasher-external-tool _check-phoneflashtool_version

# FIXME: main core binaries shall go to $(OUT)/firmware/main_core
image: _check_setup_was_done $(OUT)/firmware/ble_core/image.bin bootloader $(OUT)/firmware/bootupdater.bin $(OUT)/firmware/quark.bin $(OUT)/firmware/arc.bin $(OUT)/firmware/FSRom.bin $(OUT)/firmware/quark.signed.bin $(OUT)/firmware/arc.signed.bin $(OUT)/firmware/bootupdater.signed.bin flash_files

$(OUT)/firmware/ble_core:
	mkdir -p $(OUT)/firmware/ble_core

ifdef USE_PREBUILT_BLE_CORE
$(OUT)/firmware/ble_core/image.bin: _check_setup_was_done $(OUT)/firmware/ble_core
	cp $(T)/prebuilt/ble_core/* $(OUT)/firmware/ble_core/
else
$(OUT)/firmware/ble_core/image.bin: _check_setup_was_done
	@echo
	@echo $(ANSI_CYAN)"Building BLE Core"$(ANSI_OFF)
	$(AT)$(MAKE) -f $(T)/packages/intel/curie-ble/Makefile \
		-C $(OUT)/quark_se/ble_core/ image T=$(T) BUILDVARIANT=$(BUILDVARIANT)
	$(AT)$(T)/tools/scripts/build_utils/add_binary_version_header.py \
		--major $(VERSION_MAJOR) \
		--minor $(VERSION_MINOR) \
		--patch $(VERSION_PATCH) \
		--version_string ATP1BLE000-$(VERSION_STRING_SUFFIX) $@ $(DEV_NULL)

	@echo $(ANSI_CYAN)"Done BLE Core"$(ANSI_OFF)
endif

$(OUT)/firmware/FSRom.bin: _check_setup_was_done | $(OUT)/firmware
	@echo
	@echo $(ANSI_CYAN)"Building FSRom"$(ANSI_OFF)
	$(AT)mkdir -p $(OUT)/bootstrap/quark
	$(AT)$(MAKE) -f $(T)/bsp/bootable/bootstrap/quark/Makefile \
		-C $(OUT)/bootstrap/quark/ T=$(T)
	$(AT)cp $(OUT)/bootstrap/quark/FSRom.bin $(OUT)/firmware/FSRom.bin
	@echo $(ANSI_CYAN)"Done FSRom"$(ANSI_OFF)

$(OUT)/firmware/quark.elf: _check_setup_was_done .force
	@echo
	@echo $(ANSI_CYAN)"Building Quark"$(ANSI_OFF)
# Generates $(OUT)/quark_se/quark/zephyr/quark.elf
	$(AT)mkdir -p $(OUT)/quark_se/quark/zephyr
	$(AT)/bin/bash -c "export T=$(T); \
		source $(ZEPHYR_BASE)/zephyr-env.sh; \
		$(MAKE) -C $(T)/projects/curie_common/quark_main \
			PROJECT_INCLUDES=$(PROJECT_PATH)/include \
			BUILDVARIANT=$(BUILDVARIANT)"
# Copy the result to $(OUT)/firmware/quark.elf
	$(AT)if [ `cat $(OUT)/BUILDVARIANT.txt` = unittests_nano ]; then \
		cp $(OUT)/quark_se/quark/zephyr/nanokernel.elf $(OUT)/firmware/quark.elf; \
	else \
		cp $(OUT)/quark_se/quark/zephyr/microkernel.elf $(OUT)/firmware/quark.elf; \
	fi

$(OUT)/firmware/arc.elf: _check_setup_was_done .force
	@echo
	@echo $(ANSI_CYAN)"Building Arc"$(ANSI_OFF)
	$(AT)/bin/bash -c "export T=$(T); \
		export OUT=$(OUT); \
		source $(ZEPHYR_BASE)/zephyr-env.sh; \
		$(MAKE) -C $(T)/projects/curie_common/arc_main \
			PROJECT_INCLUDES=$(PROJECT_PATH)/include \
			BUILDVARIANT=$(BUILDVARIANT)"
	$(AT)cp $(OUT)/quark_se/arc/zephyr/nanokernel.elf $(OUT)/firmware/arc.elf

$(OUT)/firmware/arc.bin: $(OUT)/firmware/arc.elf
	$(AT)cp $(OUT)/quark_se/arc/zephyr/nanokernel.bin $@
	$(AT)$(T)/tools/scripts/build_utils/add_binary_version_header.py \
		--major $(VERSION_MAJOR) \
		--minor $(VERSION_MINOR) \
		--patch $(VERSION_PATCH) \
		--version_string ATP1ARC000-$(VERSION_STRING_SUFFIX) $@ $(DEV_NULL)
	@echo $(ANSI_CYAN)"Done Arc"$(ANSI_OFF)

$(OUT)/firmware/%.signed.bin: $(OUT)/firmware/%.bin
	$(AT) $(TOOL_SIGN) -f 0 -i $< -o $@ -s $(BOOT_SIGNING_KEY)

$(OUT)/firmware/quark.bin: $(OUT)/firmware/quark.elf
	$(AT)$(T)/external/gcc-i586-pc-elf/bin/i586-pc-elf-objcopy -O binary $(OUT)/firmware/quark.elf $@
	$(AT)$(T)/tools/scripts/build_utils/add_binary_version_header.py \
		--major $(VERSION_MAJOR) \
		--minor $(VERSION_MINOR) \
		--patch $(VERSION_PATCH) \
		--version_string ATP1LAK000-$(VERSION_STRING_SUFFIX) $@ $(DEV_NULL)
	@echo $(ANSI_CYAN)"Done Quark"$(ANSI_OFF)

$(OUT)/firmware/FSRam.bin: $(OUT)/firmware/arc.bin $(OUT)/firmware/quark.bin
	$(AT)$(T)/projects/curie_common/build/scripts/Create384KImage.py \
		-I $(OUT)/firmware/quark.bin -S $(OUT)/firmware/arc.bin -O $(OUT)/firmware/FSRam.bin

$(OUT)/quark_se/quark/bootloader/bootloader.bin: _check_setup_was_done $(OUT)/quark_se/cos/cos.c | $(OUT)/firmware
	@echo
	@echo $(ANSI_CYAN)"Building Bootloader"$(ANSI_OFF)
	$(AT)$(MAKE) -f $(T)/bsp/bootable/bootloader/Makefile T=$(T) \
		BOOTLOADER_ROOT=$(BOOTLOADER_ROOT) \
		BOOTLOADER_DEFCONFIG=$(BOOTLOADER_DEFCONFIG) \
		PROJECT_INCLUDES=$(PROJECT_PATH)/include/ \
		OUT=$(OUT)/quark_se/quark/bootloader \
		BOARD=$(BOARD) \
		COS_BLOB=$(OUT)/quark_se/cos/cos.c \
		bootloader
	$(AT)ln -sf $(OUT)/quark_se/quark/bootloader/bootloader.bin $(OUT)/firmware/bootloader_quark.bin
	$(AT)ln -sf $(OUT)/quark_se/quark/bootloader/bootloader.elf $(OUT)/firmware/bootloader_quark.elf
	@echo $(ANSI_CYAN)"Done Bootloader"$(ANSI_OFF)

.PHONY: bootloader
bootloader: $(OUT)/quark_se/quark/bootloader/bootloader.bin

$(OUT)/quark_se/bootupdater/bootupdater.bin: _check_setup_was_done $(OUT)/quark_se/
	@echo
	@echo $(ANSI_CYAN)"Building Bootupdater"$(ANSI_OFF)
	$(AT)$(MAKE) -C $(T)/bsp/bootable/bootupdater/ image \
		T=$(T) \
		OUT=$(OUT)/quark_se/bootupdater \
		VERSION_MAJOR=$(VERSION_MAJOR) \
		VERSION_MINOR=$(VERSION_MINOR) \
		VERSION_PATCH=$(VERSION_PATCH) \
		VERSION_STRING_SUFFIX=$(VERSION_STRING_SUFFIX) \
		PROJECT_INCLUDES=$(PROJECT_PATH)/include/ \
		PROJECT_PATH=$(T)/bsp/bootable/bootupdater/ \
		BOARD=$(BOARD)

$(OUT)/firmware/bootupdater.bin: $(OUT)/quark_se/bootupdater/bootupdater.bin $(OUT)/firmware
	cp $< $@

bootupdater: $(OUT)/firmware/bootupdater.bin

$(OUT)/quark_se/cos/cos.bin: _check_setup_was_done $(OUT)/quark_se/
	@echo
	@echo $(ANSI_CYAN)"Building COS"$(ANSI_OFF)
	$(AT)$(MAKE) -C $(T)/bsp/bootable/cos/ image \
		T=$(T) \
		OUT=$(OUT)/quark_se/cos \
		PROJECT_PATH=$(T)/bsp/bootable/cos/ \
		PROJECT_INCLUDES=$(PROJECT_PATH)/include/ \
		BOARD=$(BOARD)

$(OUT)/quark_se/cos/cos.c: $(OUT)/quark_se/cos/cos.bin
	$(AT)cd $(OUT)/quark_se/cos/ && xxd -i cos.bin cos.c

$(OUT)/firmware/cos.bin: $(OUT)/quark_se/cos/cos.bin
	$(AT)mkdir -p $(OUT)/firmware
	$(AT)cp $< $@

cos: $(OUT)/firmware/cos.bin $(OUT)/quark_se/cos/cos.c
.PHONY: __flash_openocd_tree

# Flags used to pre processor mapping headers for linker script
EXTRA_LINKERSCRIPT_CMD_OPT = -I$(T)/bsp/bootable/bootloader/include
EXTRA_LINKERSCRIPT_CMD_OPT += -I$(PROJECT_PATH)/include

OPENOCD_SCRIPT_DIR := $(T)/tools/config/flash/openocd

$(OUT)/firmware/partition.conf: $(OUT)/firmware/partition.conf.s
	@echo $(ANSI_RED)"[mAS]"$(ANSI_OFF) $(OUT)/firmware/partition.conf
	$(AT)$(T)/external/gcc-i586-pc-elf/bin/i586-pc-elf-gcc -E -P \
		-o $(OUT)/firmware/partition.conf -ansi -D__ASSEMBLY__ -x assembler-with-cpp \
		$(OPENOCD_SCRIPT_DIR)/partition.conf.s $(EXTRA_LINKERSCRIPT_CMD_OPT)

_project_flash_files: __flash_openocd_tree $(OUT)/firmware/partition.conf
	@echo "Copying flash scripts"
	$(AT)cp $(T)/projects/curie_common/factory/custom_factory_data_oem.py $(OUT_FLASH)/custom_factory_data_oem.py
	$(AT)cp $(T)/projects/curie_common/factory/curie_factory_data.py $(OUT_FLASH)/curie_factory_data.py
	@echo "Creating erase images"
	$(AT)dd if=/dev/zero of=$(OUT)/firmware/erase_panic.bin bs=2048 count=2			$(SILENT_DD)
	$(AT)$(T)/projects/curie_common/factory/create_ff_file.py 2048 3 $(OUT)/firmware/erase_factory_nonpersistent.bin
	$(AT)$(T)/projects/curie_common/factory/create_ff_file.py 2048 3 $(OUT)/firmware/erase_factory_persistent.bin
	$(AT)dd if=/dev/zero of=$(OUT)/firmware/erase_data.bin bs=2048 count=4			$(SILENT_DD)

_project_flash: __flash_openocd_tree $(OUT)/firmware/partition.conf

__flash_openocd_tree:
	$(AT)rsync -rupE $(IN_FLASH)/openocd/ $(OUT_FLASH)/

$(PUB)/build_report: | pub
	$(AT)mkdir -p $(PUB)/build_report

build_info: image | pub $(PUB)/build_report
	$(AT)$(T)/external/gcc-i586-pc-elf/bin/i586-pc-elf-readelf -e $(OUT)/firmware/quark.elf \
		> $(OUT)/firmware/quark.stat
	$(AT)$(T)/external/gcc-arc-elf32/bin/arc-elf32-readelf -e $(OUT)/firmware/arc.elf \
		> $(OUT)/firmware/arc.stat
ifndef USE_PREBUILT_BLE_CORE
	$(AT)$(T)/external/gcc-arm/bin/arm-none-eabi-readelf -e $(OUT)/firmware/ble_core/image.elf \
		> $(OUT)/firmware/ble_core/image.stat
endif
	$(AT)$(T)/external/gcc-i586-pc-elf/bin/i586-pc-elf-readelf -e $(OUT)/firmware/bootloader_quark.elf \
		> $(OUT)/firmware/bootloader_quark.stat
	$(AT)$(CPP) -include $(OUT)/quark_se/arc/kbuild/config.h -I$(PROJECT_PATH)/include \
		$(T)/projects/curie_common/build/scripts/build_info_data.h -o $(OUT)/build_info_data.h
	$(AT)$(CPP) -include $(OUT)/quark_se/arc/kbuild/config.h -I$(PROJECT_PATH)/include \
		$(T)/projects/curie_common/build/scripts/partitions_info.h -o $(OUT)/partitions_info.h
	$(AT)PYTHONPATH="$(PYTHONPATH):$(T)/projects/curie_common/build/scripts/" \
		python $(T)/tools/scripts/build_utils/generate_build_info.py \
			$(OUT) $(PUB)/build_info-$(BUILD_TAG).json \
			> $(OUT)/build_info.json
	$(AT)cat $(OUT)/build_info.json
ifndef USE_PREBUILT_BLE_CORE
	python $(T)/tools/scripts/build_utils/generate_memory_details.py \
		$(OUT)/ $(T)/ $(T)/tools/scripts/build_utils/features_list.json --ble_core 1 \
		> $(PUB)/build_report/$(PROJECT)-$(BOARD)-$(BUILDVARIANT)_memory_report-$(BUILD_TAG).html
else
	$(AT)python $(T)/tools/scripts/build_utils/generate_memory_details.py \
		$(OUT)/ $(T)/ $(T)/tools/scripts/build_utils/features_list.json \
		> $(PUB)/build_report/$(PROJECT)-$(BOARD)-$(BUILDVARIANT)_memory_report-$(BUILD_TAG).html
endif

package: image build_info flash_files | pub
	$(AT)cp $(T)/tools/scripts/panic/*.py $(OUT)/firmware/
	$(AT)mkdir -p $(PUB)/$(PROJECT)
	$(AT)cd $(OUT)/firmware ; \
		cp ../build_info.json .; \
		zip $(if $(VERBOSE),,-q) -r $(PUB)/$(PROJECT)/`cat $(OUT)/package_prefix.txt`.zip \
			*.elf *.bin *.cfg *.conf ble_core/* interface/*/* board/* build_info.json auto.json \
			build_info.json crash.json dump.json factory.json flash.json recover.json test.json *

TESTS_REPORT_FILE ?= $(PUB)/test_report.json

tests: package | pub
	PYTHONPATH="$(PYTHONPATH):$(T)/projects/curie_common/build/scripts/" \
		python $(T)/projects/curie_common/build/scripts/tests_run.py $(T)/.. $(TESTS_REPORT_FILE)


sanity_check: package | pub
	PYTHONPATH="$(PYTHONPATH):$(T)/projects/curie_common/build/scripts/" \
		python $(T)/projects/curie_common/build/scripts/package_sanity_check.py $(T)/..

%_menuconfig:
	$(AT)$(MAKE) -f $(T)/build/config.mk menuconfig \
		T=$(T) \
		OUT=$(OUT)/quark_se/$*/ \
		KCONFIG_ROOT=$(T)/Kconfig

%_defconfig:
	$(AT)$(MAKE) -f $(T)/build/config.mk defconfig \
		T=$(T) \
		OUT=$(OUT)/quark_se/$*/ \
		KCONFIG_ROOT=$(T)/Kconfig

%_savedefconfig:
	$(AT)$(MAKE) -f $(T)/build/config.mk savedefconfig \
		T=$(T) \
		OUT=$(OUT)/quark_se/$*/ \
		KCONFIG_ROOT=$(T)/Kconfig
