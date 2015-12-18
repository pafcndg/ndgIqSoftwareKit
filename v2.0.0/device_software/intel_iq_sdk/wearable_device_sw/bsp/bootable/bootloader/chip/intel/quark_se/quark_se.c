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

#include "machine/soc/intel/quark_se/scss_registers.h"

#if defined(CONFIG_USB)
#include <usb/usb_api.h>
#include <usb/usb.h>
#include <usb/usb_driver_interface.h>
#include <usb/usb_shared_interface.h>
#include "usb_setup.h"
#include <utils.h>		/* get_32k_time */
#endif

#if defined(CONFIG_DNX)
#include <bootlogic.h>
#endif

#if defined(CONFIG_USB_DFU)
#include <dfu.h>
#include <dfu_spi_flash.h>
#endif

#if defined(CONFIG_SWD)
#include <swd/swd.h>
#endif

#include <printk.h>
#include <partition.h>
#include <gpio/gpio.h>
#include <gpio/soc_gpio.h>
#include <uart.h>

#include <mem.h>
#include <bootlogic.h>

#ifndef NULL
#define NULL 0
#endif

#define SOC_UART1_BASE	0xB0002400

#if defined(CONFIG_USB) && defined(CONFIG_USB_DFU) && defined(CONFIG_DNX)
uint8_t usb_gpio;
#endif

#define OSC_INTERNAL 1
#define OSC_EXTERNAL 0
#define INTERNAL_OSC_TRIM 0x240

#define DFU_DELAY_BEFORE_RESET (1000 * 32) /* 1 sec DFU delay before reset */

void set_oscillator(int internal)
{
	if (internal) {
		/* Start internal oscillator (with trim) */
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_CFG1) |=
		    INTERNAL_OSC_TRIM << OSC0_CFG1_INTERNAL_OSC_TRIM_BIT |
		    OSC0_CFG1_INTERNAL_OSC_EN_MASK;
		/* Wait internal oscillator ready */
		while (!((MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_STAT1)
				   & OSC0_STAT1_LOCK_INTERNAL)));
		/* Trim internal oscillator */
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_CFG1) =
		    INTERNAL_OSC_TRIM << OSC0_CFG1_INTERNAL_OSC_TRIM_BIT |
		    OSC0_CFG1_INTERNAL_OSC_EN_MASK;
	} else {
		/* Set clk to 32MHz, external oscillator, 5.5pF load */
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_CFG1) |=
		    OSC0_CFG1_XTAL_OSC_TRIM_5_55_PF |
		    OSC0_CFG1_XTAL_OSC_EN_MASK;
		/* Wait internal regulator ready */
		while (!((MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_STAT1)
				   & OSC0_STAT1_LOCK_XTAL))) ;
		/* Switch to external oscillator */
		MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_OSC0_CFG1) =
		    OSC0_CFG1_XTAL_OSC_TRIM_5_55_PF |
		    (OSC0_CFG1_XTAL_OSC_EN_MASK | OSC0_CFG1_XTAL_OSC_OUT_MASK);
	}
}

/* Soc Specific initialization */
void soc_init(void)
{
	/* Reset AON counter */
	uint32_t time = get_32k_time();
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONC_CFG) =
		AONC_CNT_DIS;
	while(time <= get_32k_time()) {
		__asm__ volatile("nop");
	}
	MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, SCSS_AONC_CFG) =
		AONC_CNT_EN;

#ifdef CONFIG_QUARK_SE_SWITCH_INTERNAL_OSCILLATOR
	set_oscillator(OSC_INTERNAL);
#else
	set_oscillator(OSC_EXTERNAL);
#endif

#if defined(CONFIG_USB) && defined(CONFIG_USB_DFU) && defined(CONFIG_DNX)
	usb_driver_os_dep->printk = printk;
	usb_driver_os_dep->alloc = balloc;
	usb_driver_os_dep->free = bfree;

	SET_PIN_MODE(7, QRK_PMUX_SEL_MODEA);

