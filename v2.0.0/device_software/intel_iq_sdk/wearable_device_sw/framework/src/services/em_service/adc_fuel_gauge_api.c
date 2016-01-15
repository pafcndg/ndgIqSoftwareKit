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

#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_internal.h"
#include "cfw/cfw_debug.h"
#include "machine.h"
#include "infra/port.h"
#include "infra/log.h"
#include "infra/wakelock_ids.h"
#include "infra/pm.h"

#include "service_queue.h"

#include "services/gpio_service/gpio_service.h"
#include "services/adc_service/adc_service.h"
#include "services/em_service/battery_LUT.h"
#include "services/em_service/battery_service_api.h"
#include "services/em_service/battery_property_api.h"
#include "fuel_gauge_api.h"
#include "charging_sm.h"

/* Define input of GPIO for enabling adc conversion */
#ifdef CONFIG_FG_USE_SS_GPIO
#define GPIO_SVC_EN				SS_GPIO_SERVICE_ID
#else
#define GPIO_SVC_EN				SOC_GPIO_SERVICE_ID
#endif
#define SS_GPIO_SW_FG_TEMP_EN	(uint8_t)(CONFIG_FG_EN_GPIO_ID)	/**< Drive this PIN to enable battery measure access*/

#define FG_FIRST_VOLTAGE_MEASURE		3000	/**< (ms) first voltage measure after boot*/
#define FG_DFLT_VOLTAGE_PERIOD_MEASURE	30000	/**< (ms) default period to realize voltage measure*/
#define FG_LOAD_SWITCH_DELAY			4		/**< (ms) period to realize load switch*/
#if defined(CONFIG_BOARD_CURIE_CRB) || defined(CONFIG_BOARD_CURIE_ORB)
	#define FG_DFLT_TEMPERATURE_PERIOD_MEASURE	60000	/**< (ms) default period to realize temperature measure*/
#else
	#define FG_DFLT_TEMPERATURE_PERIOD_MEASURE	0	/**< No ADC measure*/
#endif

#define BATT_ADC_TO_MV(x)			(((x)*CONFIG_BATT_ADC_FACTOR)/100)

#define TEMP_ADC_FACTOR(x)			( 117780 -46*x)	/* formula after experimentation in thermal room y=117.78-0.0459x*/
#define TEMP_ADC_TO_DEGRE(x)		((TEMP_ADC_FACTOR((x)))/1000)

#define FG_FULL_CHARGE				100

#define FG_INVALID_SOC_VAL			(1 + FG_FULL_CHARGE)	/**< Fuel gauge should never be equal to this value */

#define BATT_LEVEL_FULL_NO_CHARGE	4250
#define LOOKUP_INDEX_10PRCT			10 	/**< index within lookup table related to voltage
							corresponding to 10% of charge*/
#define LOOKUP_INDEX_30PRCT			11 	/**< index within lookup table related to voltage
							corresponding to 30% of charge*/
#define LOOKUP_INDEX_50PRCT			12 	/**< index within lookup table related to voltage
							corresponding to 50% of charge*/
#define LOOKUP_INDEX_70PRCT			13 	/**< index within lookup table related to voltage
							corresponding to 70% of charge*/
#define LOOKUP_INDEX_90PRCT			14 	/**< index within lookup table related to voltage
							corresponding to 90% of charge*/

#define FG_INITIAL_TEMPERATURE		20 	/**< temperature at Boot time */

#define FG_DFLT_LOW_ALARM_THRESHOLD	20 	/**< low alarm threshold at boot time*/

#define FG_DFLT_CRITICAL_ALARM_THRESHOLD	5 	/**< critical alarm threshold at boot time*/

#define FG_DFLT_SHUTDOWN_ALARM_THRESHOLD	3200	/**< (mV) Shutdown alarm threshold */

#define FG_CLR_THRESHOLD			5 	/**< Difference between evt_set_threshold and evt_clear_threshold*/

#define FB_FILTER_COUNT_VALUE		3	/**<  Number of consecutive value used during adc filter*/
#define FB_FILTER_DIFF_MAX_INTER_MEASURE	10 	/**< (mV) We should not detect more than 10mV between 2 consecutive measures*/
#define FG_FILTER_ERROR_MAX			2	/**< Max of consecutive inconsistent value*/
/*
 * @struct adc_request_info_t
 * @brief Save ADC request information
 */
struct adc_request_info_t {
	int channel; /**< ADC channel of latest request*/
	bool is_adc_in_use;/**< equal to true if waiting for an ADC response*/
};
/*
 * @struct fg_evt_report_t
 * @brief contains event status
 * @remark There are two thresholds :
 * 	- evt_set_threshold to generate an event
 * 	- evt_clear_threshold to reset alarm/critical/status
 */
struct fg_evt_report_t {
	uint8_t evt_set_threshold;	/**< sending an event if soc becomes below this value*/
	uint8_t evt_clear_threshold; 	/**< clear boolean is_evt_reporting if soc becomes superior too this value*/
	bool is_evt_reporting;		/**< equal to TRUE once event is reporting*/
};

/*
 * @struct fg_level_report_t
 * @brief Contains level report configuration
 */
struct fg_level_report_t {
	struct fg_evt_report_t fg_low_report;
	struct fg_evt_report_t fg_critical_report;
	uint16_t fg_shutdown_set_threshold;
	bool is_save_done;
};

struct fg_timer_cfg_t {
	uint16_t remaining_time;
	uint16_t interval;
};

struct fg_measure_cfg_t {
	struct fg_timer_cfg_t temp_cfg;
	struct fg_timer_cfg_t voltage_cfg;
};

