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

#ifndef _FUELGAUGE_API_H
#define _FUELGAUGE_API_H

typedef enum {
	FG_STATUS_SUCCESS,
	FG_STATUS_NOT_INITIALIZED,
	FG_STATUS_MEMORY_ERROR,
	FG_STATUS_ERROR_ADC_SERVICE,
	FG_STATUS_ERROR_GPIO_SERVICE,
	FG_STATUS_ERROR_NOT_SUPPORTED,
	FG_STATUS_ERROR_PARAMETER,
	FG_STATUS_ERROR_OUT_OF_RANGE,
	FG_STATUS_ERROR_IPC,
	FG_INVALID_LOOKUP_TABLE,
	FG_STATUS_WAITING_FOR_SERVICE,
	FG_STATUS_ERROR_TIMER_INIT,
	FG_STATUS_ERROR_SET_MEASURE_INTERVAL,
	FG_STATUS_NOT_IMPLEMENTED
} fg_status_t;

typedef enum {
	FG_EVT_SOC_UPDATED,
	FG_EVT_LOW_LEVEL,
	FG_EVT_CRITICAL_LEVEL,
	FG_EVT_SHUTDOWN_MANDATORY
}fg_event_t;

typedef void (* fg_callback_t) (fg_event_t fg_event, bs_evt_content_t* bs_evt_content);

typedef struct{
	fg_callback_t fg_callback;
}fg_event_callback_t;


/**
 * @brief Initialize the Fuel Gauge Interface.
 * @param[in] fg_svc_queue.
 * @param[in] fg_event_callback.
 * @param[in] bs_fuel_gauge_status.
 * @retval result of the operation.
 */
fg_status_t fg_init(void * fg_svc_queue,fg_event_callback_t* fg_event_callback, void (*bs_fuel_gauge_status)(void));


/**
 * @brief Close the Fuel Gauge Interface.
 * @param[in] none.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_close(void);

/**
 * @brief Fuel Gauge Get Latest Battery StateOfCharge.
 * @param[in] none.
 * @retval Latest Battery StateOfCharge [SOC in %].
 */
uint8_t fg_get_battery_soc(void);

/**
 * @brief Fuel Gauge Update Battery StateOfCharge [SOC].
 * @param[in] msg  Request message.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_update_battery_soc(struct cfw_message *msg);

/**
 * @brief Fuel Gauge set Battery low level alarm threshold.
 * @param[in] none.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_set_low_level_alarm_threshold(uint8_t low_level_alarm_threshold);

/**
 * @brief Get Battery low level alarm threshold.
 * @param[in,out] low_level_alarm_threshold: low level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_low_level_alarm_threshold(uint8_t* low_level_alarm_threshold);
/**
 * @brief Fuel Gauge set Battery critical level alarm threshold.
 * @param[in] none.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_set_critical_level_alarm_threshold(uint8_t
						  critical_level_alarm_threshold);

/**
 * @brief Get Battery critical level alarm threshold.
 * @param[in,out] critical_level_alarm_threshold: critical level threshold.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_critical_level_alarm_threshold(uint8_t*
						  critical_level_alarm_threshold);

/**
 * @brief Fuel Gauge get Battery Full Charge Capacity.
 * @param[out] Battery Full Charge Capacity [mAh].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_full_charge_capacity(uint16_t * bat_fcc);

/**
 * @brief Fuel Gauge get Battery Remaining Charge Capacity.
 * @param[out] Battery Remaining Charge Capacity [mAh].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_remaining_charge_capacity(uint16_t * bat_rm);

/**
 * @brief Fuel Gauge get Battery Temperature.
 * @param[in,out] temp Battery Temperature [°C].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_temperature(int16_t* temp);

/**
 * @brief Fuel Gauge get Battery charge cycle.
 * @param[in,out] bat_charge_cycle Battery charge cycle.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_charge_cycle(uint16_t* bat_charge_cycle);

/**
 * @brief Fuel Gauge get Battery Chemical Identifier.
 * @param[out] Chemical Identifier of the Battery profile currently used.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_chem_id(uint16_t * bat_chem_id);

/**
 * @brief Fuel Gauge get Battery Cell-Pack Voltage.
 * @param[in,out] batt_voltage Battery Cell-Pack Voltage [mV].
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_battery_voltage(uint16_t* batt_voltage);

/**
 * @brief Fuel Gauge get Battery Device Type.
 * @param[out]  Battery Device Type.
 * @retval FG_STATUS_SUCCESS if Ok
 */
fg_status_t fg_get_device_type(uint16_t * bat_dev_type);
/*
 * @brief Set interval between two measure related to temperature
 * @parm[in] temp_interval New interval
 * @return FG_STATUS_SUCCESS if OK
 * @remark interval have to be superior to FG_TIMER_PERIOD
 */
fg_status_t fg_set_temp_interval(uint16_t temp_interval);
/*
 * @brief Set interval between two measure related to voltage
 * @parm[in] batt_interval New interval
 * @return FG_STATUS_SUCCESS if OK
 * @remark interval have to be superior to FG_TIMER_PERIOD
 */
fg_status_t fg_set_voltage_interval(uint16_t batt_interval);
#endif //_FUELGAUGE_API_H
