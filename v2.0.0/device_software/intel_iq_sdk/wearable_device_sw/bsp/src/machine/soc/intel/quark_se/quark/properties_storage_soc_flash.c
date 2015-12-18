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

#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "util/compiler.h"
#include "infra/properties_storage.h"
#include "infra/panic.h"
#include "infra/log.h"
#include "project_mapping.h"

#include "drivers/soc_flash.h"

/*
 * Implementation of the properties_storage API on top of the Quark SE
 * SOC Flash driver.
 *
 * A flash partition is a range of nb_blocks contiguous blocks starting at
 * start_block.
 * Offsets are counted in bytes starting from byte 0 of the first block.
 *
 * Example:
 *
 *        BLOCK11             BLOCK12             BLOCK13
 * [-------------------|-------------------|-------------------]
 * here, start_block == 11, nb_blocks == 3
 * The first byte of BLOCK11 is at offset 11 * block_size.
 *
 * Each block has a 4 bytes header which is simply a 32 bits number which is
 * incremented each time a new block is used. If this header is 0xffffffff the
 * block is assumed to be free.
 *
 * Properties are saved on flash sequentially with the following format:
 * | 8 Bytes header | Data of variable length | Optional padding |
 *
 * A property cannot be stored on more than 1 block, i.e. when a new property
 * value needs to be written, it must fit in the remaining space of the block.
 *
 * The last property in a block is followed by 8 bytes set at value zero.
 */

#define NEXT_MULTIPLE_OF_4(x) (((x)+3) & ~3)
#define BLOCK_FOR_OFFSET(part, offset) ((offset) / (part)->block_size)
#define NEXT_BLOCK(part, block) \
    ((part)->start_block + (((block) - (part)->start_block) + 1) % \
    (part)->nb_blocks)
#define OLDEST_BLOCK(part) BLOCK_FOR_OFFSET((part), (part)->current_read_offset)
#define BLOCK_IS_LAST_FREE(part, block) \
    (NEXT_BLOCK((part), (block)) == OLDEST_BLOCK(part))

typedef struct {
	/* Start block number */
	uint16_t start_block;
	/* Number of blocks in this partition */
	uint16_t nb_blocks;
	/* Size of one block in bytes */
	uint16_t block_size;
	/* Offset of the first valid entry in bytes */
	uint32_t current_read_offset;
	/* Offset of the first available byte in bytes */
	uint32_t current_write_offset;
	/* Offset of the last written property in bytes  */
	uint32_t previous_write_offset;
	/* Incremented each time a new block is started */
	uint32_t last_written_block_header;
} flash_partition_t;

static flash_partition_t reset_persistent_partition = {
	.start_block = FACTORY_RESET_PERSISTENT_START_BLOCK,
	.nb_blocks = FACTORY_RESET_PERSISTENT_END_BLOCK -
	    FACTORY_RESET_PERSISTENT_START_BLOCK + 1,
	.block_size = EMBEDDED_FLASH_BLOCK_SIZE,
};

static flash_partition_t not_persistent_partition = {
	.start_block = FACTORY_RESET_NON_PERSISTENT_START_BLOCK,
	.nb_blocks = FACTORY_RESET_NON_PERSISTENT_END_BLOCK -
	    FACTORY_RESET_NON_PERSISTENT_START_BLOCK + 1,
	.block_size = EMBEDDED_FLASH_BLOCK_SIZE,
};

#define PROPERTY_FLAG_NONE          0xFF /* 0b11111111 */
#define PROPERTY_FLAG_OBSOLETE      0xFE /* 0b11111110 */

/* The header for each block is a 32 bits value */
#define BLOCK_HEADER_SIZE 4
/* A free header is == 0xffffffff */
#define UNUSED_BLOCK_HEADER 0xffffffff

/* An entry header on the flash. It is followed by the actual data,
 * padded to 4 bytes */
typedef struct
{
	uint32_t key;
	/* Length of the corresponding data */
	uint16_t len;
	/* The entry is obsolete if bit 0 is 0
	 * It is the last one in a block if bit 1 is 0 */
	uint8_t pflags;
	uint8_t reserved;
} property_flash_header_t;
#define PROPERTY_HEADER_SIZE 8

