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

#include <stdbool.h>
#include "utils.h"
#include "panic.h"
#include "panic_dump.h"

/* Get end of RAM address to handle ARC panic dump */
extern uint8_t __panic_end__[];

/* Quark SE dump locations in RAM (ARC and QRK) */
#define PANIC_QUARK_MAGIC_ADDR ((uint32_t*) (QUARK_PANIC_RAM_ADDR)-1)
#define PANIC_ARC_MAGIC_ADDR ((uint32_t*) (ARC_PANIC_RAM_ADDR)-1)

/* Align address on 32bits (add 3 then clear LSBs) */
#define PANIC_ALIGN_32(x) (((uint32_t) (x) + 3) & ~(3))

void panic_nvm_init(uint32_t ** where, uint32_t size)
{
	while (1) {
		switch (*(*where + BASE_FLASH_ADDR/4)) {
		case PANIC_DATA_MAGIC:
			{
			unsigned int dump_size =
				((struct panic_data_flash_header*)
				((uint8_t*)(*where) + BASE_FLASH_ADDR))->struct_size;
			/* Compute last flash address to write on, aligned on 32bits */
			uint32_t *last_addr = (uint32_t*)
				PANIC_ALIGN_32((uint32_t) (*where) + size + dump_size);
			/* Check that last_addr is valid and did not overflow */
			if (last_addr >= (uint32_t*)PANIC_NVM_END || last_addr <= *where) {
				goto erase_nvm;
			}
			/* Here, panic magic found. Increment where ptr and start over */
			*where = (uint32_t*) PANIC_ALIGN_32((uint32_t)(*where) + dump_size);
			continue;
			}
		case -1:
			/* Free NVM, check that enough free space is available */
			if (PANIC_ALIGN_32((uint32_t)(*where) + size) >= PANIC_NVM_END) {
				goto erase_nvm;
			}
			return;
		default:
			/* NVM panic parsing error, erase flash */
			goto erase_nvm;
		}
	}

erase_nvm:
	/* Erase panic NVM pages
	   and update where ptr to panic NVM first page */
	*where = (uint32_t*)PANIC_NVM_BASE;
	panic_nvm_erase(PANIC_NVM_BASE, PART_PANIC_SIZE);
}

void panic_dump(void)
{
	/* Load flash memory base address to store panic dump */
	uint32_t *pnvm = (uint32_t *) PANIC_NVM_BASE;
	uint32_t size_qrk, size_arc = 0;

	/* Get QRK and ARC panic dumps */
	panic_volatile_get(PANIC_QUARK_MAGIC_ADDR, &size_qrk);
	size_qrk = PANIC_ALIGN_32(size_qrk);
	panic_volatile_get(PANIC_ARC_MAGIC_ADDR, &size_arc);
	size_arc = PANIC_ALIGN_32(size_arc);

	if (!(size_qrk + size_arc)) {
		/* No panic dump found */
		return;
	}

	/* Check that flash panic partition has free space, erase if not */
	panic_nvm_init(&pnvm, size_qrk + size_arc);

	if (size_qrk) {
		/* QRK dump detected */
		panic_nvm_write(PANIC_QUARK_MAGIC_ADDR, pnvm, size_qrk);
		/* Clear panic magic */
		*PANIC_QUARK_MAGIC_ADDR = 0;
	}

	if (size_arc) {
		/* ARC dump detected */
		panic_nvm_write(PANIC_ARC_MAGIC_ADDR,
				(uint32_t*)(((uint8_t*) pnvm) + size_qrk),
				size_arc);
		/* Clear panic magic */
		*PANIC_ARC_MAGIC_ADDR = 0;
	}
}

void panic_nvm_erase(uint32_t where, uint32_t size)
{
	unsigned int start_block = where/ERASE_PAGE_SIZE;
	soc_flash_block_erase(start_block, size);
}

void panic_nvm_write(uint32_t * panic_volatile, uint32_t * panic_nvm, uint32_t size)
{
	unsigned int retlen, i;
	/* Copy panic header, on dword basis */
	for (i = 0; i < sizeof(struct panic_data_flash_header)/4; i++) {
		soc_flash_write((uint32_t) (panic_nvm + i),
				1,
				&retlen, panic_volatile - i);
	}
	/* Copy arch specific panic data */
	soc_flash_write((uint32_t) (panic_nvm + i),
			size / 4 - i,
			&retlen,
			panic_volatile - size / 4 + 1);
}
