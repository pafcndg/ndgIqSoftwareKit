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

#include "drivers/soc_flash.h"
#include "drivers/soc_rom.h"
#include "util/cunit_test.h"

#include "machine.h"
#include "storage.h"
#include "project_mapping.h"

#define TST_BLOCK_SIZE  2048

#define TST_LEN         30

#define TST_BLOCK       (FACTORY_RESET_PERSISTENT_START_BLOCK)
#define TST_ADDRESS     (TST_BLOCK * TST_BLOCK_SIZE)

#define TST_ROM_ADDRESS     (0x400-4*10)
#define TST_ROM_BLOCK       (TST_ROM_ADDRESS/TST_BLOCK_SIZE)


#define FLASH_CU_ASSERT(cdt, msg, ...) \
    do { CU_ASSERT("", (cdt)); \
    if(!(cdt)) { \
        cu_print(msg "\n", __VA_ARGS__); \
        return DRV_RC_FAIL; } \
    } while(0)

#ifdef DEBUG
static void print_buffer(uint32_t *data, unsigned int len)
{
    unsigned int i, j;

    for(i=0; i<(len>>3)+1; i++)
    {
        for(j=0; j<8; j++)
        {
            if(j+i*8 >= len) {
                break;
            }
            cu_print("%x ", data[j+i*8]);
        }
        cu_print("\n");
    }
}
#endif

static DRIVER_API_RC flash_test_flash()
{
    uint32_t data[TST_LEN+10], data_read[TST_LEN+10];
    unsigned int retlen, i;
    DRIVER_API_RC ret;

    for(i=0; i<TST_LEN; i++) {
        data[i] = i+3;
    }

    data[0] = 0xdeadface;
    data[1] = 0xface1010;
    data[2] = 0xabcdef12;

    // Erase page
    ret = soc_flash_block_erase(TST_BLOCK, 1);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash erase failed (%d)", ret);
    // Write new data
    ret = soc_flash_write(TST_ADDRESS+5*4, TST_LEN, &retlen, data);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash write failed (%d), %d dword written", ret, retlen);
    FLASH_CU_ASSERT(retlen == TST_LEN, "Flash write bytes does not match (%d/%d)", retlen, TST_LEN);
    // Read data
    ret = soc_flash_read(TST_ADDRESS, TST_LEN+10, &retlen, data_read);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash read failed (%d)", ret);
    FLASH_CU_ASSERT(retlen == (TST_LEN+10), "Flash read dword does not match (%d/%d)", retlen, TST_LEN+10);

#ifdef DEBUG
    print_buffer(data_read, TST_LEN+10);
#endif

    // Validate written data
    for(i=0; i<5; i++) {
        FLASH_CU_ASSERT(data_read[i] == ~(0), "Flash verify dword %d does not match (0x%x != 0x%x)",
                                            i, ~(0), data_read[i]);
    }
    for(i=5; i<TST_LEN+5; i++) {
        FLASH_CU_ASSERT(data_read[i] == data[i-5], "Flash verify dword %d does not match (0x%x != 0x%x)",
                                            i, data[i-5], data_read[i]);
    }
    for(i=TST_LEN+5; i<TST_LEN+10; i++) {
        FLASH_CU_ASSERT(data_read[i] == ~(0), "Flash verify dword %d does not match (0x%x != 0x%x)",
                                            i, ~(0), data_read[i]);
    }

    return DRV_RC_OK;
}

#ifdef COMPILE_ROM_DRIVER
static DRIVER_API_RC flash_test_rom()
{
    uint32_t data[TST_LEN+10], data_read[TST_LEN+10];
    unsigned int retlen, i;
    DRIVER_API_RC ret;

    for(i=0; i<TST_LEN; i++) {
        data[i] = i+3;
    }

    data[0] = 0xdeadface;
    data[1] = 0xface1010;
    data[2] = 0xabcdef12;

    // Erase page
    ret = soc_rom_block_erase(TST_ROM_BLOCK, 1);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "ROM erase failed (%d)", ret);
    // Write new data
    ret = soc_rom_write(TST_ROM_ADDRESS+5*4, TST_LEN, &retlen, data);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "ROM write failed (%d), %d dword written", ret, retlen);
    FLASH_CU_ASSERT(retlen == TST_LEN, "ROM write bytes does not match (%d/%d)", retlen, TST_LEN);
    // Read data
    ret = soc_rom_read(TST_ROM_ADDRESS, TST_LEN+10, &retlen, data_read);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "ROM read failed (%d)", ret);
    FLASH_CU_ASSERT(retlen == (TST_LEN+10), "ROM read dword does not match (%d/%d)", retlen, TST_LEN+10);

#ifdef DEBUG
    print_buffer(data_read, TST_LEN+10);
#endif

    // Validate written data
    for(i=0; i<5; i++) {
        FLASH_CU_ASSERT(data_read[i] == ~(0), "ROM verify dword %d does not match (0x%x != 0x%x)",
                                            i, ~(0), data_read[i]);
    }
    for(i=5; i<TST_LEN+5; i++) {
        FLASH_CU_ASSERT(data_read[i] == data[i-5], "ROM verify dword %d does not match (0x%x != 0x%x)",
                                            i, data[i-5], data_read[i]);
    }
    for(i=TST_LEN+5; i<TST_LEN+10; i++) {
        FLASH_CU_ASSERT(data_read[i] == ~(0), "ROM verify dword %d does not match (0x%x != 0x%x)",
                                            i, ~(0), data_read[i]);
    }

    return DRV_RC_OK;
}
#endif

void flash_test(void)
{
    DRIVER_API_RC ret;

    cu_print("##################################################\n");
    cu_print("# Purpose of soc flash tests: Erase, write, read #\n");
    cu_print("##################################################\n");
    cu_print("Do not use flash from 0x%x to 0x%x\n", TST_ADDRESS, TST_ADDRESS+(TST_LEN<<2));

    ret = flash_test_flash();
    CU_ASSERT("Test for flash driver failed", ret == DRV_RC_OK);

#ifdef COMPILE_ROM_DRIVER
    cu_print("Do not use ROM from 0x%x to 0x%x\n", TST_ROM_ADDRESS, TST_ROM_ADDRESS+(TST_LEN<<2));
    ret = flash_test_rom();
    CU_ASSERT("Test for ROM driver failed", ret == DRV_RC_OK);
#endif
}
