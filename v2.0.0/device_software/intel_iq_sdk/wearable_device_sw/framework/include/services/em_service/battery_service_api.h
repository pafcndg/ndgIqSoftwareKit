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

#ifndef _BATT_SVC_API_H_
#define _BATT_SVC_API_H_

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "services/services_ids.h"
#include "services/em_service/power_supply_type.h"

#include <stdint.h>

/**
 * @defgroup battery_service Battery Service
 * Handle the multiple functionalities of Battery Service
 * @ingroup services
*/



/**
 * @defgroup battery_service_api  Battery Services API
 * Defines the interface for Battery Services
 * @ingroup battery_service
 * @{
*/

/*
 * @enum batt_status_t
 * battery error list
 */
typedef enum {
	BATT_STATUS_SUCCESS = 0,		/*!< Operation succeed */
	BATT_STATUS_PENDING,			/*!< Operation is pending */
	BATT_STATUS_IN_USE,			/*!< Operation already in use */
	BATT_STATUS_NOT_IMPLEMENTED,		/*!< Functionality not implemented */
	BATT_STATUS_ERROR_ADC_SERVICE,		/*!< ADC service does not function properly */
	BATT_STATUS_ERROR_VALUE_OUT_OF_RANGE, 	/*!< Out of range value Error */
	BATT_STATUS_IPC_ERROR,			/*!< Internal Error */
	BATT_STATUS_ERROR_FUEL_GAUGE,		/*!< Fuel Gauging Error */
	BATT_STATUS_ERROR_NO_MEMORY,		/*!< Memory Error */
	BATT_STATUS_ERROR			/*!< Generic Error */
} batt_status_t;

typedef enum {
	BS_CMD_BATT_LEVEL=0,		/*!<Getting Battery State Of Charge */
	BS_CMD_BATT_STATUS,			/*!< Getting Battery status (CHARGING, DISCHARGING, MAINTENANCE) */
	BS_CMD_BATT_VBATT,			/*!<  Getting Battery Voltage */
	BS_CMD_BATT_TEMPERATURE,	/*!< Getting Battery Temperature */
	BS_CMD_BATT_GET_CHARGE_CYCLE,	/*!< Getting number of Charging cycle */
	BS_CMD_CHG_STATUS,			/*!< Getting Charger status (CONNECTED, NOT CONNECTED)*/
	BS_CMD_CHG_TYPE,			/*!< Getting Charger type (USB, QI)*/
	BS_CMD_SET_MEASURE_INTERVAL,	/*!< Setting the interval between 2 ADC measurement*/
	BS_CMD_LOW_LEVEL_ALARM,		/*!< Getting Low level alarm threshold */
	BS_CMD_CRITICAL_LEVEL_ALARM /*!< Getting Critical level alarm threshold */
} bs_data_info_t;

/** Message ID base offset */
#define MSG_ID_BATT_BASE                                (MSG_ID_BATT_SERVICE_BASE + 0x00)
/** Message ID Response offset */
#define MSG_ID_BATT_RSP_BASE                            (MSG_ID_BATT_SERVICE_BASE + 0x40)
/** Message ID Events offset */
#define MSG_ID_BATT_EVT_BASE                            (MSG_ID_BATT_SERVICE_BASE + 0x80)

/** message ID for Setting Low Level Alarm */
#define MSG_ID_BATT_SVC_GET_BATTERY_INFO_REQ         	(MSG_ID_BATT_BASE + 0x1)
/** message ID for Setting Low Level Alarm */
#define MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_REQ         (MSG_ID_BATT_BASE + 0x2)
/** message ID for Charging source Request */
#define MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_REQ    (MSG_ID_BATT_BASE + 0x3)
/** message ID for Battery Charging State Request */
#define MSG_ID_BATT_SVC_SET_SHUTDOWN_LEVEL_ALARM_REQ	(MSG_ID_BATT_BASE + 0x4)
/** message ID for Charge Cycle Request */
#define MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_REQ		(MSG_ID_BATT_BASE + 0x5)

