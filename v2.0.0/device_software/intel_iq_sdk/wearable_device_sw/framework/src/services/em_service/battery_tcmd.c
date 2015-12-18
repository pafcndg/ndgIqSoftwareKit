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

#include "infra/tcmd/handler.h"

#include "services/em_service/battery_service_api.h"
#include "battery_service_tcmd.h"

#define TCMD_LEVEL_HEADER 	"battery level"
#define TCMD_STATUS_HEADER 	"battery status"
#define TCMD_VBATT_HEADER 	"battery vbatt"
#define TCMD_TEMPERATURE_HEADER "battery temperature"
#define TCMD_BATT_CYCLE_HEADER	"battery cycle"
#define TCMD_CHARGER_STATUS 	"charger status"
#define TCMD_CHARGER_TYPE 	"charger type"
#define TCMD_BATT_ADC_PERIOD	"battery period"
#define TCMD_REQ_LENGTH    2

/**
 * @addtogroup infra_tcmd
 * @{
 */

/**
 * @defgroup infra_tcmd_bs Battery Service Test Commands
 * Interfaces to support Battery Service Test Commands.
 * @{
 */


/**
 * Test command to get battery level : battery level
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void battery_tcmd_level(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_LEVEL_HEADER);

		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_BATT_LEVEL,NULL);
}
DECLARE_TEST_COMMAND(battery, level, battery_tcmd_level);

/**
 * Test command to get battery status : battery status
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void battery_tcmd_status(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_STATUS_HEADER);
		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_BATT_STATUS,NULL);
}
DECLARE_TEST_COMMAND(battery, status, battery_tcmd_status);

/**
 * Test command to get battery voltage : battery vbatt
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void battery_tcmd_vbatt(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_VBATT_HEADER);
		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_BATT_VBATT,NULL);
}
DECLARE_TEST_COMMAND(battery, vbatt, battery_tcmd_vbatt);

/**
 * Test command to get battery temperature : battery temperature
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void battery_tcmd_temperature(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_TEMPERATURE_HEADER);
		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_BATT_TEMPERATURE,NULL);
}
DECLARE_TEST_COMMAND(battery, temperature, battery_tcmd_temperature);

#ifdef CONFIG_TCMD_BATTERY

/**
 * Test command to get battery charge cycle : battery cycle
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void battery_tcmd_cycle_get(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_BATT_CYCLE_HEADER);
		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_BATT_GET_CHARGE_CYCLE,NULL);
}
DECLARE_TEST_COMMAND_ENG(battery, cycle, battery_tcmd_cycle_get);
#endif

/**
 * Test command to set ADC measure interval : battery period
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 * @remark      usage:      battery period vbatt|temp <period_second>
 *                          If <period_second> is equal to 0, corresponding measure is suspended
 */
void battery_period_tcmd_status(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	 struct period_cfg_t period_cfg = {};
	/* Check we have the correct parameters number */
	if(argc != 4) {
		TCMD_RSP_ERROR(ctx, TCMD_BATT_ADC_PERIOD);
		return;
	}

	if (!isdigit(argv[3][0])){
		TCMD_RSP_ERROR(ctx, TCMD_BATT_ADC_PERIOD);
		return;
	}

	if (!strcmp(argv[2], "vbatt"))
		period_cfg.cfg_type = 0;
	else if (!strcmp(argv[2], "temp"))
		period_cfg.cfg_type = 1;
	else{
		TCMD_RSP_ERROR(ctx, TCMD_BATT_ADC_PERIOD);
		return;
	}
	period_cfg.new_period_ms =  (uint16_t) (atoi(argv[3])) * 1000;

	battery_service_cmd_handler(ctx,BS_CMD_SET_MEASURE_INTERVAL,(void*)&period_cfg);

}
DECLARE_TEST_COMMAND(battery, period, battery_period_tcmd_status);

/**
 * Test command to get charger status : charger status
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void charger_tcmd_status(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_CHARGER_STATUS);
		return;
	}

	battery_service_cmd_handler(ctx,BS_CMD_CHG_STATUS,NULL);

}
DECLARE_TEST_COMMAND(charger, status, charger_tcmd_status);

/**
 * Test command to get charger type : charger type
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void charger_tcmd_type(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
	/* Check we have the correct parameters number */
	if(argc != TCMD_REQ_LENGTH) {
		TCMD_RSP_ERROR(ctx, TCMD_CHARGER_TYPE);
		return;
	}
	battery_service_cmd_handler(ctx,BS_CMD_CHG_TYPE,NULL);

}
DECLARE_TEST_COMMAND(charger, type, charger_tcmd_type);

/**
 * @}
 *
 * @}
 */

