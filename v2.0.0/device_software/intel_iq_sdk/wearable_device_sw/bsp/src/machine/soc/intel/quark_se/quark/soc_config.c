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

#include "drivers/spi_flash/spi_flash_mx25.h"
#include "drivers/spi_flash/spi_flash_w25qxxdv.h"
#include "drivers/serial_bus_access.h"
#include "drivers/intel_qrk_rtc.h"
#include "drivers/intel_qrk_wdt.h"
#include "drivers/ns16550_pm.h"
#include "drivers/soc_flash.h"
#include "drivers/usb_pm.h"
#include "storage.h"
#include "board.h"
#include "drivers/soc_gpio.h"
#include "machine/soc/intel/quark_se/soc_config.h"
#include "infra/wakelock_ids.h"
#include "machine/soc/intel/quark_se/arc/soc_register.h"
#include "machine/soc/intel/quark_se/quark/uart_tcmd_client.h"
#include "drivers/soc_comparator.h"
#include "drivers/led/soc_led.h"
#include "drivers/lp5562.h"
#include "drivers/apds9190.h"
#include "drivers/display/hd44780.h"
#include "project_mapping.h"
#include "drivers/haptic/drv2605.h"
#include "drivers/intel_qrk_pwm.h"
#include "drivers/ipc_uart_ns16550.h"
#include "drivers/intel_qrk_aonpt.h"
#include "drivers/clk_system.h"
#include "drivers/nfc_stn54e.h"

#ifdef CONFIG_BOARD_CURIE_101
#define SPI_FLASH_CS SPI_SE_1
#else
#define SPI_FLASH_CS SPI_SE_3
#endif

DECLARE_SUSPEND_BLOCKERS(
#if defined(CONFIG_USB_PM_SUSPEND_BLOCKERS)
	{
		.cb = usb_pm_is_suspend_allowed,
		.dev = &pf_device_usb_pm
	},
#endif
	);

/* Create all bus instances */
#ifdef CONFIG_INTEL_QRK_SPI

/* Configuration of sba master devices (bus) */
static struct sba_master_cfg_data qrk_sba_spi_0_cfg = {
	.bus_id				= SBA_SPI_MASTER_0,
	.config.spi_config		= {
		.speed			= 250,                  /*!< SPI bus speed in KHz   */
		.txfr_mode		= SPI_TX_RX,            /*!< Transfer mode */
		.data_frame_size	= SPI_8_BIT,            /*!< Data Frame Size ( 4 - 16 bits ) */
		.slave_enable		= SPI_FLASH_CS,         /*!< Slave Enable, Flash Memory is on CS3 or CS1 */
		.bus_mode		= SPI_BUSMODE_0,        /*!< SPI bus mode is 0 by default */
		.spi_mode_type		= SPI_MASTER,           /*!< SPI 0 is in master mode */
		.loopback_enable	= 0                     /*!< Loopback disabled by default */
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= PERIPH_CLK_GATE_CTRL,
		.bits_mask		= SPI0_CLK_GATE_MASK,
	},
};

static struct sba_master_cfg_data qrk_sba_spi_1_cfg = {
	.bus_id				= SBA_SPI_MASTER_1,
	.config.spi_config		= {
		.speed			= 250,                  /*!< SPI bus speed in KHz   */
		.txfr_mode		= SPI_TX_RX,            /*!< Transfer mode */
		.data_frame_size	= SPI_8_BIT,            /*!< Data Frame Size ( 4 - 16 bits ) */
		.slave_enable		= SPI_SE_3,             /*!< Slave Enable, NFC device */
		.bus_mode		= SPI_BUSMODE_0,        /*!< SPI bus mode is 0 by default */
		.spi_mode_type		= SPI_MASTER,           /*!< SPI 0 is in master mode */
		.loopback_enable	= 0                     /*!< Loopback disabled by default */
	},
	.clk_gate_info	= &(struct clk_gate_info_s) {
		.clk_gate_register	= PERIPH_CLK_GATE_CTRL,
		.bits_mask	= SPI1_CLK_GATE_MASK,
	},
};

struct device pf_bus_sba_spi_0 = {
		.id = SBA_SPI0_ID,
		.driver = &serial_bus_access_driver,
		.priv = &qrk_sba_spi_0_cfg
	};