#ifdef CONFIG_SERIAL
	/* UART1 pins output in quark for curie */
	SET_PIN_MODE(16, QRK_PMUX_SEL_MODEC);
	SET_PIN_MODE(17, QRK_PMUX_SEL_MODEC);
	uart_init(SOC_UART1_BASE, 115200);
	pr_info("BOOTLADER INIT\n\r");
#endif

	gpio_cfg_data_t pin_cfg = {
		.gpio_type = GPIO_INPUT,
		.int_type = LEVEL,
		.int_polarity = ACTIVE_LOW,
		.int_debounce = DEBOUNCE_OFF,
		.int_ls_sync = LS_SYNC_OFF,
		.gpio_cb = NULL
	};

	if ((soc_gpio_set_config(0, 7, &pin_cfg))
	    != DRV_RC_OK)
		pr_info("error configure gpio\n");
	usb_gpio = soc_gpio_read(0, 7);

	pr_info("usb status: %d\n", usb_gpio);
#endif

#if defined(CONFIG_MISC)
	// Force BLE Core reset even if SWD debug has been entered
	swd_init();
	swd_debug_mode_reset_to_normal();
#endif
}

#if defined(CONFIG_USB) && defined(CONFIG_USB_DFU) && defined(CONFIG_DNX)
extern void usb_shared_interface_init();
void usb_init(void)
{
	usb_shared_interface_init();
	if (usb_gpio == 0) {
		return;
	}
	set_oscillator(OSC_EXTERNAL);
	platform_usb_init();
	usb_driver_init(SOC_USB_BASE_ADDR);
	dfu_class_init();
}

/*
 * Support DnX on DFU over USB only
 * No support on *modem over UART
 * FIXME: move to a more common place once proper USB api available
 * FIXME: add usb vbus detection in loop
 */
extern int dfu_reset;
uint8_t dfu_busy;
void dnx(void)
{
	uint32_t saved_date, current_date;
	uint32_t timeout_ticks;
	uint32_t dfu_reset_ticks = 0;
	saved_date = get_32k_time();
	current_date = get_32k_time();
	if (usb_gpio == 0) {
		return;
	}
	timeout_ticks = CONFIG_DNX_TIMEOUT_S * 32000;

	//Force waiting update of quark and arc after bootupdater reboot
	if (get_boot_target() == TARGET_FLASHING)
		dfu_busy = 1;
	else
		dfu_busy = 0;

	while (dfu_reset ||
	       (dfu_busy || ((saved_date + timeout_ticks) > current_date))) {
		current_date = get_32k_time();
		poll_usb();
		if (dfu_reset){
			if (dfu_reset_ticks){
				if ((get_32k_time() - dfu_reset_ticks) > DFU_DELAY_BEFORE_RESET ){
					reboot(TARGET_MAIN);
				}
			} else {
				dfu_reset_ticks = get_32k_time();
			}
		}
	}
	platform_usb_release();

#ifdef CONFIG_QUARK_SE_SWITCH_INTERNAL_OSCILLATOR
	set_oscillator(OSC_INTERNAL);
#endif
}

struct flash_block {
	uint32_t block_start;
	uint32_t block_count;
};

/*
 * FIXME: rom does not have same start address
 * FIXME: add callback for each type of alternate (rom, flash, swd)
 */

/*
 * alt=10, name="cache"
 * alt=9, name="snor"
 * alt=8, name="ble_core"
 * alt=7, name="sensor_core"
 * alt=6, name="logs"
 * alt=5, name="bootupdater"
 * alt=4, name="panic"
 * alt=3, name="bootloader_update"
 * alt=2, name="x86_app"
 * alt=1, name="x86_boot"
 * alt=0, name="x86_rom"
 */

