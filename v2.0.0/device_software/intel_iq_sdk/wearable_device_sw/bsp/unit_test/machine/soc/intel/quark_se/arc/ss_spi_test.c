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

#include "drivers/ss_spi_iface.h"
#include "util/cunit_test.h"

#define ID_REG          (0xD0)
#define CALLIB_0        (0xE1)
#define CTRL_MEAS       (0xF4)
#define CONFIG          (0xF5)
#define HUM_LSB         (0xFE)

#define WRITE_CMD       ~(1 << 7)

#define TIMEOUT         0xFFFFF

spi_cfg_data_t spi_cfg;

volatile uint8_t ss_spi_xfer_complete = 0;
volatile uint8_t ss_spi_err_detect = 0;

static void ss_spi_xfer(uint32_t dev_id)
{
    ss_spi_xfer_complete = 1;
}

static void ss_spi_err(uint32_t dev_id)
{
     ss_spi_err_detect = 1;
}

static void wait_xfer_or_err(){
    uint64_t timeout = TIMEOUT;
    while(timeout--){
        if ((ss_spi_xfer_complete) || (ss_spi_err_detect))
            break;
    }
    CU_ASSERT("SS SPI receive error", ss_spi_err_detect == 0);
    CU_ASSERT("timeout: SS SPI receive not complete", ss_spi_xfer_complete == 1);
}


static void reset_callback(void)
{
    ss_spi_xfer_complete = 0;
    ss_spi_err_detect = 0;
}

void spi_write_reg(SPI_CONTROLLER spi_controller, uint8_t reg, uint8_t value)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[2] = {0};

    buffer[0] = reg & WRITE_CMD;
    buffer[1] = value;

    reset_callback();
    ret = ss_spi_transfer(spi_controller, buffer, sizeof(buffer), NULL , 0, spi_cfg.slave_enable);
    CU_ASSERT ("SPI write failure", ret == DRV_RC_OK);
    wait_xfer_or_err();
}


void spi_write_word_reg(SPI_CONTROLLER spi_controller, uint8_t reg, uint16_t value)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t buffer[4] = {0};

    reset_callback();

    buffer[0] = reg & WRITE_CMD;
    buffer[1] = value;
    buffer[2] = (reg+1) & WRITE_CMD;
    buffer[3] = value >> 8;

    ret = ss_spi_transfer(spi_controller, buffer, sizeof(buffer), NULL , 0, spi_cfg.slave_enable);
    CU_ASSERT ("SPI write failure", ret == DRV_RC_OK);
    wait_xfer_or_err();
}

void spi_read_reg(SPI_CONTROLLER spi_controller, uint8_t reg, uint8_t expected_val)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint8_t read_data = 0;

    reset_callback();

    ret = ss_spi_transfer(spi_controller, &reg, 1, &read_data , 1, spi_cfg.slave_enable);
    CU_ASSERT ("SPI read failure", ret == DRV_RC_OK);
    wait_xfer_or_err();
#ifdef DEBUG
    cu_print("read value %x expected value %x\n", read_data, expected_val);
#endif
    CU_ASSERT ("SPI expected value differs from the read value", expected_val == read_data);
}