struct adc_filter_t {
	uint8_t error_count;
	enum e_state last_charger_state;
	uint8_t vbatt_table_count;
	uint16_t vbatt_table[FB_FILTER_COUNT_VALUE];
};
static struct adc_filter_t adc_filter = {};

static struct adc_request_info_t adc_request_info = { };

static cfw_service_conn_t *adc_service_conn = NULL;		/**< ADC service handler */
static bool adc_init_done = false;				/**< equal to 'true' once adc init done */

static cfw_service_conn_t *fg_gpio_service_conn = NULL; 	/**< GPIO service handler */
static bool gpio_init_done = false;				/**< equal to 'true' once gpio init done */

static int16_t current_temperature = FG_INITIAL_TEMPERATURE;
static uint16_t current_batt_voltage = 0;
static uint8_t current_battery_soc = FG_INVALID_SOC_VAL; /**< latest measured SOC */

static fg_event_callback_t g_fg_event_callback;

static struct fg_level_report_t fg_level_report = {}; /**< saved current level report informations */

static  T_TIMER adc_timer = NULL;
static  T_TIMER adc_load_switch_timer = NULL;

/* PM Parameters for ADC GPIO Operations */
struct pm_wakelock fg_wakelock;

static cfw_client_t * g_client = NULL;

struct battery_properties_t g_battery_properties = {};

static uint8_t sw_enable_count = 0;

static bool soc_first_measure = true;

static struct fg_measure_cfg_t fg_measure_cfg = {
		{FG_DFLT_TEMPERATURE_PERIOD_MEASURE,FG_DFLT_TEMPERATURE_PERIOD_MEASURE},
		{FG_FIRST_VOLTAGE_MEASURE,FG_DFLT_VOLTAGE_PERIOD_MEASURE}
};

static fg_status_t fg_set_shutdown_level_alarm_threshold(uint16_t
						shutdown_level_alarm_threshold);
static fg_status_t fg_init_timer(void);

/*
 * @brief Initialised GPIO for enabling adc conversion
 * @return none
 */
static fg_status_t fg_gpio_init(void)
{
	fg_status_t fg_status = FG_STATUS_ERROR_GPIO_SERVICE;

	if (NULL != fg_gpio_service_conn) {

		/* Output configuration */
		gpio_configure(fg_gpio_service_conn, SS_GPIO_SW_FG_TEMP_EN, 1,NULL);

		/* fg_wakelock Init */
                pm_wakelock_init(&fg_wakelock, GPIO_ADC_WAKELOCK);

		fg_status = FG_STATUS_SUCCESS;
	}
	return fg_status;
}
/*
 * @brief Changed GPIO state
 * @return none
 */
static fg_status_t fg_set_gpio_state(uint8_t index, uint8_t val, void* param)
{
	fg_status_t fg_status = FG_STATUS_ERROR_GPIO_SERVICE;

	if ((NULL != fg_gpio_service_conn) &&
	    (val <= 1)) {
		gpio_set_state(fg_gpio_service_conn, index, val, param);
		fg_status = FG_STATUS_ERROR_NOT_SUPPORTED;
	}
	return fg_status;
}
/*
 * @brief Enabling conversion
 * @return FG_STATUS_SUCCESS if Ok
 * @remark Need to be call before initiated any sw_enable_t conversion
 */
static fg_status_t fg_set_sw_enable(void* parameter)
{
	fg_status_t fg_status = FG_STATUS_SUCCESS;
	sw_enable_count++;
	pm_wakelock_acquire(&fg_wakelock);
	fg_status = fg_set_gpio_state(SS_GPIO_SW_FG_TEMP_EN, 1, parameter);

	return fg_status;
}

/*
 * @brief Disabling conversion
 * @return FG_STATUS_SUCCESS if Ok
 * @remark Need to be call after any ADC conversion
 */
static fg_status_t fg_clear_sw_enable(void)
{
	fg_status_t fg_status = FG_STATUS_SUCCESS;
	sw_enable_count--;
	if (sw_enable_count < 0)
	{
		pr_error(LOG_MODULE_FG,"Negative sw enable count");
		sw_enable_count = 0;
	}
	if (0 == sw_enable_count){
		fg_status = fg_set_gpio_state(SS_GPIO_SW_FG_TEMP_EN,0,NULL);
		 pm_wakelock_release(&fg_wakelock);
	}
	return fg_status;

}

/*
 * @brief Notify upper layer if need
 * @parm[in] fuel_gauge Current fuel Gauge
 * @param[in,out] fg_evt_report
 * @param[in,out] is_evt_to_send Equal to true if an event need to be sending
 * @return FG_OP_OK if Ok
 */
static inline fg_status_t fg_check_threshold(uint8_t current_fuel_gauge,
		struct fg_evt_report_t* fg_evt_report,
		bool *is_evt_to_send) {

	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if ((NULL != fg_evt_report) &&
	    (NULL != is_evt_to_send)) {

		*is_evt_to_send = false;

		if (current_fuel_gauge < fg_evt_report->evt_clear_threshold) {
			if(current_fuel_gauge < fg_evt_report->evt_set_threshold) {
				if (true != fg_evt_report->is_evt_reporting) {
					fg_evt_report->is_evt_reporting = true;
					*is_evt_to_send = true;
				}
			}
		} else {
			/* fuel gauge superior to clear_threshold*/
			fg_evt_report->is_evt_reporting = false;
		}

		fg_status = FG_STATUS_SUCCESS;
	}
	return fg_status;
}

/*
 * @brief Notify upper layer if need
 * @parm[in] battery_soc_measured State Of charge measured
 * @param[in] batt_voltage_mv Battery voltage
 * @return FG_OP_OK if Ok
 */
