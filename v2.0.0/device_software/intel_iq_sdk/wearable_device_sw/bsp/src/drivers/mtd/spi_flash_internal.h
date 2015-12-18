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

#ifndef _SPI_FLASH_INTERNAL_
#define _SPI_FLASH_INTERNAL_

#include <stdint.h>
#include "infra/device.h"

struct spi_flash_info {
	uint32_t rdid;

	uint32_t flash_size;
	uint32_t page_size;
	uint32_t sector_size;
	uint32_t block_size;
	uint32_t large_block_size;

	uint32_t ms_sector_erase;
	uint32_t ms_block_erase;
	uint32_t ms_large_block_erase;
	uint32_t ms_chip_erase;
	uint32_t ms_max_erase;

	uint32_t cmd_release_deep_powerdown;
	uint32_t cmd_deep_powerdown;
	uint32_t cmd_read_id;
	uint32_t cmd_read_status;
	uint32_t cmd_read_security;
	uint32_t cmd_write_en;
	uint32_t cmd_page_program;
	uint32_t cmd_read;
	uint32_t cmd_sector_erase;
	uint32_t cmd_block_erase;
	uint32_t cmd_large_block_erase;
	uint32_t cmd_chip_erase;

	uint32_t status_wel_bit;
	uint32_t status_wip_bit;
	uint32_t status_secr_pfail_bit;
	uint32_t status_secr_efail_bit;
};

struct spi_flash_driver {
	struct driver drv; /*!< Generic SPI Flash driver init/suspend/resume */
	struct spi_flash_info info; /*!< Flash constant registers info */
};

/**
 *  Generic spi_flash_init function
 *
 *  @param  dev: spi flash device to use
 *
 *  @return 0 on success
 */
int spi_flash_init(struct device *device);

#endif /* _SPI_FLASH_INTERNAL_ */
