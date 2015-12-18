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

#include "infra/wakelock_ids.h"
#include "machine/soc/intel/quark_se/soc_config.h"
#include "drivers/serial_bus_access.h"
#include "drivers/sensor/bmi160_bus.h"
#include "drivers/sensor/bme280_bus.h"
#include "drivers/ss_adc.h"
#include "drivers/eiaextensions.h"
#include "machine/soc/intel/quark_se/arc/soc_register.h"
#include "drivers/io_config.h"
#include "drivers/soc_gpio.h"
#include "drivers/ss_gpio_iface.h"
#include "drivers/soc_comparator.h"
#include "drivers/usb_pm.h"
#include "drivers/ble_core_pm.h"
#include "drivers/clk_system.h"
#include "util/misc.h"

#ifdef CONFIG_PATTERN_MATCHING_DRV
#include "intel_qrk_pattern_matching.h"
#endif

#define PLATFORM_INIT(devices, buses) \
	init_devices(devices, (unsigned int)(sizeof(devices)/sizeof(*devices)), \
				 buses,   (unsigned int)(sizeof(buses)/sizeof(*buses)))

DECLARE_SUSPEND_BLOCKERS(
#if defined(CONFIG_USB_PM_SUSPEND_BLOCKERS)
	{
		.cb = usb_pm_is_suspend_allowed,
		.dev = &pf_device_usb_pm
	},
#endif
#ifdef CONFIG_BLE_CORE_SUSPEND_BLOCKER_PM
	{
		.cb = ble_core_pm_can_sleep,
		.dev = &pf_device_ble_core_pm
	},
#endif
	);

#ifdef CONFIG_SS_SPI

/* sba ss_spi1 devices (to statically init device tree) */
#if defined(CONFIG_BMI160) && defined(CONFIG_BMI160_SPI)
struct sba_device pf_sba_device_spi_bmi160 = {
		.dev.id = SPI_BMI160_ID,
		.dev.driver = &spi_bmi160_driver,
		.dev.parent = &pf_bus_sba_ss_spi_1,
		.addr.cs = BMI160_PRIMARY_BUS_ADDR,
	};
#endif

#if defined(CONFIG_BME280) && defined (CONFIG_BME280_SPI)
struct sba_device pf_sba_device_spi_bme280 = {
		.dev.id = SPI_BME280_ID,
		.dev.driver = &sba_bme280_driver,
		.dev.parent = &pf_bus_sba_ss_spi_1,
		.addr.cs = BME280_SBA_ADDR,
	},
#endif

#endif /* CONFIG_SS_SPI */

#ifdef CONFIG_SS_I2C

/* sba ss_i2c0 devices (to statically init device tree) */

#if defined(CONFIG_BMI160) && !defined(CONFIG_BMI160_SPI)
struct sba_device pf_sba_device_i2c_bmi160 = {
		.dev.id = I2C_BMI160_ID,
		.dev.driver = &i2c_bmi160_driver,
		.dev.parent = &pf_bus_sba_ss_i2c_0,
		.addr.slave_addr = BMI160_PRIMARY_BUS_ADDR,
	};
#endif
#if defined(CONFIG_BME280) && defined(CONFIG_BME280_I2C)
struct sba_device pf_sba_device_i2c_bme280 = {
	.dev.id = I2C_BME280_ID,
	.dev.driver = &sba_bme280_driver,
	.dev.parent = &pf_bus_sba_ss_i2c_0,
	.addr.slave_addr = BME280_SBA_ADDR,
	};
#endif

#endif /* CONFIG_SS_I2C */


/* Configuration of sba master devices (bus) */

/* Array of arc bus devices (sba buses etc ...) */

#ifdef CONFIG_INTEL_QRK_SPI
static struct sba_master_cfg_data qrk_sba_soc_spi_0_cfg = {
	.bus_id				= SBA_SPI_MASTER_0,
	.config.spi_config		= {
		.speed			= 250,                  /*!< SPI bus speed in KHz */
		.txfr_mode		= SPI_TX_RX,            /*!< Transfer mode */
		.data_frame_size	= SPI_8_BIT,            /*!< Data Frame Size ( 4 - 16 bits ) */
		.slave_enable		= SPI_SE_3,             /*!< Slave Enable, Flash Memory is on CS3 */
		.bus_mode		= SPI_BUSMODE_0,        /*!< SPI bus mode is 0 by default */
		.spi_mode_type		= SPI_MASTER,           /*!< SPI 0 is in master mode */
		.loopback_enable	= 0                     /*!< Loopback disabled by default */
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= PERIPH_CLK_GATE_CTRL,
		.bits_mask		= SPI0_CLK_GATE_MASK,
	},
};
struct device pf_bus_sba_soc_spi_0 = {
	.id = SBA_SOC_SPI_0_ID,
	.driver = &serial_bus_access_driver,
	.priv = &qrk_sba_soc_spi_0_cfg,
};
#endif

