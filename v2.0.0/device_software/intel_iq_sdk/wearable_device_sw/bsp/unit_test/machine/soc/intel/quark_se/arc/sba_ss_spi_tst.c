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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "os/os.h"
#include "drivers/serial_bus_access.h"

#include "util/cunit_test.h"

#define ID_REG          (0xD0)
#define CALLIB_0        (0xE1)
#define CTRL_MEAS       (0xF4)
#define CONFIG          (0xF5)
#define HUM_LSB         (0xFE)

#define WRITE_CMD       ~(1 << 7)

#define TIMEOUT         50000000 //0xFFFFF

#define DEVICE_ADDR     SPI_SE_1

static uint8_t ID_REG_CMD[1] =     { ID_REG };
static uint8_t CTRL_MEAS_CMD[1] =  { CTRL_MEAS };

/*
 * Requests and requests buffs
 *
 */
#define NB_REQ          4
static sba_request_t *tx_request[NB_REQ];
static sba_request_t *trx_request[NB_REQ];

volatile uint8_t sba_ss_spi_xfer_complete = 0;
volatile uint8_t sba_ss_spi_err_detect = 0;
volatile uint8_t sba_ss_spi_tx_complete = 0;

/*
 * Time-out test
 */
#define     LOCAL_DELAY               50000000

static void sba_ss_spi_callback_trx(sba_request_t *request)
{
    sba_ss_spi_xfer_complete = 1;
}

static void sba_ss_spi_callback_tx(sba_request_t *request)
{
    sba_ss_spi_tx_complete = 1;
}

static uint8_t wait_tx_complete(uint32_t nbtx) {
    uint32_t delay_tx = LOCAL_DELAY;

    while (sba_ss_spi_tx_complete < nbtx) {
        delay_tx--;
        if (delay_tx == 0) {
            return false;
        }
    }
    sba_ss_spi_tx_complete = 0;
    return true;
}

static uint8_t wait_trx_complete(uint32_t nbtx) {
    uint32_t delay_trx = LOCAL_DELAY;

    while (sba_ss_spi_xfer_complete < nbtx) {
        delay_trx--;
        if (delay_trx == 0) {
            return false;
        }
    }
    sba_ss_spi_xfer_complete = 0;
    return true;
}

static void reset_callback(void)
{
    sba_ss_spi_xfer_complete = 0;
    sba_ss_spi_tx_complete = 0;
    sba_ss_spi_err_detect = 0;
}

static DRIVER_API_RC param_request( sba_request_t * request[], uint8_t requestNumber, uint8_t *tx_buff, uint8_t tx_len, uint8_t *rx_buff, uint8_t rx_len, uint8_t full_duplex){

    request[requestNumber]->tx_buff = tx_buff;
    request[requestNumber]->tx_len = tx_len;
    request[requestNumber]->rx_buff = rx_buff;
    request[requestNumber]->rx_len = rx_len;
    request[requestNumber]->full_duplex = full_duplex;

    return sba_exec_request(request[requestNumber]);
}

static void init_single_sba_requests_spi()
{
    tx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    trx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);

    /* Creation of a Tx request */
    tx_request[0]->request_type    = SBA_TX;
    tx_request[0]->tx_len          = 2;
    tx_request[0]->full_duplex     = 0;
    tx_request[0]->addr.cs         = DEVICE_ADDR;
    tx_request[0]->bus_id          = SBA_SS_SPI_MASTER_0;
    tx_request[0]->status          = 1;
    tx_request[0]->callback        = sba_ss_spi_callback_tx;

    /* Creation of a Transaction request */
    trx_request[0]->request_type   = SBA_TRANSFER;
    trx_request[0]->tx_len         = 2;
    trx_request[0]->rx_len         = 2;
    trx_request[0]->full_duplex    = 0;
    trx_request[0]->addr.cs        = DEVICE_ADDR;
    trx_request[0]->bus_id         = SBA_SS_SPI_MASTER_0;
    trx_request[0]->status         = 1;
    trx_request[0]->callback       = sba_ss_spi_callback_trx;
}

static void end_single_test(void)
{
    bfree(tx_request[0]);
    bfree(trx_request[0]);
}

static void sba_ss_spi_write_reg(uint8_t* reg, uint8_t value)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[2] = {0};

    buffer[0] = (*reg) & WRITE_CMD;
    buffer[1] = value;
    reset_callback();

    ret = param_request(tx_request, 0, buffer, sizeof(buffer), NULL, 0, 0);     // Perform request: TX write reg cmd
    CU_ASSERT ("SPI write failure", ret == DRV_RC_OK);
    ret = wait_tx_complete(1);
    CU_ASSERT("Time-out wait tx expire \n", ret == true);                      // Alert if TRX Timeout error
}