static fg_status_t fg_notify(uint8_t battery_soc_measured,int16_t batt_voltage_mv)
{
	fg_status_t fg_status = FG_STATUS_SUCCESS;
	bool is_evt_to_send = false;
	fg_event_t evt_idx = FG_EVT_LOW_LEVEL;
	struct fg_evt_report_t* fg_evt_report = NULL;
	bs_evt_content_t bs_evt_content = {};

	bs_evt_content.bat_soc = battery_soc_measured;

	if (current_battery_soc != battery_soc_measured) {
		g_fg_event_callback.fg_callback (FG_EVT_SOC_UPDATED,&bs_evt_content);
	}

	fg_evt_report = &fg_level_report.fg_low_report;
	for (evt_idx = FG_EVT_LOW_LEVEL;
	     evt_idx <= FG_EVT_CRITICAL_LEVEL;
	     evt_idx++) {
		fg_status = fg_check_threshold(battery_soc_measured,
				fg_evt_report,
				&is_evt_to_send);
		if (( FG_STATUS_SUCCESS == fg_status) &&
		    (true  == is_evt_to_send)) {
			g_fg_event_callback.fg_callback (evt_idx,&bs_evt_content);
		}
		fg_evt_report = &fg_level_report.fg_critical_report;
	}

	if (batt_voltage_mv < fg_level_report.fg_shutdown_set_threshold) {
		if (false == fg_level_report.is_save_done)
			battery_properties_save(&g_battery_properties);
		fg_level_report.is_save_done = true;
		g_fg_event_callback.fg_callback (FG_EVT_SHUTDOWN_MANDATORY,&bs_evt_content);
	} else {
		fg_level_report.is_save_done = false;
	}

	return fg_status;
}

/*
 * @brief Update battery properties
 * @parm[in] battery_soc_measured last battery soc measured
 * @remark hundred of battery charge cycle is compute if discharge state
 */
static void fg_set_battery_properties(uint8_t battery_soc_measured) {


	if ( (false != charging_sm_is_charging()) &&
	   (battery_soc_measured >= current_battery_soc)) {
		g_battery_properties.battery_charge_delta_soc += battery_soc_measured - current_battery_soc;
		if (FG_FULL_CHARGE <= g_battery_properties.battery_charge_delta_soc) {
			g_battery_properties.battery_charge_cyles++;
			g_battery_properties.battery_charge_delta_soc -= FG_FULL_CHARGE;
		}
	}
	g_battery_properties.battery_initial_capacity = battery_soc_measured;
}
/*
 * @brief Get Percentage of battery capacity from linearization
 * @param[in] voltage_x Voltage corresponding to first point
 * @param[in] percent_x Percentage corresponding to the first point
 * @param[in] voltage_y Voltage corresponding to second point
 * @param[in] percent_y Percentage corresponding to the second point
 * @param[in] voltage_batt Battery voltage from which percentage is required
 * @param[in,out] percent_batt Percentage corresponding to voltage_batt
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_linearize(uint16_t voltage_x,
			 uint16_t percent_x,
			 uint16_t voltage_y,
			 uint16_t percent_y,
			 uint16_t voltage_batt, uint8_t * percent_batt)
{
	fg_status_t fg_status = FG_INVALID_LOOKUP_TABLE;
	uint32_t temp = 0;

	if ((voltage_y != voltage_x) && (NULL != percent_batt)) {
		/*!
		 * Y1 = a.X1 + b
		 * Y2 = a.X2 + b
		 * a = (Y2 - Y1)/(X2 - X1)
		 * percent = a.Voltage + b
		 * percent = Voltage.(Y2 - Y1)/(X2 - X1) + b
		 * percent = Y1 + (Voltage - X1).(Y2 -Y1)/(X2 -X1)
		 */
		temp =
		    (uint32_t) ((voltage_batt - voltage_x) * (percent_y -
							      percent_x));
		*percent_batt =
		    percent_x + (uint16_t) temp / (voltage_y - voltage_x);
		fg_status = FG_STATUS_SUCCESS;
	}
	return fg_status;
}