#ifdef CONFIG_INTEL_QRK_I2C
static struct sba_master_cfg_data qrk_sba_soc_i2c_1_cfg = {
	.bus_id				= SBA_I2C_MASTER_1,
	.config.i2c_config		= {
		.speed			= I2C_SLOW,
		.addressing_mode	= I2C_7_Bit,
		.mode_type		= I2C_MASTER
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= PERIPH_CLK_GATE_CTRL,
		.bits_mask		= I2C1_CLK_GATE_MASK,
	},
};

struct device pf_bus_sba_soc_i2c_1 = {
	.id = SBA_SOC_I2C1,
	.driver = &serial_bus_access_driver,
	.priv = &qrk_sba_soc_i2c_1_cfg,
};
#endif

#ifdef CONFIG_SS_SPI
static struct sba_master_cfg_data arc_sba_ss_spi_0_cfg = {
	.bus_id				= SBA_SS_SPI_MASTER_0,
	.config.spi_config		= {
		.speed			= 20,                   /*!< SPI bus speed in KHz */
		.txfr_mode		= SPI_TX_RX,            /*!< Transfer mode */
		.data_frame_size	= SPI_8_BIT,            /*!< Data Frame Size ( 4 - 16 bits ) */
		.slave_enable		= SPI_SE_1,             /*!< Slave Enable, Flash Memory is on CS3  */
		.bus_mode		= SPI_BUSMODE_0,        /*!< SPI bus mode is 0 by default */
		.spi_mode_type		= SPI_MASTER,           /*!< SPI 0 is in master mode */
		.loopback_enable	= 0                     /*!< Loopback disabled by default */
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= SS_PERIPH_CLK_GATE_CTL,
		.bits_mask		= SS_SPI0_CLK_GATE_MASK,
	},
};

static struct sba_master_cfg_data arc_sba_ss_spi_1_cfg = {
	.bus_id				= SBA_SS_SPI_MASTER_1,
	.config.spi_config		= {
		.speed			= 250,                          /*!< SPI bus speed in KHz */
		.txfr_mode		= SPI_TX_RX,                    /*!< Transfer mode */
		.data_frame_size	= SPI_8_BIT,                    /*!< Data Frame Size ( 4 - 16 bits ) */
		.slave_enable		= BMI160_PRIMARY_BUS_ADDR,      /*!< Slave Enable, Flash Memory is on CS3  */
		.bus_mode		= SPI_BUSMODE_0,                /*!< SPI bus mode is 0 by default */
		.spi_mode_type		= SPI_MASTER,                   /*!< SPI 0 is in master mode */
		.loopback_enable	= 0                             /*!< Loopback disabled by default */
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= SS_PERIPH_CLK_GATE_CTL,
		.bits_mask		= SS_SPI1_CLK_GATE_MASK,
	},
};

struct device pf_bus_sba_ss_spi_0 = {
	.id = SBA_SS_SPI_0_ID,
	.driver = &serial_bus_access_driver,
	.priv = &arc_sba_ss_spi_0_cfg,
};
struct device pf_bus_sba_ss_spi_1 = {
	.id = SBA_SS_SPI_1_ID,
	.driver = &serial_bus_access_driver,
	.priv = &arc_sba_ss_spi_1_cfg,
};
#endif

#ifdef CONFIG_SS_I2C
static struct sba_master_cfg_data arc_sba_ss_i2c_0_cfg = {
	.bus_id					= SBA_SS_I2C_MASTER_0,
	.config.i2c_config		= {
		.speed				= I2C_FAST,
		.addressing_mode	= I2C_7_Bit,
		.mode_type			= I2C_MASTER,
#ifdef BOARD_ATLASPEAK_APP
		.slave_adr			= BMI160_I2C_ADDR1
#else
		.slave_adr			= BME280_SBA_ADDR
#endif
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= SS_PERIPH_CLK_GATE_CTL,
		.bits_mask			= SS_I2C0_CLK_GATE_MASK,
	},
};

struct device pf_bus_sba_ss_i2c_0 = {
	.id = SBA_SS_I2C_0_ID,
	.driver = &serial_bus_access_driver,
	.priv = &arc_sba_ss_i2c_0_cfg,
};
#endif


/* List of devices */

#ifdef CONFIG_CLK_SYSTEM
struct device pf_device_ss_clk_gate = {
	.id = SS_CLK_GATE,
	.driver = &clk_system_driver,
	.priv = &(struct clk_gate_info_s)
	{
		.clk_gate_register = SS_PERIPH_CLK_GATE_CTL,
		.bits_mask = SS_CLK_GATE_INIT_VALUE,
	}
};

