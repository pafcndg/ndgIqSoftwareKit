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

#include "machine/soc/intel/quark_se/soc_config.h"
#include "machine/soc/intel/quark_se/scss_registers.h"
#include "drivers/eiaextensions.h"

#define lr __builtin_arc_lr
#define sr __builtin_arc_sr

/* Do not use the funciton provided by infra/time.h as it does calls to OS. */
static uint32_t get_uptime_ms()
{
	return ((uint64_t)SCSS_REG_VAL(SCSS_AONC_CNT) * 1000) / 32768;
}

static void fuel_gauge_init(void)
{
	int timestamp;

	/* NOTE: one day we might want to use ss_gpio driver instead */
	/* set bit GPIO_SS 14 (bit 6 from 8b1) output to 1*/
	sr(lr(AR_IO_GPIO_8B1_SWPORTA_DDR) | 0x40, AR_IO_GPIO_8B1_SWPORTA_DDR);
	sr(lr(AR_IO_GPIO_8B1_SWPORTA_DR) | 0x40, AR_IO_GPIO_8B1_SWPORTA_DR);

	/* wait 8 ms until fuel gauge can be read */
	timestamp = get_uptime_ms() + 8;
	while (get_uptime_ms() < timestamp) ;
}

void board_init(void)
{
	fuel_gauge_init();
}