/** message ID for State of Charge Response */
#define MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP            (MSG_ID_BATT_RSP_BASE + MSG_ID_BATT_SVC_GET_BATTERY_INFO_REQ)
/** message ID for Setting Low Level Alarm Response */
#define MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_RSP           (MSG_ID_BATT_RSP_BASE + MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_REQ)
/** message ID for Setting Critical Level Alarm Response */
#define MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_RSP    (MSG_ID_BATT_RSP_BASE + MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_REQ)
/** message ID for Battery Charging State Response */
#define MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_RSP        (MSG_ID_BATT_RSP_BASE + MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_REQ)

/** message ID for Battery Fully Charged Event */
#define MSG_ID_BATT_SVC_FULLY_CHARGED_EVT               (MSG_ID_BATT_EVT_BASE + 0x01)
/** message ID for Low level Reached Event */
#define MSG_ID_BATT_SVC_LEVEL_LOW_EVT                   (MSG_ID_BATT_EVT_BASE + 0x02)
/** message ID for Critical level Reached Event */
#define MSG_ID_BATT_SVC_LEVEL_CRITICAL_EVT              (MSG_ID_BATT_EVT_BASE + 0x03)
/** message ID for Battery level changed Event */
#define MSG_ID_BATT_SVC_LEVEL_UPDATED_EVT               (MSG_ID_BATT_EVT_BASE + 0x04)
/** message ID for Charger connected Event */
#define MSG_ID_BATT_SVC_CHARGER_CONNECTED_EVT           (MSG_ID_BATT_EVT_BASE + 0x05)
/** message ID for Charger disconnected Event */
#define MSG_ID_BATT_SVC_CHARGER_DISCONNECTED_EVT        (MSG_ID_BATT_EVT_BASE + 0x06)
/** message ID for Shutdown Level Reached Event */
#define MSG_ID_BATT_SVC_LEVEL_SHUTDOWN_EVT              (MSG_ID_BATT_EVT_BASE + 0x07)

/**
 * @brief Setting alarm message structure request
 */
typedef struct bs_set_level_alarm_msg {
	struct cfw_message rsp_header;
	uint8_t level_alarm;
} bs_set_level_alarm_msg_t;

/**
 * @brief Getting Alarm message structure message structure response
 */
typedef struct bs_get_level_alarm_msg {
	uint32_t status;
	uint8_t level_alarm;
} bs_get_level_alarm_msg_t;

/**
 * @brief Getting State Of Charge message structure response
 */
typedef struct bs_get_soc_rsp_msg {
	uint32_t status;
	uint8_t bat_soc;
} bs_get_soc_rsp_msg_t;

/**
 * @brief Getting Charging source message structure response
 */
typedef struct bs_get_charging_source_rsp_msg {
	uint32_t status;
	batt_svc_chg_src_t charging_source;
} bs_get_charging_source_rsp_msg_t;

/**
 * @brief Getting Battery charging state message structure response
 */
typedef struct bs_is_battery_charging_rsp_msg {
	uint32_t status;
	bool is_charging;
} bs_is_battery_charging_rsp_msg_t;

/**
 * @brief Getting battery presence message structure response
 */
typedef struct bs_is_battery_present_rsp_msg {
	struct cfw_rsp_message rsp_header;
	bool is_battery_present;
} bs_is_battery_present_rsp_msg_t;

/**
 * @brief Getting charger connection message structure response
 */
typedef struct bs_is_charger_connected_rsp_msg {
	uint32_t status;
	bool is_charger_connected;
} bs_is_charger_connected_rsp_msg_t;

/**
 * @brief Getting battery voltage message structure response
 */
typedef struct bs_get_voltage_rsp_msg {
	uint32_t status;
	uint16_t bat_vol;
} bs_get_voltage_rsp_msg_t;

/**
 * @brief Getting battery temperature message structure response
 */