#define IS_ENTRY_OBSOLETE(prop_header) \
    (((prop_header).pflags & ~PROPERTY_FLAG_OBSOLETE) == 0)

/* This implementation maintains an index of all properties in RAM,
 * the index is re-generated at startup by scanning the content of the
 * blocks allocated to the properties storage */
typedef struct
{
	uint32_t key;
	uint16_t len;
	uint8_t used;    /* false if the element is unused */
	uint8_t reserved;
	uint32_t offset; /* in byte, from byte 0 of block 0 (possibly outside partition) */
} property_info_t;

static property_info_t ram_cache[PROPERTIES_STORAGE_MAX_NB_PROPERTIES];

static property_info_t* get_property_info(uint32_t key)
{
	for (int i = 0; i < PROPERTIES_STORAGE_MAX_NB_PROPERTIES; ++i)
	{
		if (ram_cache[i].key == key && ram_cache[i].used == true)
			return &ram_cache[i];
	}
	return NULL;
}
static property_info_t* alloc_property_info()
{
	for (int i = 0; i < PROPERTIES_STORAGE_MAX_NB_PROPERTIES; ++i)
	{
		if (ram_cache[i].used == false)
		{
			ram_cache[i].used = true;
			return &ram_cache[i];
		}
	}
	return NULL;
}

static void free_property_info(property_info_t* p)
{
	assert(p->used == true);
	p->used = false;
}

static void clear_all_property_info()
{
	for (int i = 0; i < PROPERTIES_STORAGE_MAX_NB_PROPERTIES; ++i)
		ram_cache[i].used = false;
}

static bool is_entry_last_in_block(uint32_t offset, const property_flash_header_t* pfh)
{
	/* An entry is last in block if the following 8 bytes are equal to zero */
	uint32_t d[2];
	unsigned int ret_len;
	DRIVER_API_RC __maybe_unused ret = soc_flash_read(offset + NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + pfh->len), 2, &ret_len, d);
	assert(ret == DRV_RC_OK && ret_len == 2);
	return d[0] == 0 && d[1] == 0;
}

static bool set_entry_last_in_block(uint32_t offset, const property_flash_header_t* pfh)
{
	static uint32_t d[2] = {0, 0};
	unsigned int ret_len;
	DRIVER_API_RC ret = soc_flash_write(offset + NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + pfh->len), 2, &ret_len, d);
	return ret == DRV_RC_OK && ret_len == 2;
}

/* Find next free offset in block */
static uint32_t find_next_free_offset(const flash_partition_t* part,
    uint16_t block, bool* status)
{
	*status = true;
	uint32_t offset = block * part->block_size + BLOCK_HEADER_SIZE; /* starts after header */
	property_flash_header_t prop_header = {0};
	while (1) {
		unsigned int ret_len;
		DRIVER_API_RC __maybe_unused ret = soc_flash_read(offset, 2, &ret_len,
		    (uint32_t*)&prop_header);
		if (ret != DRV_RC_OK || ret_len != 2)
		{
			*status = false;
			return 0;
		}
		if (prop_header.key == 0xffffffff)
			break;
		offset += NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + prop_header.len);
	}
	return offset;
}

/* Determine which blocks are the oldest and more recently written
 * As we can't recover errors at this level, just return false if
 * an error occured */
