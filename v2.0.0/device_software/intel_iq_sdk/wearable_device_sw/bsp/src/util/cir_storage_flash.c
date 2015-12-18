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

#include <stddef.h>

/* Flash storage */
#include "soc_config.h"

#include "util/cir_storage.h"
#include "cir_storage_flash_internal.h"

#define READ_PTR(x) x->pointers.data_pointer_read
#define WRITE_PTR(x) x->pointers.data_pointer_write

typedef enum pointer_type {
	POINTER_READ = 0,
	POINTER_WRITE,
} pointer_type;

static int32_t get_pointers(cir_storage_flash_t *storage,
			    pointers_block_t *pointers);
static int32_t set_pointers(cir_storage_flash_t *storage,
			    pointers_block_t *pointers);

int32_t cir_storage_flash_init(cir_storage_flash_t *storage)
{
	pointers_block_t local_pointers;

	if (storage->parent.size%storage->block_size != 0) {
		return -1;
	}

	if (storage->read(storage->address_start,
			  sizeof(pointers_block_t),
			  (uint8_t *)&local_pointers) != 0) {
		return -1;
	}

	/* If an init as already been done */
	if (local_pointers.data_pointer_read != 0xFFFFFFFF) {
		/* Initialize pointers with previous value */
		if (get_pointers(storage, &storage->pointers) != 0){
			return -1;
		}
		return 0;
	}

	/* Storage first init */
	READ_PTR(storage) = storage->address_start + storage->block_size;
	WRITE_PTR(storage) = READ_PTR(storage);
	storage->pointers_index = storage->address_start;

	/* Erase the two firsts blocks (pointers block and first data block) */
	if (storage->erase(storage->block_start, 2) != 0) {
		return -1;
	}

	/* Store our pointers at the beginning of the first block */
	return storage->write(storage->address_start,
			      sizeof(pointers_block_t),
			      (uint8_t *)&storage->pointers);
}

static uint8_t manage_block_overflow(cir_storage_flash_t *storage, uint32_t size, pointer_type pointer_type)
{
	uint32_t pointer;

	if (pointer_type == POINTER_READ) {
		pointer = READ_PTR(storage);
	} else {
		pointer = WRITE_PTR(storage);
	}

	/* If there is not enough place on the current block, we read at the beginning
	   of the next block */
	if (storage->block_size - (pointer%storage->block_size) < size) {
		pointer = storage->block_size*(pointer/storage->block_size+1);
		/* If we are at the end of the circular storage, we move the read pointer
		   on the first data block */
		if (pointer + size > (storage->address_start + storage->parent.size)) {
			pointer = storage->address_start+storage->block_size;
		}
		if (pointer_type == POINTER_READ) {
			READ_PTR(storage) = pointer;
		} else {
			WRITE_PTR(storage) = pointer;
		}

		return 1;
	}
	return 0;
}

cir_storage_err_t cir_storage_push(cir_storage_t *self,
                                   uint8_t *buf,
                                   uint32_t len)
{
	cir_storage_flash_t *storage = (cir_storage_flash_t *)self;

	if ( (len > storage->block_size) || (len <= 0) || (buf == NULL) ) {
		return CBUFFER_STORAGE_ERROR;
	}

	if (manage_block_overflow(storage, len, POINTER_WRITE)) {
		if (storage->erase(WRITE_PTR(storage)/storage->block_size, 1) !=0) {
			return CBUFFER_STORAGE_ERROR;
		}

		/* If read pointer is in the erased block, we move it on the next one */
		if ((WRITE_PTR(storage) / storage->block_size) ==
		    (READ_PTR(storage) / storage->block_size)) {
			READ_PTR(storage) = storage->address_start+storage->block_size+
					(WRITE_PTR(storage) - storage->address_start)%
					(storage->parent.size - storage->block_size);
		}
	}

	if (storage->write(WRITE_PTR(storage), len, buf) != 0) {
		return CBUFFER_STORAGE_ERROR;
	}

	/* We increase data pointer write and copy it in pointers block */
	WRITE_PTR(storage) += len;

	if (set_pointers(storage, &storage->pointers) != 0) {
		return CBUFFER_STORAGE_ERROR;
	}

	return CBUFFER_STORAGE_SUCCESS;
}


