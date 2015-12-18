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

#include "stdbool.h"
#include "os/os.h"
#include "drivers/data_type.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_debug.h"
#include "cfw/cfw_messages.h"
#include "infra/log.h"

#include "services/services_ids.h"
#include "services/em_service/battery_service_api.h"
#include "services/em_service/battery_service.h"
#include "charging_sm.h"
#include "fuel_gauge_api.h"
#include "services/em_service/battery_property_api.h"
#include "service_queue.h"


#define BATT_S_SOC_MAX		100
#define BATT_GRANULARITY	5

/**********************************************************
 ************** Local definitions  ************************
 **********************************************************/
static uint8_t batt_prev_level = 101;

void bs_client_connected(conn_handle_t * instance);
void bs_client_disconnected(conn_handle_t * instance);
static void init_done();

static service_t battery_service = {.service_id = BATTERY_SERVICE_ID,
	.client_connected = bs_client_connected,
	.client_disconnected = bs_client_disconnected,
};

/*******************************************************************************
 *********************** SERVICE CONTROL IMPLEMENTATION ************************
 ******************************************************************************/
/**@brief Function to open a service previously opened by the battery service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
static void battery_handle_open_service_request(struct cfw_message *msg)
{
	return;
}

/**@brief Function to close a service previously opened by the battery service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
static void battery_handle_close_service_request(struct cfw_message *msg)
{
	fg_close();

	return;
}

/**@brief Function to registers events for a service being opened by the Battery service.
 * @param[in]  msg Message from framework.
 * @return   none.
 */
static void battery_register_events_for_service(struct cfw_message *msg)
{
	return;
}

/**@brief Function to handle result of events registration to the framework.
 * @param[in]  msg Message from framework
 * @return   none.
 */
static void battery_handle_events_registration(struct cfw_message *msg)
{
	return;
}

/*******************************************************************************
 *********************** SERVICE IMPLEMENTATION ********************************
 ******************************************************************************/

/**@brief Change the battery level for the granularity defined.
 * @param[in]  pointer of battery level value
 * @return   none.
 */
static void battery_fg_filter(uint8_t* batt_level)
{
	uint8_t bat_temp = BATT_S_SOC_MAX + 1;
	bat_temp = *batt_level % BATT_GRANULARITY;
	if (bat_temp > (BATT_GRANULARITY/2))
		*batt_level = *batt_level + (BATT_GRANULARITY - (bat_temp));
	else
		*batt_level = *batt_level - (bat_temp);
}

/**@brief Callback to manage results of requests for getting the battery SOC.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_get_soc(bs_get_soc_rsp_msg_t *bs_get_soc)
{
	uint8_t bat_soc = BATT_S_SOC_MAX + 1;

	bat_soc = fg_get_battery_soc();
	battery_fg_filter(&bat_soc);

	if (BATT_S_SOC_MAX >= bat_soc) {
		((bs_get_soc_rsp_msg_t *)bs_get_soc)->bat_soc = bat_soc;
		((bs_get_soc_rsp_msg_t *)bs_get_soc)->status = BATT_STATUS_SUCCESS;
	} else {
		((bs_get_soc_rsp_msg_t *)bs_get_soc)->status = BATT_STATUS_ERROR_VALUE_OUT_OF_RANGE;
	}
	return;
}



/**@brief Callback to manage results of requests of getting the battery low level alarm threshold.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_get_low_level_alarm(bs_get_level_alarm_msg_t *bs_get_low_level_alarm)
{
	uint8_t low_level_alarm_threshold = 0;
		if (FG_STATUS_SUCCESS == fg_get_low_level_alarm_threshold(&low_level_alarm_threshold)) {
			bs_get_low_level_alarm->status = BATT_STATUS_SUCCESS;
			bs_get_low_level_alarm->level_alarm = low_level_alarm_threshold;
		} else {
			bs_get_low_level_alarm->status = BATT_STATUS_ERROR_FUEL_GAUGE;
		}
	return;
}

/**@brief Callback to manage results of requests of getting the battery critical level alarm threshold.
 * @param[in]  msg  Request message.
 * @return   none
*/
static void handle_bs_get_critical_level_alarm(bs_get_level_alarm_msg_t *bs_get_critical_level_alarm)
{
	uint8_t critical_level_alarm_threshold = 0;

		if (FG_STATUS_SUCCESS == fg_get_critical_level_alarm_threshold(&critical_level_alarm_threshold)) {
			bs_get_critical_level_alarm->status = BATT_STATUS_SUCCESS;
			bs_get_critical_level_alarm->level_alarm = critical_level_alarm_threshold;
		} else {
			bs_get_critical_level_alarm->status  = BATT_STATUS_ERROR_FUEL_GAUGE;
		}
	return;
}