static bool init_read_write_offset_from_flash(flash_partition_t* part)
{
	uint16_t b;
	uint32_t min_used_block = 0xffffffff;
	uint32_t min_used_block_header = 0xffffffff;
	uint32_t max_used_block = 0;
	uint32_t max_used_block_header = 0xffffffff;
	for (b = part->start_block; b < part->start_block + part->nb_blocks; ++b)
	{
		unsigned int ret_len;
		uint32_t block_header;
		DRIVER_API_RC ret = soc_flash_read(b*part->block_size, 1,
		    &ret_len, &block_header);
		if (ret != DRV_RC_OK || ret_len != 1)
			return false;
		if (block_header == UNUSED_BLOCK_HEADER)
			continue;
		if (block_header < min_used_block_header)
		{
			min_used_block_header = block_header;
			min_used_block = b;
		}
		if (block_header > max_used_block_header ||
		    max_used_block_header == UNUSED_BLOCK_HEADER)
		{
			max_used_block_header = block_header;
			max_used_block = b;
		}
	}

	/* Case of the yet unused partition */
	if (min_used_block_header == UNUSED_BLOCK_HEADER &&
	    max_used_block_header == UNUSED_BLOCK_HEADER)
	{
		/* Init the first partition */
		unsigned int ret_len;
		part->last_written_block_header = 0;
		DRIVER_API_RC ret = soc_flash_write(part->start_block *
		     part->block_size, 1, &ret_len,
		     (uint32_t*)&(part->last_written_block_header));
		if (ret != DRV_RC_OK)
			return false;
		part->current_write_offset = part->start_block*part->block_size + BLOCK_HEADER_SIZE;
		part->current_read_offset = part->current_write_offset;
		part->previous_write_offset = part->current_write_offset;
		return true;
	}

	/* As blocks are initialized with incrementing numbers, a difference larger
	 * than the number of blocks indicate that there was something wrong:
	 * abort.
	 * Note that after 2^32 block written we have a wrap-around, leading to a
	 * failure in certain conditions. We ignore this here as we assume the
	 * flash memory will be dead before, as typical life expectency is 10000
	 * writes per block. */
	if (max_used_block_header - min_used_block_header >= part->nb_blocks)
		return false;

	/* starts after header */
	part->current_read_offset = min_used_block*part->block_size + BLOCK_HEADER_SIZE;
	part->last_written_block_header = max_used_block_header;
	bool status;
	part->current_write_offset = find_next_free_offset(part, max_used_block, &status);
	part->previous_write_offset = part->current_write_offset;
	return status;
}


/* Return the offset where to read the next property. It doesn't mean that there
 * is one at this offset */
static uint32_t get_next_property_offset(const flash_partition_t* part,
	uint32_t offset, const property_flash_header_t* prop_header)
{
	/* When this flag is set, we know that this entry is the last one in this
	 * block and that the next block is already used by the next entry */
	if (is_entry_last_in_block(offset, prop_header))
	{
		uint16_t next_block = NEXT_BLOCK(part, BLOCK_FOR_OFFSET(part, offset));
		return next_block * part->block_size + BLOCK_HEADER_SIZE;
	}
	return offset + NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + prop_header->len);
}

/* Fill the global properties index in RAM
 * As we can't recover errors at this level, just return false if
 * an error occured */
static bool fill_index_from_flash(const flash_partition_t* part)
{
	/* Case of a completely empty store */
	if (part->current_read_offset == part->current_write_offset)
		return true;

	/* We can now assume that there is a valid header at position
	 * part->current_read_offset */

	uint32_t offset = part->current_read_offset;
	property_flash_header_t prop_header = {0};

	/* Read first property */
	unsigned int ret_len;
	DRIVER_API_RC ret = soc_flash_read(offset, 2, &ret_len,
	    (uint32_t*)&prop_header);
	if (ret != DRV_RC_OK || ret_len != 2 || prop_header.key == 0xffffffff)
		return false;
	while (1)
	{
		if (!IS_ENTRY_OBSOLETE(prop_header))
		{
			/* The entry is the most up-to-date one for this property, store it
			 * in our RAM index */
			property_info_t* p = alloc_property_info();
			/* As we reload a previous valid storage, we can't overflow by
			 * design */
			assert(p);

			p->key = prop_header.key;
			p->len = prop_header.len;
			p->offset = offset;
		}
		offset = get_next_property_offset(part, offset, &prop_header);
		ret = soc_flash_read(offset, 2, &ret_len, (uint32_t*)&prop_header);
		if (ret != DRV_RC_OK || ret_len != 2)
			return false;
		if (prop_header.key == 0xffffffff)
			break;
	}
	return true;
}

static void format_partition(flash_partition_t* part)
{
	DRIVER_API_RC __maybe_unused ret = soc_flash_block_erase(part->start_block,
	    part->nb_blocks);
	pr_warning(LOG_MODULE_MAIN, "Formatting flash blocks %u through %u...",
	    part->start_block, part->start_block+part->nb_blocks);
	assert(ret == DRV_RC_OK);
}

