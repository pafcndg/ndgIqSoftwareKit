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

# Content to be included in a project's top-level Makefile.mk
# This file defines shared variables and targets for:
# - creating doxygen documentation
# - versioning the binary images
# - dealing with multiple build configurations

# In order to work properly, your top-level project Makefile.mk must define the
# following variables:
# - T: top level source directory
# - OUT: build directory
# - BUILDVARIANT: a build variant name like debug, release, unittests etc..
# - BOARD: a small case board name like 'ctb', 'testboard' etc.. Use 'all_boards'
#          if the project has a single board
# - PROJECT: the small case, short name for the project, e.g. 'hello_world'
# Furthermore, you may want to override all variables set with ?= to customize
# the build behaviour.

ifndef T
$(error No top level source directory T specified)
endif

ifndef OUT
$(error No output directory (OUT) specified)
endif

ifeq ($(findstring setup,$(MAKECMDGOALS)),setup)

# Some build parameters MUST be specified during setup

ifndef BUILDVARIANT
$(error No build variant (BUILDVARIANT) specified)
endif

ifndef BOARD
$(error No board (BOARD) specified)
endif

ifndef PROJECT
$(error No project short name (PROJECT) specified)
endif

else

# Restore build parameters specified during setup
-include $(OUT)/build_setup.mk

endif # Setup

ifdef PROJECT_PATH
export PROJECT_PATH
endif

#### Versioning, setup and build info
# Use a default build tag when none is set by the caller
NOWDATE     := $(shell date +"%Y%m%d%H%M%S")
BUILD_TAG   ?= custom_build_$(USER)@$(HOSTNAME)$(NOWDATE)

include $(T)/build/wearable_device_sw_version.mk

# Version of the project, set to 1.0.0 when PV is reached
# These values should be overriden in the project's Makefile rather than
# modified here
VERSION_MAJOR  ?= 0
VERSION_MINOR  ?= 0
VERSION_PATCH ?= 13

# Get build number from environment or generate from YYWW
ifeq ($(BUILD_NUMBER),)
BUILD_NUMBER_PADDED := $(shell date +"%M%S")
else
BUILD_NUMBER_PADDED := $(shell printf "%04d" $(BUILD_NUMBER))
endif
BUILD_NUMBER_TRUNCATED = $(shell printf "%.4s" $(BUILD_NUMBER_PADDED))

# If BUILD_TYPE is defined, use its first letter
ifneq ($(BUILD_TYPE),)
BUILD_LETTER = $(shell printf "%c" $(BUILD_TYPE) | tr a-z A-Z)
else
# Custom Build
BUILD_LETTER = C
BUILD_TYPE = custom
endif

# By default use the following:
# Year: %g, Workweek: %V (+1 to be aligned with the Intel WW calendar), Type: C (=Custom build), BuildNumber: %M%S
VERSION_STRING_SUFFIX ?= $(shell date +"%g")$(shell printf "%.*d" 2 $(shell echo `date +"%V"`+1 | bc))$(BUILD_LETTER)$(BUILD_NUMBER_TRUNCATED)

ifeq ($(ARTIFACTORY_BUILD_URL),)
DOWNLOAD_DIR_URL := file://$(T)/../pub
else
DOWNLOAD_DIR_URL := $(ARTIFACTORY_BUILD_URL)
endif

#### Declare global variables for pretty output
include $(T)/build/verbosity.mk

pub:
	$(AT)mkdir -p $(T)/../$@

.PHONY: setup clean list .force setup