struct device pf_device_mlayer_clk_gate = {
	.id = MLAYER_CLK_GATE,
	.driver = &clk_system_driver,
	.priv = &(struct clk_gate_info_s)
	{
		.clk_gate_register = MLAYER_AHB_CTL,
		.bits_mask = MLAYER_CLK_GATE_INIT_VALUE,
	}
};
#endif
#ifdef CONFIG_SS_ADC
struct device pf_device_ss_adc = {
		.id = SS_ADC_ID,
		.driver = &ss_adc_driver,
		.priv = &(struct adc_info_t){
			.reg_base = AR_IO_ADC0_SET,
			.creg_slv = AR_IO_CREG_SLV0_OBSR,
			.creg_mst = AR_IO_CREG_MST0_CTRL,
			.rx_vector = IO_ADC0_INT_IRQ,
			.err_vector = IO_ADC0_INT_ERR,
			.fifo_tld = IO_ADC0_FS / 2,
			.adc_irq_mask = SCSS_REGISTER_BASE +
					INT_SS_ADC_IRQ_MASK,
			.adc_err_mask = SCSS_REGISTER_BASE +
					INT_SS_ADC_ERR_MASK,
			.wakelock.id = ADC_WAKELOCK,
			.clk_gate_info = &(struct clk_gate_info_s){
				.clk_gate_register = SS_PERIPH_CLK_GATE_CTL,
				.bits_mask = SS_ADC_CLK_GATE_MASK,
			},
		},
	};
#endif
#ifdef CONFIG_SOC_GPIO_AON
struct device pf_device_soc_gpio_aon = {
		.id = SOC_GPIO_AON_ID,
		.driver = &soc_gpio_driver,
		.priv = &(gpio_info_t){
			.reg_base = SOC_GPIO_AON_BASE_ADDR,
			.no_bits = SOC_GPIO_AON_BITS,
			.gpio_int_mask = INT_AON_GPIO_MASK,
			.vector = SOC_GPIO_AON_INTERRUPT,
			.gpio_isr = gpio_aon_isr,
			.gpio_cb = (gpio_callback_fn[SOC_GPIO_AON_BITS]) { NULL },
			.gpio_cb_arg = (void *[SOC_GPIO_AON_BITS]) { NULL }
		}
	};
#endif
#ifdef CONFIG_SOC_GPIO_32
struct device pf_device_soc_gpio_32 = {
		.id = SOC_GPIO_32_ID,
		.driver = &soc_gpio_driver,
		.priv = &(gpio_info_t) {
			.reg_base = SOC_GPIO_BASE_ADDR,
			.no_bits = SOC_GPIO_32_BITS,
			.gpio_int_mask = INT_GPIO_MASK,
			.vector = SOC_GPIO_INTERRUPT,
			.gpio_isr = gpio_isr,
			.gpio_cb = (gpio_callback_fn[SOC_GPIO_32_BITS]) { NULL },
			.gpio_cb_arg = (void *[SOC_GPIO_32_BITS]) { NULL }
		}
	};
#endif
#ifdef CONFIG_SS_GPIO
struct device pf_device_ss_gpio_8b0 = {
		.id = SS_GPIO_8B0_ID,
		.driver = &ss_gpio_driver,
		.priv = &(gpio_info_t) {
			.reg_base = AR_IO_GPIO_8B0_SWPORTA_DR,
			.no_bits = SS_GPIO_8B0_BITS,
			.gpio_int_mask = INT_SS_GPIO_0_INTR_MASK,
			.vector = IO_GPIO_8B0_INT_INTR_FLAG,
			.gpio_isr = ss_gpio_8b0_ISR,
			.gpio_cb = (gpio_callback_fn[SS_GPIO_8B0_BITS]) { NULL },
			.gpio_cb_arg = (void *[SS_GPIO_8B0_BITS]) { NULL }
		}
	};

struct device pf_device_ss_gpio_8b1 = {
		.id = SS_GPIO_8B1_ID,
		.driver = &ss_gpio_driver,
		.priv = &(gpio_info_t) {
			.reg_base = AR_IO_GPIO_8B1_SWPORTA_DR,
			.no_bits = SS_GPIO_8B1_BITS,
			.gpio_int_mask = INT_SS_GPIO_1_INTR_MASK,
			.vector = IO_GPIO_8B1_INT_INTR_FLAG,
			.gpio_isr = ss_gpio_8b1_ISR,
			.gpio_cb = (gpio_callback_fn[SS_GPIO_8B1_BITS]) { NULL },
			.gpio_cb_arg = (void *[SS_GPIO_8B1_BITS]) { NULL }
		}
	};
