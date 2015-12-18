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

#include <d_flip_flop.h>
#include <gpio/soc_gpio.h>

#ifndef CONFIG_D_FF_D_PIN
#error You need to set CONFIG_D_FF_D_PIN
#endif

#ifndef CONFIG_D_FF_CK_PIN
#error You need to set CONFIG_D_FF_CK_PIN
#endif

static struct
{
	uint32_t	d;
	uint32_t	ck;
	uint32_t	gpio_port;
} d_flip_flop = {
	.d  = CONFIG_D_FF_D_PIN,
	.ck = CONFIG_D_FF_CK_PIN,
	/* static settings */
	.gpio_port = SOC_GPIO_32
};

void d_flip_flop_set(uint8_t value) {
	uint8_t tmp = !!value; /* be sure to have 0 or 1 */
	/* 74LVC79-type flip-flop accept 160MHz clk and 2.5ns setup time.
	 * At 32MHz processor, no need to add any delay.*/
	soc_gpio_write(d_flip_flop.gpio_port, d_flip_flop.d, tmp);
	soc_gpio_write(d_flip_flop.gpio_port, d_flip_flop.ck, 1);
	soc_gpio_write(d_flip_flop.gpio_port, d_flip_flop.ck, 0);
	soc_gpio_write(d_flip_flop.gpio_port, d_flip_flop.d, 0);
}
