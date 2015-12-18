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

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <fs/fat/diskio.h>		/* FatFs lower layer API */
#include <fs/fat/ffconf.h>
#include <drivers/spi_flash.h>
#include "machine.h"

static uint32_t fs_flash_size = 0;
static uint32_t fs_flash_page_size = 0;
static uint32_t fs_flash_sector_size = 0;
static uint32_t fs_flash_block_size = 0;
static uint32_t fs_flash_large_block_size = 0;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive number */
)
{
	uint8_t stat = 0;
	uint8_t rc_stat = 0;

	uint8_t write_protect_mask = 0x7C;

	struct device *spi = (struct device *)&pf_sba_device_flash_spi0;

	if (pdrv != 0) {
		return RES_PARERR;
	}

	spi_flash_get_status(spi, &stat);

	/* verify the erase/write in progress bit */
	if (stat & write_protect_mask) {
		rc_stat |= STA_PROTECT;
	}

	return rc_stat;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive number to identify the drive */
)
{
#if 0
	/*
	 * These values were pulled from spi_flash_defs.h and are kept here for
	 * reference purposes.
	 */
	fs_flash_size = 0x1000000;		// memory size in bytes
	fs_flash_page_size 0x100;		// page size in bytes
	fs_flash_sector_size = 0x1000;		// sector size in bytes
	fs_flash_block32k_size = 0x8000;	// block size in bytes
	fs_flash_block_size = 0x10000;		// block size in bytes
#endif

	DSTATUS stat = STA_NOINIT;

	/* Pull the IOCTL values from the SPI now and store them */
	spi_flash_ioctl(NULL, &fs_flash_size, STORAGE_SIZE);
	spi_flash_ioctl(NULL, &fs_flash_page_size, STORAGE_PAGE_SIZE);
	fs_flash_sector_size = _MAX_SS;		// from ffconf.h
	spi_flash_ioctl(NULL, &fs_flash_block_size, STORAGE_BLOCK_SIZE);
	spi_flash_ioctl(NULL, &fs_flash_large_block_size,
			STORAGE_LARGE_BLOCK_SIZE);

	stat &= ~STA_NOINIT;
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,		/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRIVER_API_RC ret;
	unsigned int retlen;
	uint8_t *data = (uint8_t *)buff;
	uint8_t status = 0;
	uint32_t byte_count = 0;
	uint32_t byte_sector = 0;
	struct device *spi;

	if (pdrv != 0 || count == 0 || buff == 0) {
		return RES_PARERR;
	}

	spi = (struct device *)&pf_sba_device_flash_spi0;

	spi_flash_get_status(spi, &status);

	/* Verify the disk is ready for action */
	if (status & STA_NOINIT) {
		return RES_NOTRDY;
	}

	if (status & STA_PROTECT) {
		return RES_WRPRT;
	}

	byte_count = fs_flash_sector_size * count;
	byte_sector = fs_flash_sector_size * sector;

	ret = spi_flash_read_byte(spi, byte_sector, byte_count, &retlen, data);

	if (ret != DRV_RC_OK) {
		return RES_ERROR;
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,		/* Physical drive number to identify */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count		/* Number of sectors to write */
)
{
	DRIVER_API_RC ret;
	unsigned int retlen;
	uint8_t *data = (uint8_t *)buff;
	uint8_t status = 0;
	uint32_t byte_count = 0;
	uint32_t byte_sector = 0;

	struct device *spi = (struct device *)&pf_sba_device_flash_spi0;

	if (pdrv != 0 || count == 0)
		return RES_PARERR;

	spi_flash_get_status(spi, &status);

	/* Verify the disk is ready for action */
	if (status & STA_NOINIT) {
		return RES_NOTRDY;
	}

	if (status & STA_PROTECT) {
		return RES_WRPRT;
	}

	/* Erase before write */
	ret = spi_flash_sector_erase(spi, sector, count);

	if (ret != DRV_RC_OK) {
		return RES_ERROR;
	}

	byte_count = fs_flash_sector_size * count;
	byte_sector = fs_flash_sector_size * sector;

	ret = spi_flash_write_byte(spi, byte_sector, byte_count, &retlen, data);

	if (ret != DRV_RC_OK) {
		return RES_ERROR;
	}

	return ret;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT ret = RES_OK;

        switch (cmd) {
        case CTRL_SYNC :
		/* Wait for end of internal write process of the drive */
                break;
        case GET_SECTOR_COUNT :
		/* Should be 4096 */
		*(DWORD*)buff = fs_flash_size / fs_flash_sector_size;
		break;
	case GET_SECTOR_SIZE :
		/* Should be 512 although the flash supports 4096 */
		*(DWORD*)buff = fs_flash_sector_size;
		break;
	case GET_BLOCK_SIZE :
		/* BLOCK_SIZE is in unit of sectors */
		*(DWORD*)buff = fs_flash_block_size / fs_flash_sector_size;
		break;
	case CTRL_TRIM :
		break;
	default:
		ret = RES_PARERR;
        }

	return ret;
}
#endif