static const struct flash_block flash_blocks[] = {
	/* rom */
	{ROM_PAGE_START, ROM_PAGE_NR},
	/* x86_boot */
	{BOOT_PAGE_START, BOOT_PAGE_NR},
	/* x86_app */
	{QUARK_START_PAGE, QUARK_NB_PAGE},
	/* Factory reset non persistent data */
	{FACTORY_RESET_NON_PERSISTENT_START_BLOCK,
		FACTORY_RESET_NON_PERSISTENT_NB_BLOCKS},
	/* panic */
	{DEBUGPANIC_START_BLOCK, DEBUGPANIC_NB_BLOCKS},
	/* Factory reset persistent data */
	{FACTORY_RESET_PERSISTENT_START_BLOCK,
		FACTORY_RESET_PERSISTENT_NB_BLOCKS},
	/* data */
	{APPLICATION_DATA_START_BLOCK, APPLICATION_DATA_NB_BLOCKS},
	/* sensor_core */
	{ARC_START_PAGE, ARC_NB_PAGE},
};

void dfu_download_cleanup(struct dfu_ops *ops)
{
	uint32_t address;
	switch (ops->alternate) {
	case 2: /* x86_app or bootupdater */
		address = flash_blocks[ops->alternate].block_start * PAGE_SIZE + 0x40000000;

		if (!secure_boot(address)) {
		    ops->status = errVERIFY;
		    ops->state = dfuERROR;
		} else {
		    ops->status = statusOK;
		    ops->state = dfuIDLE;
		}

		break;
	default:
		ops->status = statusOK;
		ops->state = dfuIDLE;
		break;
	}

	if (ops->status != statusOK)
		soc_flash_block_erase(flash_blocks[ops->alternate].block_start,
				      flash_blocks[ops->alternate].block_count);
}

/* For now prohibit write on some alternates.
 * A DFU_CLRSTATUS request clears the status/state and allows
 * further (valid) transfers */
void dfu_forbidden_transfer(struct dfu_ops *ops)
{
	ops->state = dfuERROR;
	ops->status = errTARGET;
}

void dfu_flash_write(struct dfu_ops *ops)
{
	usb_device_request_t *setup_packet = ops->device_request;
	int retlen;
	int ret;
	unsigned int address = (flash_blocks[ops->alternate].block_start
				+ UGETW(setup_packet->wValue)) * PAGE_SIZE;
	unsigned int start_blk = flash_blocks[ops->alternate].block_start;

	if (UGETW(setup_packet->wValue)
	    > flash_blocks[ops->alternate].block_count - 1) {
		ops->state = dfuERROR;
		ops->status = errADDRESS;
		return;
	}

	pr_info("Flash erase: %d \n", start_blk + UGETW(setup_packet->wValue));
	ret = soc_flash_block_erase(start_blk + UGETW(setup_packet->wValue), 1);
	pr_info("Flash return %d\n", ret);
	if (ret != DRV_RC_OK) {
		ops->state = dfuERROR;
		ops->status = errERASE;
		return;
	}
	pr_info("Flash write: %x len %d\n", 0x40000000 + address, ops->len);
	if ((ret = soc_flash_write(address, ops->len / 4, &retlen,
			      (unsigned char *)(ops->data))) == DRV_RC_OK){
	    ops->state = dfuDNLOAD_IDLE;
	} else {
	    ops->state = dfuERROR;
	    ops->status = errWRITE;
	}
	pr_info("Flash return %d len:%d\n", ret, retlen);
}

void dfu_soc_flash_read(struct dfu_ops *ops, unsigned int base_address)
{
	usb_device_request_t *setup_packet = ops->device_request;
	int i;
	unsigned int address = UGETW(setup_packet->wValue) * PAGE_SIZE;

	if (UGETW(setup_packet->wValue)
	    >= flash_blocks[ops->alternate].block_count) {
		ops->state = dfuIDLE;
		ops->len = 0;
		return;
	}

	ops->len = flash_blocks[ops->alternate].block_count * PAGE_SIZE - address;
	if (ops->len >= PAGE_SIZE){
		ops->len = PAGE_SIZE;
		ops->state = dfuUPLOAD_IDLE;
	} else {
		// short frame
		ops->state = dfuIDLE;
	}

	address += flash_blocks[ops->alternate].block_start * PAGE_SIZE;

	for (i = 0; i < ops->len / 4; i++) {
		*(((int *)ops->data) + i) =
		    MMIO_REG_VAL_FROM_BASE(base_address, address + 4 * i);
	}
}

void dfu_flash_read(struct dfu_ops *ops)
{
	dfu_soc_flash_read(ops, 0x40000000);
}