/* Intialize the property storage from the flash content. In case of errors, we
 * violently re-format the partition.. */
void properties_storage_init(void)
{
	int run_iter = 0;
restart:
	run_iter++;
	clear_all_property_info();
	/* If run_iter > 3, this mean we already formatted everything and it still
	 * doesn't work, this means the board is quite broken */
	if (run_iter > 3)
		panic(88);

	/* Init the read/write offset */
	if (!init_read_write_offset_from_flash(&reset_persistent_partition))
	{
		format_partition(&reset_persistent_partition);
		goto restart;
	}
	if (!init_read_write_offset_from_flash(&not_persistent_partition))
	{
		format_partition(&not_persistent_partition);
		goto restart;
	}

	/* Fill the RAM index from the flash entries */
	if (!fill_index_from_flash(&reset_persistent_partition))
	{
		format_partition(&reset_persistent_partition);
		goto restart;
	}
	if (!fill_index_from_flash(&not_persistent_partition))
	{
		format_partition(&not_persistent_partition);
		goto restart;
	}
}

void properties_storage_format_all(void)
{
	format_partition(&reset_persistent_partition);
	format_partition(&not_persistent_partition);
	properties_storage_init();
}

/* Copy the entry at src_offset to the current write offset */
static void copy_entry_if_not_obsolete(flash_partition_t* part,
	uint32_t* src_offset, bool* last_in_block)
{
	uint8_t tmp[PROPERTY_HEADER_SIZE + PROPERTIES_STORAGE_MAX_VALUE_LEN];
	property_flash_header_t* prop_header = (property_flash_header_t*)tmp;

	unsigned int ret_len;
	DRIVER_API_RC ret = soc_flash_read(*src_offset, 2, &ret_len, (uint32_t*)tmp);
	assert(ret == DRV_RC_OK && ret_len == 2);
	assert(prop_header->key != 0xffffffff);

	*last_in_block = is_entry_last_in_block(*src_offset, prop_header);

	const unsigned int value_size = NEXT_MULTIPLE_OF_4(prop_header->len) / 4;

	/* Scraps obsolete content */
	if (!IS_ENTRY_OBSOLETE(*prop_header))
	{
		ret = soc_flash_read(*src_offset + PROPERTY_HEADER_SIZE, value_size,
		    &ret_len, (uint32_t*)(tmp + PROPERTY_HEADER_SIZE));
		assert(ret == DRV_RC_OK && ret_len == value_size);
		/* Reset all flags */
		prop_header->pflags = PROPERTY_FLAG_NONE;
		ret = soc_flash_write(part->current_write_offset, 2 + value_size,
		    &ret_len, (uint32_t*)tmp);
		assert(ret == DRV_RC_OK && ret_len == 2 + value_size);

		property_info_t *prop_info = get_property_info(prop_header->key);
		/* Update RAM index for this entry */
		assert(prop_info);
		prop_info->offset = part->current_write_offset;

		part->previous_write_offset = part->current_write_offset;
		part->current_write_offset += PROPERTY_HEADER_SIZE + value_size * 4;
	}

	*src_offset += PROPERTY_HEADER_SIZE + value_size * 4;
}

static properties_storage_status_t make_entry_obsolete_in_flash(uint32_t offset)
{
	/* The element was already present in the RAM index, make the previous
	 * entry obsolete */
	property_flash_header_t pfh = {0};
	unsigned int ret_len;
	DRIVER_API_RC ret = soc_flash_read(offset, 2, &ret_len, (uint32_t*)&pfh);
	assert(ret == DRV_RC_OK && ret_len == 2);

	assert(!IS_ENTRY_OBSOLETE(pfh));
	pfh.pflags &= PROPERTY_FLAG_OBSOLETE;
	ret = soc_flash_write(offset, 2, &ret_len, (uint32_t*)&pfh);
	if (ret != DRV_RC_OK)
		panic(67);
	return ret;
}

/* Initialize the next block to prepare writting in it. This may involve
 * shifting the oldest blocks content into the new one to get rid of obsolete
 * properties to make space */
