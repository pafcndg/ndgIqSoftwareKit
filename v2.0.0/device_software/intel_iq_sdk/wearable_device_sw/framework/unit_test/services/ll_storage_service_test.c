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

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_messages.h"
#include "machine.h"
#include "storage.h"
#include "services/service_tests.h"
#include "services/ll_storage_service/ll_storage_service.h"
#include "services/ll_storage_service/ll_storage_service_api.h"
#include "drivers/data_type.h"
#include "project_mapping.h"

#define TST_DATA_LEN         29
#define TST_BLOCK_LEN        1
#define TST_BLOCK_START      0
#define DATA_LEN_4B          ((TST_BLOCK_LEN * EMBEDDED_FLASH_BLOCK_SIZE) / sizeof (uint32_t))
#define SPI_DATA_LEN_4B      ((TST_BLOCK_LEN * SERIAL_FLASH_BLOCK_SIZE) / sizeof (uint32_t))

static bool ll_storage_init_done = false;
static cfw_client_t * ll_storage_client;
static cfw_service_conn_t *ll_storage_service_conn = NULL;

static uint32_t blks_written = 0;
static uint32_t actual_written = 0;
static uint32_t blks_read = 0;
static uint32_t actual_read = 0;
static bool blk_erase = false;
static bool part_erase = false;
static bool spi_test = false;
static bool failed_resp = false;

#ifdef DEBUG
static void print_buffer(uint32_t *data, uint32_t len)
{
    uint32_t i;

    for(i=0; i < (len / sizeof(uint32_t)); i++)
    {
        if (!(i % 8))
            cu_print("\n");
        cu_print("%x ", data[i]);
    }
    if (len % sizeof (uint32_t)) {
        uint32_t mask = 0xffffffff << (len % sizeof (uint32_t));
        cu_print("%x ",data[i] & ~(mask));
    }
    cu_print("\n");
}
#endif

static bool check_read_buffer(uint32_t * buffer, uint32_t len)
{
    uint32_t i;

    if ((buffer[0] != 0xdeadface) || (buffer[1] != 0xb16b00b5) ||
            (buffer[2] != 0xface1010) || (buffer[3] != 0xabcdef12) ||
            (buffer[4] != 0x3456789a))
                return false;

    for (i = 0; i < ((len / sizeof(uint32_t)) - 5); i++)
        if (buffer[i+5] != i+1)
            return false;

    if (len % sizeof(uint32_t)) {
           uint32_t mask = 0xffffffff << (len % sizeof(uint32_t));
           if ((buffer[(len / sizeof(uint32_t))] & ~(mask)) != ((i+1) & ~(mask)))
               return false;
    }
    return true;
}

static bool check_erase_buffer(uint32_t * buffer, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < (len / sizeof(uint32_t)); i++)
        if (buffer[i] != 0xffffffff)
        return false;

    if (len % sizeof(uint32_t)) {
        uint32_t mask = 0xffffffff << (len % sizeof(uint32_t));
        if ((buffer[i] & ~(mask)) != (0xffffffff & ~(mask)))
            return false;
    }
    return true;
}

