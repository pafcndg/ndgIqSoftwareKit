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

#ifndef __CTB_PARTITION_H__
#define __CTB_PARTITION_H__

#include "project_mapping.h"

#define PAGE_SIZE 2048
#define BASE_FLASH_ADDR 0x40000000
#define VERSION_HEADER_SIZE 0x30

#define ROM_PAGE_START 0
#define ROM_PAGE_NR 4
#define ROM_PAGE_START_ADDR 0xffffe000
#define BOOTSTRAP_FLASH_START_ADDR 0xffffe400

#define BOOT_PAGE_START 0
#define BOOT_PAGE_NR 30
#define BOOT_FLASH_START_ADDR (BASE_FLASH_ADDR + (BOOT_PAGE_START * PAGE_SIZE))
#define BOOT_RAM_START_ADDR 0xA8000400
#define BOOT_FLASH_SIZE (BOOT_PAGE_NR * PAGE_SIZE)
#define BOOT_RAM_SIZE (81920 - 1024)

#define CACHE_PARTITION_ADRESS    SPI_FOTA_START_BLOCK * 4096
#define CACHE_PARTITION_SIZE      SPI_FOTA_NB_BLOCKS * 4096

#define OTA_PARTITION_ADRESS 0

#define BLE_CORE_FLASH_SIZE (262144)
#define BLE_CORE_FLASH_BLOCK_NBR 128
/* FOTA must preserve the blocks from index 246 that contain pairing info*/
#define BLE_CORE_FOTA_BLOCK_NBR  122

#define BLE_PART_DEVICE_MANAGER_START   246
#define BLE_PART_DEVICE_MANAGER_SIZE    1

/** USB Driver static data shared between
 * QRK Bootloader and QRK App
 * NOTE: Must match device/quark_se/common/include/quark_se_mapping.h
 */
#define BOOTLOADER_SHARED_DATA  0xa8000400

#endif /*__CTB_PARTITION_H__ */
