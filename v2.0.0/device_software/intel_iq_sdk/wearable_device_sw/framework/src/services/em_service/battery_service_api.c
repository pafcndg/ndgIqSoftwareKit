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
#include "cfw/cfw_internal.h"
#include "cfw/cfw_debug.h"
#include "cfw/cfw_messages.h"

#include "services/em_service/battery_service.h"
#include "services/em_service/battery_service_api.h"
/****************************************************************************************
 *********************** SERVICE API IMPLEMENTATION **************************************
 ****************************************************************************************/
/**
 * @brief Battery Service: get the latest battery Information.
 * @param[in] c: service connection.
 * @param[in] batt_info_id: info id
 * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_get_battery_info(cfw_service_conn_t * c, bs_data_info_t batt_info_id, void *priv)
{
	batt_status_t batt_status = BATT_STATUS_IPC_ERROR;

	struct cfw_message *p_msg = cfw_alloc_message_for_service(c,
						MSG_ID_BATT_SVC_GET_BATTERY_INFO_REQ,
						sizeof
						(bs_get_status_batt_msg_t),
						priv);

	if (NULL != p_msg) {
		((bs_get_status_batt_msg_t *) p_msg)->batt_info_id =
				batt_info_id;
		if (E_OS_OK == cfw_send_message(p_msg)) {
			batt_status = BATT_STATUS_SUCCESS;
		}
	} else {
		batt_status = BATT_STATUS_ERROR_NO_MEMORY;
	}
	return batt_status;
}

/**
 * @brief Battery Service: set the battery level of alarm.
 * @param[in] c: service connection.
 * @param[in] low_level_alarm: level of the alarm.
 * @param[in] message_id: type of information needed
 *				MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_REQ
 * 				MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_REQ
 * 				MSG_ID_BATT_SVC_SET_SHUTDOWN_LEVEL_ALARM_REQ
 * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_set_level_alarm_thr(cfw_service_conn_t * c,
					 uint8_t level_alarm, int message_id, void *priv)
{

	batt_status_t batt_status = BATT_STATUS_IPC_ERROR;

	if (level_alarm > 100) {
		batt_status = BATT_STATUS_ERROR_VALUE_OUT_OF_RANGE;
	} else {
		struct cfw_message *p_msg = cfw_alloc_message_for_service(c,
						message_id,
						sizeof
						(bs_set_level_alarm_msg_t),
						priv);

		if (NULL != p_msg) {
			((bs_set_level_alarm_msg_t *) p_msg)->level_alarm =
			    level_alarm;

			if (E_OS_OK == cfw_send_message(p_msg)) {
				batt_status = BATT_STATUS_SUCCESS;
			}
		} else {
			batt_status = BATT_STATUS_ERROR_NO_MEMORY;
		}
	}
	return batt_status;
}

/**
 * @brief Battery Service: set ADC measure interval.
 * @param[in] c: service connection.
 * @param[in] period_cfg: contains configuration.
 * @param[in] priv: additional data. NULL if not used.
 * @retval BATT_STATUS_SUCCESS if OK.
 */
batt_status_t bs_set_measure_interval(cfw_service_conn_t * c, struct period_cfg_t *period_cfg,void *priv)
{
	batt_status_t batt_status = BATT_STATUS_IPC_ERROR;

	struct cfw_message *p_msg = cfw_alloc_message_for_service(c,
					MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_REQ,
					sizeof
					(bs_set_measure_interval_msg_t),
					priv);

	if ((NULL != p_msg) && (NULL != period_cfg)) {
		((bs_set_measure_interval_msg_t *) p_msg)->period_cfg.new_period_ms = period_cfg->new_period_ms;
		((bs_set_measure_interval_msg_t *) p_msg)->period_cfg.cfg_type = period_cfg->cfg_type;

		if (E_OS_OK == cfw_send_message(p_msg)) {
			batt_status = BATT_STATUS_SUCCESS;
		}
	} else {
		batt_status = BATT_STATUS_ERROR_NO_MEMORY;
	}

	return batt_status;
}
