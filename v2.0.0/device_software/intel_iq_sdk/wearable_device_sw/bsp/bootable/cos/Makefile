THIS_DIR   := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
T          := $(abspath $(THIS_DIR)/../../..)
OUT        := $(abspath $(T)/../out/current/quark_se/cos)
TOOLS_PATH ?= $(T)/tools
COS_LDS    ?= $(T)/bsp/bootable/cos/arc.lds

ARCH=arc
include $(T)/build/Makefile.toolchain

KCONFIG_FILE ?= $(OUT)/.config
include $(KCONFIG_FILE)

ifeq ($(BUILDVARIANT),release)
CFLAGS += -DNDEBUG=1
endif

CFLAGS += -g
CFLAGS += -I$(T)/bsp/include
CFLAGS += -I$(T)/bsp/bootable/cos/include
CFLAGS += -D__CPU_ARC__
CFLAGS += -nostartfiles
CFLAGS += -ffreestanding -nostartfiles -fno-exceptions
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -fdata-sections -ffunction-sections
# See later on if we can make this code position independent
#CFLAGS += -pie

CFLAGS += -mARCv2EM -mav2em

.PHONY: FORCE


$(OUT)/cos.lds: $(COS_LDS) | $(OUT)
	@echo $(ANSI_RED)"[cPP]"$(ANSI_OFF) $@
	$(AT)$(CC) $(CFLAGS) -E -P -o $@ -ansi -D__ASSEMBLY__  -x assembler-with-cpp -P $< -I$(PROJECT_INCLUDES)

$(OUT)/cos.elf: $(OUT)/cos.lds $(OUT)/kbuild/built-in.a
	@echo $(ANSI_RED)"[cLD]"$(ANSI_OFF) $@
	$(AT)$(LD) -o $@ \
		-T $(OUT)/cos.lds \
		--whole-archive $(OUT)/kbuild/built-in.a --no-whole-archive \
		-Map $(OUT)/cos.map

$(OUT)/cos.bin: $(OUT)/cos.elf
	@echo $(ANSI_RED)"[cX]"$(ANSI_OFF) $@
	$(AT)$(OBJCOPY) -O binary $< $@
	@echo $(ANSI_CYAN)"Done "$(ANSI_OFF)

.PHONY: cos
cos: $(OUT)/cos.bin | $(OUT)

image: cos

all: image

# This included file defines base targets like doc or $(OUT)/built-in.a
include $(T)/build/base.mk
