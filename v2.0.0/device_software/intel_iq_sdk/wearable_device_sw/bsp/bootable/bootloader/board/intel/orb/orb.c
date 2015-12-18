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

#include "gpio/soc_gpio.h"
#include "bootlogic.h"
#include "reboot.h"
#include <mtd/spi_flash.h>
#include <spi/quark_spi.h>
#include <dfu_spi_flash.h>
#include "ota.h"
#include <stdbool.h>

#if defined(CONFIG_SPI)
void dfu_spi_init(void){
	/* 	pinmuxing */
	SET_PIN_MODE(55, QRK_PMUX_SEL_MODEB);	/* SPI0_M_SCK       */
	SET_PIN_MODE(56, QRK_PMUX_SEL_MODEB);	/* SPI0_M_MISO      */
	SET_PIN_MODE(57, QRK_PMUX_SEL_MODEB);	/* SPI0_M_MOSI      */
	/* SPI0_M CS is controlled as a gpio */
	SET_PIN_MODE(58, QRK_PMUX_SEL_MODEA);	/* GPIO[24]/SPI0_M_CS_0 */
	/* spi0 clock gating */
	MMIO_REG_VAL(PERIPH_CLK_GATE_CTRL) |= SPI0_CLK_GATE_MASK;

	spi_cfg_data_t spi_cfg;

	spi_cfg.speed           = 4000;           /*!< SPI bus speed in KHz   */
	spi_cfg.txfr_mode       = SPI_TX_RX;     /*!< Transfer mode */
	spi_cfg.data_frame_size = SPI_8_BIT;     /*!< Data Frame Size ( 4 - 16 bits ) */
	spi_cfg.bus_mode        = SPI_BUSMODE_0; /*!< SPI bus mode is 0 by default */

	soc_spi_set_config(&spi_cfg);
}
#endif

#if defined(CONFIG_SPI_FLASH)
static spi_flash_info_t spi_flash = {0};

void dfu_spi_flash_init(void){
	spi_flash.slave = SPI_SE_3;
	spi_flash_init(&spi_flash);
}

spi_flash_info_t* get_spi_flash(void){
	static bool spi_flash_initialized = false;
	if (spi_flash_initialized == false){
		dfu_spi_init();
		dfu_spi_flash_init();
		spi_flash_initialized = true;
	}
	return &spi_flash;
}
#endif

void boot_ota(enum wake_sources wake_source,
	      enum reset_reasons reset_reason, enum boot_targets boot_target)
{
	set_boot_flags(BOOT_OTA_ONGOING);

	if (ota_start())
		set_boot_flags(BOOT_OTA_FAILURE);
	else
		set_boot_flags(BOOT_OTA_COMPLETE);

	reboot(TARGET_MAIN);
}
