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
#include "bq25120_driver.h"
#include "services/em_service/battery_service_api.h"
#include "services/gpio_service/gpio_service.h"
#include "fuel_gauge_api.h"
#include "infra/log.h"
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "os/os.h"
#include "infra/wakelock_ids.h"
#include "drivers/serial_bus_access.h"

#define CHARGER_I2C_ADD		0x6A	/* Charger I2C address */

#define REG_STATUS_ADD		0x00	/* Status and Ship Mode control register */
#define REG_FAULT_ADD		0x01	/* Fault and Faults masks register */
#define REG_TEMP_ADD		0x02	/* TS Control and Faults masks register */

#define BIT_MASK_STATUS		0x03
#define BIT_SHIFT_STATUS	6

#define REG_TEMP_VALUE		0x08;	/* Disable TS function */
#define REG_ICHRG_VALUE		0x58;	/* Set Fast charge control register to 22mA */
#define REG_IPRETERM_VALUE	0x26;	/* Around 20% of ISET = 4.5mA */
#define REG_VB_VALUE		0x96;	/* Set Battery voltage control register to 4.35V */

/* Define temperature threshold */
#define OVERTEMP_THRESHOLD 60

/* Define timers */
#define CH_TM_DELAY_200ms (200)
#define CH_TM_DELAY_20s (20000)	/* < charger_watchdog_timer(50s) / 2 */

/* Define call back function for charging_sm_event */
typedef void (*ch_event_fct)(enum charging_sm_event);

/* Timer definition */
static T_TIMER debounce_timer;
static T_TIMER i2c_wd_timer;	/* Timer to send an I2C request to the charger for reset
						this internal watchdog (50sec) */
static bool ch_deb_timer_started = false;
static bool charger_ready;
static bool sba_busy;	/* Use to block a new SBA request if the preview isn't done */
static struct sba_request req;
static uint8_t tx_buff[10];
static uint8_t rx_buff[10];

static cfw_service_conn_t *em_gpio_service_conn_ss;
static cfw_service_conn_t *em_gpio_service_conn_soc;

/****************************************************************************************
 *********************** LOCAL FUNCTON IMPLEMENTATION ***********************************
 ****************************************************************************************/

static void ch_timer_handler(void *timer_event);
static void ch_i2c_config(void);

/* I2C */

/**@brief Function to release sba blocker and restart i2c_wd_timer.
 * @return   none.
 */
static void ch_release_i2c(void)
{
	sba_busy = false;
	timer_stop(i2c_wd_timer, NULL);
	timer_start(i2c_wd_timer, CH_TM_DELAY_20s, NULL);
}

/**@brief Callback function for ch_i2c_reset_watchdog() function
 * @param[in] SBA request message pointer
 */
static void ch_i2c_reset_watchdog_callback(struct sba_request *req)
{
	ch_release_i2c();
}

/**@brief Function to send an I2C request to the charger
 * for reset this internal watchdog.
 * @return   none.
 */
static void ch_i2c_reset_watchdog ()
{
	if (!sba_busy)
	{
		sba_busy = true;
		tx_buff[0] = REG_STATUS_ADD;
		rx_buff[0] = 0;

		req.request_type = SBA_TRANSFER;
		req.tx_len = 1;
		req.rx_len = 1;
		req.callback = ch_i2c_reset_watchdog_callback;

		sba_exec_request(&req);
	}
}

/**@brief Callback function for ch_i2c_get_fault() function.
 * @param[in] SBA request message pointer
 */
static void ch_i2c_get_fault_callback(struct sba_request *req)
{
	ch_release_i2c();
	pr_error(LOG_MODULE_CH,"Read status: %d\nVoltage Fault: %x TS Fault: %x",
			req->status,rx_buff[0],rx_buff[1]);
}

/**@brief Function to read charger voltage/temperature faults by I2C.
 * @return   none.
 */
static void ch_i2c_get_fault (void)
{
	if (!sba_busy)
	{
		sba_busy = true;
		tx_buff[0] = REG_FAULT_ADD;
		rx_buff[0] = 0;

		req.request_type = SBA_TRANSFER;
		req.tx_len = 1;
		req.rx_len = 2;
		req.callback = ch_i2c_get_fault_callback;

		sba_exec_request(&req);
	}
}

/**@brief Callback function for ch_i2c_get_status() and
 * send charger event by callback to state machine
 * @param[in] SBA request message pointer
 */
static void ch_i2c_get_status_callback(struct sba_request *req)
{
	ch_event_fct ch_call_back_event = req->priv_data;
	uint8_t status;
	ch_release_i2c();
	pr_debug(LOG_MODULE_CH,"Read status: %d\nStatus: %x",req->status,rx_buff[0]);
	if (req->status == DRV_RC_OK)
	{
		status = ((rx_buff[0])>>BIT_SHIFT_STATUS)&BIT_MASK_STATUS;
		switch (status)
		{
		case CHARGING_STOP:
			ch_call_back_event(CHARGING_STOP);
			break;
		case CHARGING_START:
			ch_call_back_event(CHARGING_START);
			break;
		case CHARGING_COMPLETE:
			ch_call_back_event(CHARGING_COMPLETE);
			break;
		case CHARGING_FAULT:
			ch_i2c_get_fault();
			break;
		}
	}
	else
		ch_call_back_event(CHARGING_STOP);
}

