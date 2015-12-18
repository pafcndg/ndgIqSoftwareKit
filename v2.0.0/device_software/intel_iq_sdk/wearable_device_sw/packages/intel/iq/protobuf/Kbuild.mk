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

NANOPB_DIR := $(T)/external/nanopb

PROTOC = protoc
PROTOC_OPTS = --proto_path=$(SRC)/ \
	--plugin=protoc-gen-nanopb=$(NANOPB_DIR)/generator/protoc-gen-nanopb \
	-I$(NANOPB_DIR)/generator/proto

src-$(CONFIG_COMMON_IQ)  += IntelCommon.pb.c
src-$(CONFIG_BODY_IQ)    += IntelBodyIQ.pb.c
src-$(CONFIG_DEVICES_IQ) += IntelDevice.pb.c
src-$(CONFIG_USER_EVENTS_IQ)   += IntelUserEvents.pb.c
src-$(CONFIG_SYSTEM_EVENTS_IQ) += IntelSystemEvents.pb.c
src-$(CONFIG_FOTA_IQ)    += IntelFirmwareUpdate.pb.c
src-$(CONFIG_NOTIFICATIONS_IQ) += IntelNotifications.pb.c

obj-$(CONFIG_COMMON_IQ)  += IntelCommon.pb.o
obj-$(CONFIG_BODY_IQ)    += IntelBodyIQ.pb.o
obj-$(CONFIG_DEVICES_IQ) += IntelDevice.pb.o
obj-$(CONFIG_USER_EVENTS_IQ)   += IntelUserEvents.pb.o
obj-$(CONFIG_SYSTEM_EVENTS_IQ) += IntelSystemEvents.pb.o
obj-$(CONFIG_FOTA_IQ)    += IntelFirmwareUpdate.pb.o
obj-$(CONFIG_NOTIFICATIONS_IQ) += IntelNotifications.pb.o

obj-$(CONFIG_NANOPB_DECODER)  += pb_decode.o
obj-$(CONFIG_NANOPB_ENCODER)  += pb_encode.o
obj-$(CONFIG_NANOPB_COMMON)  += pb_common.o

cflags-y += -I$(NANOPB_DIR)

# Build nanopb source files
$(OUT_SRC)/pb_%.o: $(NANOPB_DIR)/pb_%.c $(OUT_SRC)/c.flags
	@echo $(ANSI_RED)"[kCC]"$(ANSI_OFF) $@
	$(AT)$(CC) $(COMPUTED_CFLAGS) -c -o $@ $<

# Build protobuf source files
$(OUT_SRC)/%.pb.o: $(OUT_SRC)/%.pb.c $(OUT_SRC)/c.flags
	@echo $(ANSI_RED)"[kCC]"$(ANSI_OFF) $@
	$(AT)$(CC) $(COMPUTED_CFLAGS) -c -o $@ $<

# Generate protobuf source files
$(OUT_SRC)/%.pb.c: $(SRC)/%.proto \
	               $(T)/external/nanopb/generator/proto/nanopb_pb2.py
	@echo $(ANSI_RED)"[Protoc]"$(ANSI_OFF) $@
	$(AT)$(PROTOC) $(PROTOC_OPTS) --nanopb_out=$(OUT_SRC) $<

# Prevent protobuf source files from being removed
.PRECIOUS: $(OUT_SRC)/%.pb.c $(OUT_SRC)/%.h

# Set up nanopb tool
$(T)/external/nanopb/generator/proto/nanopb_pb2.py:
	$(AT)$(MAKE) -C $(T)/external/nanopb/generator/proto
