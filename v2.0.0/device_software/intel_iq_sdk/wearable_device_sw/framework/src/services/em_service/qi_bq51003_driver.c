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

#include "charging_sm.h"
#include "qi_bq51003_driver.h"
#include "infra/log.h"
#include "services/em_service/battery_service_api.h"
#include "fuel_gauge_api.h"
#include "services/gpio_service/gpio_service.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"

/* Define maintenance voltage threshold at 99% */
#define MAINTENANCE_THRESHOLD_99 99

/* Define timers */
#define QI_TM_DELAY_30s (30000)
static void qi_timer_handler(void *timer_event);

/* Timer definition */
static T_TIMER maintenance_timer;
static bool ch_timer_started = false;

/* Define call back function for charging library event */
typedef void (*qi_event_fct)(enum charging_sm_event event,enum charging_sm_source src);
static qi_event_fct qi_call_back_event;

/* Define GPIO */
static cfw_service_conn_t *qi_service_conn_soc;
static cfw_service_conn_t *qi_service_conn_aon;

/****************************************************************************************
 *********************** LOCAL FUNCTON IMPLEMENTATION ***********************************
 ****************************************************************************************/

/**@brief Timer Handler function.
 * @param priv_data private data of the timer, passed at creation
 */
static void qi_timer_handler(void *priv_data)
{
	if (fg_get_battery_soc() <= MAINTENANCE_THRESHOLD_99)
	{
		qi_maintenance_disable();
	}
}

/**@brief Function to send callback function to event received.
 * @param[in]  state of pin
 * @return   none.
 */
static void qi_send_cb(bool pin_state)
{
	if(pin_state == true)
		qi_call_back_event(CHARGING_PLUGGED_OUT,SRC2);
	else
		qi_call_back_event(CHARGING_PLUGGED_IN,SRC2);
}

/**@brief Function to get pin state from received status of the event.
 * @param[in]  state of all gpio pin
 * @return   pin state.
 */
static bool qi_format_data(uint32_t state)
{
	return (state>>(AON_GPIO_QI_STATUS)) & CH_BIT_MASK_32;
}

static void qi_gpio_connect_cb(cfw_service_conn_t * conn, void * param)
{
	if ((void*)SOC_GPIO_SERVICE_ID == param)
	{
		pr_info(LOG_MODULE_CH,"GPIO OPEN SERVICE: SOC_GPIO_SERVICE_ID");
		qi_service_conn_soc = conn;
		/* Pin configuration */
		gpio_configure(qi_service_conn_soc, SOC_GPIO_EN, 1, NULL);
	}

	if ((void*)AON_GPIO_SERVICE_ID == param)
	{
		pr_info(LOG_MODULE_CH,"GPIO OPEN SERVICE: AON_GPIO_SERVICE_ID");
		qi_service_conn_aon = conn;
		/* Pin configuration */
		gpio_listen(qi_service_conn_aon,
				AON_GPIO_QI_STATUS,BOTH_EDGE,
				DEB_ON,
				NULL);
		/* Ask the state of the port */
		gpio_get_state(qi_service_conn_aon, NULL);
	}
}

/**@brief Callback function for gpio service
 * @param[in] CFW message pointer
 * @param[in] data pointer
 */
static void qi_gpio_handle_msg(struct cfw_message *msg, void *data)
{
	switch (CFW_MESSAGE_ID(msg))
	{
	case MSG_ID_CFW_CLOSE_SERVICE_RSP:
		break;
	case MSG_ID_GPIO_CONFIGURE_RSP:
		break;
	case MSG_ID_GPIO_SET_RSP:
		break;
	case MSG_ID_GPIO_GET_RSP:
		qi_send_cb(qi_format_data (((gpio_get_rsp_msg_t*)msg)->state));
		break;
	case MSG_ID_GPIO_LISTEN_RSP:
		break;
	case MSG_ID_GPIO_UNLISTEN_RSP:
		break;
	case MSG_ID_GPIO_EVT:
		qi_send_cb(((gpio_listen_evt_msg_t*)msg)->pin_state);
		break;
	default:
		pr_error(LOG_MODULE_CH,"Default cfw handler %d",
			CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

/****************************************************************************************
 *********************** ACCESS FUNCTION IMPLEMENTATION *********************************
 ****************************************************************************************/

bool qi_init(T_QUEUE parent_queue,void* call_back)
{
	OS_ERR_TYPE ch_tm_error;
	cfw_client_t * gpio_client;

	if(call_back == NULL)
	{
		pr_error(LOG_MODULE_CH,"Invalid handle");
		return false;
	}
	qi_call_back_event = call_back;

	maintenance_timer = timer_create(qi_timer_handler, NULL, QI_TM_DELAY_30s,true,
			false, &ch_tm_error);

	gpio_client = cfw_client_init(parent_queue, qi_gpio_handle_msg,NULL);
	if (gpio_client == NULL)
	{
		pr_error(LOG_MODULE_CH,"Invalid handle");
		return false;
	}
	cfw_open_service_helper(gpio_client, SOC_GPIO_SERVICE_ID,
			qi_gpio_connect_cb, (void*)SOC_GPIO_SERVICE_ID);
	cfw_open_service_helper(gpio_client, AON_GPIO_SERVICE_ID,
			qi_gpio_connect_cb, (void*)AON_GPIO_SERVICE_ID);
	return true;
}

void qi_enable(void)
{
	pr_info(LOG_MODULE_CH,"Charger ENABLED");
	gpio_set_state(qi_service_conn_soc, SOC_GPIO_EN,0,NULL);
}

void qi_disable(void)
{
	pr_info(LOG_MODULE_CH,"Charger DISABLED");
	gpio_set_state(qi_service_conn_soc, SOC_GPIO_EN,1,NULL);
}

void qi_maintenance_enable(void)
{
	if (!ch_timer_started)
	{
		timer_start(maintenance_timer, QI_TM_DELAY_30s, NULL);
		ch_timer_started = true;
	}
	qi_disable();
}

void qi_maintenance_disable(void)
{
	if (ch_timer_started)
	{
		timer_stop(maintenance_timer, NULL);
		ch_timer_started = false;
	}
	qi_enable();
}
