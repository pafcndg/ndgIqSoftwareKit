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

#ifndef __BME280_BUS_H__
#define __BME280_BUS_H__


#ifdef CONFIG_BME280_SPI
#define BME280_SBA_ADDR SPI_SE_1
#else
#define BME280_SBA_ADDR 0x76
#endif

extern struct driver sba_bme280_driver;

DRIVER_API_RC bme280_bus_read(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt);
DRIVER_API_RC bme280_bus_burst_read(uint8_t reg_addr, uint8_t * reg_data, uint32_t cnt);
DRIVER_API_RC bme280_bus_write(uint8_t reg_addr, uint8_t * reg_data, uint8_t cnt);

static inline DRIVER_API_RC bme280_write_reg(uint8_t v_addr_u8, uint8_t *v_data_u8)
{
    /* write data from register*/
    return bme280_bus_write(v_addr_u8, v_data_u8, 1);
}

static inline DRIVER_API_RC bme280_read_reg(uint8_t v_addr_u8, uint8_t *v_data_u8)
{
    /* Read data from register*/
    return bme280_bus_read( v_addr_u8, v_data_u8, 1);
}

#endif