/*
 * @brief Set current fuel gauge from Temperature and battery voltage
 * @param[in] batt_voltage_mv Current battery voltage
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_set_battery_soc(int16_t batt_voltage_mv)
{
	uint8_t lookup_size = BATTPROP_LOOKUP_TABLE_SIZE;
	uint16_t batt_level_full = 0;
	uint8_t lookup_index;
	uint16_t *p_table = NULL;
	uint8_t battery_soc_measured = 0;
	fg_status_t fg_status = FG_STATUS_ERROR_OUT_OF_RANGE;
	struct battprop_fuelgauge_t battprop_fuelgauge = { };
	bool status_sm;


	battprop_fuelgauge.is_charging = (CHARGE == charging_sm_get_state())? true:false;
	battprop_fuelgauge.temperature = current_temperature;
	status_sm = charging_sm_is_charging() ;

	if (BP_STATUS_SUCCESS ==
			battery_properties_get_lookupTable(&battprop_fuelgauge, &p_table)) {
		batt_level_full = p_table[lookup_size - 1];
		if (batt_level_full < batt_voltage_mv) {
			battery_soc_measured = FG_FULL_CHARGE;
			fg_status = FG_STATUS_SUCCESS;

		} else if (batt_voltage_mv >= p_table[LOOKUP_INDEX_90PRCT]) {

			battery_soc_measured = FG_FULL_CHARGE -1;
			fg_status = FG_STATUS_SUCCESS;
			for (lookup_index = LOOKUP_INDEX_90PRCT;
			     lookup_index <= (lookup_size - 1);
			     lookup_index++) {
				if (batt_voltage_mv < p_table[lookup_index+1]) {
					battery_soc_measured =
					    90 + (lookup_index -
						  LOOKUP_INDEX_90PRCT);
					break;
				}
			}

		} else if (batt_voltage_mv < p_table[LOOKUP_INDEX_10PRCT]) {
			for (lookup_index = 0;
			     lookup_index <= LOOKUP_INDEX_10PRCT;
			     lookup_index++) {
				if (batt_voltage_mv <= p_table[lookup_index]) {
					battery_soc_measured = lookup_index;
					fg_status = FG_STATUS_SUCCESS;
					break;
				}
			}
		} else {
			uint16_t percent_x = 10;
			for (lookup_index = LOOKUP_INDEX_10PRCT,
			     percent_x = 10;
			     lookup_index < LOOKUP_INDEX_90PRCT;
			     lookup_index++,
			     percent_x += 20) {

				if (batt_voltage_mv < p_table[lookup_index+1]) {
					fg_status = fg_linearize(p_table[lookup_index],
							percent_x,
							p_table[lookup_index + 1],
							percent_x + 20,
							batt_voltage_mv, &battery_soc_measured);
					break;
				}
			}
		}

	} else {
		fg_status = FG_INVALID_LOOKUP_TABLE;
	}

	if (FG_STATUS_SUCCESS == fg_status) {
		if(!soc_first_measure){
			if (status_sm){
				if (battery_soc_measured > current_battery_soc){
					fg_set_battery_properties(battery_soc_measured);
					fg_notify(battery_soc_measured,batt_voltage_mv);
					current_battery_soc = battery_soc_measured;
				}
			} else {
				if (battery_soc_measured < current_battery_soc){
					fg_set_battery_properties(battery_soc_measured);
					fg_notify(battery_soc_measured,batt_voltage_mv);
					current_battery_soc = battery_soc_measured;
				}
			}
		}
		else {
			soc_first_measure = false;
			fg_set_battery_properties(battery_soc_measured);
			fg_notify(battery_soc_measured,batt_voltage_mv);
			current_battery_soc = battery_soc_measured;
		}
	}
	return fg_status;

}

/*
 * @brief Parse value form ADC answer
 * @param[in] msg Message from ADC
 * @param[in,out] adc_value ADC value
 * @return FG_STATUS_SUCCESS if OK
 */
static fg_status_t fg_parse_adc_value(struct cfw_message * msg, int16_t * adc_value)
{
	fg_status_t fg_status = FG_STATUS_ERROR_ADC_SERVICE;
	DRIVER_API_RC adc_read_resp = DRV_RC_INVALID_OPERATION;

	if (NULL != msg) {
		adc_read_resp = ((adc_get_val_rsp_msg_t *) msg)->rsp_header.status;
		if ((adc_read_resp == DRV_RC_OK) &&
		  (((adc_get_val_rsp_msg_t *) msg)->reason == ADC_EVT_RX)) {
			*adc_value = ((adc_get_val_rsp_msg_t *) msg)->value;
			fg_status = FG_STATUS_SUCCESS;
		} else {
			pr_error(LOG_MODULE_FG,"adc err:(%d)",adc_read_resp);
		}
	}
	return fg_status;
}

static bool fg_is_charge_evt_detected(struct adc_filter_t *adc_filter) {
	bool is_charge_evt_detected = false;

	if (adc_filter->last_charger_state != charging_sm_get_state()) {
		is_charge_evt_detected = true;
		adc_filter->last_charger_state = charging_sm_get_state();
	}
	return is_charge_evt_detected;
}

static void fg_adc_filter_init(struct adc_filter_t *adc_filter) {
	adc_filter->vbatt_table_count = 0;
	adc_filter->error_count = 0;
	adc_filter->last_charger_state = charging_sm_get_state();
	memset(adc_filter->vbatt_table,0,FB_FILTER_COUNT_VALUE);
}


static bool fg_is_vbatt_monotonous(struct adc_filter_t *adc_filter,uint16_t * batt_voltage) {
	bool is_value_consistent = false;

	switch(adc_filter->last_charger_state) {
	case CHARGE:
		if (*batt_voltage >= adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1])
		{
			/* fg_measure_cfg.voltage_cfg.interval >> 14 (time interval / 8192) for 1.22mV per 10 second */
			if(FB_FILTER_DIFF_MAX_INTER_MEASURE + (fg_measure_cfg.voltage_cfg.interval>>14) >
			(*batt_voltage - adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1]))
				is_value_consistent = true;
		}
		else
			return false;
		break;
	case INIT:
	case DISCHARGE:
	case FAULT:
		if (*batt_voltage <= adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1])
		{
			if (FB_FILTER_DIFF_MAX_INTER_MEASURE + (fg_measure_cfg.voltage_cfg.interval>>14) >
			(adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1] - *batt_voltage))
				is_value_consistent = true;
		}
		else
			return false;
		break;
	case COMPLETE:
		if ((*batt_voltage < adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1] + FB_FILTER_DIFF_MAX_INTER_MEASURE) &&
				(*batt_voltage > adc_filter->vbatt_table[FB_FILTER_COUNT_VALUE-1] - FB_FILTER_DIFF_MAX_INTER_MEASURE))
			is_value_consistent = true;
		break;
	default: break;
	}

	if (false == is_value_consistent)
		adc_filter->error_count++;
	else
		adc_filter->error_count = 0;

	if (FG_FILTER_ERROR_MAX <= adc_filter->error_count)
	{
		is_value_consistent = true;
		adc_filter->error_count = 0;
	}

	return is_value_consistent;
}

static void fg_adc_filter_add_elt(struct adc_filter_t *adc_filter,uint16_t * batt_voltage) {
	uint8_t count = 0;

	for( count=0; count<FB_FILTER_COUNT_VALUE-1; count++) {
		adc_filter->vbatt_table[count] = adc_filter->vbatt_table[count+1];
	}
	adc_filter->vbatt_table[count] = *batt_voltage;
}

