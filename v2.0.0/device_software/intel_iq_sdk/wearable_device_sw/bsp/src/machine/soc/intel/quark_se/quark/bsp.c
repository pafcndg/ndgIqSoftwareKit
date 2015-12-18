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

#include "infra/ipc.h"
#include "infra/log.h"

#include "machine/soc/intel/quark_se/quark/soc_setup.h"
#include "machine.h"

/* Console manager setup helper */
#include "infra/console_manager.h"

/* Test command client setup */
#include "machine/soc/intel/quark_se/quark/uart_tcmd_client.h"
#include "infra/tcmd/engine.h"

T_QUEUE bsp_setup(void)
{
	T_QUEUE queue;

	/* Setup IPC and main queue */
	queue = ipc_setup();

	/* Setup the SoC hardware and logs */
	soc_setup();

	/* Start log infrastructure */
	log_start();

#ifdef CONFIG_CONSOLE_MANAGER
	/* Console manager setup */
	console_manager_init();
#endif

#ifdef CONFIG_TCMD
	/* Enable test command engine async support through the main queue */
	tcmd_async_init(queue);
#endif
#ifdef CONFIG_TCMD_CONSOLE
	/* Test commands will use the same port as the log system */
	set_tcmd_uart_port(CONFIG_UART_CONSOLE_INDEX);
#endif

	return queue;
}