setup: | pub
	@echo "Setup with the following options:"
	@echo "   PROJECT       : $(PROJECT)"
	@echo "   VERSION       : $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)"
	@echo "   BOARD         : $(BOARD)"
	@echo "   BUILDVARIANT  : $(BUILDVARIANT)"
	@echo "   BUILD_TAG     : $(BUILD_TAG)"
	@echo "   BUILD_TYPE    : $(BUILD_TYPE)"
	@echo "   WORKWEEK      : $(WORKWEEK)"
	@echo "   BUILDNUMBER   : $(BUILDNUMBER)"
	@echo
	@echo Setup buildenv in out/$(PROJECT)_$(BOARD)_$(BUILDVARIANT)
	$(AT)mkdir -p $(T)/../out/$(PROJECT)_$(BOARD)_$(BUILDVARIANT)
	$(AT)rm -f $(OUT)
	$(AT)ln -s $(T)/../out/$(PROJECT)_$(BOARD)_$(BUILDVARIANT) $(OUT)
	@echo "$(PROJECT)-$(BOARD)-$(BUILDVARIANT)-$(BUILD_TAG)" > $(OUT)/package_prefix.txt
	@echo "$(BUILDVARIANT)" > $(OUT)/BUILDVARIANT.txt
	@echo "{\"PROJECT\": \"$(PROJECT)\", \"BOARD\": \"$(BOARD)\", \"BUILDVARIANT\": \"$(BUILDVARIANT)\", \"BUILD_TAG\": \"$(BUILD_TAG)\", \"BUILD_TYPE\": \"$(BUILD_TYPE)\", \"DOWNLOAD_DIR_URL\": \"$(DOWNLOAD_DIR_URL)\"}" > $(OUT)/build_setup.json
	@echo "PROJECT_PATH=$(PROJECT_PATH)" > $(OUT)/build_setup.mk
	@echo "PROJECT=$(PROJECT)" >> $(OUT)/build_setup.mk
	@echo "BOARD=$(BOARD)" >> $(OUT)/build_setup.mk
	@echo "BUILDVARIANT=$(BUILDVARIANT)" >> $(OUT)/build_setup.mk
	$(AT)$(MAKE) _project_setup

_check_setup_was_done:
	@if [ ! -f $(OUT)/package_prefix.txt ]; then echo Please run \"make setup\" first ; exit 1 ; fi

_no_targets:
list:
	$(AT)sh -c "$(MAKE) -p _no_targets | awk -F':' '/^[a-zA-Z0-9][^\$$#\/\\t=]*:([^=]|$$)/ {split(\$$1,A,/ /);for(i in A)print A[i]}' | sort"

clean:
	@echo $(ANSI_RED)"[tRM]"$(ANSI_OFF) $@
	$(AT)rm -rf $(T)/../out

#### Documentation
# Project name, used in the documentation
PROJECT_NAME   ?= Unnamed

# Project version, used in the documentation
PROJECT_VERSION ?= $(BUILD_TAG)

# Project logo, used in the documentation
PROJECT_LOGO    ?= $(T)/doc/images/logo.png

# Path to image files, used in the documentation
IMAGE_PATH    ?= $(T)/doc/images

# A list of extra directories to scan for documentation
EXTRA_DOC_INPUT ?=

# A list of extra directories to scan for documentation images
EXTRA_IMAGES_INPUT ?=

.PHONY: doc doc_package doc_view

doc:
	@mkdir -p $(OUT)/doc
	@cp $(T)/doc/Doxyfile.in $(OUT)/doc/Doxyfile
	@sed -i 's|@ROOT_DIR@|$(T)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@OUT_DIR@|$(OUT)/doc|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@PROJECT_NAME@|$(PROJECT_NAME)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@PROJECT_VERSION@|$(PROJECT_VERSION)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@PROJECT_LOGO@|$(PROJECT_LOGO)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@IMAGE_PATH@|$(IMAGE_PATH)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@EXTRA_DOC_INPUT@|$(EXTRA_DOC_INPUT)|g' $(OUT)/doc/Doxyfile
	@sed -i 's|@EXTRA_IMAGES_INPUT@|$(EXTRA_IMAGES_INPUT)|g' $(OUT)/doc/Doxyfile
	doxygen $(OUT)/doc/Doxyfile 2>&1 | ( ! grep . )
	@echo "Documentation generated in $(OUT)/doc/"
	@echo "Type \"make doc_view\" to view it in your web browser"

doc_package: doc
	@mkdir -p $(T)/../pub
	cd $(OUT)/doc/ ; zip -r $(T)/../pub/developers_doc-$(BUILD_TAG).zip html

doc_view: doc
	@xdg-open $(OUT)/doc/html/index.html

#### Code & style checks
.PHONY: check_code_style

check_code_style:
	$(T)/tools/scripts/checkstyle.py -c  $(EXTRA_CHECK_STYLE_INPUT)

#### host rules
include $(T)/build/host/check.mk
include $(T)/build/host/tools.mk

#### device rules
include $(T)/build/device/flash.mk
include $(T)/build/device/debug.mk
include $(T)/build/device/ota.mk

#### help rules
include $(T)/build/help.mk
