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

#ifndef __BMI160_BUS_H__
#define __BMI160_BUS_H__

#define BMI160_SPI_ADDR     SPI_SE_1
#define BMI160_I2C_ADDR1    0x68 /**< SDO=0 */
#define BMI160_I2C_ADDR2    0x69 /**< SDO=1 */
#define BMI160_BMM150_I2C_ADDRESS   0x10/**< I2C address of BMM150*/

#ifdef CONFIG_BMI160_SPI
#define BMI160_PRIMARY_BUS_ADDR BMI160_SPI_ADDR
extern struct driver spi_bmi160_driver;
#else
#define BMI160_PRIMARY_BUS_ADDR BMI160_I2C_ADDR1
extern struct driver i2c_bmi160_driver;
#endif
/*!
 *  @brief Used for I2C/SPI bus initialization
 *  @param dev : BMI sensor device structure
*/
DRIVER_API_RC bmi160_config_bus(struct device *dev);
 /*!
  * @brief : The function is used as I2C/SPI bus read
  * @return : Status of the 2C/SPI read
  * @param reg_addr : Address of the first register,
  * will data is going to be read
  * @param reg_data : This data read from the sensor,
  * which is hold in an array
  * @param cnt : The no of byte of data to be read
  */
DRIVER_API_RC bmi160_bus_read(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);
/*!
  * @brief : The function is used as I2C/SPI bus burst read
  * @return : Status of the 2C/SPI read
  * @param reg_addr : Address of the first register,
  * will data is going to be read
  * @param reg_data : This data read from the sensor,
  * which is hold in an array
  * @param cnt : The no of byte of data to be read
  */
DRIVER_API_RC bmi160_bus_burst_read(uint8_t reg_addr, uint8_t * reg_data, uint32_t cnt);
 /*!
  * @brief : The function is used as I2C/SPI bus write
  * @return : Status of the I2C write
  * @param reg_addr : Address of the first register,
  * will data is going to be written
  * @param reg_data : It is a value hold in the array,
  *     will be used for write the value into the register
  * @param cnt : The no of byte of data to be write
  */
DRIVER_API_RC bmi160_bus_write(uint8_t reg_addr, uint8_t  *reg_data, uint8_t cnt);

static inline DRIVER_API_RC bmi160_write_reg(uint8_t v_addr_u8, uint8_t *v_data_u8)
{
    /* write data from register*/
    return bmi160_bus_write(v_addr_u8, v_data_u8, 1);
}

static inline DRIVER_API_RC bmi160_read_reg(uint8_t v_addr_u8, uint8_t *v_data_u8)
{
    /* Read data from register*/
    return bmi160_bus_read( v_addr_u8, v_data_u8, 1);
}

#endif
