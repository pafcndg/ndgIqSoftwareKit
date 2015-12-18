/*******************************************************************************
 *
 * BSD LICENSE
 *
 * Copyright(c) 2015 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>

#include <nanokernel.h>
#include <microkernel.h>
#include <arch/cpu.h>

#include "os/os.h"
#include "drivers/serial_bus_access.h"
#include "machine.h"

#include "util/cunit_test.h"

/*
 * Purpose : This test validates the implementation of queuing for spi transaction.
 * Requirement : This test consists in writing and reading registers of MX25U12835F Flash SPI memory
 */

static void test_sba_single_add(void);
static void test_sba_multiple_add(void);
static void test_sba_multiple_add_fiber(void);

/*
 * Callback counters
 */
volatile uint8_t spi_tx_complete = 0;
volatile uint8_t spi_rx_complete = 0;
volatile uint8_t spi_trx_complete = 0;
volatile uint8_t spi_WREN_ack = 0;
volatile uint8_t spi_Write_In_Progress = 0;

/*
 * SPI HARDWARE WORKAROUND
 *
 * To comply with hardware workaround used with Quark SPI_M0 Chip Select management.
 * Check intel_qrk_spi.c file for more details.
 */
 #define    HW_SPI_WORKAROUND

/*
 * Debug
 */
#define     DEBUG_TRACE

/*
 * Time-out test
 */
#define     DELAY               50000000

/*
 * SPI defines
 */
#define     DEVICE_ADDR         SPI_SE_3        /* Flash memory Chip Select is SPI_M0_CS3 */
#define     SPI_DEFAULT_SPEED   250             /* Default SPI bus speed in kHz */

/*
 *  MX25U12835F series command hex codes definition
 */
#define     FLASH_CMD_RDSR      0x05            /* RDSR */
#define     FLASH_CMD_WREN      0x06            /* WRite ENable */
#define     FLASH_CMD_PP        0x02            /* Page Program */
#define     FLASH_CMD_READ      0x03            /* Read command (1 x I/O) */
#define     FLASH_CMD_SE        0x20            /* Sector Erase */

/*
 * Fiber defines
 */
#define     FIBER_PRIORITY      7
#define     STACK_SIZE          256

/*
 * Requests and requests buffs
 *
 */
#define     NB_REQ              4
#define     CMD_LEN             24

static sba_request_t *tx_request[NB_REQ];
static sba_request_t *rx_request[NB_REQ];
static sba_request_t *trx_request[NB_REQ];

static uint8_t rx_buff [NB_REQ][CMD_LEN]  = { { 0 }, { 0 } };
static uint8_t cmdWREN [1] = { FLASH_CMD_WREN };
static uint8_t cmdRDSR [1] = { FLASH_CMD_RDSR };
static uint8_t cmdSE   [4] = { FLASH_CMD_SE, 0, 0, 0 };   //fixed erase at address 0

/*
 * Strings array to ease strcmp() use
 */
static uint8_t test_buffer[NB_REQ][6] = { {"hello\0"}, {"world\0"}, {"12345\0"}, {"abcde\0"} };

/*
 * Predefined arrays with spi flash memory commands and arguments
 */
/* Write commands */
static uint8_t write_cmd_buff[NB_REQ][10] = {
                                               {FLASH_CMD_PP, 0x00, 0x01, 0x00, 'h', 'e', 'l', 'l', 'o', 0x00},
                                               {FLASH_CMD_PP, 0x00, 0x02, 0x00, 'w', 'o', 'r', 'l', 'd', 0x00},
                                               {FLASH_CMD_PP, 0x00, 0x03, 0x00, '1', '2', '3', '4', '5', 0x00},
                                               {FLASH_CMD_PP, 0x00, 0x04, 0x00, 'a', 'b', 'c', 'd', 'e', 0x00}
                                            };

/* Read commands */
static uint8_t read_cmd_buff[NB_REQ][4] = {
                                             {FLASH_CMD_READ, 0x00, 0x01, 0x00},
                                             {FLASH_CMD_READ, 0x00, 0x02, 0x00},
                                             {FLASH_CMD_READ, 0x00, 0x03, 0x00},
                                             {FLASH_CMD_READ, 0x00, 0x04, 0x00}
                                          };

/*
 * Fiber variables
 */

static void fiber_spi_client(void);

char fiber_stack[STACK_SIZE];
struct nano_sem SEM_0;

/*
 *  Useful Macros
 */
