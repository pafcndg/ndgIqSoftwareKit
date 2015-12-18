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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "drivers/eiaextensions.h"

#include <nanokernel.h>
#include "zephyr/common.h"
#include "util/workqueue.h"

#include "machine.h"

#include "infra/log.h"
#include "infra/log_impl_cbuffer.h"
#include "infra/ipc.h"
#include "infra/tcmd/engine.h"
#include "infra/port.h"
#include "infra/pm.h"
#include "infra/device.h"
#include "infra/time.h"

#include "cfw/cfw.h"
#include "services/gpio_service/gpio_service.h"
#include "services/adc_service/adc_service.h"
#ifdef CONFIG_SERVICES_SENSOR_IMPL
#include "sc_exposed.h"
#include "services/sensor_service/sensor_svc_api.h"
#endif
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"

#include "drivers/ss_gpio_iface.h"

#include "machine/soc/intel/quark_se/arc/soc_setup.h"

#define LOG_STACK_SIZE 600
char log_fiber_stack[LOG_STACK_SIZE];

/* This is needed by eventual math function usage */
int __errno;

#define WQ_STACK_SIZE 640
char wq_stack[WQ_STACK_SIZE];
void workFiber(int p1, int p2)
{
	T_QUEUE queue = queue_create(10, NULL);
	workqueue_init(queue, &default_work_queue);
	workqueue_run(&default_work_queue);
}

static T_QUEUE service_mgr_queue;


void services_init()
{
	service_mgr_queue = queue_create(64, NULL);
	pr_debug(LOG_MODULE_MAIN, "Ports: %p services: %p %d", shared_data->ports,
		shared_data->services, shared_data->service_mgr_port_id);


	_cfw_init_proxy(service_mgr_queue, shared_data->ports,
			shared_data->services, shared_data->service_mgr_port_id);
	ipc_async_init(service_mgr_queue);
	set_cpu_message_sender(CPU_ID_QUARK, ipc_async_send_message);
	set_cpu_free_handler(CPU_ID_QUARK, ipc_async_free_message);
	pr_info(LOG_MODULE_MAIN, "cfw init done");
	/* Test Commands initialization */
	tcmd_async_init(service_mgr_queue, "arc");
	gpio_service_init(service_mgr_queue, SS_GPIO_SERVICE_ID);
	gpio_service_init(service_mgr_queue, AON_GPIO_SERVICE_ID);
	adc_service_init(service_mgr_queue, SS_ADC_SERVICE_ID);
#ifdef CONFIG_SERVICES_SENSOR_IMPL
	sensor_core_create();
	ss_service_init(service_mgr_queue);
#endif
	pr_info(LOG_MODULE_MAIN, "Service init done");
}

static void fst_sys_init(void)
{
	uint32_t    sreg;

	/* Start AON Counter */
	SCSS_REG_VAL(SCSS_AONC_CFG) |= 0x00000001;

	/* Enable instruction cache - TODO fix magic numbers */
	sreg = _lr(0x11);
	sreg &= 0xfffffffe;
	_sr(sreg, 0x11);  // Bit 0 of Aux Reg 0x11.

	pr_debug(LOG_MODULE_MAIN, "IRQ Reg: %p\n", _lr(0xe));
}

void main(void)
{
	struct cfw_message * message;
	T_QUEUE_MESSAGE m;
	OS_ERR_TYPE err = E_OS_OK;

	/* Setup interrupt priorities, clocks etc .... */
	fst_sys_init();

	os_init();
	log_init();
	soc_setup();
	services_init();

	task_fiber_start(&log_fiber_stack[0], LOG_STACK_SIZE,(nano_fiber_entry_t)log_task, 0, 0, 51, 0);
	task_fiber_start(&wq_stack[0], WQ_STACK_SIZE, workFiber, 0, 0, 0, 0);
	pr_info(LOG_MODULE_MAIN, "Platform init done");

	/* Sync IPC channel */
	SOC_MBX_INT_UNMASK(IPC_QRK_SS_REQ);
	/* Async IPC channel */
	SOC_MBX_INT_UNMASK(IPC_QRK_SS_ASYNC);

	pr_debug(LOG_MODULE_MAIN, "PM initialized");
	uint32_t time_last_ping = get_uptime_ms();
	while(1) {
		m = NULL;
		queue_get_message(service_mgr_queue, &m, OS_WAIT_FOREVER, &err);
		message = (struct cfw_message *) m;
		if ( err == E_OS_OK && message != NULL ) {
			port_process_message(&message->m);
		}
		if (get_uptime_ms()-time_last_ping>30000)
		{
			pr_info(LOG_MODULE_MAIN, "Keep alive notification");
			time_last_ping = get_uptime_ms();
		}
	}
}