#endif
#ifdef CONFIG_SOC_COMPARATOR
struct device pf_device_soc_comparator = {
		.id = COMPARATOR_ID,
		.driver = &soc_comparator_driver,
		.priv = (struct cmp_cb[CMP_COUNT]) {}
	};
#endif
#if defined(CONFIG_USB_PM) && defined(CONFIG_BOARD_CURIE_APP)
struct device pf_device_usb_pm = {
		.id = USB_PM_ID,
		.driver = &usb_pm_driver,
		.priv = &(struct usb_pm_info) {
			.evt_dev = &pf_device_soc_gpio_aon,
			.interrupt_source = USB_AON_IRQ_SOURCE,
			.source_pin = 0, // USB Vbus is connected on gpio aon 0 for APP
#ifdef CONFIG_SOC_GPIO_32
			.vusb_enable_dev = &pf_device_soc_gpio_32,
			.vusb_enable_pin = 28,
#endif
		}
	};
#endif
#ifdef CONFIG_BLE_CORE_SUSPEND_BLOCKER_PM
struct device pf_device_ble_core_pm = {
		.id = BLE_CORE_PM_ID,
		.driver = &ble_core_pm_driver,
		.priv = &(struct ble_core_pm_info) {
			.gpio_dev = &pf_device_soc_gpio_aon,
			.wakeup_pin = BLE_QRK_INT_PIN // gpio aon 5 is BLE_QRK_INT on Curie
		}
	};
#endif
#ifdef CONFIG_PATTERN_MATCHING_DRV
struct device pf_device_pattern_matching = {
		.id = QRK_PATTERN_MATCHING_ID,
		.driver = &qrk_pattern_matching_driver,
		.priv = &(struct pattern_matching_info_t) {
			.clk_gate_info = &(struct clk_gate_info_s) {
				.clk_gate_register = MLAYER_AHB_CTL,
				.bits_mask = CCU_PATTERN_MATCHING_CLK_GATE_MASK,
			},
		}
	};
#endif


/* Array of arc platform devices (on die memory, spi slave etc ...) */

static struct device *arc_platform_devices[] = {

#ifdef CONFIG_CLK_SYSTEM
	&pf_device_ss_clk_gate,

	&pf_device_mlayer_clk_gate,
#endif
#ifdef CONFIG_SS_ADC
	&pf_device_ss_adc,
#endif
#ifdef CONFIG_SOC_GPIO_AON
	&pf_device_soc_gpio_aon,
#endif
#ifdef CONFIG_SOC_GPIO_32
	&pf_device_soc_gpio_32,
#endif
#ifdef CONFIG_SS_GPIO
	&pf_device_ss_gpio_8b0,
	&pf_device_ss_gpio_8b1,
#endif
#ifdef CONFIG_SOC_COMPARATOR
	&pf_device_soc_comparator,
#endif
#if defined(CONFIG_USB_PM) && defined(CONFIG_BOARD_CURIE_APP)
	&pf_device_usb_pm,
#endif
#ifdef CONFIG_BLE_CORE_SUSPEND_BLOCKER_PM
	&pf_device_ble_core_pm,
#endif
#ifdef CONFIG_PATTERN_MATCHING_DRV
	&pf_device_pattern_matching,
#endif

/* Bus devices */
#ifdef CONFIG_INTEL_QRK_SPI
	&pf_bus_sba_soc_spi_0,
#endif
#ifdef CONFIG_INTEL_QRK_I2C
	&pf_bus_sba_soc_i2c_1,
#endif
#ifdef CONFIG_SS_SPI
	&pf_bus_sba_ss_spi_0,

/* SS SPI1 bus and devices */
	&pf_bus_sba_ss_spi_1,
#if defined(CONFIG_BMI160) && defined(CONFIG_BMI160_SPI)
	(struct device*)&pf_sba_device_spi_bmi160,
#endif
#if defined(CONFIG_BME280) && defined (CONFIG_BME280_SPI)
	(struct device*)&pf_sba_device_spi_bme280,
#endif

#endif

/* SS I2C0 bus and devices */
#ifdef CONFIG_SS_I2C
	&pf_bus_sba_ss_i2c_0,
#if defined(CONFIG_BMI160) && !defined(CONFIG_BMI160_SPI)
	(struct device*)&pf_sba_device_i2c_bmi160,
#endif
#if defined(CONFIG_BME280) && defined(CONFIG_BME280_I2C)
	(struct device*)&pf_sba_device_i2c_bme280,
#endif
#endif

};

void init_all_devices()
{
	// Init plateform devices and buses
	init_devices(arc_platform_devices, ARRAY_SIZE(arc_platform_devices));
}
