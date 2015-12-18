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

#ifndef CHARGING_SM_H_
#define CHARGING_SM_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "em_service/power_supply_type.h"
#include "os/os_types.h"

/**
 * @ref handler_ps_event and handler_charger_event request message enumerate
 */
enum charging_sm_event
{
	CHARGING_START,
	CHARGING_STOP,
	CHARGING_COMPLETE,
	CHARGING_OVERTEMP,
	CHARGING_PLUGGED_IN,
	CHARGING_PLUGGED_OUT
};

/**
 * @ref handler_ps_event request message enumerate
 */
enum charging_sm_source
{
	NONE,
	SRC1,
	SRC2,
	BOTH
};

/* Automate state definition */
enum e_state
{
	INIT,
	DISCHARGE,
	CHARGE,
	COMPLETE,
	OVERTEMP
};

/**@brief Function to initialize charging state machine.
 * @param[in]  cb callback called when init done
 * @return   true if function success, false if function failed.
 */
bool charging_sm_init(T_QUEUE parent_queue,void* call_back);

/**@brief Accessor function which return the type of charger.
 * @param[in]  none
 * @return   bool true if battery is in charge.
 */
bool charging_sm_is_charging(void);

/**@brief Accessor function which return charger connection information.
 * @param[in]  none
 * @return   bool true if power supply is connected.
 */
bool charging_sm_is_charger_connected(void);

/**@brief Accessor function which return the type of source.
 * @param[in]  none
 * @return   batt_svc_chg_src_t type of source.
 */
batt_svc_chg_src_t charging_sm_get_source(void);

/**@brief Accessor function which return the state of charger.
 * @param[in]  none
 * @return   bool true if battery is in charge.
 */
enum e_state charging_sm_get_state(void);

#endif /* CHARGING_SM_H_ */
