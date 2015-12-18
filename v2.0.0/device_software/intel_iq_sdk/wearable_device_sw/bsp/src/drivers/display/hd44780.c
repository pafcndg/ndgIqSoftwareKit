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

#include "machine.h"
#include "drivers/display/hd44780.h"
#include "scss_registers.h"
#include "drivers/soc_gpio.h"
#include "os/os.h"
#include "infra/log.h"
#include "infra/time.h"

/*
	SPI0_M_MOSI ( GPIO 23 )
	SPI0_M_SCK (GPIO 21)
	SPI0_M_CS (GPIO 24)
*/

#define PORT_DDR   MMIO_REG_VAL(SOC_GPIO_BASE_ADDR + SOC_GPIO_SWPORTA_DDR)
#define PORT       MMIO_REG_VAL(SOC_GPIO_BASE_ADDR + SOC_GPIO_SWPORTA_DR)
#define CLOCK_BIT  21
#define DATA_BIT   23
#define LATCH_BIT  24

static int bl = 0;
static struct device *gpio_dev = NULL;

static void delay_us(uint32_t us)
{
	int timeout = get_uptime_32k() + (us + 30 ) / 30;
	while(get_uptime_32k() < timeout)
		;
}

static void delay_ms(uint32_t ms)
{
	int timeout = get_uptime_32k() + 32*ms;
	while(get_uptime_32k() < timeout)
		;
}

void shift_init()
{
	SET_PIN_MODE(55, QRK_PMUX_SEL_MODEA);
	SET_PIN_MODE(57, QRK_PMUX_SEL_MODEA);
	SET_PIN_MODE(58, QRK_PMUX_SEL_MODEA);

	gpio_cfg_data_t config;
	config.gpio_type = GPIO_OUTPUT;
	config.int_type = EDGE;
	config.int_polarity = ACTIVE_LOW;
	config.int_debounce = DEBOUNCE_OFF;
	config.int_ls_sync = LS_SYNC_OFF;
	config.gpio_cb = NULL;
	config.gpio_cb_arg = NULL;

	soc_gpio_set_config(gpio_dev, CLOCK_BIT, &config);
	soc_gpio_set_config(gpio_dev, DATA_BIT, &config);
	soc_gpio_set_config(gpio_dev, LATCH_BIT, &config);
}

void shift_out(unsigned char c)
{
	shift_init();
	c |= bl;
	int i;
	for (i=0; i<8; i++) {
		soc_gpio_write(gpio_dev, CLOCK_BIT, 0);
		soc_gpio_write(gpio_dev, CLOCK_BIT, 0);
		soc_gpio_write(gpio_dev, DATA_BIT, c&1);
		soc_gpio_write(gpio_dev, CLOCK_BIT, 1);
		c>>=1;
	}
	soc_gpio_write(gpio_dev, LATCH_BIT, 1);
	soc_gpio_write(gpio_dev, LATCH_BIT, 1);
	soc_gpio_write(gpio_dev, LATCH_BIT, 0);
}

#define RS_BIT 6
#define EN_BIT 5
#define BL_BIT 0

void hd44780_write4(char c, char rs)
{
	c &= 0xf;
	shift_out((c<<1) | (rs << RS_BIT)); /* Write data */
	shift_out((c<<1) | (rs << RS_BIT) | (1 << EN_BIT)); /* Pulse EN */
	delay_us(10);
	shift_out((c<<1) | ((rs << RS_BIT) & ~(1 << EN_BIT))); /* Clear EN */
	delay_us(100);
}

void hd44780_data(char data)
{
	hd44780_write4(data>>4, 1);
	hd44780_write4(data&0xf, 1);
}

void hd44780_command(char data)
{
	hd44780_write4(data>>4, 0);
	hd44780_write4(data&0xf, 0);
}

void hd44780_home()
{
	hd44780_command(COMMAND_HOME);
	delay_ms(2);
}

void hd44780_clear()
{
	hd44780_command(COMMAND_CLEAR);
	delay_ms(2);
}

void hd44780_goto(int line, int col)
{
	int offset = line == 1 ? 64 : 0;
	hd44780_command(COMMAND_DDADDR | (offset + col));
}

void hd44780_puts(char * str)
{
	while(*str) {
		hd44780_data(*str);
		str++;
	}
}

static T_TIMER timer;

static void timer_cb(void * param)
{
	bl = 0;
	shift_out(0);
}

void hd44780_backlight(int backlight)
{
	bl = backlight ? 1 : 0;
	timer_stop(timer, NULL);
	if (bl) {
		timer_start(timer, 10000, NULL);
	}
}

int hd44780_init(struct device *dev)
{
	timer = timer_create(timer_cb, "", 10000, 0, 1, NULL);

	// Get gpio device handler
	gpio_dev = &pf_device_soc_gpio_32;
	shift_init();
	bl = 1;
	shift_out(1<<BL_BIT);

	hd44780_write4(0x3, 0);
	delay_ms(5);
	hd44780_write4(0x3, 0);
	delay_ms(5);
	hd44780_write4(0x3, 0);
	delay_us(10);
	hd44780_write4(0x2, 0);

	hd44780_command(COMMAND_FUNC | FUNC_NUM_LINES); /* Function SET */
	hd44780_command(COMMAND_ONOFF); /* Display OFF */
	hd44780_clear();
	/* Entry mode Left / Increment */
	hd44780_command(COMMAND_MODE | MODE_INC_DEC);
	/* Display ON */
	hd44780_command(COMMAND_ONOFF | ONOFF_DISPLAY);
	return 0;
}

struct driver hd44780_driver = {
	.init = hd44780_init,
	.suspend = NULL,
	.resume = NULL
};

