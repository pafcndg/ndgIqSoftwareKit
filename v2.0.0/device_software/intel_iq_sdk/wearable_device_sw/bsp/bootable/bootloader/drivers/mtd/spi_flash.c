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

#include "mtd/spi_flash.h"
#include <spi/quark_spi.h>
#include <stdbool.h>
#include <printk.h>

#ifdef CONFIG_SPI_FLASH_W25Q16DV
#include "mtd/spi_flash_w25qxxdv_defs.h"
#elif defined(CONFIG_SPI_FLASH_MX25)
#include "mtd/spi_flash_mx25.h"
#else
#error Please define the device for which you build the driver
#endif

/*! Erase operations list */
typedef enum {
	ER_SECTOR,		/*!< Erase sector */
	ER_BLOCK,		/*!< Erase block */
	ER_LARGE_BLOCK,		/*!< Erase lage block */
	ER_CHIP			/*!< Erase all flash chip */
} ER_TYPE;

DRIVER_API_RC spi_flash_init(spi_flash_info_t * flash_dev)
{
	uint32_t rdid;
	DRIVER_API_RC ret = DRV_RC_FAIL;

	// Compute block and sector counts
	flash_dev->sector_size = FLASH_SECTOR_SIZE,
	    flash_dev->block_size = FLASH_BLOCK32K_SIZE,
	    flash_dev->large_block_size = FLASH_BLOCK_SIZE,
	    flash_dev->mem_size = FLASH_SIZE,
	    flash_dev->sector_count =
	    flash_dev->mem_size / flash_dev->sector_size;
	flash_dev->block_count = flash_dev->mem_size / flash_dev->block_size;
	flash_dev->large_block_count =
	    flash_dev->mem_size / flash_dev->large_block_size;

	// Check flash RDID (probe device)
	if ((spi_flash_get_rdid(flash_dev, &rdid) != DRV_RC_OK)
	    || (rdid != FLASH_RDID_VALUE)) {
		pr_debug("rdid check failed (0x%x)", rdid);
		return DRV_RC_FAIL;
	}
	return DRV_RC_OK;
}

DRIVER_API_RC spi_flash_get_rdid(spi_flash_info_t * flash_dev, uint32_t * rdid)
{
	uint8_t command;
	DRIVER_API_RC ret;

	command = FLASH_CMD_RDID;
	*rdid = 0;
	ret =
	    soc_spi_transfer(&command, 1, (uint8_t *) rdid, 3, 0,
			     flash_dev->slave);

	return ret;
}

DRIVER_API_RC spi_flash_read_byte(spi_flash_info_t * flash_dev,
				  uint32_t address, unsigned int len,
				  unsigned int *retlen, uint8_t * data)
{
	DRIVER_API_RC ret = DRV_RC_OK;

	uint8_t command[4];

	*retlen = 0;

	command[0] = FLASH_CMD_READ;
	command[1] = (uint8_t) (address >> 16);
	command[2] = (uint8_t) (address >> 8);
	command[3] = (uint8_t) address;

	if ((ret =
	     soc_spi_transfer(command, 4, data, len, 0,
			      flash_dev->slave)) == DRV_RC_OK) {
		*retlen = len;
	}

	return ret;
}

DRIVER_API_RC spi_flash_read(spi_flash_info_t * flash_dev, uint32_t address,
			     unsigned int len, unsigned int *retlen,
			     uint32_t * data)
{
	DRIVER_API_RC ret =
	    spi_flash_read_byte(flash_dev, address, len << 2, retlen,
				(uint8_t *) data);
	*retlen = (*retlen >> 2);
	return ret;
}

/*  write support */

static DRIVER_API_RC spi_flash_get_status(spi_flash_info_t * flash_dev,
					  uint8_t * status)
{
	uint8_t command;

	command = FLASH_CMD_RDSR;
	*status = 0;
	return soc_spi_transfer(&command, 1, status, 1, 0, flash_dev->slave);
}

static DRIVER_API_RC spi_flash_get_rdscur(spi_flash_info_t * flash_dev,
					  uint8_t * rdscur)
{
	uint8_t command;

	command = FLASH_CMD_RDSCUR;
	*rdscur = 0;
	return soc_spi_transfer(&command, 1, rdscur, 1, 0, flash_dev->slave);
}

static DRIVER_API_RC spi_flash_write_enable(spi_flash_info_t * flash_dev)
{
	uint8_t command;
	uint8_t status = 0;
	DRIVER_API_RC ret;

	command = FLASH_CMD_WREN;

	// Send write enable command
	if ((ret =
	     soc_spi_transfer(&command, 1, NULL, 0, 0,
			      flash_dev->slave)) != DRV_RC_OK) {
		return ret;
	}

	command = FLASH_CMD_RDSR;

	// Read status register to check if write is enabled
	if ((ret =
	     soc_spi_transfer(&command, 1, &status, 1, 0,
			      flash_dev->slave)) != DRV_RC_OK) {
		return ret;
	}

	return (status & FLASH_WEL_BIT) ? DRV_RC_OK : DRV_RC_FAIL;
}

