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

#include "em_board_config.h"
#include "charging_sm.h"

#ifdef CONFIG_QUARK_CHARGER
#include "quark_se_charger_driver.h"
#endif
#ifdef CONFIG_QI_BQ51003
#include "qi_bq51003_driver.h"
#endif
#ifdef CONFIG_USB_POWER_SUPPLY
#include "usb_power_supply_driver.h"
#endif
#ifdef CONFIG_BQ25120
#include "bq25120_driver.h"
#endif

static struct power_supply_fct source0 =
{
	.init = NULL,
	.charge_enable = NULL,
	.charge_disable = NULL,
	.maintenance_enable = NULL,
	.maintenance_disable = NULL,
	.source_type = CHARGING_NONE
};

#ifdef CONFIG_USB_POWER_SUPPLY

static struct power_supply_fct source1 =
{
	.init = usb_power_supply_init,
#ifdef CONFIG_QI_BQ51003
	.charge_enable = qi_enable,
	.charge_disable = qi_disable,
#else
	.charge_enable = usb_power_supply_enable,
	.charge_disable = usb_power_supply_disable,
#endif
	.maintenance_enable = usb_power_supply_maintenance_enable,
	.maintenance_disable = usb_power_supply_maintenance_disable,
	.source_type = CHARGING_USB
};

#else

static struct power_supply_fct source1 =
{
	.init = NULL,
	.charge_enable = NULL,
	.charge_disable = NULL,
	.maintenance_enable = NULL,
	.maintenance_disable = NULL,
	.source_type = CHARGING_NONE
};

#endif

#ifdef CONFIG_QI_BQ51003

static struct power_supply_fct source2 =
{
	.init = qi_init,
	.charge_enable = qi_enable,
	.charge_disable = qi_disable,
	.maintenance_enable = qi_maintenance_enable,
	.maintenance_disable = qi_maintenance_disable,
	.source_type = CHARGING_WIRELESS
};

#else

static struct power_supply_fct source2 =
{
	.init = NULL,
	.charge_enable = NULL,
	.charge_disable = NULL,
	.maintenance_enable = NULL,
	.maintenance_disable = NULL,
	.source_type = CHARGING_NONE
};

#endif

#ifdef CONFIG_QUARK_CHARGER

static struct charger_fct charger =
{
	.init = ch_init
};

#else

#ifdef CONFIG_BQ25120

static struct charger_fct charger =
{
	.init = ch_init
};

#else

static struct charger_fct charger =
{
	.init = NULL
};

#endif
#endif

/****************************************************************************************
 *********************** ACCESS FUNCTION IMPLEMENTATION *********************************
 ****************************************************************************************/

void get_source(struct power_supply_fct **src0,
		struct power_supply_fct **src1,
		struct power_supply_fct **src2,
		struct charger_fct **chrg)
{
	*src0 = &source0;
	*src1 = &source1;
	*src2 = &source2;
	*chrg = &charger;
}

