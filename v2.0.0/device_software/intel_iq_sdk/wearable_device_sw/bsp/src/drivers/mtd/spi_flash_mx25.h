/* INTEL CONFIDENTIAL Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential information
 * of Intel or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 *
 ******************************************************************************/

#ifndef HW_FLASH_DEFS_H
#define HW_FLASH_DEFS_H

/* Status Registers
 *    S7     S6     S5     S4     S3     S2     S1     S0
 *  +-------------------------------------------------------+
 *  | SRP0 | SEC  |  TB  | BP2  | BP1  | BP0  | WEL  | BUSY |
 *  +-------------------------------------------------------+
 *
 * BUSY - Erase/Write In Progress - 1 device is executing a command, 0 ready for command
 * WEL  - Write Enable Latch - 1 write enable is received, 0 completeion of a Write Disable, Page Program, Erase, Write Status Register
 *
 *    S15    S14    S13    S12    S11    S10    S9     S8
 *  +-------------------------------------------------------+
 *  | SUS  | CMP  | LB3  | LB2  | LB1  | xxx  | QE   | SRP1 |
 *  +-------------------------------------------------------+
 *
 *    S23        S22    S21    S20    S19    S18    S17    S16
 *  +----------------------------------------------------------+
 *  | HOLD/RST | DRV1 | DRV0 | xxx  | xxx  | WPS  | xxx  | xxx |
 *  +----------------------------------------------------------+
 */

// relevant status register bits
#define FLASH_WIP_BIT         (0x01)      // ***YYY Write-In-Progress bit
#define FLASH_WEL_BIT         (0x02)      // ***YYY Write-Enable-Latch bit
#define FLASH_SRWD_BIT        (0x80)      // ***YYY Status-Register, Write-Protect bit
#define FLASH_TB_BIT          (0x8)       // Top Bottom (T/B) bit
#define FLASH_SR_BP_OFFSET    (2)         // Status-Register, Block-Protection bits offset.
                                             // BP0 is at bit 2 in the status register
// relevant configuration register bits

// relevant security register bits
#define FLASH_SECR_WPSEL_BIT  (0x80)       // Security-Register, Write-Protection type bit
#define FLASH_SECR_EFAIL_BIT  (0x40)       // Security-Register, Erase Fail bit
#define FLASH_SECR_PFAIL_BIT  (0x20)       // Security-Register, Program Fail bit

#define FLASH_PAGE_SIZE       (0x100)     // page size in units of bytes (256)
#define FLASH_SECTOR_SIZE     (0x1000)    // sector size in units of bytes (4096)
#define FLASH_BLOCK32K_SIZE   (0x8000)    // block size in units of bytes (32768)
#define FLASH_BLOCK_SIZE      (0x10000)   // block size in units of bytes (65536)

// nominal operation timings (see p.94 of datasheet)
#define FLASH_SECTOR_ERASE_MS       (35)
#define FLASH_BLOCK_ERASE_MS        (200)
#define FLASH_LARGE_BLOCK_ERASE_MS  (350)
#define FLASH_CHIP_ERASE_MS         (2000) // ?? Value not found in datasheet
#define FLASH_MAX_ERASE_MS          (2000) // Maximum timeout for an erase operation

//   "*"      means command may be used in future code
//   "***"    means command is currently used in the code.
//   "***YYY" means command is used and is the same for both MXIC and Winbond flash.
//   "***XXX" means command is used but different for MXIC and Winbond flash

//ID comands
#define FLASH_CMD_RDID        0x9F        // ***YYY RDID (Read Identification)
#define FLASH_CMD_RES         0xAB        // RES (Read Electronic ID)
#define FLASH_CMD_REMS        0x90        // REMS (Read Electronic & Device ID)
#define FLASH_CMD_QPIID       0xAF        // QPIID (QPI ID Read)

