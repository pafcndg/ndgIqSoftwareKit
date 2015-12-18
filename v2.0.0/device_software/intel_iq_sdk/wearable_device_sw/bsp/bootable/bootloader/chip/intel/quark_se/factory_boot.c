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

#include <partition.h>
#include <bootlogic.h>
#include <mtd/spi_flash.h>

spi_flash_info_t *get_spi_flash(void);

void boot_factory(enum wake_sources wake_source,
			 enum reset_reasons reset_reason,
			 enum boot_targets boot_target)
{
	/* Non persistent factory data */
	soc_flash_block_erase(FACTORY_RESET_NON_PERSISTENT_START_BLOCK,
			      FACTORY_RESET_PERSISTENT_NB_BLOCKS);
	/* Application data */
	soc_flash_block_erase(APPLICATION_DATA_START_BLOCK,
			      APPLICATION_DATA_NB_BLOCKS);
	/* ble data */
	swd_init();
	swd_page_erase(BLE_PART_DEVICE_MANAGER_START);
#if defined(CONFIG_SPI_FLASH)
	/* erase spi flash partitions */
	spi_flash_info_t *spi_flash = get_spi_flash();
	/* Fota partition */
	spi_flash_sector_erase(spi_flash, SPI_FOTA_START_BLOCK, SPI_FOTA_NB_BLOCKS);
	/* Activity data partition */
	spi_flash_sector_erase(spi_flash, SPI_APPLICATION_DATA_START_BLOCK, SPI_APPLICATION_DATA_NB_BLOCKS);
	/* System Event partition */
	spi_flash_sector_erase(spi_flash, SPI_SYSTEM_EVENT_START_BLOCK, SPI_SYSTEM_EVENT_NB_BLOCKS);
#endif

	swd_debug_mode_reset_to_normal();
	boot_main(wake_source, reset_reason, TARGET_FACTORY);
}
