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

#include "ota.h"
#include "quark_se/board_intel.h"
#include "quark_se/boot_config.h"
#include "utils.h"
#include "lzgdecode.h"
 
extern struct ota_ops ota_ops;

static uint32_t ota_erase_page_swd(uint32_t address)
{
	/* The Nordic pages are actually two kbytes long */
	swd_page_erase(address*2);
	swd_page_erase(address*2+1);
}

static uint32_t ota_erase_page_soc(uint32_t address)
{
	unsigned int count = 1;
	soc_flash_block_erase(address, count);
}

static uint32_t flash_arc(struct ota *ota, uint32_t index)
{
	uint32_t err_code;

	ota_ops.write = LZG_write_soc;
	ota_ops.read = LZG_read_soc;
	ota_ops.erase = ota_erase_page_soc;
	ota->progress = OTA_ARC_STARTED;
	err_code = flash_part(ota, ARC_START_PAGE, ARC_NB_PAGE, index, ARC);
	ota->progress = OTA_ARC_DONE;

	return err_code;
}

static uint32_t flash_quark(struct ota *ota, uint32_t index)
{
	uint32_t err_code;

	ota_ops.write = LZG_write_soc;
	ota_ops.read = LZG_read_soc;
	ota_ops.erase = ota_erase_page_soc;
	ota->progress = OTA_QUARK_STARTED;
	err_code = flash_part(ota, QUARK_START_PAGE, QUARK_NB_PAGE,
			      index, QUARK);
	ota->progress = OTA_QUARK_DONE;

	return err_code;
}

static uint32_t flash_ble_core(struct ota *ota, uint32_t index)
{
	uint32_t err_code;
	ota_ops.write = LZG_write_swd;
	ota_ops.erase = ota_erase_page_swd;
	ota_ops.read = LZG_read_swd;
	swd_init();
	ota->progress = OTA_BLE_STARTED;
	flash_part(ota, 0, BLE_CORE_FOTA_BLOCK_NBR, index, BLE_CORE);
	ota->progress = OTA_BLE_DONE;

	return err_code;
}

static uint32_t flash_bootloader(struct ota *ota, uint32_t index)
{
	uint32_t err_code;
	ota_ops.write = LZG_write_soc;
	ota_ops.read = LZG_read_soc;
	ota_ops.erase = ota_erase_page_soc;
	ota->progress = OTA_BOOTLOADER_STARTED;
	err_code = flash_part(ota, BOOT_PAGE_START, BOOT_PAGE_NR,
			      index, BOOTLOADER);
	ota->progress = OTA_BOOTLOADER_DONE;
	return err_code;
}

uint32_t ota_flash_all(struct ota * ota)
{
	uint32_t err_code = OTA_FAILURE;
	uint32_t i, j;

	const struct {
		uint32_t(*do_ota) (struct ota * ota, uint32_t index);
		uint8_t magic[3];
	} do_ota[] = {
		[ARC] = {flash_arc, "ARC"},
		[QUARK] = {flash_quark, "QRK"},
		[BLE_CORE] = {flash_ble_core,"BLE"},
#if defined(CONFIG_FLASH_BOOTLOADER)
		[BOOTLOADER] = {flash_bootloader, "BLR"}
#endif
		};

	for (i = 0; i < ARRAY_SIZE(ota->header.binaries); i++) {
		struct ota_binary_header *p_bhdr = &ota->header.binaries[i];
		if (p_bhdr->length == 0)
			continue;

		for (j = 0; j < ARRAY_SIZE(do_ota); j++) {
			if (!memcmp(p_bhdr->magic, do_ota[j].magic,
				    sizeof(p_bhdr->magic))) {
				err_code = do_ota[j].do_ota(ota, i);
				break;
			}
		}
		if (err_code)
			break;
	}
	return err_code;
}

uint32_t ota_get_cache_offset(struct ota * ota)
{
	return OTA_PARTITION_ADRESS;
}