static uint16_t fg_adc_filter_average(struct adc_filter_t *adc_filter) {
	uint16_t result = 0;
	uint8_t count = 0;

	for(count=0; count<adc_filter->vbatt_table_count; count++) {
		result += adc_filter->vbatt_table[count];
	}
	result = result/adc_filter->vbatt_table_count;

	return result;
}
/*
 * @brief Adding filter for battery voltage
 * @param[in] batt_voltage Battery voltage measured
 * @return battery voltage filtered
 */
static uint16_t fg_adc_filter(uint16_t * batt_voltage)
{
	uint16_t vbatt_filtered = 0;

	if ( true == fg_is_charge_evt_detected(&adc_filter))
		fg_adc_filter_init(&adc_filter);

	if (FB_FILTER_COUNT_VALUE <= adc_filter.vbatt_table_count) {
		if (true == fg_is_vbatt_monotonous(&adc_filter,batt_voltage))
			fg_adc_filter_add_elt(&adc_filter,batt_voltage);

	} else {
		adc_filter.vbatt_table[adc_filter.vbatt_table_count] = *batt_voltage;
		adc_filter.vbatt_table_count++;
	}

	vbatt_filtered =  fg_adc_filter_average(&adc_filter);

	return vbatt_filtered;
}

/*
 * @brief Get Battery Voltage from ADC voltage
 * @param[in] adc_count ADC numeric value
 * @param[in,out] batt_voltage Result of the conversion
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_convert_voltage(int16_t adc_count, uint16_t * batt_voltage)
{
	uint16_t temp = 0;
	fg_status_t fg_status = FG_STATUS_ERROR_OUT_OF_RANGE;
	if (NULL != batt_voltage) {
		temp = BATT_ADC_TO_MV(adc_count);
		*batt_voltage = fg_adc_filter(&temp);
		fg_status = FG_STATUS_SUCCESS;
	} else {
		fg_status = FG_STATUS_ERROR_PARAMETER;
	}
	return fg_status;
}

/*
 * @brief Get temperature (in Celcus) from ADC voltage
 * @param[in] adc_count ADC numeric value
 * @param[in,out] temperature Result of the conversion
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_convert_temp(int16_t adc_count, int16_t * temperature)
{
	fg_status_t fg_status = FG_STATUS_ERROR_OUT_OF_RANGE;

	if (NULL != temperature) {
		*temperature = TEMP_ADC_TO_DEGRE(adc_count);
		fg_status = FG_STATUS_SUCCESS;
	} else {
		fg_status = FG_STATUS_ERROR_PARAMETER;
	}
	return fg_status;
}

/*
 * @brief Send ADC conversion result to the one which did the request
 * @param[in] channel Channel from where conversion come from
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_response_by_channel(struct cfw_message * msg, int channel)
{

	fg_status_t fg_status = FG_STATUS_ERROR_IPC;
	int16_t adc_value = 0;
	uint16_t batt_voltage = 0;
	int16_t temperature = 0;

	fg_status = fg_parse_adc_value(msg, (int16_t *) &adc_value);

	switch (channel) {
	case ADC_VOLTAGE_CHANNEL:

		if (FG_STATUS_SUCCESS == fg_status) {
			fg_status = fg_convert_voltage(adc_value, &batt_voltage);
			if (FG_STATUS_SUCCESS == fg_status) {
				current_batt_voltage = batt_voltage;
				fg_status = fg_set_battery_soc(batt_voltage);
				if (FG_STATUS_SUCCESS != fg_status )
					pr_error(LOG_MODULE_FG,"unable to retrieve battery fuel gauge");
			}
		}
		/* Disabling conversion */
		fg_clear_sw_enable();
		break;
	case ADC_TEMPERATURE_CHANNEL:

		if (FG_STATUS_SUCCESS == fg_status) {
			fg_status = fg_convert_temp(adc_value,&temperature);
			if (FG_STATUS_SUCCESS == fg_status) {
				current_temperature = temperature;
#ifdef DEBUG_BATTERY_SERVICE
				pr_info(LOG_MODULE_FG,"Batt Temp [%dC] \n",current_temperature);
#endif

			}
		}
		/* Disabling conversion */
		fg_clear_sw_enable();
		break;
	default:
		fg_status = FG_STATUS_ERROR_NOT_SUPPORTED;
		break;
	}
	return fg_status;
}
/**
 * @brief Adc load switch timer callback
 * @param[in] data from ADC.
 * @return none
 */
static void fg_load_switch_timer_callback (void* channel)
{
	adc_get_value(adc_service_conn, (uint32_t)channel, NULL);
}
static void (*fg_init_done_cb)(void) = NULL;

static void service_connection_cb(cfw_service_conn_t * conn, void * param)
{
	fg_status_t fg_status = FG_STATUS_NOT_INITIALIZED;

	if ((void*)SS_ADC_SERVICE_ID == param) {
#ifdef DEBUG_BATTERY_SERVICE
		pr_info(LOG_MODULE_FG,"ADC service open\n");
#endif
		adc_service_conn = conn;
		adc_init_done = true;
	} else if ((void*)GPIO_SVC_EN == param) {
#ifdef DEBUG_BATTERY_SERVICE
		pr_info(LOG_MODULE_FG,"GPIO service open\n");
#endif
		fg_gpio_service_conn = conn;
		gpio_init_done = true;
		fg_gpio_init();
	}

	if ((gpio_init_done == true) && (adc_init_done == true) && (fg_init_done_cb != NULL)){
		fg_status = fg_init_timer();
		if (fg_status == FG_STATUS_SUCCESS) {
			if (BP_STATUS_SUCCESS != battery_properties_get(&g_battery_properties) ) {
				pr_error(LOG_MODULE_FG,"batt_prop err");
			} else {
				fg_set_shutdown_level_alarm_threshold(g_battery_properties.battery_terminate_voltage);
				current_battery_soc = g_battery_properties.battery_initial_capacity;
				fg_init_done_cb();
			}
		}
	}

}

