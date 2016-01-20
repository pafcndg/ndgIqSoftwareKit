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
#include "services/em_service/battery_service.h"
#include "infra/log.h"
#include "em_board_config.h"

static struct charger_fct* charger;
static struct power_supply_fct* source0;
static struct power_supply_fct* source1;
static struct power_supply_fct* source2;
static struct power_supply_fct* current_source = NULL;

static enum e_state state;
static enum charging_sm_source power_supply;

/* Define call back handler for power supply signal event */
static void handler_ps_event(enum charging_sm_event event,enum charging_sm_source src);

/* Define call back handler for soc_charger_driver signal event */
static void handler_charger_event(enum charging_sm_event event);


/* Define call back function for Battery_Service signal event */
typedef void (*sm_event_fct)(enum e_bs_ch_event event);
static sm_event_fct sm_call_back_event;

/****************************************************************************************
 ******************** STATE (LOCAL) FUNCTION IMPLEMENTATION *****************************
 ****************************************************************************************/

static void state_init(void)
{
	pr_info(LOG_MODULE_CH,"CHARGER state: INIT");
	state=INIT;
}

static void state_discharge(void)
{
	pr_info(LOG_MODULE_CH,"CHARGER state: DISCHARGE");
	state=DISCHARGE;
}

static void state_charge(void)
{
	state=CHARGE;
	pr_info(LOG_MODULE_CH,"CHARGER state: CHARGE");
}

static void state_complete(void)
{
	pr_info(LOG_MODULE_CH,"CHARGER state: COMPLETE");
	state=COMPLETE;
	sm_call_back_event(BS_CH_EVENT_CHARGE_COMPLETE);
	if ((current_source != NULL) && (current_source->maintenance_enable != NULL))
		current_source->maintenance_enable();
}

static void state_fault(void)
{
	pr_info(LOG_MODULE_CH,"CHARGER state: FAULT\n");
	state=FAULT;
}

/****************************************************************************************
 *********************** LOCAL FUNCTON IMPLEMENTATION ***********************************
 ****************************************************************************************/

/**@brief Function for update the power supply pointer.
 */
static void update_source(void)
{
	switch (power_supply)
	{
	case NONE: current_source = source0; break;
	case SRC1: current_source = source1; break;
	case SRC2: current_source = source2; break;
	/* Warning: check hardware priority to match with it */
	case BOTH: current_source = source1; break;
	default: break;
	}
}


/**@brief Callback handler function for soc charger.
 * @param[in]  name of event (enum charging_sm_event)
 */
static void handler_charger_event(enum charging_sm_event event)
{
	switch(event)
	{
	case CHARGING_START:
		switch(state)
		{
		case INIT:
			sm_call_back_event(BS_CH_EVENT_INIT_DONE);
			state_charge(); break;

		case DISCHARGE: state_charge(); break;

		case COMPLETE: state_charge(); break;

		default: break;
		}
		break;

	case CHARGING_STOP:
		switch(state)
		{
		case INIT:
			sm_call_back_event(BS_CH_EVENT_INIT_DONE);
			state_discharge(); break;

		case CHARGE: state_discharge(); break;

		default: break;
		}
		break;

	case CHARGING_COMPLETE:
		switch(state)
		{
		case INIT:
			sm_call_back_event(BS_CH_EVENT_INIT_DONE);
			state_complete(); break;

		case CHARGE: state_complete(); break;

		default: break;
		}
		break;

	case CHARGING_FAULT:
		switch(state)
		{
		case INIT:
			sm_call_back_event(BS_CH_EVENT_INIT_DONE);
			state_fault(); break;

		case CHARGE: state_fault(); break;

		default: break;
		}
		break;

	default: break;
	}
}

/**@brief Callback handler function for power supply.
 * @param[in]  name of event (enum charging_sm_event)
 * @param[in]  name of source (enum charging_sm_source)
 */
static void handler_ps_event(enum charging_sm_event event,enum charging_sm_source src)
{
	switch(event)
	{
	case CHARGING_PLUGGED_IN:
		pr_debug(LOG_MODULE_CH,"CHARGER CB recv: CHARGING_PLUGGED_IN %d\n",src);
		if (src == NONE)
		{
			pr_error(LOG_MODULE_CH,
					"CHARGER CB recv: ERROR => NONE isn't a plugged source");
			break;
		}
		/* Power supply update */
		power_supply = power_supply | src;
		update_source();
		sm_call_back_event(BS_CH_EVENT_CHARGER_CONNECTED);
		break;
	case CHARGING_PLUGGED_OUT:
		pr_debug(LOG_MODULE_CH,"CHARGER CB recv: CHARGING_PLUGGED_OUT %d\n",src);
		if (src == NONE)
		{
			pr_error(LOG_MODULE_CH,
					"CHARGER CB recv: ERROR => NONE isn't an unplugged source");
			break;
		}
		/* Power supply update */
		power_supply = power_supply & (~src);
		update_source();
		/* In case of all power supply are disconnected,
		 * force charging state to "DISCHARGE" */
		if (power_supply == NONE)
		{
			sm_call_back_event(BS_CH_EVENT_CHARGER_DISCONNECTED);
			if (state != INIT)
			{
				if (state != DISCHARGE)
					state_discharge();
			}
		}
		break;
	default :
		break;
	}
}

/****************************************************************************************
 *********************** ACCESS FUNCTION IMPLEMENTATION *********************************
 ****************************************************************************************/

bool charging_sm_init(T_QUEUE parent_queue,void* call_back)
{
	sm_call_back_event = call_back;
	power_supply = NONE;
	get_source(&source0, &source1, &source2, &charger);
	state_init();
	if (source1->init != NULL)
		if(!source1->init(parent_queue, handler_ps_event))
			return false;
	if (source2->init != NULL)
		if(!source2->init(parent_queue, handler_ps_event))
			return false;
	if (charger->init != NULL)
		if(!charger->init(parent_queue, handler_charger_event))
			return false;
	return true;
}

bool charging_sm_is_charging()
{
	if ((state == CHARGE)||(state == COMPLETE))
		return true;
	else
		return false;
}

bool charging_sm_is_charger_connected()
{
	if (power_supply == NONE)
		return false;
	else
		return true;
}

batt_svc_chg_src_t charging_sm_get_source()
{
	return current_source->source_type;
}

enum e_state charging_sm_get_state()
{
	return state;
}
