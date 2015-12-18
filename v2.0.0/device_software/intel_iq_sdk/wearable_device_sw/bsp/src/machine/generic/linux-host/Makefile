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

all: dirs $(OUT)/host/cfw_console $(OUT)/host/cfw_server $(OUT)/host/cfw_test

OBJS := bsp/src/machine/generic/linux-host/src/ipc_uart.o
OBJS += bsp/src/machine/generic/linux-host/src/host_common.o
OBJS += bsp/src/machine/generic/linux-host/src/test_timer.o
OBJS += bsp/src/util/list.o
OBJS += bsp/src/infra/port.o
OBJS += bsp/src/infra/log.o
OBJS += bsp/src/infra/log_impl_printk.o
OBJS += bsp/src/os/linux/os_linux.o
OBJS += framework/src/cfw/service_api.o
OBJS += framework/src/cfw/cfw_debug.o
OBJS += framework/src/cfw/client_api.o
OBJS += framework/src/services/ble_service/rpc_serialize.o
OBJS += framework/src/services/ble_service/rpc_deserialize.o

CLIENT_OBJS := $(OBJS:%=$(OUT)/host/cli/%)
CLIENT_OBJS += $(OUT)/host/cli/framework/src/cfw/service_manager_proxy.o
CLIENT_OBJS += $(OUT)/host/cli/framework/src/services/test_service/test_service_api.o
CLIENT_OBJS += $(OUT)/host/cli/bsp/src/machine/generic/linux-host/src/client_main.o

SERVER_OBJS := $(OBJS:%=$(OUT)/host/srv/%)
SERVER_OBJS += $(OUT)/host/srv/framework/src/cfw/service_manager.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/test_service/test_service.o
SERVER_OBJS += $(OUT)/host/srv/bsp/src/machine/generic/linux-host/src/server_main.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_api.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_dis.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_bas.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_ispp.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_core_int.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/test/ble_service_test.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_utils.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/ble_service_test.o
SERVER_OBJS += $(OUT)/host/srv/framework/src/services/ble_service/gatt.o
SERVER_OBJS += $(OUT)/host/srv/packages/intel/connectivity/ispp/src/ble_ispp.o
SERVER_OBJS += $(OUT)/host/srv/packages/intel/connectivity/iasp/src/iasp.o

COMBINED_OBJS := $(OBJS:%=$(OUT)/host/comb/%)
COMBINED_OBJS += $(OUT)/host/comb/framework/src/cfw/service_manager.o
COMBINED_OBJS += $(OUT)/host/comb/framework/src/services/test_service/test_service_api.o
COMBINED_OBJS += $(OUT)/host/comb/framework/src/services/test_service/test_service.o
COMBINED_OBJS += $(OUT)/host/comb/bsp/src/machine/generic/linux-host/src/test_main.o

CFLAGS := -I$(T)/bsp/include/machine/generic/linux-host/
CFLAGS += -I$(T)/framework/include
CFLAGS += -I$(T)/bsp/include/
CFLAGS += -I$(T)/bsp/include/machine/soc/intel/quark_se/
CFLAGS += -I$(T)/framework/include/services/
CFLAGS += -I$(T)/framework/src/services/ble_service/
CFLAGS += -I$(T)/packages/intel/connectivity/ispp/include/
CFLAGS += -I$(T)/packages/intel/connectivity/iasp/include/
CFLAGS += -I$(T)/bsp/src/machine/generic/linux-host/src/
CFLAGS += -DSVC_MANAGER_DEBUG -DDEBUG -Werror -g3 -O1 -std=c99 -D_C99
CFLAGS += -DLINUX_HOST_RUNTIME -m32
CFLAGS += -MD

CFLAGS += -DCONFIG_BLUETOOTH_PERIPHERAL -DCONFIG_SERVICES_BLE_CORE_IMPL

SERVER_FLAGS := $(CFLAGS) -DCONFIG_PORT_IS_MASTER -DCONFIG_PORT_MULTI_CPU_SUPPORT -DCONFIG_BLE_CORE_TEST -DCONFIG_SERVICES_BLE_ISPP -DCONFIG_SERVICES_BLE_IASP -DCONFIG_BT_GATT_BLE_MAX_SERVICES=2
CLIENT_FLAGS := $(CFLAGS) -DCONFIG_PORT_MULTI_CPU_SUPPORT
COMBINED_FLAGS := $(CFLAGS) -DCONFIG_HAS_SHARED_MEM -DCONFIG_PORT_IS_MASTER

# switched to 32bit compiling, to allign with the UART target, which is not an 64bit architecture
LDFLAGS := -m32


DIRS_BASE = \
    $(OUT)/host/$(BASE) \
    $(OUT)/host/$(BASE)/bsp/src/machine/generic/linux-host/src\
    $(OUT)/host/$(BASE)/framework/src/cfw \
    $(OUT)/host/$(BASE)/framework/src/test \
    $(OUT)/host/$(BASE)/framework/src/services/test_service \
    $(OUT)/host/$(BASE)/framework/src/services/ble_service \
    $(OUT)/host/$(BASE)/framework/src/services/ble_service/test \
    $(OUT)/host/$(BASE)/bsp/src/os/bare \
    $(OUT)/host/$(BASE)/bsp/src/os/linux \
    $(OUT)/host/$(BASE)/bsp/src/util/ \
    $(OUT)/host/$(BASE)/bsp/src/infra/ \
    $(OUT)/host/$(BASE)/packages/intel/connectivity/ispp/src/ \
    $(OUT)/host/$(BASE)/packages/intel/connectivity/iasp/src/

BASE := comb
DIRS_COMB := $(DIRS_BASE)
BASE := srv
DIRS_SRV := $(DIRS_BASE)
BASE := cli
DIRS_CLI := $(DIRS_BASE)

DIRS := $(DIRS_COMB) $(DIRS_SRV) $(DIRS_CLI)

$(DIRS):
	@echo $(ANSI_RED)"[hMD]"$(ANSI_OFF) $@
	$(AT)mkdir -p $@

dirs: $(DIRS)

$(OUT)/host/cfw_test: $(COMBINED_OBJS)
	@echo $(ANSI_RED)"[hMD]"$(ANSI_OFF) $@
	$(AT)gcc $(LDFLAGS) -o $@ $(COMBINED_OBJS)

$(OUT)/host/cfw_console: $(CLIENT_OBJS)
	@echo $(ANSI_RED)"[hLD]"$(ANSI_OFF) $@
	$(AT)gcc $(LDFLAGS) -o $@ $(CLIENT_OBJS)

$(OUT)/host/cfw_server: $(SERVER_OBJS)
	@echo $(ANSI_RED)"[hLD]"$(ANSI_OFF) $@
	$(AT)gcc $(LDFLAGS) -o $@ $(SERVER_OBJS)

$(OUT)/host/srv/%.o: $(T)/%.c
	@echo $(ANSI_RED)"[hCC]"$(ANSI_OFF) $@
	$(AT)gcc $(SERVER_FLAGS) -c -o $@ $<

$(OUT)/host/cli/%.o: $(T)/%.c
	@echo $(ANSI_RED)"[hCC]"$(ANSI_OFF) $@
	$(AT)gcc $(CLIENT_FLAGS) -c -o $@ $<

$(OUT)/host/comb/%.o: $(T)/%.c
	@echo $(ANSI_RED)"[hCC]"$(ANSI_OFF) $@
	$(AT)gcc $(COMBINED_FLAGS) -c -o $@ $<

DEPS = $(shell find $(OUT)/host/ -type f -name '*.d')
-include $(DEPS)