/**
* \brief Generic Callback for IPC message
*/
static void fg_handle_msg(struct cfw_message *msg, void *data)
{
	switch(CFW_MESSAGE_ID(msg)) {
	case MSG_ID_CFW_CLOSE_SERVICE_RSP:
		break;
	case MSG_ID_ADC_GET_VAL_RSP:
		fg_response_by_channel(msg, adc_request_info.channel);
		adc_request_info.is_adc_in_use = false;
		if (adc_load_switch_timer){
			timer_delete(adc_load_switch_timer, NULL);
			adc_load_switch_timer = NULL;
		}
		break;
	case MSG_ID_GPIO_CONFIGURE_RSP:
		if ( 0 != ((gpio_configure_rsp_msg_t*)msg)->rsp_header.status) {
			pr_error(LOG_MODULE_FG,"gpio_conf err");
		}
		break;
	case MSG_ID_GPIO_SET_RSP:
		if ( 0 != ((gpio_set_rsp_msg_t*)msg)->rsp_header.status) {
			pr_error(LOG_MODULE_FG,"gpio_set err");
		}
		if (NULL != msg->priv){
			OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;
			if (adc_load_switch_timer == NULL)
			{
				adc_load_switch_timer = timer_create(fg_load_switch_timer_callback,
						(void*)(*(uint32_t*)msg->priv),
						FG_LOAD_SWITCH_DELAY,
						false,
						true,
						&err);
			}
			if (E_OS_OK != err) {
				pr_error(LOG_MODULE_FG,"fg_load_switch err");
			}
		}
		break;
	default: break;
	}
	cfw_msg_free(msg);
}

/**
 * @brief Compute the clear_threshold
 * @param[in] set_threshold:
 * @param[in,out] clear_threshold:
 * @retval FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_compute_clear_threshold(uint8_t set_threshold, uint8_t* clear_threshold) {

	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != clear_threshold) {
		*clear_threshold = FG_CLR_THRESHOLD + set_threshold;
		if (FG_FULL_CHARGE <= *clear_threshold) {
			*clear_threshold = FG_FULL_CHARGE;
		}
	}
	return fg_status;
}

/*
 * @brief Initialized init level report
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_init_level_report(void) {

	fg_status_t fg_status = FG_STATUS_SUCCESS;

	fg_set_low_level_alarm_threshold(FG_DFLT_LOW_ALARM_THRESHOLD);
	fg_level_report.fg_low_report.is_evt_reporting = false;

	fg_set_critical_level_alarm_threshold(FG_DFLT_CRITICAL_ALARM_THRESHOLD);
	fg_level_report.fg_critical_report.is_evt_reporting = false;

	fg_level_report.fg_shutdown_set_threshold = FG_DFLT_SHUTDOWN_ALARM_THRESHOLD;
	fg_level_report.is_save_done = false;
	return fg_status;
}

/**
 * @brief Saving list of event callback
 * @param[in] fg_event_callback list of event callback.
 * @return FG_STATUS_SUCCESS if Ok
 */
static fg_status_t fg_save_callback(fg_event_callback_t* fg_event_callback)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	memset(&g_fg_event_callback, 0, sizeof(fg_event_callback_t));

	if(NULL != fg_event_callback) {
		if(NULL != fg_event_callback->fg_callback) {
			g_fg_event_callback.fg_callback = fg_event_callback->fg_callback;
			fg_status = FG_STATUS_SUCCESS;
		}
	}

	return (fg_status);
}

static void fg_start_voltage_conversion(void) {
	if (adc_init_done && gpio_init_done &&
	    (false == adc_request_info.is_adc_in_use)) {
		adc_request_info.is_adc_in_use = true;
		adc_request_info.channel = ADC_VOLTAGE_CHANNEL;
		/*enabling conversion */
		fg_set_sw_enable((void *)&adc_request_info.channel);
	}
}

static void fg_start_temp_conversion(void) {
	if (adc_init_done && gpio_init_done &&
	    (false == adc_request_info.is_adc_in_use)) {
		adc_request_info.is_adc_in_use = true;
		adc_request_info.channel = ADC_TEMPERATURE_CHANNEL;
		/*enabling conversion */
		fg_set_sw_enable((void *)&adc_request_info.channel);
	}
}

/*
 * @brief Set interval between two measure related to temperature
 * @parm[in] temp_interval New interval
 * @return FG_STATUS_SUCCESS if OK
 * @remark if temp_interval is equal to zero, temperature measure are suspended
 */
fg_status_t fg_set_temp_interval(uint16_t temp_interval) {
	fg_status_t fg_status = FG_STATUS_ERROR_SET_MEASURE_INTERVAL;
#if defined(CONFIG_BOARD_CURIE_CRB) || defined(CONFIG_BOARD_CURIE_ORB)
	fg_measure_cfg.temp_cfg.interval = temp_interval;
	fg_status = FG_STATUS_SUCCESS;
#else
	fg_measure_cfg.temp_cfg.interval = 0;
#endif
	return fg_status;
}

/*
 * @brief Set interval between two measure related to voltage
 * @parm[in] batt_interval New interval
 * @return FG_STATUS_SUCCESS if OK
 * @remark if batt_interval is equal to zero, vbatt measure are suspended
 */