/**@brief Callback to manage results of requests to know if the battery is charging
 * @param[in]  msg  Request message.
 * @return   none
*/
static void handle_bs_is_battery_charging(bs_is_battery_charging_rsp_msg_t *bs_is_battery_charging)
{

	bs_is_battery_charging->is_charging = charging_sm_is_charging();
	bs_is_battery_charging->status = BATT_STATUS_SUCCESS;
	return;
}

/**@brief Callback to manage results of requests to know if the charger is connected
 * @param[in]  msg  Request message.
 * @return   none
*/
static void handle_bs_is_charger_connected(bs_is_charger_connected_rsp_msg_t *bs_is_charger_connected)
{
	bs_is_charger_connected->is_charger_connected = charging_sm_is_charger_connected();
	bs_is_charger_connected->status = BATT_STATUS_SUCCESS;
	return;
}

/**@brief Callback to manage results of requests of getting the battery charging source.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_get_charging_source(bs_get_charging_source_rsp_msg_t *bs_get_charging_source)
{
	bs_get_charging_source->charging_source = charging_sm_get_source();
	bs_get_charging_source->status = BATT_STATUS_SUCCESS;
	return;
}

/**@brief Callback to manage results of requests of getting the battery voltage.
 * @param[in]  msg  Request message.
 * @return   none
*/

static void handle_bs_get_voltage(bs_get_voltage_rsp_msg_t *bs_get_voltage)
{

	fg_status_t fg_status = FG_STATUS_ERROR_ADC_SERVICE;
	uint16_t  batt_voltage = 0;

		fg_status = fg_get_battery_voltage(&batt_voltage);
		switch(fg_status) {
		case FG_STATUS_SUCCESS:
			bs_get_voltage->status = BATT_STATUS_SUCCESS;
			bs_get_voltage->bat_vol = batt_voltage;
			break;
		case FG_STATUS_ERROR_ADC_SERVICE:
			bs_get_voltage->status = BATT_STATUS_ERROR_ADC_SERVICE;
			break;
		default:
			bs_get_voltage->status = BATT_STATUS_ERROR;
			break;
		}
	return;
}

/**@brief Callback to manage results of requests of getting the battery temperature.
 * @param[in]  msg  Request message.
 * @return   none
*/
static void handle_bs_get_temperature(bs_get_temperature_rsp_msg_t *bs_get_temperature)
{
	fg_status_t fg_status = FG_STATUS_ERROR_ADC_SERVICE;
	int16_t temp = 0;

	fg_status = fg_get_battery_temperature(&temp);
	switch(fg_status) {
	case FG_STATUS_SUCCESS:
		bs_get_temperature->status = BATT_STATUS_SUCCESS;
		bs_get_temperature->bat_temp = temp;
		break;
	case FG_STATUS_ERROR_ADC_SERVICE:
		bs_get_temperature->status = BATT_STATUS_ERROR_ADC_SERVICE;
		break;
	default:
		bs_get_temperature->status = BATT_STATUS_ERROR;
		break;
	}
	return;
}