void sba_ss_spi_write_word_reg(uint8_t reg, uint16_t value)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[4] = {0};

    reset_callback();

    buffer[0] = reg & WRITE_CMD;
    buffer[1] = value;
    buffer[2] = (reg+1) & WRITE_CMD;
    buffer[3] = value >> 8;

    ret = param_request(tx_request, 0, buffer, sizeof(buffer), NULL, 0, 0);     // Perform request: TX write reg cmd
    CU_ASSERT ("SPI write failure", ret == DRV_RC_OK);
    ret = wait_tx_complete(1);
    CU_ASSERT("Time-out wait tx expire \n", ret == true);                      // Alert if TRX Timeout error
}

void sba_ss_spi_read_reg(uint8_t* reg, uint8_t expected_val)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t read_data[1] = { 0 };

    reset_callback();

    ret = param_request(trx_request, 0, reg, 1, read_data, 1, 0);     // Perform request: TRX Read value
    CU_ASSERT ("SPI read failure", ret == DRV_RC_OK);
    ret = wait_trx_complete(1);
    CU_ASSERT("Time-out wait trx expire \n", ret == true);            // Alert if TRX Timeout error

#ifdef DEBUG
    cu_print("read value %x expected value %x\n", read_data, expected_val);
#endif
    CU_ASSERT("SPI expected value differs from the read value", expected_val == (uint8_t)(*read_data));
}

void sba_ss_spi_read_word_reg(uint8_t reg, uint16_t expected_val)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint16_t read_data = 0;

    reset_callback();

    ret = param_request(trx_request, 0, &reg, 1, (uint8_t*)&read_data, 2, 0);     // Perform request: TX write reg cmd
    CU_ASSERT ("SPI read failure", ret == DRV_RC_OK);
    ret = wait_trx_complete(1);
    CU_ASSERT("Time-out wait trx expire \n", ret == true);                        // Alert if TRX Timeout error

#ifdef DEBUG
    cu_print("read value %x expected value %x\n", read_data, expected_val);
#endif

    CU_ASSERT ("SPI expected value differs from the read value", expected_val == read_data);
}
#ifdef DEBUG
static void dump_data(uint8_t * data, int len)
{
    int i;
    for (i=0; i < len ; i++) {
        if (i%8 == 0) cu_print("\n");
        cu_print("%x ", data[i]);
    }
    cu_print("\n");
}
#endif

void sba_ss_transfer_block_spi(void)
{
    uint8_t write_buffer[1] = {CALLIB_0};
    uint8_t read_buffer[HUM_LSB - CALLIB_0] = {0};
    DRIVER_API_RC ret = 0;

    cu_print("read %d bytes\n", sizeof(read_buffer));
    reset_callback();

    ret = param_request(trx_request, 0, write_buffer, 1, read_buffer, sizeof(read_buffer), 0);      // Perform request: TX write reg cmd
    CU_ASSERT("Sending block bytes TX buffer failed\n", (ret == DRV_RC_OK));
    ret = wait_trx_complete(1);
    CU_ASSERT("Time-out wait trx expire \n", ret == true);                                          // Alert if TRX Timeout error

#ifdef DEBUG
    dump_data(read_buffer, sizeof(read_buffer));
#endif
    CU_ASSERT("ss spi written value differs from the read value", read_buffer[CTRL_MEAS - CALLIB_0] == 0x04);
    CU_ASSERT("ss spi written value differs from the read value", read_buffer[CONFIG - CALLIB_0] == 0x40);
}

void sba_ss_read_block_spi(void)
{
    uint8_t read_buffer[HUM_LSB - CALLIB_0] = {0xff};
    DRIVER_API_RC ret = 0;

    cu_print("read block 2 : %d bytes\n", sizeof(read_buffer));
    reset_callback();

    ret = param_request(trx_request, 0, NULL, 0, read_buffer, sizeof(read_buffer), 0);      // Perform request: TX write reg cmd
    ret = wait_trx_complete(1);
    CU_ASSERT("Time-out wait trx expire \n", ret == true);                                  // Alert if TRX Timeout error

    CU_ASSERT("Sending block bytes TX buffer failed\n", (ret == DRV_RC_OK));
#ifdef DEBUG
    dump_data(read_buffer, sizeof(read_buffer));
#endif
}

void sba_ss_unit_spi(void)
{
    cu_print("#######################################\n");
    cu_print("# Purpose of SS SPI tests :           #\n");
    cu_print("#     BME280 is connected to ss spi0  #\n");
    cu_print("#     1 - Read BME280 ID              #\n");
    cu_print("#     2 - Write BME280 registers      #\n");
    cu_print("#######################################\n");

    init_single_sba_requests_spi();
    cu_print("read register \n");
    sba_ss_spi_read_reg(ID_REG_CMD, 0x60);

    cu_print("write register \n");
    sba_ss_spi_write_reg(CTRL_MEAS_CMD, 0x08);
    sba_ss_spi_read_reg(CTRL_MEAS_CMD, 0x08);

    cu_print("write word register\n");
    sba_ss_spi_write_word_reg(CTRL_MEAS, 0x4004);

    cu_print("read word register\n");
    sba_ss_spi_read_word_reg(CTRL_MEAS, 0x4004);

    cu_print("read block\n");
    sba_ss_transfer_block_spi();

    end_single_test();
}
