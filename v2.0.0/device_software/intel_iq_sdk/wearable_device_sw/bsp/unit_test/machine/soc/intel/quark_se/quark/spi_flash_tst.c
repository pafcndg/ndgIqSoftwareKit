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

#include <string.h> // For strcmp
#include "drivers/spi_flash.h"
#include "util/cunit_test.h"
#include "machine.h"

#define FLASH_DEVICE    "spi_flash0"
#define TST_BLOCK_SIZE  0x8000
#define TST_LEN         30
#define TST_ADDRESS     (0x8000-10*4)
#define TST_BLOCK       (TST_ADDRESS/TST_BLOCK_SIZE)

#define FLASH_CU_ASSERT(cdt, msg, ...) \
    do { CU_ASSERT("", (cdt)); \
    if(!(cdt)) { \
        cu_print(msg "\n", ## __VA_ARGS__); \
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

#define erase_test(type, start, count, ret_rc) \
    do {ret = spi_flash_ ## type ## _erase(spi_flash_handler, start, count); \
        CU_ASSERT("erase " #type " for item " #start " (" #count ") failed", ((DRIVER_API_RC)ret_rc == ret)); \
    } while(0)

static DRIVER_API_RC spi_flash_test_1(struct device *spi_flash_handler)
{
    uint32_t data[TST_LEN+10], data_read[TST_LEN+10] = {0};
    uint32_t rdid = 0;
    unsigned int retlen, i;
    DRIVER_API_RC ret;

    for(i=0; i<TST_LEN; i++) {
        data[i] = i;
    }

    data[0] = 0xdeadface;
    data[1] = 0xface1010;
    data[2] = 0xabcdef12;

    // Get flash RDID
    ret = spi_flash_get_rdid(spi_flash_handler, &rdid);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash get RDID failed (%d) 0x%x", ret, rdid);
    FLASH_CU_ASSERT(rdid == 0x003825c2, "Flash wrong RDID detected: 0x%x", rdid);

    erase_test(block, 510, 2, DRV_RC_OK);
    erase_test(block, 511, 1, DRV_RC_OK);
    erase_test(block, 511, 2, DRV_RC_OUT_OF_MEM);
    erase_test(block, 1000, 23, DRV_RC_OUT_OF_MEM);

    erase_test(sector, 4094, 2, DRV_RC_OK);
    erase_test(sector, 4095, 1, DRV_RC_OK);
    erase_test(sector, 4095, 2, DRV_RC_OUT_OF_MEM);
    erase_test(sector, 12255, 23, DRV_RC_OUT_OF_MEM);

    // Erase page
    ret = spi_flash_block_erase(spi_flash_handler, TST_BLOCK, 2);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash erase failed (%d)", ret);
    // Write new data
    ret = spi_flash_write(spi_flash_handler, TST_ADDRESS+5*4, TST_LEN, &retlen, data);
    FLASH_CU_ASSERT(ret == DRV_RC_OK, "Flash write failed (%d), %d dword written", ret, retlen);
    FLASH_CU_ASSERT(retlen == TST_LEN, "Flash write bytes does not match (%d/%d)", retlen, TST_LEN);
    // Read data
    ret = spi_flash_read(spi_flash_handler, TST_ADDRESS, TST_LEN+10, &retlen, data_read);
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

#define FLASH_CU_ASSERT_NORET(cdt, msg, ...) \
    do { CU_ASSERT("", (cdt)); \
    if(!(cdt)) { \
        cu_print(msg "\n", ## __VA_ARGS__); \
        return; } \
    } while(0)

void spi_flash_test(void)
{
    DRIVER_API_RC ret;

    cu_print("##################################################\n");
    cu_print("# Purpose of spi flash tests: Erase, write, read #\n");
    cu_print("##################################################\n");
    cu_print("Do not use flash from 0x%x to 0x%x\n", TST_ADDRESS, TST_ADDRESS+(TST_LEN<<2));

    struct device *spi_flash_handler = (struct device *)&pf_sba_device_flash_spi0;
    // Run spi flash test
    ret = spi_flash_test_1(spi_flash_handler);
    CU_ASSERT("Test for spi flash driver failed", ret == DRV_RC_OK);
}
