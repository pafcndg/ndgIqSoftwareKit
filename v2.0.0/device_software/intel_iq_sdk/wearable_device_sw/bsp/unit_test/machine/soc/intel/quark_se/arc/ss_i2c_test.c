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

#include "util/cunit_test.h"
#include "drivers/ss_i2c_iface.h"
#include "drivers/serial_bus_access.h"

#define APDS9900_DEVICE                 (0x39)
#define TIMEOUT 0xFFFFF

#define APDS9900_WAIT_TIME              (0x03)

/* I2C access types */
#define APDS9900_CMD                    (0x80)
#define APDS9900_TYPE_MASK              (0x03 << 5)
#define APDS9900_TYPE_REPEATED_BYTE     (0x00 << 5)
#define APDS9900_TYPE_AUTO_INCREMENT    (0x01 << 5)
#define APDS9900_TYPE_SPECIAL           (0x03 << 5)

volatile uint8_t i2c_tx_complete = 0;
volatile uint8_t i2c_rx_complete = 0;
volatile uint8_t i2c_err_detect = 0;

static void ss_i2c_rx(uint32_t dev_id)
{
    i2c_rx_complete = 1;
}

static void ss_i2c_tx(uint32_t dev_id)
{
    i2c_tx_complete = 1;
}

static void ss_i2c_err(uint32_t dev_id)
{
    i2c_err_detect = 1;
}

static void wait_rx_or_err(){
    uint64_t timeout = TIMEOUT;
    while(timeout--)
        if ((i2c_rx_complete) || (i2c_err_detect))
            break;
    CU_ASSERT("I2C receive error", i2c_err_detect == 0);
    CU_ASSERT("timeout: I2C receive not complete", timeout != 0);
}

static void wait_tx_or_err(){
    uint64_t timeout = TIMEOUT;
    while(timeout--)
        if ((i2c_tx_complete) || (i2c_err_detect))
               break;
    CU_ASSERT("I2C write error", i2c_err_detect == 0);
    CU_ASSERT("timeout: I2C write not complete", timeout != 0);
}

static void wait_dev_ready(I2C_CONTROLLER controller_id){
    uint64_t timeout = TIMEOUT;
    while(timeout--)
        if (ss_i2c_status(controller_id) == I2C_OK)
                break;
}

static void reset_callback(void)
{
    i2c_rx_complete = 0;
    i2c_tx_complete = 0;
    i2c_err_detect = 0;
}

void i2c_write_reg(I2C_CONTROLLER controller_id, uint8_t reg, uint8_t value, uint32_t target_addr)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[2] = {0};

    reset_callback();

    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_REPEATED_BYTE;

    buffer[0] = reg;
    buffer[1] = value;

    ret = ss_i2c_write(controller_id, buffer, sizeof(buffer), target_addr);
    wait_tx_or_err();
    wait_dev_ready(controller_id);
    CU_ASSERT ("I2C write failure", ret == DRV_RC_OK);
}

void i2c_transfer_reg(I2C_CONTROLLER controller_id, uint8_t reg, uint8_t expected_val, uint32_t target_addr)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t read_data = 0;

    reset_callback();

    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_REPEATED_BYTE;

    ret = ss_i2c_transfer(controller_id, &reg, 1, &read_data, 1, target_addr);
    wait_rx_or_err();
    wait_dev_ready(controller_id);
    cu_print("READ DATA %x\n", read_data);
    CU_ASSERT ("I2C read failure", ret == DRV_RC_OK);
    CU_ASSERT ("I2C expected value differs from the read value", expected_val == read_data);
}

void i2c_select_reg(I2C_CONTROLLER controller_id, uint8_t reg, uint32_t target_addr)
{
    DRIVER_API_RC ret = DRV_RC_OK;

    reset_callback();

    reg &= ~APDS9900_TYPE_MASK;
    reg |= APDS9900_CMD | APDS9900_TYPE_AUTO_INCREMENT;

    ret = ss_i2c_write(controller_id, &reg, 1, target_addr);
    wait_tx_or_err();
    wait_dev_ready(controller_id);
    CU_ASSERT ("I2C read failure", ret == DRV_RC_OK);
}

void i2c_read_reg(I2C_CONTROLLER controller_id, uint8_t *data, uint32_t size, uint32_t target_addr)
{
    DRIVER_API_RC ret = DRV_RC_OK;

    reset_callback();

    ret = ss_i2c_read(controller_id, data, size, target_addr);
    wait_rx_or_err();
    wait_dev_ready(controller_id);
    CU_ASSERT ("I2C read failure", ret == DRV_RC_OK);
#ifdef DEBUG
    for (uint32_t i = 0; i < size; i++)
		cu_print("READ DATA %x\n", data[i]);
#endif
}

static void enable_i2c(struct sba_master_cfg_data* sba_dev)
{
    i2c_cfg_data_t cfg = {0};
    DRIVER_API_RC ret = DRV_RC_OK;

    CU_ASSERT("Invalid sba i2c device", sba_dev != NULL);

    cfg.speed = I2C_SLOW;
    cfg.addressing_mode = I2C_7_Bit;
    cfg.mode_type = I2C_MASTER;
    cfg.cb_tx = ss_i2c_tx;
    cfg.cb_rx = ss_i2c_rx;
    cfg.cb_err = ss_i2c_err;

    ret = ss_i2c_set_config(get_bus_id_from_sba(sba_dev->bus_id), &cfg);
    CU_ASSERT ("I2C configuration failure", ret == DRV_RC_OK);
    ret = ss_i2c_clock_enable(sba_dev);
    CU_ASSERT ("I2C clock enable failure", ret == DRV_RC_OK);
    wait_dev_ready(get_bus_id_from_sba(sba_dev->bus_id));
}

static void disable_i2c(struct sba_master_cfg_data* sba_dev)
{
    DRIVER_API_RC ret = DRV_RC_OK;

    ret = ss_i2c_clock_disable(sba_dev);
    CU_ASSERT("I2C clock disable failed", ret == DRV_RC_OK);
}

void ss_unit_i2c(void)
{
    uint32_t nb = 5;
    uint8_t read_data[nb];
    uint8_t write_data = 0x55;

    // ss i2c0 bus is connected to APDS9900 proximity sensor
    cu_print("#####################################################\n");
    cu_print("# SS I2C0 is connected to APDS9900 proximity sensor #\n");
    cu_print("#                                                   #\n");
    cu_print("# Purpose of ss i2c tests :                         #\n");
    cu_print("#            Write and read register                #\n");
    cu_print("#            Select a register and start reading    #\n");
    cu_print("#####################################################\n");

    enable_i2c((struct sba_master_cfg_data*)(pf_bus_sba_ss_i2c_0.priv));

    i2c_write_reg(I2C_SENSING_0, APDS9900_WAIT_TIME, write_data, APDS9900_DEVICE);
    i2c_transfer_reg(I2C_SENSING_0, APDS9900_WAIT_TIME, write_data, APDS9900_DEVICE);

    i2c_select_reg(I2C_SENSING_0, APDS9900_WAIT_TIME, APDS9900_DEVICE);
    i2c_read_reg(I2C_SENSING_0, read_data, nb, APDS9900_DEVICE);

    disable_i2c((struct sba_master_cfg_data*)(pf_bus_sba_ss_i2c_0.priv));

}
