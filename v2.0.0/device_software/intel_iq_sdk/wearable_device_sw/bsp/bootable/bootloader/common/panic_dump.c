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

static bool panic_found;

__weak void panic_dump(void)
{
	uint32_t *pvolatile = (uint32_t *) PANIC_VOLATILE_END;
	uint32_t *pnvm = (uint32_t *) PANIC_NVM_BASE;
	uint32_t size;
	uint32_t err_code;

	err_code = panic_volatile_get(pvolatile, &size);
	if (err_code)
		return;

	panic_nvm_init(&pnvm, size);
	panic_nvm_write(pvolatile, pnvm, size);
}

__weak uint32_t panic_volatile_get(uint32_t * panic, uint32_t * size)
{
	panic_found = false;
	if (*panic == PANIC_DATA_MAGIC) {
		/* get dump size */
		*size = ((struct panic_data_footer*)(panic+1)-1)->struct_size;
		panic_found = true;
		return PANIC_SUCCESS;
	}
	*size = 0;
	return PANIC_FAILURE;
}

__weak void panic_nvm_init(uint32_t ** where, uint32_t size)
{
	while (**where != 0xFFFFFFFF && *where < (uint32_t *) PANIC_NVM_END)
		*where += 32;

	/* reached end of panic partition */
	if (*where >= (uint32_t *) PANIC_NVM_END) {
		*where = (uint32_t *) PANIC_NVM_BASE;
		panic_nvm_erase(PANIC_NVM_BASE, PART_PANIC_SIZE);
	}

}

__weak void panic_nvm_erase(uint32_t where, uint32_t size)
{
}

__weak void panic_nvm_write(uint32_t * panic_volatile, uint32_t * panic_nvm,
			    uint32_t size)
{
	uint32_t i;
	uint32_t v;

	for (i = 0; i < size / 4; i++) {
		panic_nvm_write_word((panic_nvm + i), *(panic_volatile - i));
		v = *(panic_nvm + i);
		if (v == *(panic_volatile - i))
			*(panic_volatile - i) = 0;
	}
}

__weak void panic_nvm_write_word(uint32_t * address, uint32_t value)
{
	*address = value;
}

__weak bool panic_occurred(void)
{
	return panic_found;
}