void reset_callbacks(void){
    spi_rx_complete = 0;
    spi_tx_complete = 0;
    spi_trx_complete = 0;
}

static uint8_t wait_tx_complete(uint32_t nbtx) {
    uint32_t delay_tx = DELAY;

    while (spi_tx_complete < nbtx) {
        delay_tx--;
        if (delay_tx == 0) {
            return false;
        }
    }
    spi_tx_complete = 0;
    return true;
}

static uint8_t wait_trx_complete(uint32_t nbtx) {
    uint32_t delay_trx = DELAY;

    while (spi_trx_complete < nbtx) {
        delay_trx--;
        if (delay_trx == 0) {
            return false;
        }
    }
    spi_trx_complete = 0;
    return true;
}

/*
 * Callbacks
 */
static void sba_spi_callback_tx(sba_request_t *request)
{
    CU_ASSERT("Status not ok\n", request->status == 0);
    spi_tx_complete += 1;
}

static void sba_spi_callback_rx(sba_request_t *request)
{
    CU_ASSERT("Status not ok\n", request->status == 0);
    spi_rx_complete += 1;
}

static void sba_spi_callback_trx(sba_request_t *request)
{
    CU_ASSERT("Status not ok\n", request->status == 0);
    spi_trx_complete += 1;
    spi_WREN_ack = ((rx_buff[0][0] & 0x2) == 0x2) ? 1 : 0;              //check for WREN ack
    spi_Write_In_Progress = ((rx_buff[0][0] & 0x1) == 0x1) ? 1 : 0;     //check for Flash memory availability
}

/*
 *
 * Functions
 *
 */
static DRIVER_API_RC param_request( sba_request_t * request[], uint8_t requestNumber, uint8_t *tx_buff, uint8_t tx_len, uint8_t *rx_buff, uint8_t rx_len, uint8_t full_duplex){

    request[requestNumber]->tx_buff = tx_buff;
    request[requestNumber]->tx_len = tx_len;
    request[requestNumber]->rx_buff = rx_buff;
    request[requestNumber]->rx_len = rx_len;
    request[requestNumber]->full_duplex = full_duplex;

    return sba_exec_request(request[requestNumber]);
}

static DRIVER_API_RC send_wren_request(void){                                   // Send a WREN and check acknowledge

    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t nbReq = 0;
    spi_WREN_ack = 0;

    do{
        cu_print("Request: %d\n", nbReq);
        cu_print("Sends WREN... ");
        ret = param_request(tx_request, 0, cmdWREN, 1, NULL, 0, 0);             // Perform request: TX WREN cmd
        CU_ASSERT("sba_exec_request(WREN) return not ok\n", ret == DRV_RC_OK);  // Alert if TX error

        ret = param_request(trx_request, 0, cmdRDSR, 1, rx_buff[0], 1, 0);      // Perform request: Read Status Register value
        CU_ASSERT("sba_exec_request(RDSR) return not ok\n", ret == DRV_RC_OK);  // Alert if TRX error
        ret = wait_trx_complete(1);                                             // Wait for end of trx
        CU_ASSERT("Time-out wait trx expire \n", ret == true);                  // Alert if TRX Timeout error

        nbReq ++;                                                               // Increment max number of request variable
    }while((!spi_WREN_ack) && (nbReq < 5));                                     // until we receive a WREN = 1 from Flash or exceed 5 attempts

#ifdef DEBUG_TRACE
    if (nbReq >=5){
        cu_print(" Failed\n");
        cu_print("rx_buff[0][0]: %x\n", rx_buff[0][0]);
    }else{
        cu_print(" OK\n");
    }
#endif

    return ret;
}

static DRIVER_API_RC send_se_request(void){                                     // Send Sector Erase request (4K bytes from providen offset address)

    DRIVER_API_RC ret = DRV_RC_OK;

    ret = send_wren_request();                                                  // Send WREN request
    CU_ASSERT("send_tx_request(WREN) return not ok\n", ret == true);            // Correct behavior ends by a true

    ret = param_request(tx_request, 0, cmdSE, 4, NULL, 0, 0);                   // Send Sector Erase request
    CU_ASSERT("send_tx_request(SE) return not ok\n", ret == DRV_RC_OK);         // correct behavior ends by a DRV_RC_OK

    return ret;
}