//Register comands
#define FLASH_CMD_WRSR        0x01        // ***YYY WRSR (Write Status Register)
#define FLASH_CMD_RDSR        0x05        // ***YYY RDSR (Read Status Register)
#define FLASH_CMD_WRSCUR      0x2F        // * WRSCUR (Write Security Register)
#define FLASH_CMD_RDSCUR      0x2B        // ***XXX RDSCUR (Read Security Register)
#define FLASH_CMD_RDCR        0x15        // * RDCR (Read Configuration Register)
#define FLASH_CMD_RDFBR       0x16        // RDFBR (read fast boot Register)
#define FLASH_CMD_WRFBR       0x17        // WRFBR (write fast boot Register)
#define FLASH_CMD_ESFBR       0x18        // ESFBR (erase fast boot Register)
#define FLASH_CMD_WRLR        0x2C        // WRLR (write lock Register)
#define FLASH_CMD_RDLR        0x2D        // RDLR (read lock Register)
#define FLASH_CMD_RDSPB       0xE2        // RDSPB (read SPB status)
#define FLASH_CMD_WRSPB       0xE3        // WRSPB (write SPB bit)
#define FLASH_CMD_ESSPB       0xE4        // ESSPB (erase all SPB status)
#define FLASH_CMD_SPBLK       0xA6        // SPBLK (SPB lock set)
#define FLASH_CMD_RDSPBLK     0xA7        // SPBLK (read SPB lock register)
#define FLASH_CMD_WRPASS      0x28        // WRPASS (write password Register)
#define FLASH_CMD_RDPASS      0x27        // RDPASS (read password Register)
#define FLASH_CMD_PASSULK     0x29        // RDPASS (password unlock)
#define FLASH_CMD_RDDPB       0xE0        // RDDPB (read DPB register)
#define FLASH_CMD_WRDPB       0xE1        // WRDPB (write DPB register)

//READ comands
#define FLASH_CMD_READ        0x03        // ***YYY READ (1 x I/O)
#define FLASH_CMD_2READ       0xBB        // 2READ (2 x I/O)
#define FLASH_CMD_4READ       0xEB        // 4READ (4 x I/O)
#define FLASH_CMD_FASTREAD    0x0B        // FAST READ (Fast read data).
#define FLASH_CMD_DREAD       0x3B        // DREAD (1In/2 Out fast read)
#define FLASH_CMD_QREAD       0x6B        // QREAD (1In/4 Out fast read)
#define FLASH_CMD_RDSFDP      0x5A        // RDSFDP (Read SFDP)

//Program comands
#define FLASH_CMD_WREN        0x06        // ***YYY WREN (Write Enable)
#define FLASH_CMD_WRDI        0x04        // WRDI (Write Disable)
#define FLASH_CMD_PP          0x02        // ***YYY PP (page program)
#define FLASH_CMD_4PP         0x38        // 4PP (Quad page program)

//Erase comands
#define FLASH_CMD_SE          0x20        // ***YYY SE (Sector Erase)
#define FLASH_CMD_BE32K       0x52        // BE32K (Block Erase 32kb)
#define FLASH_CMD_BE          0xD8        // ***YYY BE (Block Erase)
#define FLASH_CMD_CE          0x60        // CE (Chip Erase) hex code: 60 or C7

//Mode setting comands
#define FLASH_CMD_DP          0xB9        // ***YYY DP (Deep Power Down)
#define FLASH_CMD_RDP         0xAB        // ***YYY RDP (Release from Deep Power Down)
#define FLASH_CMD_ENSO        0xB1        // ENSO (Enter Secured OTP)
#define FLASH_CMD_EXSO        0xC1        // EXSO (Exit Secured OTP)
#define FLASH_CMD_EQIO        0x35        // EQIO (Enable Quad I/O)
#define FLASH_CMD_WPSEL       0x68        // WPSEL (Enable block protect mode)

//Reset comands
#define FLASH_CMD_RSTEN       0x66        // RSTEN (Reset Enable)
#define FLASH_CMD_RST         0x99        // RST (Reset Memory)
#define FLASH_CMD_RSTQIO      0xF5        // RSTQIO (Reset Quad I/O)

//Security comands
#define FLASH_CMD_GBLK        0x7E        // GBLK (Gang Block Lock)
#define FLASH_CMD_GBULK       0x98        // GBULK (Gang Block Unlock)

//Suspend/Resume comands
#ifdef PGM_ERS_0xB0
#define FLASH_CMD_PGM_ERS_S   0xB0        // PGM/ERS Suspend (Suspends Program/Erase) old: 0xB0
#define FLASH_CMD_PGM_ERS_R   0x30        // PGM/ERS Erase (Resumes Program/Erase) old: 0x30
#else
#define FLASH_CMD_PGM_ERS_S   0x75        // PGM/ERS Suspend (Suspends Program/Erase) old: 0xB0
#define FLASH_CMD_PGM_ERS_R   0x7A        // PGM/ERS Erase (Resumes Program/Erase) old: 0x30
#endif

#define FLASH_CMD_NOP         0x00        // NOP (No Operation)

#endif
