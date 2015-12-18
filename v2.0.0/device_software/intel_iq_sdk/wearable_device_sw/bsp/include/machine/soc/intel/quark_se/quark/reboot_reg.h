/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef REBOOT_REG_H
#define REBOOT_REG_H

#include "machine/soc/intel/quark_se/scss_registers.h"

/* Mapping of General Purpose Sticky Scratchpad 0
 *
 *  31                                                               0
 * *--------------*----------------*----------------*----------------*
 * |  BOOT FLAGS  |  RESET REASONS |  BOOT TARGETS  | WAKE SOURCES   | SCSS_GPS0
 * *--------------*----------------*----------------*----------------*
 */

#define BOOT_TARGETS_POS            0x08
#define BOOT_TARGETS_MASK           (0xFF << BOOT_TARGETS_POS)

#define GET_REBOOT_REG(addr, mask, pos)	\
	((MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, addr) & mask) >> pos)

#define SET_REBOOT_REG(addr, mask, pos, val) \
	do { \
		uint32_t reg = MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, addr); \
		reg &= ~mask; \
		reg |= val << pos; \
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, addr) = \
			(MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, \
						addr) & ~mask) | reg; \
	} while (0)

#endif