typedef struct bs_get_temperature_rsp_msg {
	uint32_t status;
	int16_t bat_temp;
} bs_get_temperature_rsp_msg_t;

/**
 * @brief Getting Charge Cycle message structure response
 */
typedef struct bs_get_charge_cycle_rsp_msg {
	uint32_t status;
	uint16_t bat_charge_cycle;
} bs_get_charge_cycle_rsp_msg_t;

/**
 * @brief Event data message structure
 */
typedef struct bs_evt_content {
	uint8_t bat_soc; /*battery_state_of_charge*/
} bs_evt_content_t;

/**
 * @brief Event message structure
 */
typedef struct bs_listen_evt_msg {
	struct cfw_message header;
	bs_evt_content_t bs_evt_content;
} bs_listen_evt_msg_t;
/**
 * @brief Type of period configuration
 */
typedef enum {
	cfg_voltage = 0,
	cfg_temperature,
}cfg_type_t;

struct period_cfg_t {
	cfg_type_t cfg_type;
	uint16_t new_period_ms;
};

/**
 * @brief ADC measure interval configuration message structure
 */
typedef struct bs_set_measure_interval_msg {
	struct cfw_message header;
	struct period_cfg_t period_cfg;
} bs_set_measure_interval_msg_t;


/**
 * @brief battery status request structure message
 */
typedef struct bs_get_status_batt_msg {
	struct cfw_message header;
	bs_data_info_t batt_info_id;
} bs_get_status_batt_msg_t;

/**
 * @brief battery status response structure message
 */
typedef  struct bs_get_status_batt_rsp_msg {
	struct cfw_rsp_message rsp_header;
	bs_data_info_t batt_info_id;
	union  {
		bs_get_level_alarm_msg_t bs_get_low_level_alarm ;
		bs_get_level_alarm_msg_t bs_get_critical_level_alarm ;
		bs_get_soc_rsp_msg_t bs_get_soc ;
		bs_get_charging_source_rsp_msg_t bs_get_charging_source ;
		bs_is_battery_charging_rsp_msg_t bs_is_battery_charging ;
		bs_is_battery_present_rsp_msg_t bs_is_battery_present ;
		bs_is_charger_connected_rsp_msg_t bs_is_charger_connected ;
		bs_get_voltage_rsp_msg_t bs_get_voltage ;
		bs_get_temperature_rsp_msg_t bs_get_temperature ;
		bs_get_charge_cycle_rsp_msg_t bs_get_charge_cycle ;
	} ;
} bs_get_status_batt_rsp_msg_t ;

/**
 * @brief Initialize and register the Battery Service.
 * @param[in] batt_svc_queue: Battery Service queue for messages.
 * @param[in] service_id Battery service ID.
 * @return   BATT_STATUS_SUCCESS if Ok
 */
batt_status_t bs_init(void *batt_svc_queue, int service_id);

/**
 * @brief Battery Service: get the latest battery Information.
 * @param[in] c: service connection.
 * @param[in] batt_info_id: info id
 * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_get_battery_info(cfw_service_conn_t * c, bs_data_info_t batt_info_id, void *priv);

/**
 * @brief Battery Service: set the battery level of alarm.
 * @param[in] c: service connection.
 * @param[in] level_alarm: level of the alarm.
 * @param[in] message_id: type of information needed
 *				MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_REQ
 * 				MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_REQ
 * 				MSG_ID_BATT_SVC_SET_SHUTDOWN_LEVEL_ALARM_REQ * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_set_level_alarm_thr(cfw_service_conn_t * c,
					 uint8_t level_alarm,int message_id, void *priv);
/**
 * @brief Battery Service: set ADC measure interval.
 * @param[in] c: service connection.
 * @param[in] period_cfg: contains configuration.
 * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_set_measure_interval(cfw_service_conn_t * c, struct period_cfg_t *period_cfg,void *priv);
/** @} */

#endif //_BATT_SVC_API_H_
