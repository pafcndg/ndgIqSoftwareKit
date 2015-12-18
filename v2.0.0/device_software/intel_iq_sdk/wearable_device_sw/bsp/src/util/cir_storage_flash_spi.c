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

/* Flash storage */
#include "drivers/spi_flash.h"
#include "soc_config.h"
#include "project_mapping.h"

#include "util/cir_storage.h"
#include "util/cir_storage_flash_spi.h"
#include "cir_storage_flash_internal.h"
#include "drivers/serial_bus_access.h"

static int32_t spi_flash_0_read(uint32_t address,
				uint32_t data_size,
				uint8_t *data);
static int32_t spi_flash_0_write(uint32_t address,
				 uint32_t data_size,
				 uint8_t *data);
static int32_t spi_flash_0_erase(uint32_t first_block_to_erase,
				 uint32_t nb_blocks_to_erase);


cir_storage_t *cir_storage_flash_spi_init(uint32_t size, uint32_t block_start)
{
	cir_storage_flash_t *storage = balloc(sizeof(*storage), NULL);
	storage->parent.size = size;
	storage->block_start = block_start;
	storage->block_size = SERIAL_FLASH_BLOCK_SIZE;
	storage->address_start = block_start*SERIAL_FLASH_BLOCK_SIZE;
	storage->read = spi_flash_0_read;
	storage->write = spi_flash_0_write;
	storage->erase = spi_flash_0_erase;
	if (cir_storage_flash_init(storage) == 0) {
		return &(storage->parent);
	} else {
		bfree(storage);
		return NULL;
	}
}

static int32_t spi_flash_0_read(uint32_t address,
				uint32_t data_size,
				uint8_t *data)
{
	uint32_t i;
	unsigned int data_read_len;

	for (i = 0; i < data_size; i++) {
		if (spi_flash_read_byte(&pf_sba_device_flash_spi0.dev,
					address+i,
					1,
					&data_read_len,
					&data[i]) != DRV_RC_OK)
			return -1;
	}

	return 0;
}

static int32_t spi_flash_0_erase(uint32_t first_block_to_erase,
				 uint32_t nb_blocks_to_erase)
{
	if (spi_flash_sector_erase(&pf_sba_device_flash_spi0.dev,
				   first_block_to_erase,
				   nb_blocks_to_erase) != DRV_RC_OK)
		return -1;

	return 0;
}

static int32_t spi_flash_0_write(uint32_t address,
				 uint32_t data_size,
				 uint8_t *data)
{
	unsigned int data_write_len;

	if (spi_flash_write_byte(&pf_sba_device_flash_spi0.dev,
				 address,
				 data_size,
				 &data_write_len,
				 data) != DRV_RC_OK)
		return -1;

	return 0;
}