void spi_read_word_reg(SPI_CONTROLLER spi_controller, uint8_t reg, uint16_t expected_val)
{
    DRIVER_API_RC ret = DRV_RC_OK;
    uint16_t read_data = 0;

    reset_callback();

    ret = ss_spi_transfer(spi_controller, &reg, 1, (uint8_t*)&read_data , 2, spi_cfg.slave_enable);
    CU_ASSERT ("SPI read failure", ret == DRV_RC_OK);
    wait_xfer_or_err();
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

void transfer_block_spi(SPI_CONTROLLER spi_controller)
{
    uint8_t write_buffer[1] = {CALLIB_0};
    uint8_t read_buffer[HUM_LSB - CALLIB_0] = {0};
    DRIVER_API_RC ret = 0;
    //uint32_t idx;
    cu_print("read %d bytes\n", sizeof(read_buffer));
    reset_callback();
    ret = ss_spi_transfer(spi_controller, write_buffer, 1, read_buffer, \
                          sizeof(read_buffer), spi_cfg.slave_enable);
    CU_ASSERT("Sending block bytes TX buffer failed\n", (ret == DRV_RC_OK));
    wait_xfer_or_err();
#ifdef DEBUG
    dump_data(read_buffer, sizeof(read_buffer));
#endif
    CU_ASSERT("ss spi written value differs from the read value", read_buffer[CTRL_MEAS - CALLIB_0] == 0x04);
    CU_ASSERT("ss spi written value differs from the read value", read_buffer[CONFIG - CALLIB_0] == 0x40);
}

void read_block_spi(SPI_CONTROLLER spi_controller)
{
    uint8_t read_buffer[HUM_LSB - CALLIB_0] = {0xff};
    DRIVER_API_RC ret = 0;
    //uint32_t idx;
    cu_print("read block 2 : %d bytes\n", sizeof(read_buffer));
    reset_callback();
    ret = ss_spi_transfer(spi_controller, 0, 0, read_buffer, \
                          sizeof(read_buffer), spi_cfg.slave_enable);
    wait_xfer_or_err();
    CU_ASSERT("Sending block bytes TX buffer failed\n", (ret == DRV_RC_OK));
#ifdef DEBUG
    dump_data(read_buffer, sizeof(read_buffer));
#endif
}

static void init_spi(struct sba_master_cfg_data* sba_dev, SPI_SLAVE_ENABLE spi_slave, SPI_BUS_MODE spi_bus_mode, uint8_t loopback)
{
    DRIVER_API_RC ret = 0;

    CU_ASSERT("Invalid sba spi device", sba_dev != NULL);

    spi_cfg.speed              = 20;
    spi_cfg.data_frame_size    = SPI_8_BIT;
    spi_cfg.slave_enable       = spi_slave;
    spi_cfg.bus_mode           = spi_bus_mode;
    spi_cfg.cb_xfer            = ss_spi_xfer;
    spi_cfg.cb_err             = ss_spi_err;

    ret = ss_spi_clock_enable(sba_dev);
    CU_ASSERT("SPI enable clock: failed", ret == DRV_RC_OK);

    ret = ss_spi_set_config(get_bus_id_from_sba(sba_dev->bus_id), &spi_cfg);
    CU_ASSERT("SPI set config: failed", ret == DRV_RC_OK);
}



void ss_unit_spi(void)
{

    cu_print("#######################################\n");
    cu_print("# Purpose of SS SPI tests :           #\n");
    cu_print("#     BME280 is connected to ss spi0  #\n");
    cu_print("#     1 - Read BME280 ID              #\n");
    cu_print("#     2 - Write BME280 registers      #\n");
    cu_print("#######################################\n");

    init_spi((struct sba_master_cfg_data*)(pf_bus_sba_ss_spi_0.priv),
	     SPI_SE_1, SPI_BUSMODE_0, 0);
    cu_print("read register\n");
    spi_read_reg(SPI_SENSING_0, ID_REG, 0x60);

    cu_print("write register\n");
    spi_write_reg(SPI_SENSING_0, CTRL_MEAS, 0x08);
    spi_read_reg(SPI_SENSING_0, CTRL_MEAS, 0x08);

    cu_print("write word register\n");
    spi_write_word_reg(SPI_SENSING_0, CTRL_MEAS, 0x4004);

    cu_print("read word register\n");
    spi_read_word_reg(SPI_SENSING_0, CTRL_MEAS, 0x4004);

    cu_print("read block\n");
    transfer_block_spi(SPI_SENSING_0);

    ss_spi_deconfig((struct sba_master_cfg_data*)(pf_bus_sba_ss_spi_0.priv));
}
