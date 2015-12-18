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

#ifndef __SOC_CONFIG_H_
#define __SOC_CONFIG_H_
#include <stdint.h>

#define CPU_ID_QUARK  0
#define CPU_ID_ARC  1
#define CPU_ID_BLE  2
#define CPU_ID_HOST 3
#define NUM_CPU     4

/**
 * QRK / ARC global shared structure. This structure lies in the beginning of
 * the RAM.
 */
struct platform_shared_block_ {
    /** Arc reset vector */
    unsigned int arc_start;
    /** Port table address */
    void * ports;
    /** Service table address */
    void * services;
    /** Port id of the service manager */
    uint16_t service_mgr_port_id;
    /** ARC boot synchronization flag.
     * This value is set to 0 prior to start ARC, and is polled until set to 1
     * by ARC in order to allow QRK to wait for ARC to be started. Useful for
     * debugging ARC startup code.
     */
    uint8_t arc_ready;

    /** used to send suspend resume arc core
     * bit usage
     * [0-7] 	PM_POWERSTATE
     * [8-9] 	ACK
     * [16-31]	Magic number
     */
    uint32_t pm_request;

    /** ARC wakelocks status info variables
     * Used in order to share if any wakelock
     * is taken, on ARC side.
     */
    uint8_t any_arc_wakelock_taken;

    /** QRK wakelocks status info variables
     * Used in order to share if any wakelock
     * is taken, on QRK side.
     */
    uint8_t any_lmt_wakelock_taken;

};

#define RAM_START           0xA8000000

#define shared_data ((volatile struct platform_shared_block_ *) RAM_START)

/* Use a ROM address as a temporary factory_data pointer */
#define FACTORY_DATA_ADDR 0xffffe000
#define FACTORY_DATA_ROM_ADDR 0x0  /* used for soc_rom driver, corresponds to 0xffffe000 */

#define ADC_VOLTAGE_CHANNEL                 4   /**< ADC channel for Battery voltage measure*/
#define ADC_TEMPERATURE_CHANNEL             10   /**< ADC channel for Battery temperature measure*/

/* GPIO */
// soc gpio 32 bit count
#if defined(CONFIG_SOC_GPIO_32)
#define SOC_GPIO_32_BITS    (32)
#endif

// soc gpio aon bit count
#if defined(CONFIG_SOC_GPIO_AON)
#define SOC_GPIO_AON_BITS    (6)
#endif

#define SS_GPIO_8B0_BITS    (8)
#define SS_GPIO_8B1_BITS    (8)

/* BLE Core PM */
#define BLE_QRK_INT_PORT SOC_GPIO_AON_ID
#define BLE_QRK_INT_PIN  5

#define QRK_BLE_INT_PORT SOC_GPIO_32_ID
#define QRK_BLE_INT_PIN  5

/* IPC channel definition */
#define IPC_SS_QRK_REQ 5
#define IPC_SS_QRK_ACK 6
#define IPC_SS_QRK_ASYNC 4

#define IPC_QRK_SS_REQ 0
#define IPC_QRK_SS_ACK 1
#define IPC_QRK_SS_ASYNC 7

/* I2C */
/*!
* List of all controllers in system ( IA and SS )
*/

typedef enum {
    SOC_I2C_0 = 0,     /*!< General Purpose I2C controller 0, accessible by both processing entities */
    SOC_I2C_1,         /*!< General Purpose I2C controller 1, accessible by both processing entities */
} SOC_I2C_CONTROLLER_PF;

/* SPI */

/*!
 * List of all controllers in host processor
 */
typedef enum {
    SOC_SPI_MASTER_0 = 0,     /* SPI master controller 0, accessible by both processing entities */
    SOC_SPI_MASTER_1,         /* SPI master controller 1, accessible by both processing entities */
    SOC_SPI_SLAVE_0           /* SPI slave controller */
}SOC_SPI_CONTROLLER_PF;

/* SOC COMPARATOR */
/*!
 * Number of analog comparator in Quark
 */
#if defined(CONFIG_SOC_COMPARATOR)
#define CMP_COUNT	    19
#endif