void handle_msg(struct cfw_message *msg, void *data)
{
    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_CFW_OPEN_SERVICE_RSP:
        ll_storage_service_conn  = (cfw_service_conn_t *)((cfw_open_conn_rsp_msg_t*)msg)->service_conn;
        ll_storage_init_done = true;
        cu_print("LL Storage open\n");
        break;
    case MSG_ID_LL_ERASE_BLOCK_RSP:
        cu_print("LL Write Block : MSG_ID_LL_ERASE_BLOCK_RSP\n");
        if (((ll_storage_erase_block_rsp_msg_t*)msg)->rsp_header.status == DRV_RC_OK) {
            blk_erase = true;
            cu_print("LL Erase Block : successful\n");
        } else {
            failed_resp = true;
            cu_print("LL Erase Block : MSG_ID_LL_ERASE_BLOCK_RSP FAILED\n");
        }
        break;
    case MSG_ID_LL_READ_PARTITION_RSP:
        cu_print("LL Read Partition : MSG_ID_LL_READ_PARTITION_RSP\n");
        if (((ll_storage_read_partition_rsp_msg_t*)msg)->rsp_header.status == DRV_RC_OK) {
            actual_read = ((ll_storage_read_partition_rsp_msg_t*)msg)->actual_read_size;
#ifdef DEBUG
            print_buffer(((ll_storage_read_partition_rsp_msg_t*)msg)->buffer, TST_DATA_LEN);
#endif
            if (part_erase)
                CU_ASSERT("Incorrect read values",
                        check_erase_buffer(((ll_storage_read_partition_rsp_msg_t*)msg)->buffer, TST_DATA_LEN) == true);
            else
                CU_ASSERT("Incorrect read values",
                        check_read_buffer(((ll_storage_read_partition_rsp_msg_t*)msg)->buffer, TST_DATA_LEN) == true);
            bfree(((ll_storage_read_partition_rsp_msg_t*)msg)->buffer);
            cu_print("LL Read Partition: actual_read=%d\n", actual_read);
        } else {
            failed_resp = true;
            cu_print("LL Read Partition : MSG_ID_LL_READ_PARTITION_RSP FAILED\n");
        }
        break;
    case MSG_ID_LL_WRITE_PARTITION_RSP:
        cu_print("LL Write Partition : MSG_ID_LL_WRITE_PARTITION_RSP\n");
        if (((ll_storage_write_partition_rsp_msg_t*)msg)->rsp_header.status == DRV_RC_OK) {
            if (((ll_storage_write_partition_rsp_msg_t*)msg)->write_type == ERASE_REQ) {
                part_erase = true;
                cu_print("LL Write Partition : Erase successful\n");
            } else {
            actual_written = ((ll_storage_write_partition_rsp_msg_t*)msg)->actual_size;
            cu_print("LL Write Partition: bytes written=%d\n", actual_written);
            }
        } else {
           failed_resp = true;
           cu_print("LL Read Partition : MSG_ID_LL_WRITE_PARTITION_RSP FAILED\n");
        }
        break;
    default:
        cu_print("default cfw handler\n");
        break;
    }
    cfw_msg_free(msg);
}

DRIVER_API_RC test_init ()
{
   DRIVER_API_RC ret =  DRV_RC_FAIL;

   ll_storage_client = cfw_client_init(get_test_queue(), handle_msg, NULL);

   SRV_WAIT(!cfw_service_registered(LL_STOR_SERVICE_ID), 0xFFFFFF);

   if (cfw_service_registered(LL_STOR_SERVICE_ID)) {
       cu_print("LL Storage Service registered, open it\n");
       cfw_open_service_conn(ll_storage_client, LL_STOR_SERVICE_ID, "LL storage service open");
   } else {
       cu_print("LL Storage Service NOT registered\n");
       goto out;
   }

   SRV_WAIT(!ll_storage_init_done, OS_WAIT_FOREVER);

   if (!ll_storage_init_done)
       goto out;

   ret = DRV_RC_OK;
   cu_print("LL Storage initialization done\n");

out:
    CU_ASSERT("Service initialization failure\n", ret == DRV_RC_OK);
    return ret;
}