/**@brief Function read charger status by I2C.
 * @return   none.
 */
static void ch_i2c_get_status (void)
{
	if (!sba_busy)
	{
		sba_busy = true;
		tx_buff[0] = REG_STATUS_ADD;
		rx_buff[0] = 0;		/* clean buffer */

		req.request_type = SBA_TRANSFER;
		req.tx_len = 1;
		req.rx_len = 1;
		req.callback = ch_i2c_get_status_callback;

		sba_exec_request(&req);
	}
}

/**@brief Callback function for ch_i2c_config()
 * @param[in] SBA request message pointer
 */
static void ch_i2c_config_callback(struct sba_request *req)
{
	pr_debug(LOG_MODULE_CH,"Config status: %d",req->status);
	ch_release_i2c();
	if (req->status == DRV_RC_OK)
	{
		ch_enable(0);
		charger_ready = true;
	}
		/* Ask the state of the GPIO to use debounce timer
		 * and give 200ms for the charger to refresh these
		 * registers and call ch_i2c_get_status() or
		 * ch_i2c_config()*/
		gpio_get_state(em_gpio_service_conn_ss, NULL);

}

/**@brief Function to make the charger configuration by I2C.
 * @return   none.
 */
static void ch_i2c_config(void)
{
	if (!sba_busy)
	{
		sba_busy = true;
		tx_buff[0] = REG_TEMP_ADD;
		tx_buff[1] = REG_TEMP_VALUE;
		tx_buff[2] = REG_ICHRG_VALUE;
		tx_buff[3] = REG_IPRETERM_VALUE;
		tx_buff[4] = REG_VB_VALUE;
		ch_enable(1);
		req.bus_id = SBA_I2C_MASTER_1;
		req.addr.slave_addr = CHARGER_I2C_ADD;
		req.tx_buff = tx_buff;
		req.rx_buff = rx_buff;
		req.request_type = SBA_TX;
		req.tx_len = 5;
		req.tx_buff = tx_buff;
		req.callback = ch_i2c_config_callback;

		sba_exec_request(&req);
	}
}

/* GPIO */

/**@brief Debounce timer Handler function.
 * @param priv_data private data of the timer, passed at creation
 */
static void ch_timer_handler(void *priv_data)
{
	ch_deb_timer_started = false;
	if (charger_ready)
		ch_i2c_get_status();
	else
		ch_i2c_config();
}

/**@brief Function to (re)start debounce timer.
 * @return   none.
 */
static void ch_start_debounce_timer()
{
	if (ch_deb_timer_started == true)
		timer_stop(debounce_timer, NULL);
	else
		ch_deb_timer_started = true;
	timer_start(debounce_timer, CH_TM_DELAY_200ms, NULL);
}

static void ch_gpio_connect_cb(cfw_service_conn_t * handle, void * param)
{
	if ((void*)SS_GPIO_SERVICE_ID == param)
	{
		pr_debug(LOG_MODULE_CH,"GPIO OPEN SERVICE: SS_GPIO_SERVICE_ID");
		em_gpio_service_conn_ss = handle;
		/* Pin configuration */
		gpio_listen(em_gpio_service_conn_ss,
				SS_GPIO_INT,BOTH_EDGE,
				DEB_OFF, NULL);
		/* Ask the state of the GPIO to use debounce timer
		 * and give 200ms for the system to finish these
		 * initializations before use I2C */
		gpio_get_state(em_gpio_service_conn_ss, NULL);
	}
	if ((void*)SOC_GPIO_SERVICE_ID == param)
	{
		pr_info(LOG_MODULE_CH,"GPIO OPEN SERVICE: SOC_GPIO_SERVICE_ID");
		em_gpio_service_conn_soc = handle;
		/* Pin configuration */
		gpio_configure(em_gpio_service_conn_soc, GPIO_SOC_CD, 1, NULL);
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
	case MSG_ID_GPIO_EVT:
	case MSG_ID_GPIO_GET_RSP:
		ch_start_debounce_timer();
		break;
	default:
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
	cfw_open_service_helper(gpio_client, SOC_GPIO_SERVICE_ID,
				ch_gpio_connect_cb, (void*)SOC_GPIO_SERVICE_ID);
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
	debounce_timer = timer_create(ch_timer_handler, NULL, CH_TM_DELAY_200ms,
			false, false, &ch_tm_error);
	i2c_wd_timer = timer_create(ch_i2c_reset_watchdog, NULL, CH_TM_DELAY_20s,
				true, true, &ch_tm_error);
	ch_deb_timer_started = false;
	charger_ready = false;
	sba_busy = false;
	req.priv_data = call_back;
	return ch_gpio_init(parent_queue);
}

void ch_enable(uint8_t state)
{
	gpio_set_state(em_gpio_service_conn_soc, GPIO_SOC_CD,state,NULL);
}
