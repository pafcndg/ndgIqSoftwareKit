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

#include "charging_handler.h"
#include "em_service/battery_service_api.h"
#include "infra/reboot.h"

#include "infra/log.h"

enum {
	MAIN_EVENT_FULL_BATT,
	MAIN_EVENT_LEVEL_SHUTDOWN
};

enum {
	MAIN_EVENT_UNPLUG_CHRG,
	MAIN_EVENT_PLUG_CHRG
};

static int charger_state = MAIN_EVENT_PLUG_CHRG;
static int battery_state = MAIN_EVENT_FULL_BATT;

static void service_opened_cb(cfw_service_conn_t * handle, void * param)
{
	/* Request for charger status */
	bs_get_battery_info(handle, BS_CMD_CHG_STATUS, NULL);
}

static void message_handler(struct cfw_message * msg, void * param)
{
	switch(CFW_MESSAGE_ID(msg)) {

	case MSG_ID_BATT_SVC_GET_BATTERY_INFO_RSP:{
		bs_get_status_batt_rsp_msg_t *batt_stat = (bs_get_status_batt_rsp_msg_t*)msg;
		if (batt_stat->batt_info_id == BS_CMD_CHG_STATUS){
			bs_is_charger_connected_rsp_msg_t *batt_chg = (bs_is_charger_connected_rsp_msg_t*) &batt_stat->bs_is_charger_connected;
			charger_state = batt_chg->is_charger_connected ?
					MAIN_EVENT_PLUG_CHRG : MAIN_EVENT_UNPLUG_CHRG;
		}
	}
	break;

	case MSG_ID_BATT_SVC_LEVEL_SHUTDOWN_EVT:
		battery_state = MAIN_EVENT_LEVEL_SHUTDOWN;
		if (charger_state == MAIN_EVENT_UNPLUG_CHRG) {
			pr_info(LOG_MODULE_MAIN, "BATTERY LEVEL SHUTDOWN");
			low_batt_shutdown();
		}
	break;

	case MSG_ID_BATT_SVC_CHARGER_CONNECTED_EVT:
		charger_state = MAIN_EVENT_PLUG_CHRG;
	break;

	case MSG_ID_BATT_SVC_CHARGER_DISCONNECTED_EVT:
		charger_state = MAIN_EVENT_UNPLUG_CHRG;
		if (battery_state == MAIN_EVENT_LEVEL_SHUTDOWN) {
			pr_info(LOG_MODULE_MAIN, "BATTERY LEVEL SHUTDOWN");
			low_batt_shutdown();
		}
	break;

	}

	message_free(&msg->m);
}

void charging_handler_init(T_QUEUE queue)
{
	cfw_client_t * cfw_handle = cfw_client_init(queue, message_handler, NULL);

	int bs_events[] = {
		MSG_ID_BATT_SVC_LEVEL_SHUTDOWN_EVT,
		MSG_ID_BATT_SVC_CHARGER_CONNECTED_EVT,
		MSG_ID_BATT_SVC_CHARGER_DISCONNECTED_EVT
	};
	cfw_open_service_helper_evt(cfw_handle, BATTERY_SERVICE_ID, bs_events,
			sizeof(bs_events)/sizeof(*bs_events), service_opened_cb,
			(void *)BATTERY_SERVICE_ID);

}