static DRIVER_API_RC write_in_progress(void){                                   // Check Flash write status

    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t spi_Write_In_Progress = 1, nbRequest = 0;

    do{
        ret = param_request(trx_request, 0, cmdRDSR, 1, rx_buff[0], 1, 0);      // Perform request: RX Status Register value
        CU_ASSERT("sba_exec_request(trx) return not ok\n", ret == DRV_RC_OK);   // Alert if RX error
        ret = wait_trx_complete(1);                                             // Wait for end of rx
        CU_ASSERT("Time-out wait trx expire \n", ret == true);                  // Alert if RX Timeout error

        nbRequest++;                                                            // Increment max number of request variable
    }while(spi_Write_In_Progress && (nbRequest < 5));                           // Wait while Write In Progress == 1

    return ret;
}

static uint8_t verify_erase(uint8_t nb){                                        // Check if proper Sector Erase occured

    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t error = 0;
    reset_callbacks();

    for(uint8_t i = 0; i < nb; i++){
        ret = param_request(trx_request, i, read_cmd_buff[i], 4, rx_buff[i], 6, 0);
        CU_ASSERT("sba_exec_request(trx) return not ok\n", ret == DRV_RC_OK);
    }
    ret = wait_trx_complete(nb);
    CU_ASSERT("Time-out wait trx expire \n", ret == true);

    for(uint8_t i = 0; i < nb; i++){                                    // For each previous READ
        for(uint8_t j = 0; j < 6; j++){
            if(rx_buff[i][j] != 0xFF){                                  // Check if we READ 0xFF (value read after Erase)
                error++;
            }
        }
    }

    return error;
}

/*
 * Test init functions
 */
static void init_single_sba_requests_spi()
{
    tx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    rx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    trx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);

    /* Creation of a Tx request */
    tx_request[0]->request_type    = SBA_TX;
    tx_request[0]->tx_len          = 2;
    tx_request[0]->full_duplex     = 0;
    tx_request[0]->addr.cs         = DEVICE_ADDR;
    tx_request[0]->bus_id          = SBA_SPI_MASTER_0;
    tx_request[0]->status          = 1;
    tx_request[0]->callback        = sba_spi_callback_tx;

    /* Creation of a Rx request */
    rx_request[0]->request_type    = SBA_RX;
    rx_request[0]->rx_len          = 2;
    rx_request[0]->full_duplex     = 0;
    rx_request[0]->addr.cs         = DEVICE_ADDR;
    rx_request[0]->bus_id          = SBA_SPI_MASTER_0;
    rx_request[0]->status          = 1;
    rx_request[0]->callback        = sba_spi_callback_rx;

    /* Creation of a Transaction request */
    trx_request[0]->request_type   = SBA_TRANSFER;
    trx_request[0]->tx_len         = 2;
    trx_request[0]->rx_len         = 2;
    trx_request[0]->full_duplex    = 0;
    trx_request[0]->addr.cs        = DEVICE_ADDR;
    trx_request[0]->bus_id         = SBA_SPI_MASTER_0;
    trx_request[0]->status         = 1;
    trx_request[0]->callback       = sba_spi_callback_trx;
}

static void init_multiple_sba_requests_spi() {
    uint8_t i;

    for (i = 0; i < NB_REQ; i++) {
        tx_request[i] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
        rx_request[i] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
        trx_request[i] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);

        /* Creation of a Tx request */
        tx_request[i]->request_type    = SBA_TX;
        tx_request[i]->tx_len          = 2;
        tx_request[i]->full_duplex     = 0;
        tx_request[i]->addr.cs         = DEVICE_ADDR;
        tx_request[i]->bus_id          = SBA_SPI_MASTER_0;
        tx_request[i]->status          = 1;
        tx_request[i]->callback        = sba_spi_callback_tx;

        /* Creation of a Rx request */
        rx_request[i]->request_type    = SBA_RX;
        rx_request[i]->rx_len          = 2;
        rx_request[i]->full_duplex     = 0;
        rx_request[i]->addr.cs         = DEVICE_ADDR;
        rx_request[i]->bus_id          = SBA_SPI_MASTER_0;
        rx_request[i]->status          = 1;
        rx_request[i]->callback        = sba_spi_callback_rx;

        /* Creation of a Transaction request */
        trx_request[i]->request_type   = SBA_TRANSFER;
        trx_request[i]->tx_len         = 2;
        trx_request[i]->rx_len         = 2;
        trx_request[i]->full_duplex    = 0;
        trx_request[i]->addr.cs        = DEVICE_ADDR;
        trx_request[i]->bus_id         = SBA_SPI_MASTER_0;
        trx_request[i]->status         = 1;
        trx_request[i]->callback       = sba_spi_callback_trx;
    }
}

