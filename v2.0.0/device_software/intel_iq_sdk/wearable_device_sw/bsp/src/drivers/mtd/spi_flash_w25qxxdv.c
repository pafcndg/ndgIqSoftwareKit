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

#include "spi_flash_w25qxxdv.h"

#include "spi_flash_internal.h"

const struct spi_flash_driver spi_flash_w25qxxdv_driver = {
	.drv.init = spi_flash_init,
	.info = {
		.rdid = FLASH_RDID_VALUE,

		.flash_size = FLASH_SIZE,
		.page_size = FLASH_PAGE_SIZE,
		.sector_size = FLASH_SECTOR_SIZE,
		.block_size = FLASH_BLOCK32K_SIZE,
		.large_block_size = FLASH_BLOCK_SIZE,

		.ms_sector_erase = FLASH_SECTOR_ERASE_MS,
		.ms_block_erase = FLASH_BLOCK_ERASE_MS,
		.ms_large_block_erase = FLASH_LARGE_BLOCK_ERASE_MS,
		.ms_chip_erase = FLASH_CHIP_ERASE_MS,
		.ms_max_erase = FLASH_MAX_ERASE_MS,

		.cmd_release_deep_powerdown = FLASH_CMD_RDP,
		.cmd_deep_powerdown = FLASH_CMD_DP,
		.cmd_read_id = FLASH_CMD_RDID,
		.cmd_read_status = FLASH_CMD_RDSR,
		.cmd_read_security = FLASH_CMD_RDSCUR,
		.cmd_write_en = FLASH_CMD_WREN,
		.cmd_page_program = FLASH_CMD_PP,
		.cmd_read = FLASH_CMD_READ,
		.cmd_sector_erase = FLASH_CMD_SE,
		.cmd_block_erase = FLASH_CMD_BE32K,
		.cmd_large_block_erase = FLASH_CMD_BE,
		.cmd_chip_erase = FLASH_CMD_CE,

		.status_wel_bit = FLASH_WEL_BIT,
		.status_wip_bit = FLASH_WIP_BIT,
		.status_secr_pfail_bit = FLASH_SECR_PFAIL_BIT,
		.status_secr_efail_bit = FLASH_SECR_EFAIL_BIT,
	},
};
