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

# This file describes development debug interface for all programs and
# projects
#
# No rules shall be modified/added/integrated without PUPDR review/approval

# FIXME: use tools.mk to define $(OPENOCD)
OPENOCD ?= $(T)/external/openocd/bin/openocd

# FIXME: why are we using host gdb!
GDB_ARC ?= $(T)/external/gcc-arc-elf32/bin/arc-elf32-gdb
GDB_QRK ?= gdb

# FIXME: project+core specific
GDB_QRK_PORT_NUMBER ?= 3334
GDB_ARC_PORT_NUMBER ?= 3333

.PHONY:
gdb-auto-load-safe-path-enable:
	$(AT)$(shell \
		if [ ! -f $(HOME)/.gdbinit ]; then \
			touch $(HOME)/.gdbinit; \
		fi )
	$(AT)$(shell \
		if [ `grep -c add-auto-load-safe-path "$(HOME)/.gdbinit"` -eq 0 ]; then \
		echo "add-auto-load-safe-path $(PWD)/.gdbinit" >> $(HOME)/.gdbinit ;   \
		fi                                                                     \
		)

debug_start: debug_stop gdb-auto-load-safe-path-enable _tmux-exists
	$(AT)TMUX="" $(TMUX_BINARY) new-session -d -n openocd -s openocd "$(OPENOCD) -s $(T)/tools/config/flash/openocd -f interface/ftdi/atpdev.cfg -f init_dbg.cfg"

debug_console_qrk: debug_start
	$(AT)echo "target remote localhost:$(GDB_QRK_PORT_NUMBER)" > .gdbinit
	$(AT)echo "monitor halt" >> .gdbinit
	$(AT)echo "symbol-file $(ELF)" >> .gdbinit
	@# Make sure that we re-resume the target after quitting gdb
	$(AT)echo "monitor resume" >> gdbdeinit
	$(AT)echo "quit" >> gdbdeinit
	-$(AT)bash -c "$(GDB_QRK) $(ELF); $(GDB_QRK) $(ELF) -x gdbdeinit > /dev/null; rm gdbdeinit; $(MAKE) debug_stop"
	@# All make command after this line will not be executed when the user quit gdb using CTRL + C

debug_console_arc: debug_start
	@$(AT)echo $(ANSI_CYAN)CAVEAT: You should have compiled source with QUARK_SE_ARC_STARTUP_DISABLED option$(ANSI_OFF)
	@$(AT)echo $(ANSI_CYAN)in order to debug ARC startup. While in GDB, type 'restart' to begin.$(ANSI_OFF)
	$(AT)echo "target remote localhost:$(GDB_ARC_PORT_NUMBER)" > .gdbinit
	$(AT)echo "symbol-file $(ELF)" >> .gdbinit
	$(AT)echo "monitor halt" >> .gdbinit
	@# Make sure that we re-resume the target after quitting gdb
	$(AT)echo "monitor resume" >> gdbdeinit
	$(AT)echo "quit" >> gdbdeinit
	-$(AT)bash -c "$(GDB_ARC) $(ELF); $(GDB_ARC) $(ELF) -x gdbdeinit > /dev/null; rm gdbdeinit; $(MAKE) debug_stop"
	@# All make command after this line will not be executed when the user quit gdb using CTRL + C

debug_stop:
	$(AT)rm .gdbinit 2> /dev/null || exit 0
	$(AT)killall openocd 2> /dev/null || exit 0
	$(AT)$(TMUX_BINARY) kill-window -t openocd 2> /dev/null || exit 0

debug_help:
	@echo "Type \"make debug_console_qrk ELF=path/to/elf_file_to_debug\" or \
	\"make debug_console_arc ELF=path/to/elf_file_to_debug\" to start the specific debug"
	@echo " environment, where:"
	@echo " ELF=[path to .elf]: load symbol from ELF when you start console."