/**@brief Callback to manage results of requests of getting battery cycle.
 * @param[in]  msg  Request message.
 * @return   none
*/
static void handle_bs_get_charge_cycle(bs_get_charge_cycle_rsp_msg_t *bs_get_charge_cycle)
{
	fg_status_t fg_status = FG_STATUS_ERROR_ADC_SERVICE;
	uint16_t bat_charge_cycle = 0;

		fg_status = fg_get_charge_cycle(&bat_charge_cycle);
		switch(fg_status) {
		case FG_STATUS_SUCCESS:
			bs_get_charge_cycle->status = BATT_STATUS_SUCCESS;
			bs_get_charge_cycle->bat_charge_cycle = bat_charge_cycle;
			break;
		default:
			bs_get_charge_cycle->status = BATT_STATUS_ERROR;
			break;
		}
	return;
}

/**@brief Callback to manage results of requests for getting the battery information.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_get_battery_info(struct cfw_message *msg)
{

	bs_get_status_batt_rsp_msg_t *resp =
		(bs_get_status_batt_rsp_msg_t *) cfw_alloc_rsp_msg(msg,
							MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP,
							sizeof(*resp));

	switch (((bs_get_status_batt_msg_t *) msg)->batt_info_id) {
	case BS_CMD_BATT_LEVEL :
		handle_bs_get_soc(&resp->bs_get_soc);
		resp->rsp_header.status = resp->bs_get_soc.status;
		break;
	case BS_CMD_BATT_STATUS:
		handle_bs_is_battery_charging(&resp->bs_is_battery_charging);
		resp->rsp_header.status = resp->bs_is_battery_charging.status;
		break;
	case BS_CMD_BATT_VBATT:
		handle_bs_get_voltage(&resp->bs_get_voltage);
		resp->rsp_header.status = resp->bs_get_voltage.status;
		break;
	case BS_CMD_BATT_TEMPERATURE:
		handle_bs_get_temperature(&resp->bs_get_temperature);
		resp->rsp_header.status = resp->bs_get_temperature.status;
		break;
	case BS_CMD_BATT_GET_CHARGE_CYCLE:
		handle_bs_get_charge_cycle(&resp->bs_get_charge_cycle);
		resp->rsp_header.status = resp->bs_get_charge_cycle.status;
		break;
	case BS_CMD_CHG_STATUS:
		handle_bs_is_charger_connected(&resp->bs_is_charger_connected);
		resp->rsp_header.status = resp->bs_is_charger_connected.status;
		break;
	case BS_CMD_CHG_TYPE:
		handle_bs_get_charging_source(&resp->bs_get_charging_source);
		resp->rsp_header.status = resp->bs_get_charging_source.status;
		break;
	case BS_CMD_LOW_LEVEL_ALARM:
		handle_bs_get_low_level_alarm(&resp->bs_get_low_level_alarm);
		resp->rsp_header.status = resp->bs_get_low_level_alarm.status;
		break;
	case BS_CMD_CRITICAL_LEVEL_ALARM:
		handle_bs_get_critical_level_alarm(&resp->bs_get_critical_level_alarm);
		resp->rsp_header.status = resp->bs_get_critical_level_alarm.status;
		break;
	default :
		break;	}

	resp->batt_info_id = ((bs_get_status_batt_msg_t *) msg)->batt_info_id ;
	cfw_send_message(resp);
	return;
}

/**@brief Callback to manage results of requests of setting the battery low level alarm threshold.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_set_low_level_alarm(struct cfw_message *msg)
{
	uint8_t low_level_alarm_threshold = 0;
	struct cfw_rsp_message *resp =
		(struct cfw_rsp_message *)cfw_alloc_rsp_msg(msg,
							MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_RSP,
							sizeof(*resp));

	if (NULL != resp) {
		low_level_alarm_threshold = ((bs_set_level_alarm_msg_t *) msg)->level_alarm;
		if (FG_STATUS_SUCCESS == fg_set_low_level_alarm_threshold(low_level_alarm_threshold)) {
			resp->status = 	BATT_STATUS_SUCCESS;
		} else {
			resp->status = BATT_STATUS_ERROR_FUEL_GAUGE;
		}

		cfw_send_message(resp);
	}
	return;
}
/**@brief Callback to manage results of requests of setting the battery critical level alarm threshold.
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_set_critical_level_alarm(struct cfw_message *msg)
{
	uint8_t critical_level_alarm_threshold = 0;
	struct cfw_rsp_message *resp =
		(struct cfw_rsp_message *)cfw_alloc_rsp_msg(msg,
							MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_RSP,
							sizeof(*resp));
	if (NULL != resp) {
		critical_level_alarm_threshold = ((bs_set_level_alarm_msg_t *) msg)->level_alarm;

		if (FG_STATUS_SUCCESS == fg_set_critical_level_alarm_threshold(critical_level_alarm_threshold)) {
			resp->status = BATT_STATUS_SUCCESS;
		} else {
			resp->status = BATT_STATUS_ERROR_FUEL_GAUGE;
		}

		cfw_send_message(resp);
	}
	return;
}
/**@brief Callback to manage results of requests of setting ADC measure interval
 * @param[in]  msg  Request message.
 * @return   none
 */
