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

#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

#include <utils.h>

/**
 * @defgroup flash_spi_driver External SPI Flash
 * External SPI Flash driver API.
 * @ingroup common_drivers
 * @{
 */

/*! Flash memory management structure */
typedef struct spi_flash_info {
	uint32_t sector_count;	/*!< Number of sectors in the memory */
	uint32_t sector_size;	/*!< Sector size in bytes */
	uint32_t block_count;	/*!< Number of blocks in the memory */
	uint32_t block_size;	/*!< Block size in bytes */
	uint32_t large_block_count;	/*!< Number of large blocks in the memory */
	uint32_t large_block_size;	/*!< Large block size in bytes */
	uint32_t mem_size;	/*!< Memory size in byte (It must be equal to block_count*block_size) */

	uint32_t slave;

} spi_flash_info_t, *spi_flash_info_pt;

/**
 *  Init SPI flash info structure
 *
 *  @param  flash_dev       : spi flash info to use
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_init(spi_flash_info_t * flash_dev);

/**
 *  Read dwords data on SPI flash
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  address         : Address (in bytes) to read (need to be 4 bytes aligned)
 *  @param  len             : Number of dword (32bits) to read
 *  @param  retlen          : Number of dword (32bits) successfully red
 *  @param  data            : Buffer to store red data
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_read(spi_flash_info_t * flash_dev, uint32_t address,
			     unsigned int len, unsigned int *retlen,
			     uint32_t * data);

/**
 *  Read dwords data on SPI flash
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  address         : Address (in bytes) to read
 *  @param  len             : Number of bytes to read
 *  @param  retlen          : Number of bytes successfully red
 *  @param  data            : Buffer to store red data
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_read_byte(spi_flash_info_t * flash_dev,
				  uint32_t address, unsigned int len,
				  unsigned int *retlen, uint8_t * data);

/**
 *  Write buffer into flash memory (1024 bytes maximum).
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  address         : Address (in bytes) to write on (need to be 4 bytes aligned)
 *  @param  len             : Number of dword (32 bits) to write
 *  @param  retlen          : Number of dword (32 bits) successfully written
 *  @param  data            : data to write
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_write(spi_flash_info_t * flash_dev, uint32_t address,
			      unsigned int len, unsigned int *retlen,
			      uint32_t * data);

/**
 *  Write buffer into flash memory (1024 bytes maximum).
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  address         : Address (in bytes) to write on
 *  @param  len             : Number of bytes to write
 *  @param  retlen          : Number of bytes successfully written
 *  @param  data            : data to write
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_write_byte(spi_flash_info_t * flash_dev,
				   uint32_t address, unsigned int len,
				   unsigned int *retlen, uint8_t * data);

/**
 *  Erase sectors (4kB) of spi flash memory
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  start_sector    : First sector to erase
 *  @param  sector_count    : Number of sectors to erase
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_sector_erase(spi_flash_info_t * flash_dev,
				     unsigned int start_sector,
				     unsigned int sector_count);

/**
 *  Erase blocks (32kB) of spi flash memory
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  start_block     : First block to erase
 *  @param  block_count     : Number of blocks to erase
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_block_erase(spi_flash_info_t * flash_dev,
				    unsigned int start_block,
				    unsigned int block_count);

/**
 *  Erase large blocks (64kB) of spi flash memory
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  start_block     : First block to erase
 *  @param  block_count     : Number of blocks to erase
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_large_block_erase(spi_flash_info_t * flash_dev,
					  unsigned int start_block,
					  unsigned int block_count);

/**
 *  Erase all SPI flash memory
 *
 *  @param  flash_dev       : spi flash info to use
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_large_chip_erase(spi_flash_info_t * flash_dev);

/**
 *  Get SPI flash memory RDID
 *
 *  @param  flash_dev       : spi flash info to use
 *  @param  rdid     : pointer to the output value for rdid
 *
 *  @return  DRV_RC_OK on success else DRIVER_API_RC error code
 */
DRIVER_API_RC spi_flash_get_rdid(spi_flash_info_t * flash_dev, uint32_t * rdid);

/** @} */

#endif /* SPI_FLASH_H_ */