static void prepare_next_block(flash_partition_t* part)
{
	/* Flag the last entry as last in its block */
	property_flash_header_t pfh = {0};
	unsigned int ret_len;
	DRIVER_API_RC __maybe_unused ret = soc_flash_read(part->previous_write_offset, 2, &ret_len, (uint32_t*)&pfh);
	assert(ret == DRV_RC_OK && ret_len == 2);
	set_entry_last_in_block(part->previous_write_offset, &pfh);

	/* Init the next free block by writing its header */
	uint16_t next_block = NEXT_BLOCK(part, BLOCK_FOR_OFFSET(part,
		part->current_write_offset));
	part->last_written_block_header++;
	if (part->last_written_block_header == UNUSED_BLOCK_HEADER)
		part->last_written_block_header = 0;
	ret = soc_flash_write(next_block*part->block_size, 1,
		&ret_len, (uint32_t*)&(part->last_written_block_header));
	if (ret != DRV_RC_OK)
		panic(67);
	part->current_write_offset = next_block*part->block_size + BLOCK_HEADER_SIZE;

	if (BLOCK_IS_LAST_FREE(part, next_block))
	{
		/* We are starting to write on the last free block, but we always
		 * need at least one completely free block so we need to free the
		 * oldest used block. For this, we copy the oldest block's
		 * content in the new one, and scrap all obsolete entries */
		uint16_t oldest_block = OLDEST_BLOCK(part);
		uint32_t src_offset = oldest_block * part->block_size + BLOCK_HEADER_SIZE;

		bool last_in_block = false;
		/* Copy oldest block in new one. Note that the write_offset of the
		 * partition is updated within the copy_entry_if_not_obsolete
		 * function */
		while (!last_in_block)
			copy_entry_if_not_obsolete(part, &src_offset, &last_in_block);

		part->current_read_offset = NEXT_BLOCK(part, oldest_block) * part->block_size + BLOCK_HEADER_SIZE;

		/* We are done copying one block into the other, we can now clear
		 * the oldest block */
		DRIVER_API_RC ret = soc_flash_block_erase(oldest_block, 1);
		if (ret != DRV_RC_OK)
			panic(67);
	}
}

properties_storage_status_t properties_storage_set(uint32_t key,
	const uint8_t* buf,	uint16_t len, bool factory_reset_persistent)
{
	if (len > PROPERTIES_STORAGE_MAX_VALUE_LEN)
		return PROPERTIES_STORAGE_INVALID_ARG;

	unsigned int ret_len;
	flash_partition_t* part = factory_reset_persistent ?
	    &reset_persistent_partition : &not_persistent_partition;

	uint16_t old_write_block = BLOCK_FOR_OFFSET(part, part->current_write_offset);

	uint32_t remaining_space_in_block = part->block_size -
				(part->current_write_offset % part->block_size);

	/* Ensure we have enough contiguous space in the currently written block */
	/* The "+8" here is important: it ensures that after writing the new
	 * property, we will still have at least 8 bytes free at the end of the
	 * block so that:
	 * - the new incremented write_offset will still lay inside the current
	 *   block
	 * - we reserve 8 bytes for potentially flagging this entry as the last
	 *   one in the block
	 */
	while (NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + len + 8) > remaining_space_in_block)
	{
		/* We don't have enough space in current block, prepare the next one */
		prepare_next_block(part);

		/* If we have wrapped around looking for a slot large enough, it
		 * means that the store is completely filled with valid entries */
		if (old_write_block == BLOCK_FOR_OFFSET(part, part->current_write_offset))
			return PROPERTIES_STORAGE_BOUNDS_ERROR;

		remaining_space_in_block = part->block_size -
			(part->current_write_offset % part->block_size);
	}

	/* From this point we know we have enough space available at
	 * current_write_offset to write the new entry. */
	uint8_t tmp[PROPERTY_HEADER_SIZE + PROPERTIES_STORAGE_MAX_VALUE_LEN];
	property_flash_header_t* prop_header = (property_flash_header_t*)tmp;
	prop_header->pflags = PROPERTY_FLAG_NONE;
	prop_header->key = key;
	prop_header->len = len;
	prop_header->reserved = 0xff;
	memcpy(tmp+PROPERTY_HEADER_SIZE, buf, len);
	uint32_t wlen = NEXT_MULTIPLE_OF_4(PROPERTY_HEADER_SIZE + len);
	DRIVER_API_RC ret = soc_flash_write(part->current_write_offset,
	    wlen/4, &ret_len, (uint32_t*)tmp);
	if (ret != DRV_RC_OK || ret_len != wlen/4)
		panic(67);
	part->previous_write_offset = part->current_write_offset;
	part->current_write_offset += wlen;
	assert(BLOCK_FOR_OFFSET(part, part->current_write_offset) ==
	    BLOCK_FOR_OFFSET(part, part->previous_write_offset));

	property_info_t* p = get_property_info(key);
	if (p != NULL)
	{
		/* The element was already present in the RAM index, make the previous
		 * entry obsolete */
		ret = make_entry_obsolete_in_flash(p->offset);
		if (ret != DRV_RC_OK)
			return PROPERTIES_STORAGE_IO_ERROR;
	}
	else
	{
		/* Allocate a new property info in our cache */
		p = alloc_property_info();
		if (p == NULL)
			return PROPERTIES_STORAGE_BOUNDS_ERROR;
		p->key = key;
	}
	p->offset = part->previous_write_offset;
	p->len = len;

	return PROPERTIES_STORAGE_SUCCESS;
}