static void handle_bs_set_measure_interval(struct cfw_message *msg)
{
	uint16_t new_period_ms = 0;
	uint8_t cfg_period;
	fg_status_t fg_status = BATT_STATUS_ERROR;
	struct cfw_rsp_message *resp =
		(struct cfw_rsp_message *)cfw_alloc_rsp_msg(msg,
							MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_RSP,
							sizeof(*resp));
	if (NULL != resp) {
		new_period_ms = ((bs_set_measure_interval_msg_t *) msg)->period_cfg.new_period_ms;
		cfg_period = ((bs_set_measure_interval_msg_t *) msg)->period_cfg.cfg_type;
		if (cfg_temperature == cfg_period) {
			fg_status = fg_set_temp_interval(new_period_ms);
		} else if (cfg_voltage == cfg_period) {
			fg_status = fg_set_voltage_interval(new_period_ms);
		}
		resp->status = fg_status;

		cfw_send_message(resp);
	}
	return;
}

/**@brief Sending an event
 * @param[in]  id  Message identifier
 * @param[in] bs_evt_content Content of battery event
 * @return   none
 */
static void bs_send_evt_msg(uint16_t id, bs_evt_content_t *bs_evt_content)
{
	bs_listen_evt_msg_t *evt = NULL;
	evt = (bs_listen_evt_msg_t*)cfw_alloc_evt_msg(&battery_service, id,
			sizeof(bs_listen_evt_msg_t));

	if ((NULL != evt) &&
		(NULL != bs_evt_content)) {
		memcpy(&evt->bs_evt_content,bs_evt_content,sizeof(bs_evt_content_t));
	}
	cfw_send_event(&evt->header);
	bfree(evt); /* message has been cloned by cfw_send_event */
}

/**@brief Function to inform registered services that the battery is fully charged
 * @return   none
 */
static void handle_bs_fully_charged_evt(void)
{
	bs_send_evt_msg(MSG_ID_BATT_SVC_FULLY_CHARGED_EVT,NULL);
}

/**@brief Function to inform registered services that the battery has reached the low level of charge
 * @param[in] bs_evt_content Content of battery event
 * @return   none
 */
static void handle_bs_level_low_evt(bs_evt_content_t *bs_evt_content)
{
	pr_warning(LOG_MODULE_BS,"[%d%%] BELOW LOW LEVEL",
			bs_evt_content->bat_soc);
	bs_send_evt_msg(MSG_ID_BATT_SVC_LEVEL_LOW_EVT,bs_evt_content);
}

/**@brief Function to inform registered services that the battery has reached the critical level of charge
 * @param[in] bs_evt_content Content of battery event
 * @return   none
 */
