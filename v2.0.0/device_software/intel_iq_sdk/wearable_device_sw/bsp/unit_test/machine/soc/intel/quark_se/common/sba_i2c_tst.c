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
#include <arch/cpu.h>
#include "drivers/serial_bus_access.h"
#include "os/os.h"

#include "machine.h"
#include "drivers/serial_bus_access.h"
#include "util/cunit_test.h"

/*
 * Purpose : This test validate the implementation of queing for i2c transaction.
 * Requirement : This test consist in write and read registers of DRV2605
 *
 */

static void test_sba_single_add(void);
static void test_sba_multiple_add(void);
static void test_sba_multiple_add_fiber(void);

/*
 ** Callback counters
 */
static volatile uint8_t i2c_tx_complete = 0;
static volatile uint8_t i2c_rx_complete = 0;

/*
 ** Time-out test
 */
#define DELAY 500000000

/*
** DRV2605
*/
#define DEVICE_ADDR                 0x5A      /*I2c device address */
#define LIBRARY_SELECTION_REG       0x03      /*Selection of library register */
#define LIBRARY_B                   0x02      /*For fast, light actuators */
#define LIBRARY_C                   0x03      /*TS2200 LIBRARY C */
#define IND_LIB_B                   0
#define IND_LIB_C                   1

/*
 * Requests and requests buffs
 *
 */
#define NB_REQ 4

static sba_request_t *tx_request[NB_REQ];
static sba_request_t *trx_request[NB_REQ];

static uint8_t tx_buff[2][2] = {{LIBRARY_SELECTION_REG,LIBRARY_B},
                                 {LIBRARY_SELECTION_REG,LIBRARY_C}};

static uint8_t trx_buff = LIBRARY_SELECTION_REG;

static uint8_t rx_buff[NB_REQ][16] = { { 0 }, { 0 } };

/*
 * Fiber
 *
 */
#define FIBER_PRIORITY 7
#define STACK_SIZE 256

void fiber_i2c_client(void);

char fiber_stack[STACK_SIZE];
struct nano_sem SEM_0;


/*
 *  Useful Macros
 */

static uint8_t wait_rx_complete(uint32_t nbtx) {
    uint32_t delay_rx = DELAY;

    while (i2c_rx_complete < nbtx) {
        delay_rx--;
        if (delay_rx == 0) {
            return false;
        }
    }
    i2c_rx_complete = 0;
    return true;
}

static uint8_t wait_tx_complete(uint32_t nbtx) {
    uint32_t delay_tx = DELAY;

    while (i2c_tx_complete < nbtx) {
        delay_tx--;
        if (delay_tx == 0) {
            return false;
        }
    }
    i2c_tx_complete = 0;
    return true;
}

/*
 * Callbacks
 */
void sba_i2c_callback_tx(sba_request_t *request)
{
    CU_ASSERT("Status not ok\n", request->status == 0);
    i2c_tx_complete += 1;
}

void sba_i2c_callback_rx(sba_request_t *request)
{
    CU_ASSERT("Status not ok\n", request->status == 0);
    i2c_rx_complete += 1;
}


/*
 * Test init functions
 */
void init_single_sba_requests()
{
    tx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    trx_request[0] = (sba_request_t *) balloc(sizeof(sba_request_t), NULL);
    /* Creation of a Tx request */
    tx_request[0]->request_type    = SBA_TX;
    tx_request[0]->tx_len          = 2;
    tx_request[0]->addr.slave_addr = DEVICE_ADDR;
    tx_request[0]->bus_id          = SBA_I2C_MASTER_1;
    tx_request[0]->status          = 1;
    tx_request[0]->callback        = sba_i2c_callback_tx;

    /* Creation of a Transaction request */
    trx_request[0]->request_type    = SBA_TRANSFER;
    trx_request[0]->tx_len          = 1;
    trx_request[0]->rx_len          = 1;
    trx_request[0]->addr.slave_addr = DEVICE_ADDR;
    trx_request[0]->bus_id          = SBA_I2C_MASTER_1;
    trx_request[0]->status          = 1;
    trx_request[0]->callback        = sba_i2c_callback_rx;
}

