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

#include "drivers/soc_rom.h"
#include "machine.h"
#include "soc_flash_defs.h"

/*! ROM memory management structure */
typedef struct rom_info_struct
{
    uint32_t           addr_base;       /*!< base address of memory */
    uint32_t           reg_base;        /*!< base address of device register set */
    unsigned int       block_count;     /*!< Number of blocks in the memory */
    unsigned int       block_size;      /*!< Block size in bytes */
    unsigned int       mem_size;        /*!< Memory size in byte (It must be equal to block_count*block_size) */
    uint8_t            is_init;         /*!< Init state of memory */
} rom_info_t, *rom_info_pt;

static rom_info_t rom_dev = {.is_init = 0,
                             .reg_base = ROM_REG_BASE_ADDR,
                             .addr_base = ROM_BASE_ADDR,
                             .block_count = 4,
                             .block_size = 0x800,
                             .mem_size = 0x2000};

DRIVER_API_RC soc_rom_read(uint32_t address, unsigned int len, unsigned int *retlen, uint32_t *data)
{
    unsigned int i;
    // Clear LSB bits 0 and 1 to align address on 4 bytes
    address &= ~(3);

    *retlen = 0;

    if(len == 0) {
        return DRV_RC_INVALID_OPERATION;
    }
    if(((len<<2)+address) > rom_dev.mem_size) {
        return DRV_RC_OUT_OF_MEM;
    }

    // Validate READ protection
    if((address < (rom_dev.mem_size>>1)) && (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, CTRL) & CTRL_ROM_RD_DIS_L)) {
        return DRV_RC_READ_PROTECTED;
    }
    if(((address+(len<<2)) > (rom_dev.mem_size>>1)) && (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, CTRL) & CTRL_ROM_RD_DIS_U)) {
        return DRV_RC_READ_PROTECTED;
    }

    for(i=0; i<len; i++) {
        data[i] = MMIO_REG_VAL_FROM_BASE(rom_dev.addr_base, address+(i<<2));
    }

    *retlen = len;

    return DRV_RC_OK;
}

DRIVER_API_RC soc_rom_write(uint32_t address, unsigned int len, unsigned int *retlen, uint32_t *data)
{
    unsigned int i;
    uint32_t pc_addr = (uint32_t)__builtin_return_address(0);

    // Align address on dword
    address &= ~(3);

    // Check input parameters
    if(len == 0) {
        return DRV_RC_INVALID_OPERATION;
    }
    if(((len<<2)+address) > rom_dev.mem_size) {
        return DRV_RC_OUT_OF_MEM;
    }
    // Check write protection
    if(MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_ROM_PROG) {
        return DRV_RC_WRITE_PROTECTED;
    }
    // Validate PC
    if((pc_addr >= (rom_dev.addr_base+address)) && (pc_addr < (rom_dev.addr_base+address+(len<<2)))) {
        return DRV_RC_ERASE_PC;
    }
    // OTP protection
    if(address <= 3) {
        return DRV_RC_INVALID_OPERATION;
    }

    // Start write operation
    MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, ROM_WR_DATA) = data[0];
    MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, ROM_WR_CTRL) = ((address<<ROM_WR_CTRL_WR_ADDR_BIT_OFFSET) | ROM_WR_CTRL_WR_REQ);
    // Compute next address
    address += 4;
    // Wait until write operation is complete
    while( (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_WR_DONE) == 0);

    for(i=1; i<len; i++) {
        // Start write operation
        MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, ROM_WR_DATA) = data[i];
        MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, ROM_WR_CTRL) = ((address<<ROM_WR_CTRL_WR_ADDR_BIT_OFFSET) | ROM_WR_CTRL_WR_REQ);

        // Because write operation takes several cpu cycles to complete,
        // we verify previous written dword after the operation is started

        // Flush the pre-fetch buffer
        SET_MMIO_BIT((volatile uint32_t*)(rom_dev.reg_base+CTRL), CTRL_PRE_FLUSH_BIT);
        CLEAR_MMIO_BIT((volatile uint32_t*)(rom_dev.reg_base+CTRL), CTRL_PRE_FLUSH_BIT);

        // Validate previous write operation
        if(__builtin_expect((MMIO_REG_VAL_FROM_BASE(rom_dev.addr_base, address-4) != data[i-1]), 0)) {
            // Write error (bad block detected ?)
            // Wait until write operation is complete
            while( (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_WR_DONE) == 0);
            *retlen = i-1;
            return DRV_RC_CHECK_FAIL;
        }
        // Compute write address for next operation
        address += 4;
        // Wait for current write operation to complete
        while( (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_WR_DONE) == 0);
    }
    // Flush the pre-fetch buffer
    SET_MMIO_BIT((volatile uint32_t*)(rom_dev.reg_base+CTRL), CTRL_PRE_FLUSH_BIT);
    CLEAR_MMIO_BIT((volatile uint32_t*)(rom_dev.reg_base+CTRL), CTRL_PRE_FLUSH_BIT);

    // Validate previous write operation
    if(__builtin_expect((MMIO_REG_VAL_FROM_BASE(rom_dev.addr_base, address-4) != data[len-1]), 0)) {
        // Write error (bad block detected ?)
        *retlen = len-1;
        return DRV_RC_CHECK_FAIL;
    }

    *retlen = len;
    return DRV_RC_OK;
}

DRIVER_API_RC soc_rom_block_erase(unsigned int start_block, unsigned int block_count)
{
    unsigned int i;
    uint32_t pc_addr = (uint32_t)__builtin_return_address(0);

    // Check input parameters
    if(block_count == 0) {
        return DRV_RC_INVALID_OPERATION;
    }
    // Warning: possible overflow in parameters not tested:
    // if block_count is huge and creates an overflow,
    // then block_count+start_block can be smaller than rom_dev.block_count
    // => we may need to test start_block < rom_dev.block_count,
    // =>                     block_count < rom_dev.block_count,
    // =>         start_block+block_count < rom_dev.block_count
    if ((block_count + start_block) >= rom_dev.block_count) {
        return DRV_RC_OUT_OF_MEM;
    }
    // Check write protection
    if(MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_ROM_PROG) {
        return DRV_RC_WRITE_PROTECTED;
    }
    // Validate PC
    if((pc_addr >= (rom_dev.addr_base+start_block*rom_dev.block_size)) &&
       (pc_addr < (rom_dev.addr_base+(start_block+block_count)*rom_dev.block_size))) {
        return DRV_RC_ERASE_PC;
    }

    for(i=0; i<block_count; i++) {
        MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, ROM_WR_CTRL) =
            (((start_block*(rom_dev.block_size))<<ROM_WR_CTRL_WR_ADDR_BIT_OFFSET) | ROM_WR_CTRL_ER_REQ);
        /* Wait until erase operation is complete.*/
        while( (MMIO_REG_VAL_FROM_BASE(rom_dev.reg_base, FLASH_STTS) & FLASH_STTS_ER_DONE) == 0);

        start_block++;
    }

    return DRV_RC_OK;
}