static void handle_bs_level_critical_evt(bs_evt_content_t *bs_evt_content)
{
	pr_warning(LOG_MODULE_BS,"[%d%%] BELOW CRITICAL LEVEL",
			bs_evt_content->bat_soc);
	bs_send_evt_msg(MSG_ID_BATT_SVC_LEVEL_CRITICAL_EVT,bs_evt_content);
}

/**@brief Function to inform registered services that the battery SOC has been updated
 * @param[in] bs_evt_content Content of battery event
 * @return   none
 */
static void handle_bs_level_updated_evt(bs_evt_content_t *bs_evt_content)
{
	bs_send_evt_msg(MSG_ID_BATT_SVC_LEVEL_UPDATED_EVT,bs_evt_content);
}

/**@brief Function to inform registered services that the system should shutdown
 * @return   none
 */
static void handle_bs_level_shutdown_evt(bs_evt_content_t *bs_evt_content)
{
	pr_warning(LOG_MODULE_BS,"BELOW SHUTDOWN LEVEL");
	bs_send_evt_msg(MSG_ID_BATT_SVC_LEVEL_SHUTDOWN_EVT,bs_evt_content);
}

/**@brief Function to inform registered services that the battery charger is connected
 * @return   none
 */
static void handle_bs_charger_connected_evt(void)
{
	bs_send_evt_msg(MSG_ID_BATT_SVC_CHARGER_CONNECTED_EVT,NULL);
}

/**@brief Function to inform registered services that the battery charger is disconnected
 * @return   none
 */
static void handle_bs_charger_disconnected_evt(void)
{
	bs_send_evt_msg(MSG_ID_BATT_SVC_CHARGER_DISCONNECTED_EVT,NULL);
}

/*@brief Function called by FG to advertises of any event
 * @param fg_event Event type
 * @param[in] bs_evt_content Content of battery event
 * @return none
 */
static void bs_evt_from_fg(fg_event_t fg_event, bs_evt_content_t *bs_evt_content) {

	uint16_t batt_voltage = 0;
	switch(fg_event) {
	case FG_EVT_LOW_LEVEL:
		handle_bs_level_low_evt(bs_evt_content);
		break;
	case FG_EVT_CRITICAL_LEVEL:
		handle_bs_level_critical_evt(bs_evt_content);
		break;
	case FG_EVT_SOC_UPDATED:
		fg_get_battery_voltage(&batt_voltage);
		pr_info(LOG_MODULE_BS,"Batt Voltage [%dmV]-> SOC[%d%%]", batt_voltage, bs_evt_content->bat_soc);
		battery_fg_filter(&bs_evt_content->bat_soc);
		if(bs_evt_content->bat_soc != batt_prev_level)
		{
			batt_prev_level = bs_evt_content->bat_soc;
			handle_bs_level_updated_evt(bs_evt_content);
		}
		break;
	case FG_EVT_SHUTDOWN_MANDATORY:
		handle_bs_level_shutdown_evt(bs_evt_content);
		break;
	default: break;
	}
}
/*@brief Function called by Charger to advertises of any event
 * @param fg_event Event type
 * @param[in] bs_evt_content Content of battery event
 * @return none
 */
static void bs_evt_from_ch(enum e_bs_ch_event ch_event) {

	switch(ch_event) {

	case BS_CH_EVENT_INIT_DONE:
		init_done();
		break;
	case BS_CH_EVENT_CHARGE_COMPLETE:
		handle_bs_fully_charged_evt();
		break;
	case BS_CH_EVENT_CHARGER_CONNECTED:
		handle_bs_charger_connected_evt();
		break;
	case BS_CH_EVENT_CHARGER_DISCONNECTED:
		handle_bs_charger_disconnected_evt();
		break;
	default :
		break;
	}
}

/**@brief Function to handle requests, responses and events
 * @param[in]  msg  Event message.
 * @return   none
 */
