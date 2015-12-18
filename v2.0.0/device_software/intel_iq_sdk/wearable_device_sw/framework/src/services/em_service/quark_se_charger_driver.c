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
#include "quark_se_charger_driver.h"
#include "services/em_service/battery_service_api.h"
#include "services/gpio_service/gpio_service.h"
#include "fuel_gauge_api.h"
#include "infra/log.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "os/os.h"


/* Define temperature threshold */
#define OVERTEMP_THRESHOLD 60

/* Define timers */
#define CH_TM_DELAY_200ms (200)
static void ch_timer_handler(void *timer_event);

/* Define call back function for charging_sm_event */
typedef void (*ch_event_fct)(enum charging_sm_event);

/* Timer definition */
static T_TIMER debounce_timer;
static bool ch_timer_started = false;
static bool gpio_state;

static cfw_service_conn_t *em_gpio_service_conn_ss;

/****************************************************************************************
 *********************** LOCAL FUNCTON IMPLEMENTATION ***********************************
 ****************************************************************************************/

/**@brief Timer Handler function.
 * @param priv_data private data of the timer, passed at creation
 */
static void ch_timer_handler(void *priv_data)
{
	ch_event_fct ch_call_back_event = priv_data;
	ch_timer_started = false;
	if (gpio_state == 0)
		ch_call_back_event(CHARGING_START);
	else
	{
		if ((fg_get_battery_soc() == 100) ||
				((charging_sm_get_state() == INIT) && charging_sm_is_charger_connected()))
			ch_call_back_event(CHARGING_COMPLETE);
		else
			ch_call_back_event(CHARGING_STOP);
	}
}

/**@brief Function to send callback function to event received.
 * @param[in]  state of pin
 * @return   none.
 */
static void ch_debounce_send_cb(bool pin_state)
{
	if (ch_timer_started == true)
	{
		timer_stop(debounce_timer, NULL);
	}
	gpio_state = pin_state;
	ch_timer_started = true;
	timer_start(debounce_timer, CH_TM_DELAY_200ms, NULL);
}

/**@brief Function to get pin state from received status of the event.
 * @param[in]  state of all gpio pin
 * @return   pin state.
 */
static bool ch_format_data(uint32_t state)
{
	return (((state>>(SS_GPIO_CHG_STATUS)) & CH_BIT_MASK_32) == CH_BIT_MASK_32);
}

static void ch_gpio_connect_cb(cfw_service_conn_t * conn, void * param)
{
	if ((void*)SS_GPIO_SERVICE_ID == param)
	{
		pr_debug(LOG_MODULE_CH,"GPIO OPEN SERVICE: SS_GPIO_SERVICE_ID");
		em_gpio_service_conn_ss = conn;
		/* Pin configuration */
		gpio_listen(em_gpio_service_conn_ss,
				SS_GPIO_CHG_STATUS,BOTH_EDGE,
				DEB_ON,
				NULL);
		/* Ask the state of the port */
		gpio_get_state(em_gpio_service_conn_ss, NULL);
	}
}

/**@brief Callback function for gpio service
 * @param[in] CFW message pointer
 * @param[in] data pointer
 */
static void ch_gpio_handle_msg(struct cfw_message *msg, void *data)
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
		ch_debounce_send_cb(ch_format_data(((gpio_get_rsp_msg_t*)msg)->state));
		break;
	case MSG_ID_GPIO_LISTEN_RSP:
		break;
	case MSG_ID_GPIO_UNLISTEN_RSP:
		break;
	case MSG_ID_GPIO_EVT:
		ch_debounce_send_cb(((gpio_listen_evt_msg_t*)msg)->pin_state);
		break;
	default:
		pr_error(LOG_MODULE_CH,"Default cfw handler %d",
			CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

/**@brief Function to subscribe GPIO service and send gpio status
 * @param[in]  parent_queue number
 * @return   false if function success, true if function failed.
 */
static bool ch_gpio_init(T_QUEUE parent_queue)
{
	cfw_client_t * gpio_client = cfw_client_init(parent_queue, ch_gpio_handle_msg,NULL);
	if (gpio_client == NULL)
	{
		pr_debug(LOG_MODULE_CH,"Invalid handle");
		return false;
	}
	cfw_open_service_helper(gpio_client, SS_GPIO_SERVICE_ID,
				ch_gpio_connect_cb, (void*)SS_GPIO_SERVICE_ID);
	return true;
}

/****************************************************************************************
 *********************** ACCESS FUNCTION IMPLEMENTATION *********************************
 ****************************************************************************************/

bool ch_init(T_QUEUE parent_queue, void* call_back)
{
	OS_ERR_TYPE ch_tm_error;
	if(call_back == NULL)
	{
		pr_debug(LOG_MODULE_CH,"Invalid handle");
		return false;
	}
	/* Pass the call_back to the timer private data */
	debounce_timer = timer_create(ch_timer_handler, call_back, CH_TM_DELAY_200ms,
			false, false, &ch_tm_error);

	ch_timer_started = false;
	return ch_gpio_init(parent_queue);
}