fg_status_t fg_set_voltage_interval(uint16_t batt_interval) {
	fg_status_t fg_status = FG_STATUS_ERROR_SET_MEASURE_INTERVAL;

	fg_measure_cfg.voltage_cfg.interval = batt_interval;
	fg_status = FG_STATUS_SUCCESS;

	return fg_status;
}
/*
 * @brief Set the next interruption time and restart timer
 * @parm[in] none.
 * @return none.
 */
static void fg_calibration_timer(void)
{
	OS_ERR_TYPE err;

	if (fg_measure_cfg.voltage_cfg.remaining_time < fg_measure_cfg.temp_cfg.remaining_time)
	{
		timer_start(adc_timer, fg_measure_cfg.voltage_cfg.remaining_time, &err);
	}

	if (fg_measure_cfg.voltage_cfg.remaining_time > fg_measure_cfg.temp_cfg.remaining_time)
	{
		timer_start(adc_timer, fg_measure_cfg.temp_cfg.remaining_time, &err);
	}

	if (fg_measure_cfg.voltage_cfg.remaining_time == fg_measure_cfg.temp_cfg.remaining_time)
	{
		fg_measure_cfg.temp_cfg.remaining_time = fg_measure_cfg.temp_cfg.remaining_time + 500;
		timer_start(adc_timer, fg_measure_cfg.voltage_cfg.remaining_time, &err);
	}

	if (E_OS_OK != err)
		pr_error(LOG_MODULE_FG,"fg_timer err: %d",err);
}
static void fg_timer_callback (void* data)
{
	if (fg_measure_cfg.voltage_cfg.remaining_time < fg_measure_cfg.temp_cfg.remaining_time)
	{
		fg_start_voltage_conversion();
		fg_measure_cfg.temp_cfg.remaining_time = fg_measure_cfg.temp_cfg.remaining_time - fg_measure_cfg.voltage_cfg.remaining_time;
		fg_measure_cfg.voltage_cfg.remaining_time = fg_measure_cfg.voltage_cfg.interval;
	}
	else
	{
		fg_start_temp_conversion();
		fg_measure_cfg.voltage_cfg.remaining_time = fg_measure_cfg.voltage_cfg.remaining_time - fg_measure_cfg.temp_cfg.remaining_time;
		fg_measure_cfg.temp_cfg.remaining_time = fg_measure_cfg.temp_cfg.interval;
	}
	fg_calibration_timer();
}

static fg_status_t fg_init_timer(void)
{
	fg_status_t fg_status = FG_STATUS_ERROR_TIMER_INIT;
	OS_ERR_TYPE err = E_OS_ERR_UNKNOWN;

	adc_timer = timer_create(fg_timer_callback,
			NULL,
			0,
			false,
			false,
			&err);

	if (E_OS_OK == err)
	{
		fg_status = FG_STATUS_SUCCESS;
	}
	else
	{
		pr_error(LOG_MODULE_FG,"fg_timer err");
	}
	fg_calibration_timer();
	return fg_status;
}

/**
 * @brief Initialize the Fuel Gauge Interface.
 * @param[in] fg_svc_queue.
 * @param[in] fg_event_callback.
 * @return FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_init(void * fg_svc_queue,fg_event_callback_t* fg_event_callback, void (*bs_fuel_gauge_status)(void))
{

	fg_status_t fg_status = FG_STATUS_NOT_INITIALIZED;
	adc_request_info.is_adc_in_use = false;

	if( (NULL != fg_svc_queue) &&
	    (NULL != fg_event_callback) ) {
		fg_status = fg_save_callback(fg_event_callback);
		if (FG_STATUS_SUCCESS == fg_status) {
			fg_init_level_report();
			fg_adc_filter_init(&adc_filter);
			fg_init_done_cb = bs_fuel_gauge_status;
			g_client = cfw_client_init(get_service_queue(), fg_handle_msg, bs_fuel_gauge_status);
			cfw_open_service_helper(g_client, SS_ADC_SERVICE_ID,
					service_connection_cb, (void*)SS_ADC_SERVICE_ID);
			cfw_open_service_helper(g_client, GPIO_SVC_EN,
					service_connection_cb, (void*)GPIO_SVC_EN);
		}
	}

	return (fg_status);
}

/**
 * @brief Close the Fuel Gauge Interface.
 * @param[in] none.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_close(void)
{

	timer_stop(adc_timer,NULL);
	battery_properties_save(&g_battery_properties);
	adc_init_done = false;
	gpio_init_done = false;
	current_battery_soc = FG_INVALID_SOC_VAL;
	fg_init_level_report();
	/*
	 * TODO unregister to every services
	 */
	return (FG_STATUS_SUCCESS);
}

/**
 * @brief Fuel Gauge Get Latest Battery StateOfCharge.
 * @param[in] none.
 * @retval Latest Battery StateOfCharge [SOC in %].
 */
uint8_t fg_get_battery_soc(void)
{
	return current_battery_soc;
}

