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

#include <stdint.h>
#include "ota.h"
#include "quark_se/boot_config.h"
#include "mtd/spi_flash.h"
#include "quark_se/board_intel.h"
#include <dfu_spi_flash.h>
#include "crc.h"
#include <sign.h>
#include <sha256.h>
/*
 * liblzg header for _LZG_CalcChecksum definition
 */
#include "internal.h"

#define memset __builtin_memset

#if defined(CONFIG_SECURE_UPDATE)
	#define OTA_HEADER_OFFSET CONFIG_SIGNATURE_HEADER_SIZE
#else
	#define OTA_HEADER_OFFSET 0
#endif

struct ota_ops ota_ops = {
	.erase = NULL,
	.write = NULL,
	.read = NULL ,
};


static uint32_t ota_read_bytes(uint32_t where, uint8_t * data, uint32_t size)
{
	unsigned int retlen;
	DRIVER_API_RC ret;
	spi_flash_info_t *spi_flash = get_spi_flash();

#ifdef CONFIG_WATCHDOG
	WATCHDOG_RESET();
#endif
	ret = spi_flash_read_byte(spi_flash, where, size, &retlen, data);
	if (ret != DRV_RC_OK) {
		return OTA_READ_ERROR;
	}
	return OTA_SUCCESS;
}

static uint32_t ota_erase_page(uint32_t where)
{
#ifdef CONFIG_WATCHDOG
	WATCHDOG_RESET();
#endif
	if (ota_ops.erase) {
		ota_ops.erase(where);
	}
	return OTA_SUCCESS;
}

static uint32_t ota_erase_partition(uint32_t where, uint32_t size)
{
	uint32_t i;

	for (i = 0; i < size; i++)
		ota_erase_page(where + i);

	return OTA_SUCCESS;
}

static uint32_t ota_abort(struct ota *ota)
{
	ota->progress = OTA_ABORTED;
	return ota->progress;
}

static uint32_t ota_done(struct ota *ota)
{
	ota->progress = OTA_DONE;
	return OTA_SUCCESS;
}

static uint32_t ota_get_header(struct ota *ota)
{
	uint32_t err_code;

	ota->progress = OTA_HEADER_CHECK_STARTED;
#if defined(CONFIG_SECURE_UPDATE)
	err_code = ota_read_bytes(ota_get_cache_offset(ota) * ERASE_PAGE_SIZE,
				  (uint8_t *) & (ota->sig),
				  sizeof(ota->sig));
	if (err_code)
		return err_code;
#endif
	err_code = ota_read_bytes(ota_get_cache_offset(ota) * ERASE_PAGE_SIZE
				  + OTA_HEADER_OFFSET,
				  (uint8_t *) & (ota->header),
				  sizeof(ota->header));
	if (err_code)
		return err_code;

	if (memcmp(ota->header.magic, "OTA", sizeof(ota->header.magic)) != 0) {

		return OTA_HEADER_ERROR;
	}
	ota->progress = OTA_HEADER_CHECK_DONE;

	return OTA_SUCCESS;
}

#if defined(CONFIG_SECURE_UPDATE)
static uint32_t ota_package_verify(struct ota *ota)
{
	uint32_t err_code;
	uint32_t offset;
	uint8_t buffer[32];

	sha256_context_t c __attribute__((aligned(4)));
	unsigned char md[SHA256_DIGEST_LENGTH] __attribute__((aligned(4)));
	size_t len;

	sha256_init(&c);
	len = sizeof(buffer);

	for (offset = 0; offset < ota->sig.size; offset += sizeof(buffer)) {
		if (offset + len >= ota->sig.size)
		{
			len = ota->sig.size - offset;
		}
		err_code = ota_read_bytes(ota_get_cache_offset(ota) * ERASE_PAGE_SIZE
					+ OTA_HEADER_OFFSET + offset,
					buffer, len);
		if (err_code)
			return err_code;
		sha256_update(&c, buffer, len);
	}
	sha256_final(md, &c);

	uint8_t *buf;

	buf = (uint8_t *) (ota_get_cache_offset(ota) * ERASE_PAGE_SIZE + OTA_HEADER_OFFSET);
	ota->payload = buf + ota->header.hdr_length;

	return !secure_update(& (ota->sig), md);
}
#else

static uint32_t ota_read_byte(uint32_t addr, uint8_t * data)
{
	return ota_read_bytes(addr, data, 1);
}

static uint32_t ota_package_verify(struct ota *ota)
{
	uint32_t err_code = OTA_HASH_ERROR;
	uint32_t crc = 0, dummy_data = 0;
	uint8_t *buf;

	ota->progress = OTA_PAYLOAD_CHECK_STARTED;

	buf = (uint8_t *) (ota_get_cache_offset(ota) * ERASE_PAGE_SIZE);
	ota->payload = buf + ota->header.hdr_length;

	/* CRC is computed with initial 0 value in header */
	crc = calc_crc32(crc, buf, offsetof(struct ota_header, crc),
			ota_read_byte);
	crc = calc_crc32(crc, &dummy_data, sizeof(dummy_data), NULL);
	crc = calc_crc32(crc, buf + offsetof(struct ota_header, pl_length),
			 ota->header.hdr_length + ota->header.pl_length -
			 offsetof(struct ota_header, pl_length),
			 ota_read_byte);

	if (crc == ota->header.crc) {
		err_code = OTA_SUCCESS;
	}

	ota->progress = OTA_PAYLOAD_CHECK_DONE;
	return err_code;
}
#endif

__weak uint32_t ota_get_cache_offset(struct ota * ota)
{
	return 0;
}

__weak uint32_t ota_flash_all(struct ota * ota)
{
	return 0;
}

uint32_t flash_part(struct ota * ota, uint32_t to, uint32_t size,
		    uint32_t index, uint32_t type)
{
	const uint8_t *r_offset;
	uint8_t *w_offset;
	uint32_t w_size;
	uint32_t r_size;
	uint32_t success;

	r_offset = ota->payload + ota->header.binaries[index].offset;

	w_offset = (uint8_t *) (to * PAGE_SIZE);

	r_size = ota->header.binaries[index].length;
	w_size = size * PAGE_SIZE;

	if (w_size < r_size)
		return OTA_PART_TOO_SMALL_ERROR;

	if (ota->header.binaries[index].type != type)
		return OTA_BINARY_TYPE_ERROR;

	ota_erase_partition(to, size);
	success = LZG_Decode(r_offset, r_size, w_offset, w_size);
	if (success)
		return OTA_SUCCESS;

	return OTA_LZG_ERROR;
}

uint32_t ota_start(void)
{
	uint32_t err_code;
	struct ota ota;

	memset(&ota, 0, sizeof(ota));

	ota.progress = OTA_STARTED;

	err_code = ota_get_header(&ota);
	if (err_code)
		goto abort;
	err_code = ota_package_verify(&ota);
	if (err_code)
		goto abort;
	err_code = ota_flash_all(&ota);
	if (err_code)
		goto abort;
	return ota_done(&ota);
abort:
	ota_abort(&ota);
	return err_code;
}
