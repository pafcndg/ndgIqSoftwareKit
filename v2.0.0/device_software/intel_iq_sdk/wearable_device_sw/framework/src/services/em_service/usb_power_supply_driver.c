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

#include <stdlib.h>
#include "charging_sm.h"
#include "usb_power_supply_driver.h"
#include "infra/log.h"
#include "infra/device.h"
#include "drivers/usb_pm.h"
#include "machine/soc/intel/quark_se/soc_config.h"


/* Define call back function for charging library event */
typedef void (*usb_event_fct)(enum charging_sm_event,enum charging_sm_source);
static usb_event_fct usb_call_back_event;


/**@brief Function usb call back function.
 * @param[in] state : state of usb connexion
 * @param[in] *priv : private message
 * @return   none.
 */
static void usb_plug_event(bool state, void *priv)
{
	if (state)
		usb_call_back_event(CHARGING_PLUGGED_IN,SRC1);
	else
		usb_call_back_event(CHARGING_PLUGGED_OUT,SRC1);
}

/****************************************************************************************
 *********************** ACCESS FUNCTION IMPLEMENTATION *********************************
 ****************************************************************************************/

bool usb_power_supply_init(T_QUEUE parent_queue,void* call_back)
{
	/* USB definition */
	struct device *ps_dev = NULL;

	if(call_back == NULL)
	{
		pr_error(LOG_MODULE_CH,"Invalid handle");
		return false;
	}
	usb_call_back_event = call_back;

	/* Attach callback to USB event */
	ps_dev = &pf_device_usb_pm;

	/* Attach callback to USB_pm event */
	usb_pm_register_callback( ps_dev, usb_plug_event,NULL);
	/* Force usb state */
	if(usb_pm_is_plugged(ps_dev))
		usb_call_back_event(CHARGING_PLUGGED_IN,SRC1);
	else
		usb_call_back_event(CHARGING_PLUGGED_OUT,SRC1);

	return true;
}

void usb_power_supply_enable(void)
{
}

void usb_power_supply_disable(void)
{
}

void usb_power_supply_maintenance_enable(void)
{
}

void usb_power_supply_maintenance_disable(void)
{
}