void dfu_otp_read(struct dfu_ops *ops)
{
	dfu_soc_flash_read(ops, ROM_PAGE_START_ADDR);
}

void dfu_swd_read(struct dfu_ops *ops)
{
#if defined(CONFIG_SWD)
	usb_device_request_t *setup_packet = ops->device_request;
	int retlen;
	int ret = SWD_ERROR_FAULT;
	unsigned int address = UGETW(setup_packet->wValue) * PAGE_SIZE;

	if (address == 0)
		swd_init();

	if (address >= BLE_CORE_FLASH_SIZE) {
		ops->state = dfuIDLE;
		ops->len = 0;
		return;
	}

	ops->len = BLE_CORE_FLASH_SIZE - address;
	if (ops->len >= PAGE_SIZE){
		ops->len = PAGE_SIZE;
		ops->state = dfuUPLOAD_IDLE;
	} else {
		// short frame
		ops->state = dfuIDLE;
	}

	ret = swd_dump_image(address, (uint32_t *) (ops->data), ops->len);

	if (ret != SWD_ERROR_OK){
		ops->state = dfuERROR;
		ops->status = errUNKNOWN;
		ops->len = 0;
	}
#endif
}

void dfu_swd_write(struct dfu_ops *ops)
{
#if defined(CONFIG_SWD)
	usb_device_request_t *setup_packet = ops->device_request;
	int retlen;
	int ret = SWD_ERROR_FAULT;
	unsigned int address = UGETW(setup_packet->wValue) * PAGE_SIZE;

	if (address == 0) {
		swd_init();
		ret = swd_erase_all();
		if (ret != DRV_RC_OK) {
			ops->state = dfuERROR;
			ops->status = errERASE;
			return;
		}
	}

	if (address > BLE_CORE_FLASH_SIZE - PAGE_SIZE) {
		ops->state = dfuERROR;
		ops->status = errADDRESS;
		ops->len = 0;
		return;
	}

	ret = swd_load_image(address, (uint32_t *) (ops->data), ops->len);
	if (ret != SWD_ERROR_OK) {
		ops->state = dfuERROR;
		ops->status = errWRITE;
		return;
	}
	ret = swd_verify_image(address, (uint32_t *) (ops->data),
				     ops->len);
	if (ret != SWD_ERROR_OK) {
		ops->state = dfuERROR;
		ops->status = errVERIFY;
		return;
	}
	ops->state = dfuDNLOAD_IDLE;
#endif
}

void dfu_set_alternate(struct dfu_ops *ops)
{
	switch (ops->alternate) {
	case X86_ROM_ALT: /* rom */
		ops->write = dfu_forbidden_transfer;
		ops->read = dfu_otp_read;
		break;
	case X86_BOOT_ALT: /* bootloader */
		ops->write = dfu_forbidden_transfer;
		ops->read = dfu_flash_read;
		break;
	case X86_APP_ALT: /* quark.bin */
	case FACT_NONPERSISTENT_ALT:
	case PANIC_ALT:
	case FACT_PERSISTENT_ALT:
	case DATA_ALT:
	case SENSOR_CORE_ALT: /* arc.bin */
		ops->write = dfu_flash_write;
		ops->read = dfu_flash_read;
		break;
	case BLE_CORE_ALT:
		/* ble_core */
		ops->write = dfu_swd_write;
		ops->read = dfu_swd_read;
		break;
#if defined(CONFIG_SPI_FLASH)
	case SNOR_ALT:
		/* snor */
		ops->write = dfu_spi_flash_write;
		ops->read = dfu_spi_flash_read;
		break;
#endif
#if defined(CONFIG_CACHE_PARTITION)
	case CACHE_ALT:
		/* cache */
		ops->write = dfu_spi_flash_write;
		ops->read = dfu_spi_flash_read;
		break;
#endif
	default:
		ops->write = dfu_forbidden_transfer;
		ops->read = dfu_forbidden_transfer;
		break;
	}
}
#endif // CONFIG_USB && CONFIG_USB_DFU && CONFIG_DNX