cir_storage_err_t cir_storage_pop(cir_storage_t *self,
                                  uint8_t *buf,
                                  uint32_t buf_size)
{
	cir_storage_flash_t *storage = (cir_storage_flash_t *)self;
	if ( (buf_size > storage->block_size) || (buf_size <= 0) || (buf == NULL) ) {
		return CBUFFER_STORAGE_ERROR;
	}

	/* nothing to read */
	if (READ_PTR(storage) == WRITE_PTR(storage)) {
		return CBUFFER_STORAGE_EMPTY_ERROR;
	}

	if (storage->read(READ_PTR(storage), buf_size, buf) != 0) {
		return CBUFFER_STORAGE_ERROR;
	}

	/* We increase data pointer read and copy it in pointers block */
	return cir_storage_clear(self, buf_size);
}


cir_storage_err_t cir_storage_peek(cir_storage_t * self,
                                   uint32_t offset,
                                   uint8_t *buf,
                                   uint32_t size)
{
	int32_t ret;
	cir_storage_flash_t *storage = (cir_storage_flash_t *)self;
	if ( (size > storage->block_size) || (size <= 0) || (buf == NULL) ) {
		return CBUFFER_STORAGE_ERROR;
	}

	/* nothing to read */
	if (READ_PTR(storage) == WRITE_PTR(storage)) {
		return CBUFFER_STORAGE_EMPTY_ERROR;
	}

	ret = storage->read(READ_PTR(storage) + offset, size, buf);
	if (ret == 0) {
		return CBUFFER_STORAGE_SUCCESS;
	} else {
		return CBUFFER_STORAGE_ERROR;
	}
}

cir_storage_err_t cir_storage_clear(cir_storage_t * self, uint32_t size)
{
	cir_storage_flash_t *storage = (cir_storage_flash_t *)self;

	if (size == 0) {
		/* Put the read pointer at same location as the write pointer */
		READ_PTR(storage) = WRITE_PTR(storage);
	} else {
		/* Move the read pointer of "size" */
		READ_PTR(storage) += size;

		manage_block_overflow(storage, size, POINTER_READ);
	}

	if (set_pointers(storage, &storage->pointers) == 0)
		return CBUFFER_STORAGE_SUCCESS;
	else
		return CBUFFER_STORAGE_ERROR;
}


static int32_t get_pointers(cir_storage_flash_t *storage,
			    pointers_block_t *pointers)
{
	/* Calculation of index of search in pointers block.
	   We start to search at the end of the block to have the last pointers. */
	storage->pointers_index = storage->address_start + storage->block_size;

	/* Search the last pointers in pointers block */
	do {
		storage->pointers_index -= sizeof(pointers_block_t);

		if (storage->read(storage->pointers_index,
				  sizeof(uint32_t),
				  (uint8_t *)pointers) != 0)
			return -1;
	} while ((pointers->data_pointer_read == 0xFFFFFFFF) &&
		 (storage->pointers_index > storage->address_start));

	if (pointers->data_pointer_read == 0xFFFFFFFF) /* No pointer found */
		return -1;

	if (storage->read(storage->pointers_index,
			  sizeof(pointers_block_t),
			  (uint8_t *)pointers) != 0)
		return -1;

	return 0;
}

static int32_t set_pointers(cir_storage_flash_t *storage,
			    pointers_block_t *pointers)
{
	/* We increase last pointers address before write the new ones in flash */
	storage->pointers_index += sizeof(pointers_block_t);

	/* If there is not enough place on the pointers block, we erase it
	   and copy new pointers at the beginning of the block */
	if (storage->pointers_index%storage->block_size == 0) {
		storage->pointers_index = storage->address_start;
		if (storage->erase(storage->block_start, 1) != 0)
			return -1;
	}

	if (storage->write(storage->pointers_index, sizeof(pointers_block_t), (uint8_t *)pointers) != 0)
		return -1;

	return 0;
}