DRIVER_API_RC spi_flash_write_byte(spi_flash_info_t * flash_dev,
				   uint32_t address, unsigned int len,
				   unsigned int *retlen, uint8_t * data)
{
	DRIVER_API_RC ret = DRV_RC_OK;
	uint8_t status;

	uint8_t tx_buffer[4 + FLASH_PAGE_SIZE];

	*retlen = 0;

	// Check input parameters
	if ((len + address) > flash_dev->mem_size) {
		return DRV_RC_OUT_OF_MEM;
	}

	*retlen = len;

	// We can only program a page with PP command so we use several write operations
	unsigned int count;	// Byte count to write for next program operation
	count = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));
	if (count > len) {
		count = len;
	}

	tx_buffer[0] = FLASH_CMD_PP;

	// Loop on pages to program
	for (; len;
	     address += count, data += count, count =
	     ((len -= count) > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : len)) {
		// Enable write operation
		if ((ret = spi_flash_write_enable(flash_dev)) != DRV_RC_OK) {
			*retlen -= len;
			return ret;
		}
		// Set data for write operation
		uint32_t i;
		for (i = 0; i < count; i++) {
			tx_buffer[i + 4] = data[i];
		}

		tx_buffer[1] = (uint8_t) (address >> 16);
		tx_buffer[2] = (uint8_t) (address >> 8);
		tx_buffer[3] = (uint8_t) address;

		if ((ret =
		     soc_spi_transfer(tx_buffer, count + 4, NULL, 0, 0,
				      flash_dev->slave)) != DRV_RC_OK) {
			*retlen -= len;
			return ret;
		}
		// loop until the write operation is complete
		do {
			if ((ret =
			     spi_flash_get_status(flash_dev,
						  &status)) != DRV_RC_OK) {
				// Error detected
				*retlen -= len;
				return ret;
			}
		} while (status & FLASH_WIP_BIT);

		// Check for success
		if ((ret =
		     spi_flash_get_rdscur(flash_dev, &status)) != DRV_RC_OK) {
			// Error detected
			*retlen -= len;
			return ret;
		}
		if (status & FLASH_SECR_PFAIL_BIT) {
			// Write failed
			ret = DRV_RC_CHECK_FAIL;
			*retlen -= len;
			return ret;
		}
	}
	return ret;
}

DRIVER_API_RC spi_flash_write(spi_flash_info_t * flash_dev, uint32_t address,
			      unsigned int len, unsigned int *retlen,
			      uint32_t * data)
{
	DRIVER_API_RC ret =
	    spi_flash_write_byte(flash_dev, address, len << 2, retlen,
				 (uint8_t *) data);
	*retlen = (*retlen >> 2);
	return ret;
}

static DRIVER_API_RC spi_flash_erase(spi_flash_info_t * flash_dev,
				     ER_TYPE er_type, unsigned int start,
				     unsigned int count)
{
	DRIVER_API_RC ret = DRV_RC_OK;
	uint8_t command[16];
	uint32_t command_len = 4;
	unsigned int er_size, er_count;

	switch (er_type) {
	case ER_SECTOR:
		command[0] = FLASH_CMD_SE;
		er_size = flash_dev->sector_size;
		er_count = flash_dev->sector_count;
		break;
	case ER_BLOCK:
		command[0] = FLASH_CMD_BE32K;
		er_size = flash_dev->block_size;
		er_count = flash_dev->block_count;
		break;
	case ER_LARGE_BLOCK:
		command[0] = FLASH_CMD_BE;
		er_size = flash_dev->large_block_size;
		er_count = flash_dev->large_block_count;
		break;
	case ER_CHIP:
		command[0] = FLASH_CMD_CE;
		er_size = flash_dev->mem_size;
		er_count = 1;
		count = 1;
		start = 0;
		command_len = 1;
		break;
	default:
		return DRV_RC_INVALID_OPERATION;
	}

	if ((count + start) > er_count) {
		return DRV_RC_OUT_OF_MEM;
	}
	// TODO: Check write protection
	for (count += start; start < count; start++) {
		uint32_t address = er_size * start;

		// Enable write operation
		if ((ret = spi_flash_write_enable(flash_dev)) != DRV_RC_OK) {
			return ret;
		}

		command[1] = (uint8_t) (address >> 16);
		command[2] = (uint8_t) (address >> 8);
		command[3] = (uint8_t) address;

		if ((ret =
		     soc_spi_transfer(command, command_len, NULL, 0, 0,
				      flash_dev->slave)) != DRV_RC_OK) {
			// Error detected
			return ret;
		}
		// loop until the erase is complete
		uint8_t status;
		do {
			if ((ret =
			     spi_flash_get_status(flash_dev,
						  &status)) != DRV_RC_OK) {
				// Error detected
				return ret;
			}
		} while (status & FLASH_WIP_BIT);

		// Check for success
		if ((ret =
		     spi_flash_get_rdscur(flash_dev, &status)) != DRV_RC_OK) {
			// Error detected
			return ret;
		}
		if (status & FLASH_SECR_EFAIL_BIT) {
			// Erase failed
			return DRV_RC_CHECK_FAIL;
		}
	}
	return ret;
}

DRIVER_API_RC spi_flash_sector_erase(spi_flash_info_t * flash_dev,
				     unsigned int start_sector,
				     unsigned int sector_count)
{
	return spi_flash_erase(flash_dev, ER_SECTOR, start_sector,
			       sector_count);
}

DRIVER_API_RC spi_flash_block_erase(spi_flash_info_t * flash_dev,
				    unsigned int start_block,
				    unsigned int block_count)
{
	return spi_flash_erase(flash_dev, ER_BLOCK, start_block, block_count);
}

DRIVER_API_RC spi_flash_large_block_erase(spi_flash_info_t * flash_dev,
					  unsigned int start_block,
					  unsigned int block_count)
{
	return spi_flash_erase(flash_dev, ER_LARGE_BLOCK, start_block,
			       block_count);
}

DRIVER_API_RC spi_flash_chip_erase(spi_flash_info_t * flash_dev)
{
	return spi_flash_erase(flash_dev, ER_CHIP, 0, 1);
}