void init_multiple_sba_requests(union sba_config *config) {
    uint8_t i;

    for (i = 0; i < NB_REQ; i++) {
        tx_request[i] = (sba_request_t *) balloc(sizeof(sba_request_t),
                                                    NULL);
        trx_request[i] = (sba_request_t *) balloc(sizeof(sba_request_t),
                                                     NULL);

        /* Creation of a Tx request */
        tx_request[i]->request_type = SBA_TX;
        tx_request[i]->tx_len = 2;
        tx_request[i]->addr.slave_addr = DEVICE_ADDR;
        tx_request[i]->bus_id = SBA_I2C_MASTER_1;
        tx_request[i]->status = 1;
        tx_request[i]->callback = sba_i2c_callback_tx;

        /* Creation of a Transaction request */
        trx_request[i]->request_type = SBA_TRANSFER;
        trx_request[i]->tx_len = 1;
        trx_request[i]->rx_len = 1;
        trx_request[i]->addr.slave_addr = DEVICE_ADDR;
        trx_request[i]->bus_id = SBA_I2C_MASTER_1;
        trx_request[i]->status = 1;
        trx_request[i]->callback = sba_i2c_callback_rx;
    }
}

static void end_single_test(void)
{
    bfree(tx_request[0]);
    bfree(trx_request[0]);
}

static void end_multiple_test(void) {
    uint8_t i;

    for (i = 0; i < NB_REQ; i++) {
        bfree(tx_request[i]);
        bfree(trx_request[i]);
    }
}

/*
 * Fiber functions
 */
void fiber_i2c_client(void)
{
    DRIVER_API_RC rc;

    cu_print("\n");
    cu_print("###################################\n");
    cu_print("FIBER TEST\n");

    /* Addition of the Tx request */
    tx_request[2]->tx_buff = tx_buff[IND_LIB_B];
    rc = sba_exec_request(tx_request[2]);
    CU_ASSERT("sba_exec_request() return not ok\n",rc == DRV_RC_OK);

    /* Addition of the Transaction request */
    trx_request[2]->tx_buff = &trx_buff;
    trx_request[2]->rx_buff = rx_buff[2];
    rc = sba_exec_request(trx_request[2]);
    CU_ASSERT("sba_exec_request() return not ok\n",rc == DRV_RC_OK);

    cu_print("\n");
    cu_print("######################################\n");
}

static void test_sba_single_add(void)
{
    DRIVER_API_RC rc;

    init_single_sba_requests();

    cu_print("\n");
    cu_print("###################################\n");
    cu_print("SINGLE ADD TEST\n");

    /* Addition of the Tx request */
    tx_request[0]->tx_buff = tx_buff[IND_LIB_B];
    rc = sba_exec_request(tx_request[0]);
    CU_ASSERT("sba_exec_request() return not ok\n",rc == DRV_RC_OK);

    rc = wait_tx_complete(1);
    CU_ASSERT("Time-out wait tx expire \n",rc == true);

    /* Addition of the Transaction request */
    trx_request[0]->tx_buff = &trx_buff;
    trx_request[0]->rx_buff = rx_buff[0];
    rc = sba_exec_request(trx_request[0]);
    CU_ASSERT("sba_exec_request() return not ok\n",rc == DRV_RC_OK);

    rc = wait_rx_complete(1);
    CU_ASSERT("Time-out wait rx expire\n",rc == true);
    CU_ASSERT("i2c written value differs from the read value\n", rx_buff[0][0] == LIBRARY_B);
    cu_print("expected value = %d ; read value = %d\n",LIBRARY_B,rx_buff[0][0] );

    end_single_test();
    cu_print("######################################\n");
}