static void bs_handle_message(struct cfw_message *msg, void *param)
{

	switch (CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_OPEN_SERVICE_REQ:
		battery_handle_open_service_request(msg);
		battery_register_events_for_service(msg);
		break;
	case MSG_ID_CFW_CLOSE_SERVICE_REQ:
		battery_handle_close_service_request(msg);
		break;
	case MSG_ID_CFW_REGISTER_EVT_REQ:
		battery_handle_events_registration(msg);
		break;
	case MSG_ID_BATT_SVC_GET_BATTERY_INFO_REQ:
		handle_bs_get_battery_info(msg);
		break;
	case MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_REQ:
		handle_bs_set_low_level_alarm(msg);
		break;
	case MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_REQ:
		handle_bs_set_critical_level_alarm(msg);
		break;
	case MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_REQ:
		handle_bs_set_measure_interval(msg);
		break;
	default:
		pr_info(LOG_MODULE_BS,"SERVICE: unexpected message id: %x",
				CFW_MESSAGE_ID(msg));
		break;
	}
	cfw_msg_free(msg);
}

void bs_client_connected(conn_handle_t * instance)
{
#ifdef DEBUG_BATTERY_SERVICE
	pr_info(LOG_MODULE_BS,"SERVICE: %s ", __func__);
#endif
}

void bs_client_disconnected(conn_handle_t * instance)
{
#ifdef DEBUG_BATTERY_SERVICE
	pr_info(LOG_MODULE_BS,"SERVICE: %s ", __func__);
#endif
}

/****************************************************************************************
 ************************** SERVICE INITIALIZATION **************************************
 ****************************************************************************************/

static void * queue;

/*
 * @brief Register battery service
 * @return none
 * @remark Called by fuel gauge layer to advertise its end of initialization
 */
static void bs_fuel_gauge_init_done(void) {
	if (cfw_register_service(queue, &battery_service,
							 bs_handle_message, NULL) == -1)
		pr_error(LOG_MODULE_BS,"Cannot register Battery service");
}

/*
 * @brief Launch fuel gauge initialization
 * @param[in] bp_status status
 * @return none
 * @remark Called by battery properties layer to advertise battery properties status
 */
static void bs_properties_status(bp_status_t bp_status) {

	fg_event_callback_t  fg_event_callback= {};
	fg_event_callback.fg_callback = (fg_callback_t)bs_evt_from_fg;

	if (BP_STATUS_SUCCESS == bp_status) {
		/* Initialize interface with Fuel Gauge */
		if (FG_STATUS_SUCCESS != fg_init(get_service_queue(),&fg_event_callback, bs_fuel_gauge_init_done)) {
			pr_error(LOG_MODULE_BS,"Cannot initialized Fuel gauge");
		}
	}
}

static void init_done()
{
	if (BP_STATUS_SUCCESS != battery_properties_init(&bs_properties_status))
	{
		pr_error(LOG_MODULE_BS,"Cannot initialized battery properties");
	}
}

/**@brief Function to initialize Battery Service.
 *
 * @details Internal variable and structures are initialized, service and
 *          port handles allocated by the framework. Battery service registers
 *          to the framework.
 * @param[in]  batt_svc_queue Queue of messages to be exchanged.
 * @param[in]  service_id Battery service ID.
 * @return   BATT_STATUS_SUCCESS if Ok
 */
batt_status_t bs_init(void *batt_svc_queue, int service_id)
{

	batt_status_t batt_status = BATT_STATUS_ERROR;

	battery_service.service_id = service_id;

	queue = batt_svc_queue;

	if ((charging_sm_init(batt_svc_queue, bs_evt_from_ch) != true) &&
			(batt_status == BATT_STATUS_SUCCESS) )
	{
		pr_error(LOG_MODULE_BS,"Cannot start services need by Charger_api");
		batt_status = BATT_STATUS_ERROR;
	}

	return batt_status;
}