static void end_single_test(void)
{
    bfree(tx_request[0]);
    bfree(rx_request[0]);
    bfree(trx_request[0]);
}

static void end_multiple_test(void) {
    uint8_t i;

    for (i = 0; i < NB_REQ; i++) {
        bfree(tx_request[i]);
        bfree(rx_request[i]);
        bfree(trx_request[i]);
    }
}

/*
 * Fiber functions
 */
static void fiber_spi_client(void)
{
    DRIVER_API_RC rc;

    cu_print("-----------------------------------\n");
    cu_print("FIBER TEST\n");
    /* Send WREN */
    rc = send_wren_request();                                                   // Send WREN request before write
    CU_ASSERT("send_exec_request(WREN) return not ok\n", rc == true);           // Correct behavior ends by a true

    /* Write "world" */
    rc = param_request(tx_request, 1, write_cmd_buff[1], 10, NULL, 0, 0);       //Send write(PP) request
    CU_ASSERT("sba_exec_request(PP) return not ok\n", rc == DRV_RC_OK);         // Correct behavior returns DRV_RC_OK
    cu_print("-----------------------------------\n");
}

/*
 * Tests
 */
static void test_sba_single_add(void)
{
    DRIVER_API_RC rc;

    init_single_sba_requests_spi();
    cu_print("\n");
    cu_print("######################################\n");
    cu_print("SINGLE ADD TEST\n");

    rc = send_wren_request();
    CU_ASSERT("send_exec_request() return not ok\n", rc == true);
    reset_callbacks();

    /* Write "hello!" to address 0 on the flash memory */
    rc = param_request(tx_request, 0, write_cmd_buff[0], 10, NULL, 0, 0);           // Perform request: write flash
    CU_ASSERT("sba_exec_request(tx) return not ok\n", rc == DRV_RC_OK);             // Alert if TX error
    rc = wait_tx_complete(1);                                                       // Wait for end of tx
    CU_ASSERT("Time-out wait tx expire \n", rc == true);                            // Alert if TX Timeout error

    /* Wait for end of writing process - example */
    write_in_progress();
    cu_print("Flash Write Done\n");
    reset_callbacks();

    /* Read the same address to ensure data have been written correctly */
    rc = param_request(trx_request, 0, read_cmd_buff[0], 4, rx_buff[0], 10, 0);     // Perform request: Read data in flash
    CU_ASSERT("sba_exec_request(trx) return not ok\n", rc == DRV_RC_OK);            // Alert if TX error
    rc = wait_trx_complete(1);                                                      // Wait for end of tx
    CU_ASSERT("Time-out wait trx expire \n", rc == true);                           // Alert if TX Timeout error

    /* Ensure correct values are read */
    CU_ASSERT("SPI read value differs from the written value\n", !strncmp((char*)test_buffer[0], (char*)rx_buff[0], sizeof(test_buffer[0])-1));
    cu_print("Expected value = %s ; read value = %s \n", test_buffer[0], rx_buff[0]);

    end_single_test();
    cu_print("######################################\n");
}

static void test_sba_multiple_add(void) {
    DRIVER_API_RC rc;
    uint8_t i, success = 0;

    reset_callbacks();

    /* Configuration of the SPI */
    init_multiple_sba_requests_spi();

    cu_print("Erase: %s\n", (verify_erase(NB_REQ) == 0) ? "OK" : "FAIL");       // Check pre-test erase validity
    cu_print("\n");
    cu_print("######################################\n");
    cu_print("MULTIPLE ADD TEST\n");

    cu_print("Send multiple flash requests WITH WREN\n");
    reset_callbacks();
    /* Write NB_REQ values to the flash */
    for (i = 0; i < NB_REQ; i++) {

        rc = send_wren_request();
        CU_ASSERT("send_exec_request() return not ok\n", rc == true);           // correct behavior ends by a true

        /* Addition of the Tx request */
        rc = param_request(tx_request, i, write_cmd_buff[i], 10, NULL, 0, 0);   // Write data to the flash
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);
    }
    rc = wait_tx_complete(2*NB_REQ);                                            // For each write, we have 2 tx requests (WREN + DATA)
    CU_ASSERT("Time-out wait tx expire \n", rc == true);

    write_in_progress();        //Strongly needed ! Prevents reading error due to write in progress occuring in flash spi memory...
    cu_print("Write Done.\n");

    /* Read the NB_REQ values sent to the flash */
    for (i = 0; i < NB_REQ; i++) {
        /* Addition of the Trx request */
        rc = param_request(trx_request, i, read_cmd_buff[i], 4, rx_buff[i], 10, 0);  //Write data to the flash
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);
    }
    rc = wait_trx_complete(NB_REQ);
    CU_ASSERT("Time-out wait trx expire \n", rc == true);

    cu_print("Reading values from flash:\n");
    success = 0;                                                                //reset success counter
    for (i = 0; i < NB_REQ; i++) {
        cu_print("Value %d: %s, expected %s\n", i, rx_buff[i], test_buffer[i]);
        if ( !strcmp((char*)test_buffer[i], (char*)rx_buff[i]) ){
            success++;
        }
    }
    CU_ASSERT("SPI read value differs from the written value\n", (success == NB_REQ) );
    end_multiple_test();
    cu_print("######################################\n");
}