struct device pf_bus_sba_spi_1 = {
		.id = SBA_SPI1_ID,
		.driver = &serial_bus_access_driver,
		.priv = &qrk_sba_spi_1_cfg
	};
#endif /* CONFIG_INTEL_QRK_SPI */

#ifdef CONFIG_INTEL_QRK_I2C

static struct sba_master_cfg_data qrk_sba_i2c_0_cfg = {
	.bus_id				= SBA_I2C_MASTER_0,
	.config.i2c_config		= {
		.speed			= I2C_SLOW,
		.addressing_mode	= I2C_7_Bit,
		.mode_type		= I2C_MASTER
	},
	.clk_gate_info			= &(struct clk_gate_info_s) {
		.clk_gate_register	= PERIPH_CLK_GATE_CTRL,
		.bits_mask		= I2C0_CLK_GATE_MASK,
	},
};
static struct sba_master_cfg_data qrk_sba_i2c_1_cfg = {
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

struct device pf_bus_sba_i2c_0 = {
		.id = SBA_I2C0_ID,
		.driver = &serial_bus_access_driver,
		.priv = &qrk_sba_i2c_0_cfg,
	};
struct device pf_bus_sba_i2c_1 = {
		.id = SBA_I2C1_ID,
		.driver = &serial_bus_access_driver,
		.priv = &qrk_sba_i2c_1_cfg,
	};
#endif /* CONFIG_INTEL_QRK_I2C */

#ifdef CONFIG_INTEL_QRK_SPI

#ifdef CONFIG_SPI_FLASH_INTEL_QRK
struct sba_device pf_sba_device_flash_spi0 = {
		.dev.id = SPI_FLASH_0_ID,
#ifdef CONFIG_SPI_FLASH_W25Q16DV
		.dev.driver = (struct driver*)&spi_flash_w25qxxdv_driver,
#elif defined(CONFIG_SPI_FLASH_MX25U12835F)
		.dev.driver = (struct driver*)&spi_flash_mx25u12835f_driver,
#elif defined(CONFIG_SPI_FLASH_MX25R1635F)
		.dev.driver = (struct driver*)&spi_flash_mx25r1635f_driver,
#endif
		.dev.parent = &pf_bus_sba_spi_0,
		.addr.cs = SPI_FLASH_CS
	};
#endif

#endif /* CONFIG_INTEL_QRK_SPI */



#ifdef CONFIG_NFC_STN54E
struct sba_device pf_sba_device_nfc = {
		.dev.id = NFC_STN54E_ID,
		.dev.driver = &nfc_stn54e_driver,
#ifdef CONFIG_NFC_STN54E_ON_I2C0
		.dev.parent = &pf_bus_sba_i2c_0,
#endif
#ifdef CONFIG_NFC_STN54E_ON_I2C1
		.dev.parent = &pf_bus_sba_i2c_1,
#endif
		.dev.priv = &(struct nfc_stn54e_info){
			.wakelock.id = NFC_WAKELOCK,
			.gpio_dev = &pf_device_soc_gpio_32,
			.stn_reset_pin = CONFIG_STN54E_RST_PIN,
			.stn_irq_pin = CONFIG_STN54E_IRQ_OUT_PIN,
#ifdef CONFIG_STN54E_HAS_PWR_EN
			.stn_pwr_en_pin = CONFIG_STN54E_PWR_EN_PIN,
#endif
#ifdef CONFIG_STN54E_HAS_BOOSTER
			.booster_reset_pin = CONFIG_STN54E_BOOSTER_RST_PIN,
#endif
		},
		.addr.slave_addr = CONFIG_STN54E_I2C_ADDR,
};
#endif /* CONFIG_NFC_STN54E */

#ifdef CONFIG_LP5562_LED
struct sba_device pf_sba_device_led_lp5562 = {
		.dev.id = LED_LP5562_ID,
		.dev.driver = &led_lp5562_driver,
		.dev.parent = &pf_bus_sba_i2c_1,
		.dev.priv = &(struct lp5562_info) {
			.wakelock.id = LED_WAKELOCK,
#if defined(CONFIG_BOARD_MORPHEUS) || defined(CONFIG_BOARD_CURIE_ORB)
			.led_en_dev = &pf_device_soc_gpio_32,
			.led_en_pin = 25,
#endif
			.config = REG_CONFIG_PWM_HF | REG_CONFIG_PWR_SAVE | REG_CONFIG_INT_CLK,
			.led_map = LED_MAP(LED_EN1, B) | // LED_B = Eng1
				LED_MAP(LED_EN2, G) | // LED_G = Eng2
				LED_MAP(LED_EN3, R), // LED_R = eng3, LED_W = dc
		},
		.addr.slave_addr = 0x30,
	};
#endif /* CONFIG_LP5562_LED */

#ifdef CONFIG_DRV2605
struct sba_device pf_sba_device_drv2605 = {
		.dev.id = DRV2605_ID,
		.dev.driver = &drv2605_driver,
		.dev.parent = &pf_bus_sba_i2c_1,
		.dev.priv = &(struct drv2605_info) {},
		.addr.slave_addr = 0x5A,
	};
#endif

#ifdef CONFIG_APDS9190
struct sba_device pf_sba_device_apds9190 = {
		.dev.id = APDS9190_ID,
		.dev.driver = &apds9190_driver,
		.dev.parent = &pf_bus_sba_i2c_1,
		.dev.priv = &(struct apds9190_info) {
			.ptime = 0xFF,
			.wtime = 0,
			.pers = 0x30,
			.config = 0,
			.ppcount = 4,
			.pilt = 0,
			.piht = 0x0300
		},
		.addr.slave_addr = 0x39,
	};
#endif


/* List of devices */

struct device pf_device_clk_gate = {
		.id = QRK_CLK_GATE,
		.driver = &clk_system_driver,
		.priv = &(struct clk_gate_info_s)
		{
			.clk_gate_register = PERIPH_CLK_GATE_CTRL,
			.bits_mask = QRK_CLK_GATE_INIT_VALUE,
		}
	};


#ifdef CONFIG_INTEL_QRK_RTC
struct device pf_device_rtc = {
		.id = RTC_ID,
		.driver = &rtc_driver,
		.priv = &(struct rtc_pm_data){
			.clk_gate_info = &(struct clk_gate_info_s){
				.clk_gate_register = PERIPH_CLK_GATE_CTRL,
				.bits_mask = RTC_CLK_GATE_MASK,
			},
		},
	};
#endif


#ifdef CONFIG_INTEL_QRK_WDT
struct device pf_device_wdt = {
		.id = WDT_ID,
		.driver = &watchdog_driver,
		.priv = &(struct wdt_pm_data){
			.clk_gate_info = &(struct clk_gate_info_s){
				.clk_gate_register = PERIPH_CLK_GATE_CTRL,
				.bits_mask = WDT_CLK_GATE_MASK,
			},
		},
	};
#endif

#ifdef CONFIG_INTEL_QRK_AON_PT
struct device pf_device_aon_pt = {
		.id = AON_PT_ID,
		.driver = &aonpt_driver
	};
#endif

#ifdef CONFIG_UART_PM_NS16550
#ifdef CONFIG_IPC_UART_NS16550
struct device pf_device_uart_ns16550 = {
		.id = IPC_UART_ID,
		.driver = &ipc_uart_ns16550_driver,
		.priv = &(struct ipc_uart_info){
			.uart_num = 0,
			.irq_vector = COM1_INT_LVL,
			.irq_mask = INT_UART_0_MASK,
			.rx_wl.id = IPC_UARTRX_WAKELOCK,
			.tx_wl.id = IPC_UARTTX_WAKELOCK
		},
	};
#endif
#ifdef CONFIG_IPC_UART
/* FIXME:	CONFIG_IPC_UART_BAUDRATE is "IPC_UART" specific
 *			hence we must check if CONFIG_IPC_UART is set before
 *			allocating this device*/
struct device pf_device_uart0_pm = {
		.id = UART0_PM_ID,
		.driver = &ns16550_pm_driver,
		.priv = &(struct ns16550_pm_device){
			.uart_num = 0,
			.vector = SOC_UART0_INTERRUPT,
			.uart_int_mask = INT_UART_0_MASK,
			.init_info = &(struct uart_init_info){
				.sys_clk_freq = UART_XTAL_FREQ,
				.baud_rate = CONFIG_IPC_UART_BAUDRATE,
				.options = UART_OPTION_AFCE,
				.int_pri = COM1_INT_PRI,
			},
		},
	};
#endif

struct device pf_device_uart1_pm = {
		.id = UART1_PM_ID,
		.driver = &ns16550_pm_driver,
		.priv = &(struct ns16550_pm_device){
			.uart_num = 1,
#ifdef CONFIG_TCMD
			.uart_rx_callback = uart_console_input,
#endif
			.vector = SOC_UART1_INTERRUPT,
			.uart_int_mask = INT_UART_1_MASK,
			.init_info = &(struct uart_init_info){
				.sys_clk_freq = UART_XTAL_FREQ,
				.baud_rate = COM2_BAUD_RATE,
				.options = 0,
				.int_pri = COM2_INT_PRI,
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

#ifdef CONFIG_SOC_COMPARATOR
struct device pf_device_soc_comparator = {
		.id = COMPARATOR_ID,
		.driver = &soc_comparator_driver,
		.priv = (struct cmp_cb[CMP_COUNT]) {}
	};
#endif

#ifdef CONFIG_SOC_FLASH
struct device pf_device_soc_flash = {
		.id = SOC_FLASH_ID,
		.driver = &soc_flash_driver
	};
#endif

#if defined(CONFIG_USB_PM) && defined(CONFIG_CURIE)
struct device pf_device_usb_pm = {
		.id = USB_PM_ID,
		.driver = &usb_pm_driver,
		.priv = &(struct usb_pm_info) {
			.evt_dev = &pf_device_soc_comparator,
			.interrupt_source = USB_COMPARATOR_IRQ_SOURCE,
			.source_pin = 7, // USB Vbus is connected on comparator 7 for CTB
#ifdef CONFIG_SOC_GPIO_32
			.vusb_enable_dev = &pf_device_soc_gpio_32,
			.vusb_enable_pin = 28,
#endif
		}
	};
#endif

#ifdef CONFIG_SOC_LED
struct device pf_device_soc_led = {
		.id = SOC_LED_ID,
		.driver = &soc_led_driver,
	};
#endif

#ifdef CONFIG_DISPLAY_HD44780
struct device pf_device_display_hd44780 = {
		.id = HD44780_ID,
		.driver = &hd44780_driver,
	};
#endif

#ifdef CONFIG_INTEL_QRK_PWM
struct device pf_device_pwm = {
		.id = PWM_ID,
		.driver = &pwm_driver,
		.priv = &(struct pwm_pm_info) {
			.timer_info = (struct timer_info_s[QRK_PWM_NPWM]) {},
			.running_pwm = 0,
			.clk_gate_info = &(struct clk_gate_info_s) {
				.clk_gate_register = PERIPH_CLK_GATE_CTRL,
				.bits_mask = PWM_CLK_GATE_MASK,
			},
		},
	};
#endif


/* List of QRK platform devices (on die memory, spi slave etc ...) */

static struct device *qrk_platform_devices[] = {
	&pf_device_clk_gate,

#ifdef CONFIG_INTEL_QRK_RTC
	&pf_device_rtc,
#endif

#ifdef CONFIG_INTEL_QRK_WDT
	&pf_device_wdt,
#endif

#ifdef CONFIG_INTEL_QRK_AON_PT
	&pf_device_aon_pt,
#endif

#ifdef CONFIG_UART_PM_NS16550
#ifdef CONFIG_IPC_UART_NS16550
	&pf_device_uart_ns16550,
#endif
#ifdef CONFIG_IPC_UART
	&pf_device_uart0_pm,
#endif
	&pf_device_uart1_pm,
#endif

#ifdef CONFIG_SOC_GPIO_AON
	&pf_device_soc_gpio_aon,
#endif

#ifdef CONFIG_SOC_GPIO_32
	&pf_device_soc_gpio_32,
#endif

#ifdef CONFIG_SOC_COMPARATOR
	&pf_device_soc_comparator,
#endif

#ifdef CONFIG_SOC_FLASH
	&pf_device_soc_flash,
#endif

#if defined(CONFIG_USB_PM) && defined(CONFIG_CURIE)
	&pf_device_usb_pm,
#endif

#ifdef CONFIG_SOC_LED
	&pf_device_soc_led,
#endif

#ifdef CONFIG_DISPLAY_HD44780
	&pf_device_display_hd44780,
#endif
#ifdef CONFIG_INTEL_QRK_PWM
	&pf_device_pwm,
#endif

#ifdef CONFIG_INTEL_QRK_SPI
	&pf_bus_sba_spi_0, // SPI 0 bus and devices
#ifdef CONFIG_SPI_FLASH_INTEL_QRK
	(struct device*)&pf_sba_device_flash_spi0,
#endif
	&pf_bus_sba_spi_1, // SPI 1 bus and devices
#endif

#ifdef CONFIG_INTEL_QRK_I2C
	&pf_bus_sba_i2c_0, // I2C 0 bus and devices
#ifdef CONFIG_NFC_STN54E_ON_I2C0
	&pf_sba_device_nfc,
#endif

	&pf_bus_sba_i2c_1, // I2C 1 bus and devices
#ifdef CONFIG_LP5562_LED
	(struct device*)&pf_sba_device_led_lp5562,
#endif
#ifdef CONFIG_DRV2605
	(struct device*)&pf_sba_device_drv2605,
#endif
#ifdef CONFIG_APDS9190
	(struct device*)&pf_sba_device_apds9190,
#endif
#ifdef CONFIG_NFC_STN54E_ON_I2C1
	(struct device*)&pf_sba_device_nfc,
#endif

#endif
};

void init_all_devices(void)
{
	// Init plateform devices and buses
	init_devices(qrk_platform_devices, ARRAY_SIZE(qrk_platform_devices));
}

/* Array of qrk flash memory partitioning */
flash_partition_t storage_configuration[] =
{
	{
		.partition_id = APPLICATION_DATA_PARTITION_ID,
		.flash_id = APPLICATION_DATA_FLASH_ID,
		.start_block = APPLICATION_DATA_START_BLOCK,
		.end_block = APPLICATION_DATA_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	},
	{
		.partition_id = DEBUGPANIC_PARTITION_ID,
		.flash_id = DEBUGPANIC_FLASH_ID,
		.start_block = DEBUGPANIC_START_BLOCK,
		.end_block = DEBUGPANIC_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	},
	{
		.partition_id = FACTORY_RESET_NON_PERSISTENT_PARTITION_ID,
		.flash_id = FACTORY_RESET_NON_PERSISTENT_FLASH_ID,
		.start_block = FACTORY_RESET_NON_PERSISTENT_START_BLOCK,
		.end_block = FACTORY_RESET_NON_PERSISTENT_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	},
	{
		.partition_id = FACTORY_RESET_PERSISTENT_PARTITION_ID,
		.flash_id = FACTORY_RESET_PERSISTENT_FLASH_ID,
		.start_block = FACTORY_RESET_PERSISTENT_START_BLOCK,
		.end_block = FACTORY_RESET_PERSISTENT_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_PERSISTENT
	},
	{
		.partition_id = FACTORY_SETTINGS_PARTITION_ID,
		.flash_id = FACTORY_SETTINGS_FLASH_ID,
		.start_block = FACTORY_SETTINGS_START_BLOCK,
		.end_block = FACTORY_SETTINGS_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_PERSISTENT
	},
	{
		.partition_id = SPI_FOTA_PARTITION_ID,
		.flash_id = SPI_FOTA_FLASH_ID,
		.start_block = SPI_FOTA_START_BLOCK,
		.end_block = SPI_FOTA_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	},
	{
		.partition_id = SPI_APPLICATION_DATA_PARTITION_ID,
		.flash_id = SPI_APPLICATION_DATA_FLASH_ID,
		.start_block = SPI_APPLICATION_DATA_START_BLOCK,
		.end_block = SPI_APPLICATION_DATA_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	},
	{
		.partition_id = SPI_SYSTEM_EVENT_PARTITION_ID,
		.flash_id = SPI_SYSTEM_EVENT_FLASH_ID,
		.start_block = SPI_SYSTEM_EVENT_START_BLOCK,
		.end_block = SPI_SYSTEM_EVENT_END_BLOCK,
		.factory_reset_state = FACTORY_RESET_NON_PERSISTENT
	}
};

/* Array of qrk flash memory devices */
const flash_device_t flash_devices[] =
{
	{
		.flash_id = EMBEDDED_FLASH_ID,
		.nb_blocks = EMBEDDED_FLASH_NB_BLOCKS,
		.block_size = EMBEDDED_FLASH_BLOCK_SIZE,
		.flash_location = EMBEDDED_FLASH
	},
	{
		.flash_id = SERIAL_FLASH_ID,
		.nb_blocks = SERIAL_FLASH_NB_BLOCKS,
		.block_size = SERIAL_FLASH_BLOCK_SIZE,
		.flash_location = SERIAL_FLASH
	}
};
