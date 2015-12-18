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

#include "system_setup.h"
#include "infra/bsp.h"
#include "infra/log.h"

/* ARC start related headers */
#include "machine/soc/intel/quark_se/quark/soc_setup.h"
#include "machine.h"

/* Watchdog helper */
#include "wdt_helper.h"

/* CFW setup helper */
#include "cfw/cfw_service.h"
#include "cfw/cproxy.h"

cfw_client_t * system_setup(T_QUEUE *q, handle_msg_cb_t cb, void *cb_data)
{
	cfw_client_t * client;

	/* Initialize OS abstraction */
	os_init();

	/* Setup BSP and get main queue */
	*q = bsp_setup();

	/* start Quark watchdog */
	wdt_start(WDT_MAX_TIMEOUT_MS);

	/* Component framework and services setup */
	client = cfw_app_setup(*q, cb, cb_data);
	pr_info(LOG_MODULE_MAIN, "cfw init done");

	/* Cproxy is another (deprecated) way to communicate with the component
	 * framework, used by some test commands and some services. Initialize
	 * it here. */
	cproxy_init(*q);

	/* Initialize ARC shared structure and start it. */
	shared_data->ports = port_get_port_table();
	shared_data->services = services;
	shared_data->service_mgr_port_id = cfw_get_service_mgr_port_id();
	start_arc(0);

	return client;
}
