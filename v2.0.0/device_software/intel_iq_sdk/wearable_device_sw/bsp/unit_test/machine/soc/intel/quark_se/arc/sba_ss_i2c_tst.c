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

#include "sba_function.h"
#include "util/cunit_test.h"

#define APDS9900_DEVICE                 (0x39)

#define APDS9900_WAIT_TIME              (0x03)

/* I2C access types */
#define APDS9900_CMD                    (0x80)
#define APDS9900_TYPE_MASK              (0x03 << 5)
#define APDS9900_TYPE_REPEATED_BYTE     (0x00 << 5)
#define APDS9900_TYPE_AUTO_INCREMENT    (0x01 << 5)
#define APDS9900_TYPE_SPECIAL           (0x03 << 5)

/* Time-out test */
#define DELAY                            500000000

static volatile uint8_t i2c_xfer_complete;
static volatile uint8_t i2c_err_detect = 0;

static void ss_i2c_xfer(struct sba_request * request)
{
    i2c_xfer_complete = 1;
}

static uint8_t wait_sba_complete(uint32_t nbtx) {
    uint32_t delay_tx = DELAY;

    while (i2c_xfer_complete < nbtx) {
        delay_tx--;
        if (i2c_err_detect){
            CU_ASSERT("I2C receive error", i2c_err_detect == 0);
            return false;
        }
        if (delay_tx == 0) {
            CU_ASSERT("timeout: I2C receive not complete", delay_tx != 0);
            return false;
        }
    }
    return true;
}

static void reset_callback(void)
{
    i2c_xfer_complete = 0;
    i2c_err_detect = 0;
}

void sba_i2c_write_reg(SBA_BUSID bus_id, uint8_t reg, uint8_t value, uint32_t target_addr, void (*callback)(struct sba_request *))
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[2] = {0};

    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_REPEATED_BYTE;

    buffer[0] = reg;
    buffer[1] = value;
    ret = sba_i2c_write(bus_id, buffer, 2, target_addr, callback);

    CU_ASSERT ("I2C write failure", ret == DRV_RC_OK);
}

void sba_i2c_transfer_reg(SBA_BUSID bus_id, uint8_t reg, uint8_t* read_data, uint32_t read_data_len, uint32_t target_addr, void (*callback)(struct sba_request *))
{
    DRIVER_API_RC ret = DRV_RC_OK;


    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_REPEATED_BYTE;

    ret = sba_i2c_transfer(bus_id, &reg, 1, read_data, 1, target_addr, callback);
    CU_ASSERT ("I2C transfer reg failure", ret == DRV_RC_OK);

}

void sba_i2c_select_reg(SBA_BUSID bus_id, uint8_t reg, uint32_t target_addr, void (*callback)(struct sba_request *))
{
    DRIVER_API_RC ret = DRV_RC_OK;

    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_AUTO_INCREMENT;

    ret = sba_i2c_write(bus_id, &reg, 1, target_addr, callback);
    CU_ASSERT ("I2C select failure", ret == DRV_RC_OK);
}

void sba_i2c_read_reg(SBA_BUSID bus_id, uint8_t *data, uint32_t size, uint32_t target_addr, void (*callback)(struct sba_request *))
{
    DRIVER_API_RC ret = DRV_RC_OK;

    ret = sba_i2c_read(bus_id, data, size, target_addr, callback);
    CU_ASSERT ("I2C read failure", ret == DRV_RC_OK);
}

void sba_ss_unit_i2c(void)
{
    uint8_t read_data[5] = {};
    uint8_t write_data;

    // ss i2c0 bus is connected to APDS9900 proximity sensor
    cu_print("#####################################################\n");
    cu_print("# SS I2C0 is connected to APDS9900 proximity sensor #\n");
    cu_print("#                                                   #\n");
    cu_print("# Purpose of sba ss i2c tests :                     #\n");
    cu_print("#            use serial bus access                  #\n");
    cu_print("#            Write and read register                #\n");
    cu_print("#            Select a register and start reading    #\n");
    cu_print("#####################################################\n");

    reset_callback();
    write_data = 0x55;
    sba_i2c_write_reg(SBA_SS_I2C_MASTER_0, APDS9900_WAIT_TIME, write_data, APDS9900_DEVICE, NULL);
    sba_i2c_transfer_reg(SBA_SS_I2C_MASTER_0, APDS9900_WAIT_TIME, read_data, 1, APDS9900_DEVICE, ss_i2c_xfer);
    wait_sba_complete(1);
    cu_print("write value %x, read value %x\n",write_data, *read_data);
    CU_ASSERT("expected value differs from the read value", write_data == *read_data);

    reset_callback();
    write_data = 0x66;
    sba_i2c_write_reg(SBA_SS_I2C_MASTER_0, APDS9900_WAIT_TIME, write_data, APDS9900_DEVICE, NULL);
    sba_i2c_transfer_reg(SBA_SS_I2C_MASTER_0, APDS9900_WAIT_TIME, read_data, 1, APDS9900_DEVICE, ss_i2c_xfer);
    wait_sba_complete(1);
    cu_print("write value %x, read value %x\n",write_data, *read_data);
    CU_ASSERT("expected value differs from the read value", write_data == *read_data);

    reset_callback();
    sba_i2c_select_reg(SBA_SS_I2C_MASTER_0, APDS9900_WAIT_TIME, APDS9900_DEVICE, NULL);
    sba_i2c_read_reg(SBA_SS_I2C_MASTER_0, read_data, 5, APDS9900_DEVICE, ss_i2c_xfer);
    wait_sba_complete(1);
    cu_print("write value %x, read value %x\n", write_data, read_data[0]);
    CU_ASSERT("expected value differs from the read value", write_data == read_data[0]);
}