static void test_sba_multiple_add_fiber(void) {
    DRIVER_API_RC rc;
    uint8_t i = 0, success = 0;

    /* Configuration of the SPI */
    init_multiple_sba_requests_spi();

    cu_print("\n");
    cu_print("######################################\n");
    cu_print("MULTIPLE ADD WITH FIBER TEST\n");
    cu_print("Send multiple flash requests WITH WREN\n");

    cu_print("Erase: %s\n", (verify_erase(NB_REQ) == 0) ? "OK" : "FAIL");       //Check pre-test erase validity

    /* SHOW: Read values after erase to show effect of SE command */
    cu_print("Sends two read requests\n");
    /* Read value at "hello" location */
    rc = param_request(trx_request, 1, read_cmd_buff[0], 4, rx_buff[0], 6, 0);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* Read value at "world" location */
    rc = param_request(trx_request, 1, read_cmd_buff[1], 4, rx_buff[1], 6, 0);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* Wait for transactions to complete */
    rc = wait_trx_complete(2);
    CU_ASSERT("Time-out wait trx expire \n", rc == true);

    for (i = 0; i < 2; i++) {
        cu_print("Value %d: %s, expected 0xFFFFF...\n", i, rx_buff[i]);
    }
    /* END OF SHOW */

    /* Send WREN */
    rc = send_wren_request();
    CU_ASSERT("send_exec_request() return not ok\n", rc == true);               // correct behavior ends by a true
    reset_callbacks();

    /* Write "hello" */
    rc = param_request(tx_request, 0, write_cmd_buff[0], 10, NULL, 0, 0);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* ---------------- Create fiber context ---------------- */
    fiber_fiber_start(fiber_stack, STACK_SIZE, (nano_fiber_entry_t) fiber_spi_client,
                   0, 0, FIBER_PRIORITY, 0);
    /* ------------------------------------------------------ */

    /* wait for tx to complete */
    rc = wait_tx_complete(2);
    CU_ASSERT("Time-out wait tx expire \n", rc == true);

    cu_print("Sends two read requests\n");
    /* Read "hello" */
    rc = param_request(trx_request, 1, read_cmd_buff[0], 4, rx_buff[0], 6, 0);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* Read "world" */
    rc = param_request(trx_request, 1, read_cmd_buff[1], 4, rx_buff[1], 6, 0);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* Wait for transactions to complete */
    rc = wait_trx_complete(2);
    CU_ASSERT("Time-out wait trx expire \n", rc == true);

    success = 0;                                                                //reset success counter
    for (i = 0; i < 2; i++) {
        cu_print("Value %d: %s, expected %s\n", i, rx_buff[i], test_buffer[i]);
        if ( !strcmp((char*)test_buffer[i], (char*)rx_buff[i]) ){
            success++;
        }
    }
    CU_ASSERT("SPI read value differs from the written value\n", (success == 2) );

    end_multiple_test();
    cu_print("######################################\n");

}

static void erase_flash_sector(void){
    CU_ASSERT("Sector Erase problem\n", send_se_request() == DRV_RC_OK);
    local_task_sleep_ms(400);      // Time needed for SE, according to maximum ratings (datasheet)
}

void sba_spi_test(void) {
    cu_print("########################################################\n");
    cu_print("# Purpose of Serial Bus Access SPI tests :             #\n");
    cu_print("#            Addition of one request (Rx, Tx, Rx / Tx) #\n");
    cu_print("#            Addition of several requests              #\n");
    cu_print("#            Test error cases                          #\n");
    cu_print("########################################################\n");

    test_sba_single_add();

    erase_flash_sector();

    test_sba_multiple_add();

    erase_flash_sector();

    test_sba_multiple_add_fiber();

    erase_flash_sector();
}
