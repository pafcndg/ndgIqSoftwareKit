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

#include <microkernel.h>
#include <stddef.h>

#include "os/os.h"
#include "drivers/intel_qrk_aonpt.h"
#include "util/cunit_test.h"
#include "infra/time.h"

#define TICK_LEN		(32768)
#define DELTA_PERCENT		(5)
#define AON_TIMEOUT_TICK	(32768)	/* 1 sec @ 32768 Hz */

#define AON_TIMEOUT_SEC		(uint32_t)((AON_TIMEOUT_TICK + TICK_LEN - 1)/TICK_LEN)
#define AON_TIMEOUT_MSEC	(1000*AON_TIMEOUT_SEC)

#define AON_DELTA_TICK		((DELTA_PERCENT*AON_TIMEOUT_TICK)/100)
#define AON_DELTA_MSEC		((DELTA_PERCENT*AON_TIMEOUT_MSEC)/100)

#define RTC_INIT		1416571200	/* EPOCH 21/11/2014 12:00:00 */
#define NMB_LOOP		2
#define ONE_SHOT		1

static int test_aonpt_int = 0;
void test_aonpt_interrupt_fn(void)
{
	test_aonpt_int++;
}

void aonpt_counter_test (void)
{
	uint32_t init_time, sys_time;

	cu_print("###############################\n");
	cu_print("AONPT TEST ONE SHOT ALARM\n");
	cu_print("###############################\n");

	test_aonpt_int = 0;
	qrk_aonpt_configure(AON_TIMEOUT_TICK, test_aonpt_interrupt_fn, true);
	qrk_aonpt_start();

	init_time = (uint32_t)get_uptime_ms();
	/* wait for AON_TIMEOUT_TICK second(s). */
	do{
		sys_time = (uint32_t)get_uptime_ms();
	} while (sys_time <= (init_time + AON_TIMEOUT_MSEC));
	CU_ASSERT ("First time AONPT ints. read", test_aonpt_int == ONE_SHOT);  /* only one shot */

	/* wait AON_TIMEOUT_TICK and check test_aonpt_int is still the same (no alarm triggered) */
	init_time = (uint32_t)get_uptime_ms();
	/* wait for AON_TIMEOUT_TICK second(s). */
	do{
		sys_time = (uint32_t)get_uptime_ms();
	} while (sys_time <= init_time + AON_TIMEOUT_MSEC);
	CU_ASSERT ("Final AONPT ints. read", test_aonpt_int == ONE_SHOT);  /* still only one shot */

	cu_print("#################################\n");
	cu_print ("AONPT TEST FREE RUNNING COUNTER PERIODIC\n");
	cu_print("#################################\n");

	qrk_aonpt_configure(AON_TIMEOUT_TICK, NULL, false);
	qrk_aonpt_start();

	init_time = (uint32_t)get_uptime_ms();
	/* wait for AON_TIMEOUT second(s). */
	do{
		sys_time = (uint32_t)get_uptime_ms();
	} while (sys_time <= (init_time + AON_TIMEOUT_MSEC));
	/* if zero, means it didn't reload */
	CU_ASSERT ("Error during free running counter periodic mode", qrk_aonpt_read() != 0);


	cu_print("###############################\n");
	cu_print("AONPT TEST WITH PERIODIC ALARM \n");
	cu_print("###############################\n");

	/* stop counter */
	qrk_aonpt_stop();
	/* reset interrupt variable */
	test_aonpt_int = 0;
	qrk_aonpt_configure(AON_TIMEOUT_TICK, test_aonpt_interrupt_fn, false);
	init_time = (uint32_t)get_uptime_ms();
	/* start counter */
	qrk_aonpt_start();
	/* wait for LOOPS*AON_TIMEOUT_TICK second(s). */
	do{
		sys_time = (uint32_t)get_uptime_ms();
	} while (sys_time <= (init_time + (NMB_LOOP * AON_TIMEOUT_MSEC) + AON_DELTA_MSEC));
	qrk_aonpt_stop();

	CU_ASSERT ("Error during periodic alarm read", test_aonpt_int == NMB_LOOP);
}

void aonpt_test(void)
{
	cu_print("###############################################\n");
	cu_print("# Purpose of AONPT tests :			#\n");
	cu_print("#		 General Tests			#\n");
	cu_print("#		 Counter W/WO alarm tests	#\n");
	cu_print("###############################################\n");

	aonpt_counter_test();
}
