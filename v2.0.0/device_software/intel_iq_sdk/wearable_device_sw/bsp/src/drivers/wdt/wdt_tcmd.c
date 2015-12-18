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

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include <os/os.h>
#include "infra/tcmd/handler.h"

#include "drivers/intel_qrk_wdt.h"

/*
 * @addtogroup infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_wd Watchdog Test Commands
 * Interfaces to support Watchdog Test Commands.
 * @{
 */

/*
 * Function called at elapsed WDT timeout interrupt
 */
static void wdt_interrupt_routine()
{
	qrk_cxxxx_wdt_tickle();
}

/*
 * Enables watchdog with specified timeout.
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void wdt_enable(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	struct qrk_cxxxx_wdt_config config;

	if (argc == 3) {
	    config.timeout = atoi(argv[2]);
	    config.response_mode = MODE_INTERRUPT_RESET;
	    config.interrupt_fn = wdt_interrupt_routine;
	    if (qrk_cxxxx_wdt_set_config(&config) == DRV_RC_OK){
	    	TCMD_RSP_FINAL(ctx, NULL);
	    	return;
	    }
	}
	TCMD_RSP_ERROR(ctx, "Usage: wdt enable <timeout ms>");
}

DECLARE_TEST_COMMAND_ENG(wdt, enable, wdt_enable);

/*
 * @}
 *
 * @}
 */