static void test_sba_multiple_add(void) {
    union sba_config config;
    DRIVER_API_RC rc;
    uint8_t i;

    /* Configuration of the I2C */
    init_multiple_sba_requests(&config);

    cu_print("\n");
    cu_print("###################################\n");
    cu_print("MULTIPLE ADD TEST\n");

    for (i = 0; i < NB_REQ; i++) {
        /* Addition of the Tx request */
        tx_request[i]->tx_buff = tx_buff[IND_LIB_C];
        rc = sba_exec_request(tx_request[i]);
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);
    }

    rc = wait_tx_complete(4);
    CU_ASSERT("Time-out wait tx expire \n",rc == true);

    for (i = 0; i < NB_REQ; i++) {
        /* Addition of the transaction request */
        trx_request[i]->tx_buff = &trx_buff;
        trx_request[i]->rx_buff = rx_buff[i];
        rc = sba_exec_request(trx_request[i]);
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);
    }

    rc = wait_rx_complete(4);
    for (i = 0; i < NB_REQ; i++) {
        CU_ASSERT("Time-out wait rx expire\n", rc == true);
        CU_ASSERT("i2c written value differs from the read value\n",rx_buff[i][0] == LIBRARY_C);
        cu_print("expected value = %d ; read value = %d\n",LIBRARY_C,rx_buff[i][0] );
    }

    end_multiple_test();
    cu_print("######################################\n");
}


static void test_sba_multiple_add_fiber(void) {
    union sba_config config;
    DRIVER_API_RC rc;
    uint8_t i;

    /* Configuration of the I2C */
    init_multiple_sba_requests(&config);

    cu_print("\n");
    cu_print("###################################\n");
    cu_print("MULTIPLE ADD WITH FIBER TEST\n");

    for (i = 0; i < NB_REQ - 2; i++) {
        /* Addition of the Tx request */
        tx_request[i]->tx_buff = tx_buff[i];
        rc = sba_exec_request(tx_request[i]);
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

        /* Addition of the Trx request */
        trx_request[i]->tx_buff = &trx_buff;
        trx_request[i]->rx_buff = rx_buff[i];
        rc = sba_exec_request(trx_request[i]);
        CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);
    }

    cu_print("Creation of the fiber\n");
    fiber_fiber_start(fiber_stack, STACK_SIZE, (nano_fiber_entry_t) fiber_i2c_client,
                   0, 0, FIBER_PRIORITY, 0);

    rc = wait_rx_complete(3);
    CU_ASSERT("Time-out wait rx expire \n", rc == true);

    /* Addition of the Tx request */
    tx_request[3]->tx_buff = tx_buff[IND_LIB_C];
    rc = sba_exec_request(tx_request[3]);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    /* Addition of the Trx request */
    trx_request[3]->tx_buff = &trx_buff;
    trx_request[3]->rx_buff = rx_buff[3];
    rc = sba_exec_request(trx_request[3]);
    CU_ASSERT("sba_exec_request() return not ok\n", rc == DRV_RC_OK);

    rc = wait_rx_complete(1);

    CU_ASSERT("Time-out wait rx expire \n", rc == true);
    for (i = 0; i < NB_REQ; i++) {

        CU_ASSERT("i2c written value differs from the read value\n",rx_buff[i][0] == tx_buff[i % 2][1]);
        cu_print("expected value = %d ; read value = %d \n",tx_buff[i % 2][1],rx_buff[i][0] );
    }
    end_multiple_test();
    cu_print("######################################\n");
}


void sba_i2c_test(void) {
    cu_print("########################################################\n");
    cu_print("# Purpose of Serial Bus Access I2C tests :             #\n");
    cu_print("#            Addition of one request (Rx, Tx, Rx / Tx) #\n");
    cu_print("#            Addition of several requests              #\n");
    cu_print("#            Test error cases                          #\n");
    cu_print("########################################################\n");

    test_sba_single_add();
    test_sba_multiple_add();
    test_sba_multiple_add_fiber();
}
