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
#include "cfw/cproxy.h"

#include "infra/port.h"
#include "infra/log.h"
#include "infra/tcmd/handler.h"

#include "services/em_service/battery_service.h"
#include "services/em_service/battery_service_api.h"
#include "battery_service_tcmd.h"

#define ANS_LENGTH 			80
#define STRING_VALUE			"value"

struct _info_for_rsp {
	struct tcmd_handler_ctx *ctx;
	cfw_service_conn_t * battery_service_conn;
};

/**@brief Function to handle requests, responses
 *
 * @param[in]   msg  message.
 * @return      none
 */

static void batt_tcmd_handle_message(struct cfw_message * msg, void * param) {

	struct _info_for_rsp *info_for_rsp = (struct _info_for_rsp *)msg->priv;
	char *answer = balloc(ANS_LENGTH, NULL);

	switch(CFW_MESSAGE_ID(msg)) {

	case MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP:
		switch(((bs_get_status_batt_rsp_msg_t *)msg)->batt_info_id) {
			case BS_CMD_BATT_LEVEL:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
					== BATT_STATUS_SUCCESS) {
					snprintf(answer, ANS_LENGTH, "%s :\t%d[%%]",
							STRING_VALUE,
							((bs_get_status_batt_rsp_msg_t *)msg)->bs_get_soc.bat_soc);
					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			case BS_CMD_BATT_STATUS:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					if(((bs_get_status_batt_rsp_msg_t *)msg)->bs_is_battery_charging.is_charging)
						snprintf(answer, ANS_LENGTH, "%s : CHARGING",
							STRING_VALUE);
					else
						snprintf(answer, ANS_LENGTH, "%s : NOT CHARGING",
							STRING_VALUE);

					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx,NULL);
				break;
			case BS_CMD_BATT_VBATT:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					snprintf(answer, ANS_LENGTH, "%s :\t%d[mV]",
							STRING_VALUE,
							((bs_get_status_batt_rsp_msg_t *)msg)->bs_get_voltage.bat_vol);
					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			case BS_CMD_BATT_TEMPERATURE:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					snprintf(answer, ANS_LENGTH, "%s :\t%d[°C]",
							STRING_VALUE,
							((bs_get_status_batt_rsp_msg_t *)msg)->bs_get_temperature.bat_temp);
					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);

				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			case BS_CMD_BATT_GET_CHARGE_CYCLE:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					snprintf(answer, ANS_LENGTH, "%s :\t%d",
							STRING_VALUE,
							((bs_get_status_batt_rsp_msg_t *)msg)->bs_get_charge_cycle.bat_charge_cycle);
					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);

				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			case BS_CMD_CHG_STATUS:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					if(((bs_get_status_batt_rsp_msg_t *)msg)->bs_is_charger_connected.is_charger_connected)
						snprintf(answer, ANS_LENGTH, "%s : CONNECTED",
							STRING_VALUE);
					else
						snprintf(answer, ANS_LENGTH, "%s : NOT CONNECTED",
							STRING_VALUE);

					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			case BS_CMD_CHG_TYPE:
				if(((bs_get_status_batt_rsp_msg_t *)msg)->rsp_header.status
						== BATT_STATUS_SUCCESS) {
					switch(((bs_get_status_batt_rsp_msg_t *)msg)->bs_get_charging_source.charging_source) {
					case CHARGING_USB:
						snprintf(answer, ANS_LENGTH, "%s : USB",STRING_VALUE);
						break;
					case CHARGING_WIRELESS:
						snprintf(answer, ANS_LENGTH, "%s : Qi",STRING_VALUE);
						break;
					case CHARGING_NONE:
						snprintf(answer, ANS_LENGTH, "%s : NONE",STRING_VALUE);
						break;
					case CHARGING_DC:
						snprintf(answer, ANS_LENGTH, "%s : DC",STRING_VALUE);
						break;
					case CHARGING_UNKNOWN:
					default:
						snprintf(answer, ANS_LENGTH, "Unknown");
						break;
					}
					TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
				} else
					TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
				break;
			default:
				break;
		}
		break;
	case MSG_ID_BATT_SVC_SET_MEASURE_INTERVAL_RSP:
		if(((struct cfw_rsp_message *)msg)->status
				== BATT_STATUS_SUCCESS) {
			snprintf(answer, ANS_LENGTH, "%s : OK",STRING_VALUE);
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		} else {
			TCMD_RSP_ERROR(info_for_rsp->ctx,NULL);
		}
		break;
	case MSG_ID_BATT_SVC_SET_LOW_LEVEL_ALARM_RSP:
		if(((struct cfw_rsp_message *)msg)->status
				== BATT_STATUS_SUCCESS) {
			snprintf(answer, ANS_LENGTH, "%s : OK",STRING_VALUE);
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		} else {
			TCMD_RSP_ERROR(info_for_rsp->ctx, NULL);
		}
		break;
	case MSG_ID_BATT_SVC_SET_CRITICAL_LEVEL_ALARM_RSP:
		if(((struct cfw_rsp_message *)msg)->status
				== BATT_STATUS_SUCCESS) {
			snprintf(answer, ANS_LENGTH, "%s : OK",STRING_VALUE);
			TCMD_RSP_FINAL(info_for_rsp->ctx, answer);
		} else {
			TCMD_RSP_ERROR(info_for_rsp->ctx,NULL);
		}
		break;
	default:
		break;
	}

	cproxy_disconnect(info_for_rsp->battery_service_conn);
	bfree(info_for_rsp);
	bfree(answer);
	cfw_msg_free(msg);
}


static cfw_service_conn_t*  _get_service_conn(struct tcmd_handler_ctx *ctx)
{

	cfw_service_conn_t * batt_tcmd_client_conn = NULL;

	if ((batt_tcmd_client_conn = cproxy_connect(BATTERY_SERVICE_ID, batt_tcmd_handle_message, NULL)) == NULL) {
		TCMD_RSP_ERROR(ctx, "Cannot connect to Battery Service!");
	}

	return batt_tcmd_client_conn;
}

void battery_service_cmd_handler(struct tcmd_handler_ctx *ctx,bs_data_info_t bs_cmd, void* param)
{
	cfw_service_conn_t * batt_tcmd_client_conn = NULL;

	if (!(batt_tcmd_client_conn = _get_service_conn( ctx)))
		return;

	struct _info_for_rsp *info_for_rsp = balloc(sizeof(struct _info_for_rsp), NULL);
	info_for_rsp->ctx = ctx;
	info_for_rsp->battery_service_conn = batt_tcmd_client_conn;

	switch(bs_cmd) {
	case BS_CMD_BATT_LEVEL:
	case BS_CMD_BATT_STATUS:
	case BS_CMD_BATT_VBATT:
	case BS_CMD_BATT_TEMPERATURE:
	case BS_CMD_BATT_GET_CHARGE_CYCLE:
	case BS_CMD_CHG_STATUS:
	case BS_CMD_CHG_TYPE:
		bs_get_battery_info(batt_tcmd_client_conn, bs_cmd, info_for_rsp);
		break;
	case BS_CMD_SET_MEASURE_INTERVAL:
		bs_set_measure_interval(batt_tcmd_client_conn,(struct period_cfg_t*)param,info_for_rsp);
		break;
	case BS_CMD_LOW_LEVEL_ALARM :
		break;
	case BS_CMD_CRITICAL_LEVEL_ALARM :
		break;
	}
}
