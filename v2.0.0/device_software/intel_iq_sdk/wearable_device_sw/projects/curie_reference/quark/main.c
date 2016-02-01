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

#include <stdbool.h>
#include <stdint.h>

/* OS include, needed for task handling */
#include <zephyr.h>

/* Infra */
#include "infra/log.h"
#include "infra/ipc.h"
#include "infra/time.h"
#include "infra/pm.h"
#include "util/workqueue.h"

/* Service setup and general headers for this app */
#include "system_setup.h"
#include "cfw/cfw_messages.h"

/* Generic services API */
#include "services/service_queue.h"

/* GPIO service */
#include "services/gpio_service/gpio_service.h"

/* Battery service */
#include "services/em_service/battery_service_api.h"

/* BLE Service */
#include "services/ble_service/ble_service_api.h"

/* UI Services configruation and helper */
#include "ui_config.h"

/* BLE services helper */
#include "lib/ble/ble_app.h"

#include "util/misc.h"

/* Sensing demo helper */
#ifdef CONFIG_SENSING_HELPER
#include "sensing.h"
#endif
#ifdef CONFIG_BODY_IQ
#include "iq/body_iq.h"
#endif

/* Service counter helper */
#include "service_counter.h"

/* Watchdog helper */
#include "wdt_helper.h"

/* IQs */
#ifdef CONFIG_COMMON_IQ
#include "iq/init_iq.h"
#endif


/* List of services ids that are available in the application */
static uint16_t ref_app_services[] = {
	BLE_SERVICE_ID,
/* TODO: FIXME: Cannot list all servicees otherwise we will panic because of the
 * queue size :(
 */
/*
	SS_GPIO_SERVICE_ID,
	SOC_GPIO_SERVICE_ID,
	SS_ADC_SERVICE_ID,
	LL_STOR_SERVICE_ID,
*/
	BATTERY_SERVICE_ID,
#ifdef CONFIG_UI_SERVICE_IMPL
	UI_SVC_SERVICE_ID,
#endif
	PROPERTIES_SERVICE_ID,
#ifdef CONFIG_SERVICES_SENSOR
	ARC_SC_SVC_ID,
#endif
	AON_GPIO_SERVICE_ID,
};

/* System main queue it will be used on the component framework to add messages
 * on it. */
static T_QUEUE queue;
/* Main application client for the component framework, it will be used to
 * reference the queue processing. */
static cfw_client_t * client;

static bool must_shutdown = 0;
/** Client message handler for main application */
static void client_message_handler(struct cfw_message * msg, void * param)
{
	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_SVC_AVAIL_EVT:
		/* Service counter handler will mark a service has started and
		 * print a message once all service have started. */
		service_counter_handler(
				((cfw_svc_available_evt_msg_t*) msg)->service_id,
				 ref_app_services, ARRAY_SIZE(ref_app_services));
		break;
#ifdef CONFIG_UI_SERVICE_IMPL
	case MSG_ID_UI_BTN_SINGLE_EVT: {
		union ui_drv_evt *broadcast_evt = (union ui_drv_evt *)msg;
		/* param is 0 if press is short, positive if press is long */
		if (broadcast_evt->btn_evt.param == 1) {
			led_blink(255, 255, 255);
			must_shutdown = 1;
		}
		else if (broadcast_evt->btn_evt.param == 2)
			ble_app_clear_bonds();
		break;
	}
	case MSG_ID_UI_BTN_DOUBLE_EVT:
		/* restarts the advertisement if not connected. Stop it first */
		ble_app_stop_advertisement();
		ble_app_start_advertisement(BLE_NO_ADV_OPT);
		break;
	case MSG_ID_UI_LED_RSP:
		/* LED has blinked, properly shutdown now */
		if (must_shutdown) {
			shutdown();
		}
		break;
#endif
	}
	/* Free message once processed */
	cfw_msg_free(msg);
}

/* Services startup function */
static void services_setup(void)
{
#ifdef CONFIG_UI_SERVICE_IMPL
	/* UI service startup*/
	ui_service_start_helper(client);
	pr_info(LOG_MODULE_MAIN, "ui service init in progress...");
#endif

	/* Gpio services startup */
	gpio_service_init(get_service_queue(), SOC_GPIO_SERVICE_ID);
	pr_info(LOG_MODULE_MAIN, "SOC GPIO service init in progress...");

	/* The task storage will init low level storage service and property
	 * service */
	task_start(TASK_STORAGE);
	pr_info(LOG_MODULE_MAIN, "Storage service init in progress...");

#ifdef CONFIG_SERVICES_QUARK_SE_BATTERY_IMPL
	/* Battery service initialization */
	bs_init(queue, BATTERY_SERVICE_ID);
	pr_info(LOG_MODULE_MAIN, "Battery service init in progress...");
#endif

#ifdef CONFIG_SENSING_HELPER
	/* Initialize sensing on active queue */
	sensing_init(queue);
	pr_info(LOG_MODULE_MAIN, "Sensing init in progress...");
#endif

	/* Reset BLE core, start registering BLE service with CFW */
	ble_start_app(queue);
	pr_info(LOG_MODULE_MAIN, "BLE service init in progress...");

	/* Initialize service counter, to be able to notify when all services
	 * have started. */
	service_counter_start(client, ref_app_services);
}

void workqueue_task(void *param)
{
	pr_debug(LOG_MODULE_MAIN, "start workqueue");
	workqueue_run(&default_work_queue);
}

void init_workqueue_task()
{
	T_QUEUE q = queue_create(10, NULL);
	pr_debug(LOG_MODULE_MAIN, "Initializing workqueue");
	workqueue_init(q, &default_work_queue);
	task_start(TASK_WORKQUEUE);
}

/* Application main entry point */
void main_task(void *param)
{
	uint32_t time_last_ping;
	/* Hardware and OS setup */
	client = system_setup(&queue, client_message_handler, NULL);

	/* Init IQs before services to make sure that the user events IQ module
	 * is the first to subscribe to button press events when services are available. */
#ifdef CONFIG_COMMON_IQ
	/* IQ setup */
	init_iqs(queue);
#endif

	/* System setvices startup */
	services_setup();

#if defined(CONFIG_WORKQUEUE)
	/* Start the workqueue */
	init_workqueue_task();
#endif

	pr_info(LOG_MODULE_MAIN, "Quark SE go to main loop");

	time_last_ping = get_uptime_ms();
	/* Loop to process message queue */
	while (1) {
		OS_ERR_TYPE err = E_OS_OK;

		/* Process message with a given timeout */
		queue_process_message_wait(queue, 5000, &err);

		/* Print a keep alive message every 30 seconds*/
		if (get_uptime_ms()-time_last_ping>30000)
		{
			pr_info(LOG_MODULE_MAIN, "Keep alive notification");
			time_last_ping = get_uptime_ms();
		}

		/* Acknowledge the system watchdog to prevent panic and reset */
		wdt_keepalive();
	}

}