typedef enum {
	ROOT_DEVICE_ID    = 0,
	COMPARATOR_ID     = 1,
	RTC_ID            = 2,
	UART0_PM_ID       = 3,
	UART1_PM_ID       = 4,
	SOC_GPIO_32_ID    = 5,
	SOC_FLASH_ID      = 6,
	SOC_GPIO_AON_ID   = 7,
	SBA_SPI0_ID       = 8,
	SBA_I2C1_ID       = 9,
	SPI_FLASH_0_ID    = 10,
	SBA_SOC_SPI_0_ID  = 11,
	SBA_SOC_I2C1      = 12,
	SBA_SS_SPI_0_ID   = 13,
	SBA_SS_SPI_1_ID   = 14,
	SBA_SS_I2C_0_ID   = 15,
	SPI_BMI160_ID     = 16,
	I2C_BMI160_ID     = 17,
	USB_PM_ID         = 18,
	SOC_LED_ID        = 19,
	HD44780_ID        = 20,
	SS_ADC_ID         = 21,
	SS_GPIO_8B0_ID    = 22,
	SS_GPIO_8B1_ID    = 23,
	WDT_ID            = 24,
	DRV2605_ID        = 25,
	SBA_I2C0_ID       = 26,
	PWM_ID            = 27,
	BLE_CORE_PM_ID      = 28,
	IPC_UART_ID       = 29,
	AON_PT_ID         = 30,
	SBA_SPI1_ID       = 31,
	SS_CLK_GATE       = 32,
	MLAYER_CLK_GATE   = 33,
	QRK_CLK_GATE      = 34,
	NFC_STN54E_ID     = 35,
	APDS9190_ID       = 36,
	LED_LP5562_ID     = 37,
	QRK_PATTERN_MATCHING_ID = 38,
	SPI_BME280_ID     = 39,
	I2C_BME280_ID     = 40,
} DEVICE_ID;

/* SBA_SPI0_ID */
/* SBA_SPI1_ID */
extern struct device pf_bus_sba_spi_0;
extern struct device pf_bus_sba_spi_1;

/* SBA_SOC_SPI_0_ID */
extern struct device pf_bus_sba_soc_spi_0;

/* SBA_SS_SPI_0_ID */
/* SBA_SS_SPI_1_ID */
extern struct device pf_bus_sba_ss_spi_0;
extern struct device pf_bus_sba_ss_spi_1;

/* SBA_I2C0_ID */
/* SBA_I2C1_ID */
extern struct device pf_bus_sba_i2c_0;
extern struct device pf_bus_sba_i2c_1;

/* SBA_SS_I2C_0_ID */
extern struct device pf_bus_sba_ss_i2c_0;

/* SBA_SOC_I2C1 */
extern struct device pf_bus_sba_soc_i2c_1;

/* COMPARATOR_ID */
extern struct device pf_device_soc_comparator;

/* RTC_ID */
extern struct device pf_device_rtc;

/* IPC_UART_ID */
/* UART0_PM_ID */
/* UART1_PM_ID */
extern struct device pf_device_uart_ns16550;
extern struct device pf_device_uart0_pm;
extern struct device pf_device_uart1_pm;

/* SOC_GPIO_32_ID */
extern struct device pf_device_soc_gpio_32;

/* SOC_FLASH_ID */
extern struct device pf_device_soc_flash;

/* SOC_GPIO_AON_ID */
extern struct device pf_device_soc_gpio_aon;

/* SPI_FLASH_0_ID */
extern struct sba_device pf_sba_device_flash_spi0;

/* SPI_BMI160_ID */
extern struct sba_device pf_sba_device_spi_bmi160;

/* I2C_BMI160_ID */
extern struct sba_device pf_sba_device_i2c_bmi160;

/* USB_PM_ID */
extern struct device pf_device_usb_pm;

/* SOC_LED_ID */
extern struct device pf_device_soc_led;

/* HD44780_ID */
extern struct device pf_device_display_hd44780;

/* SS_ADC_ID */
extern struct device pf_device_ss_adc;

/* SS_GPIO_8B0_ID */
/* SS_GPIO_8B1_ID */
extern struct device pf_device_ss_gpio_8b0;
extern struct device pf_device_ss_gpio_8b1;

/* WDT_ID */
extern struct device pf_device_wdt;

/* DRV2605_ID */
extern struct sba_device pf_sba_device_drv2605;

/* PWM_ID */
extern struct device pf_device_pwm;

/* BLE_CORE_PM_ID */
extern struct device pf_device_ble_core_pm;

/* AON_PT_ID */
extern struct device pf_device_aon_pt;

/* SS_CLK_GATE */
/* MLAYER_CLK_GATE */
extern struct device pf_device_ss_clk_gate;
extern struct device pf_device_mlayer_clk_gate;

/* QRK_CLK_GATE */
extern struct device pf_device_clk_gate;

/* NFC_STN54E_ID */
extern struct sba_device pf_sba_device_nfc;

/* APDS9190_ID */
extern struct sba_device pf_sba_device_apds9190;

/* LED_LP5562_ID */
extern struct sba_device pf_sba_device_led_lp5562;

/* QRK_PATTERN_MATCHING_ID */
extern struct device pf_device_pattern_matching;

/* SPI_BME280_ID */
extern struct sba_device pf_sba_device_spi_bme280;

/* I2C_BME280_ID */
extern struct sba_device pf_sba_device_i2c_bme280;


#endif