static void service_flash_test()
{
    uint32_t i;
    uint16_t partition_id_for_block = (spi_test) ?
        SPI_FOTA_PARTITION_ID : APPLICATION_DATA_PARTITION_ID;
    uint16_t factory_reset_persistent = (spi_test) ?
        SPI_APPLICATION_DATA_PARTITION_ID : FACTORY_RESET_PERSISTENT_PARTITION_ID;
    uint16_t factory_reset_non_persistent = (spi_test) ?
        SPI_SYSTEM_EVENT_PARTITION_ID : FACTORY_RESET_NON_PERSISTENT_PARTITION_ID;
    static uint32_t* data_for_write;

    blks_written = 0;
    actual_written = 0;
    blks_read = 0;
    actual_read = 0;
    blk_erase = false;
    part_erase = false;
    failed_resp = false;

    // Erase block
    ll_storage_service_erase_block(ll_storage_service_conn, partition_id_for_block, TST_BLOCK_START, TST_BLOCK_LEN, NULL);
    SRV_WAIT((blk_erase != true) && (failed_resp != true), 0xFFFFF);

    CU_ASSERT("Storage Block Erase failure", blk_erase == true);
    failed_resp = false;

    blk_erase = false;

    ll_storage_service_erase_partition(ll_storage_service_conn, factory_reset_non_persistent, NULL);
    SRV_WAIT((part_erase != true) && (failed_resp != true), 0xFFFF);

    CU_ASSERT("Storage Partition Erase failure", part_erase == true);
    failed_resp = false;
    part_erase = false;

    ll_storage_service_erase_partition(ll_storage_service_conn, factory_reset_persistent, NULL);

    SRV_WAIT(((part_erase != true) && (failed_resp != true)), 0xFFFF);
    CU_ASSERT("Storage Block Erase failure", part_erase == true);

    failed_resp = false;

#ifdef DEBUG
    ll_storage_service_read(ll_storage_service_conn, factory_reset_non_persistent, 0, TST_DATA_LEN, NULL);
    SRV_WAIT((actual_read == 0) && (failed_resp != true), 0xFFFF);

    CU_ASSERT("Storage Read failure", actual_read == TST_DATA_LEN);
    failed_resp = false;
    actual_read = 0;
#endif

    part_erase = false;

    data_for_write = balloc(TST_DATA_LEN * sizeof(uint32_t), NULL);
    data_for_write[0] = 0xdeadface;
    data_for_write[1] = 0xb16b00b5;
    data_for_write[2] = 0xface1010;
    data_for_write[3] = 0xabcdef12;
    data_for_write[4] = 0x3456789a;

    for (i = 0; i < (TST_DATA_LEN - 5); i++) {
        data_for_write[i + 5] = i + 1;
    }

    ll_storage_service_write(ll_storage_service_conn, factory_reset_non_persistent, 0, data_for_write, TST_DATA_LEN, NULL);
    SRV_WAIT((actual_written == 0) && (failed_resp != true), 0xFFFF);

    CU_ASSERT("Storage Write failure", actual_written == TST_DATA_LEN);

    failed_resp = false;
    bfree (data_for_write);

    ll_storage_service_read(ll_storage_service_conn, factory_reset_non_persistent, 0, TST_DATA_LEN, NULL);

    SRV_WAIT((actual_read == 0) && (failed_resp != true), 0xFFFF);
    CU_ASSERT("Storage Read failure", actual_read == TST_DATA_LEN);
}

/* Destructive test which erase properties and SPI partitions */
void ll_storage_service_test(void)
{

    cu_print("####################################################\n");
    cu_print("# Purpose of Low Level Storage service tests       #\n");
#ifdef CONFIG_SPI_FLASH
    cu_print("# Run following steps on external flash :          #\n");
#endif
#ifdef CONFIG_SOC_FLASH
    cu_print("# Run following steps on on-die flash :            #\n");
#endif
    cu_print("#    Erase 1 block of data                         #\n");
    cu_print("#    Erase 1 partition of data                     #\n");
    cu_print("#    Write %d bytes of data in a partition         #\n",TST_DATA_LEN);
    cu_print("#                                and read it back  #\n");
    cu_print("####################################################\n");

    if (test_init() == DRV_RC_OK){
#ifdef CONFIG_SPI_FLASH
        /* SPI test */
        spi_test = true;
        service_flash_test();
#endif
#ifdef CONFIG_SOC_FLASH
        /* ONDIE test */
        spi_test = false;
        service_flash_test();
#endif
    }
}
