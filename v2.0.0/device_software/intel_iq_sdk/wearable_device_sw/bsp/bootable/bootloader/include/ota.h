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
#include "utils.h"
#include "sign.h"

enum OTA_TYPE {
	ARC = 0,
	QUARK = 1,
	BLE_CORE = 2,
	BOOTLOADER = 3,
	OTA_TYPE_MAX = 0xFF
};

struct ota_binary_header {
	uint8_t magic[3];
	uint8_t type;
	uint32_t version;
	uint32_t offset;
	uint32_t length;
};

struct ota_header {
	uint8_t magic[3];
	uint8_t hdr_version;
	uint16_t hdr_length;
	uint16_t platform;
	uint32_t crc;
	uint32_t pl_length;
	uint32_t build_version;
	uint32_t min_version;
	uint32_t app_min_version;
	struct ota_binary_header binaries[OTA_TYPE_MAX];
};

struct ota {
	struct signature_header sig;
	struct ota_header header;
	const uint8_t *payload;
	uint32_t progress;
};

struct ota_ops {
	void (*erase)(uint32_t address);
	uint32_t (*write)(unsigned char *address, uint32_t size, uint32_t *value);
	uint32_t (*read)(unsigned char *address, uint32_t size, uint32_t *value);
};

enum OTA_ERROR {
	OTA_SUCCESS = 0x00,
	OTA_SIG_HEADER_ERROR,
	OTA_HEADER_ERROR,
	OTA_PAYLOAD_ERROR,
	OTA_SIG_ERROR,
	OTA_HASH_ERROR,
	OTA_WRITE_ERROR,
	OTA_READ_ERROR,
	OTA_LZG_ERROR,
	OTA_PART_TOO_SMALL_ERROR,
	OTA_BINARY_TYPE_ERROR,
	OTA_SOFTDEVICE_ERROR,
	OTA_UICR_ERROR,
	OTA_SETTINGS_ERROR,
	OTA_APPLICATION_ERROR,
	OTA_BOOTLOADER_ERROR,
	OTA_FAILURE = 0xFF
};

enum OTA_PROGRESS {
	OTA_STARTED = 0x00,
	OTA_SIG_HEADER_CHECK_STARTED,
	OTA_SIG_HEADER_CHECK_DONE,
	OTA_HEADER_CHECK_STARTED,
	OTA_HEADER_CHECK_DONE,
	OTA_PAYLOAD_CHECK_STARTED,
	OTA_PAYLOAD_CHECK_DONE,
	OTA_ARC_STARTED,
	OTA_ARC_DONE,
	OTA_QUARK_STARTED,
	OTA_QUARK_DONE,
	OTA_BLE_STARTED,
	OTA_BLE_DONE,
	OTA_BOOTLOADER_STARTED,
	OTA_BOOTLOADER_DONE,
	OTA_RECOVERY_STARTED,
	OTA_RECOVERY_DONE,
	OTA_ABORTED,
	OTA_DONE = 0xFF
};

uint32_t ota_start(void);
uint32_t flash_part(struct ota *ota, uint32_t to, uint32_t size,
		    uint32_t index, uint32_t type);

uint32_t ota_flash_all(struct ota *ota);
uint32_t ota_get_cache_offset(struct ota *ota);
