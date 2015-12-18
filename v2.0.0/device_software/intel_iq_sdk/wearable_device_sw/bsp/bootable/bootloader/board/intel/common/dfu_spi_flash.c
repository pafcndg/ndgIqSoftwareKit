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

#include <mtd/spi_flash.h>
#include <partition.h>
#include <printk.h>
#include <dfu.h>
#include <dfu_spi_flash.h>

void dfu_spi_flash_read(struct dfu_ops *ops)
{
	usb_device_request_t *setup_packet = ops->device_request;
	unsigned int retlen;
	DRIVER_API_RC ret;
	unsigned int address = UGETW(setup_packet->wValue) * PAGE_SIZE;
	unsigned int partition_size;
	unsigned int partition_offset;


	spi_flash_info_t *spi_flash = get_spi_flash();

	switch (ops->alternate) {
	case SNOR_ALT:
		partition_size = spi_flash->mem_size;
		partition_offset = 0;
		break;
	case CACHE_ALT:
		partition_size = CACHE_PARTITION_SIZE;
		partition_offset = CACHE_PARTITION_ADRESS;
		break;
	default:
		partition_size = 0;
		partition_offset = 0;
		break;
	}

	if (address >= partition_size) {
		ops->state = dfuIDLE;
		ops->len = 0;
		return;
	}

	ops->len = partition_size - address;
	if (ops->len >= PAGE_SIZE){
		ops->len = PAGE_SIZE;
		ops->state = dfuUPLOAD_IDLE;
	} else {
		// short frame
		ops->state = dfuIDLE;
	}

	address += partition_offset;

	ret =
	    spi_flash_read_byte(spi_flash, address, ops->len, &retlen,
				ops->data);

	if (ret != DRV_RC_OK) {
		ops->state = dfuERROR;
		ops->status = errUNKNOWN;
		ops->len = 0;
	}
}

void dfu_spi_flash_write(struct dfu_ops *ops)
{
	usb_device_request_t *setup_packet = ops->device_request;
	unsigned int retlen;
	DRIVER_API_RC ret;
	unsigned int address = UGETW(setup_packet->wValue) * PAGE_SIZE;
	unsigned int partition_size;
	unsigned int partition_offset;


	spi_flash_info_t *spi_flash = get_spi_flash();

	switch (ops->alternate) {
	case SNOR_ALT:
		partition_size = spi_flash->mem_size;
		partition_offset = 0;
		break;
	case CACHE_ALT:
		partition_size = CACHE_PARTITION_SIZE;
		partition_offset = CACHE_PARTITION_ADRESS;
		break;
	default:
		partition_size = 0;
		partition_offset = 0;
		break;
	}

	if (address > partition_size - PAGE_SIZE) {
		ops->state = dfuERROR;
		ops->status = errADDRESS;
		ops->len = 0;
		return;
	}
	address += partition_offset;

	if (address % spi_flash->sector_size == 0) {
		uint32_t sector_nb =
		    1 + (PAGE_SIZE - 1) / spi_flash->sector_size;
		pr_debug("erase %d sector\n", sector_nb);
		ret =
		    spi_flash_sector_erase(spi_flash,
					   address / spi_flash->sector_size,
					   sector_nb);
		if (ret != DRV_RC_OK) {
			ops->state = dfuERROR;
			ops->status = errERASE;
			return;
		}
	}
	ret =
	    spi_flash_write_byte(spi_flash, address, ops->len, &retlen,
				 ops->data);
	if (ret != DRV_RC_OK) {
		ops->state = dfuERROR;
		ops->status = errWRITE;
		return;
	}
	ops->state = dfuDNLOAD_IDLE;
}