/**
 * @brief Fuel Gauge set Battery low level alarm threshold.
 * @param[in] low_level_alarm_threshold: low level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_set_low_level_alarm_threshold(uint8_t low_level_alarm_threshold)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (low_level_alarm_threshold > fg_level_report.fg_critical_report.evt_set_threshold) {
		fg_level_report.fg_low_report.evt_set_threshold = low_level_alarm_threshold;
		fg_compute_clear_threshold(fg_level_report.fg_low_report.evt_set_threshold,
				&fg_level_report.fg_low_report.evt_clear_threshold);
#ifdef DEBUG_BATTERY_SERVICE
		pr_info(LOG_MODULE_FG,"SET LOW LEVEL=%d\n",fg_level_report.fg_low_report.evt_set_threshold);
#endif

		fg_status = FG_STATUS_SUCCESS;
	}
	return fg_status;
}

/**
 * @brief Get Battery low level alarm threshold.
 * @param[in,out] low_level_alarm_threshold: low level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_low_level_alarm_threshold(uint8_t* low_level_alarm_threshold)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != low_level_alarm_threshold) {
		*low_level_alarm_threshold = fg_level_report.fg_low_report.evt_set_threshold;
		fg_status = FG_STATUS_SUCCESS;
	}

	return fg_status;
}
/**
 * @brief Fuel Gauge set Battery critical level alarm threshold.
 * @param[in] critical_level_alarm_threshold: critical level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_set_critical_level_alarm_threshold(uint8_t
						  critical_level_alarm_threshold)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (critical_level_alarm_threshold < fg_level_report.fg_low_report.evt_set_threshold) {
		fg_level_report.fg_critical_report.evt_set_threshold = critical_level_alarm_threshold;
		fg_compute_clear_threshold(fg_level_report.fg_critical_report.evt_set_threshold,
				&fg_level_report.fg_critical_report.evt_clear_threshold);
#ifdef DEBUG_BATTERY_SERVICE
		pr_info(LOG_MODULE_FG,"SET CRITICAL LEVEL=%d\n",fg_level_report.fg_critical_report.evt_set_threshold);
#endif
		fg_status = FG_STATUS_SUCCESS;
	}
	return fg_status;
}


/**
 * @brief Fuel Gauge set Battery shutdown level alarm threshold.
 * @param[in] shutdown_level_alarm_threshold: (mV) Shutdown level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 * @remark Must be below threshold specified in memory
 */
static fg_status_t fg_set_shutdown_level_alarm_threshold(uint16_t
						shutdown_level_alarm_threshold)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (shutdown_level_alarm_threshold > FG_DFLT_SHUTDOWN_ALARM_THRESHOLD) {
		fg_level_report.fg_shutdown_set_threshold = shutdown_level_alarm_threshold;
#ifdef DEBUG_BATTERY_SERVICE
		pr_info(LOG_MODULE_FG,"SET SHUTDOWN LEVEL=%d\n",fg_level_report.fg_shutdown_set_threshold);
#endif
		fg_status = FG_STATUS_SUCCESS;
	} else {
		pr_error(LOG_MODULE_FG,"terminate voltage too low %d now is %d",shutdown_level_alarm_threshold, fg_level_report.fg_shutdown_set_threshold);
	}
	return fg_status;
}

/**
 * @brief Get Battery critical level alarm threshold.
 * @param[in,out] critical_level_alarm_threshold: critical level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_critical_level_alarm_threshold(uint8_t*
						  critical_level_alarm_threshold)
{
	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != critical_level_alarm_threshold) {
		*critical_level_alarm_threshold = fg_level_report.fg_critical_report.evt_set_threshold;
		fg_status = FG_STATUS_SUCCESS;
	}

	return fg_status;
}

/**
 * @brief Fuel Gauge get Battery Full Charge Capacity.
 * @param[out] Battery Full Charge Capacity [mAh].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_full_charge_capacity(uint16_t * bat_fcc)
{
	return (FG_STATUS_NOT_IMPLEMENTED);
}

/**
 * @brief Fuel Gauge get Battery Remaining Charge Capacity.
 * @param[out] Battery Remaining Charge Capacity [mAh].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_remaining_charge_capacity(uint16_t * bat_rm)
{
	return (FG_STATUS_NOT_IMPLEMENTED);
}

/**
 * @brief Fuel Gauge get Battery Temperature.
 * @param[in,out] temp Battery Temperature [°C].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_temperature(int16_t* temp)
{

	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != temp) {
		if (true != adc_init_done) {
			fg_status =  FG_STATUS_ERROR_ADC_SERVICE;
			pr_error(LOG_MODULE_FG,"ADC svc err");
		} else {
			*temp = current_temperature;
			fg_status = FG_STATUS_SUCCESS;
		}
	}

	return fg_status;
}

/**
 * @brief Fuel Gauge get Battery charge cycle.
 * @param[in,out] bat_charge_cycle Battery charge cycle.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_charge_cycle(uint16_t* bat_charge_cycle)
{

	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != bat_charge_cycle) {
		*bat_charge_cycle = g_battery_properties.battery_charge_cyles;
		fg_status = FG_STATUS_SUCCESS;
	}

	return fg_status;
}

/**
 * @brief Fuel Gauge get Battery Chemical Identifier.
 * @param[out] Chemical Identifier of the Battery profile currently used.
 * @retval result of the operation.
 */
fg_status_t fg_get_battery_chem_id(uint16_t * bat_chem_id)
{
	return (FG_STATUS_NOT_IMPLEMENTED);
}

/**
 * @brief Fuel Gauge get Battery Cell-Pack Voltage.
 * @param[in,out] batt_voltage Battery Cell-Pack Voltage [mV].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_voltage(uint16_t* batt_voltage)
{

	fg_status_t fg_status = FG_STATUS_ERROR_PARAMETER;

	if (NULL != batt_voltage) {
		if (true != adc_init_done) {
			fg_status =  FG_STATUS_ERROR_ADC_SERVICE;
			pr_error(LOG_MODULE_FG,"ADC svc err");
		} else {
			*batt_voltage = current_batt_voltage;
			fg_status = FG_STATUS_SUCCESS;
		}
	}

	return fg_status;
}

/**
 * @brief Fuel Gauge get Battery Device Type.
 * @param[out]  Battery Device Type.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_device_type(uint16_t * bat_dev_type)
{
	return (FG_STATUS_NOT_IMPLEMENTED);
}

/**
 * @brief Fuel Gauge get Battery Effective Current.
 * @param[out] Battery Effective Current [mA].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_effective_current(uint16_t * eff_curr)
{
	return (FG_STATUS_NOT_IMPLEMENTED);
}