properties_storage_status_t properties_storage_get_info(uint32_t key,
	uint16_t* len, bool* factory_reset_persistent)
{
	/* Look for offset in the RAM index */
	property_info_t* pinfo = get_property_info(key);
	if (pinfo == NULL)
		return PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR;

	*len = pinfo->len;
	*factory_reset_persistent = false;
	if (pinfo->offset >= reset_persistent_partition.start_block * reset_persistent_partition.block_size &&
	    pinfo->offset < (reset_persistent_partition.start_block + reset_persistent_partition.nb_blocks) *
	    reset_persistent_partition.block_size)
	{
	    *factory_reset_persistent = true;
	}
	return PROPERTIES_STORAGE_SUCCESS;
}

properties_storage_status_t properties_storage_get(uint32_t key, uint8_t *buf,
	uint16_t len, uint16_t* readlen)
{
	/* Look for offset in the RAM index */
	property_info_t* pinfo = get_property_info(key);
	if (pinfo == NULL)
	{
		*readlen = 0;
		return PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR;
	}
	*readlen = pinfo->len;

	if (len < pinfo->len)
		return PROPERTIES_STORAGE_BOUNDS_ERROR;

	/* Read the multiple of 4 part first directly in the output variable */
	unsigned int ret_len;
	DRIVER_API_RC ret;

	if (pinfo->len > 4)
	{
		ret = soc_flash_read(pinfo->offset + PROPERTY_HEADER_SIZE, (pinfo->len & ~3) / 4, &ret_len,
		    (uint32_t*)buf);
		if (ret != DRV_RC_OK)
		{
			*readlen = 0;
			return PROPERTIES_STORAGE_IO_ERROR;
		}

		if ((pinfo->len & 3) == 0)
			return PROPERTIES_STORAGE_SUCCESS;
	}

	/* Read the rest */
	uint8_t data[4];
	ret = soc_flash_read((pinfo->offset + PROPERTY_HEADER_SIZE + pinfo->len) & ~3, 1, &ret_len,
	    (uint32_t*)&data);
	if (ret != DRV_RC_OK)
	{
		*readlen = 0;
		return PROPERTIES_STORAGE_IO_ERROR;
	}

	for (int i = 0; i < (pinfo->len & 3); ++i)
		buf[(pinfo->len & ~3) + i] = data[i];

	return PROPERTIES_STORAGE_SUCCESS;
}

properties_storage_status_t properties_storage_delete(uint32_t key)
{
	/* Look for offset in the RAM index */
	property_info_t* pinfo = get_property_info(key);
	if (pinfo == NULL)
		return PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR;

	make_entry_obsolete_in_flash(pinfo->offset);
	free_property_info(pinfo);

	return PROPERTIES_STORAGE_SUCCESS;
}
