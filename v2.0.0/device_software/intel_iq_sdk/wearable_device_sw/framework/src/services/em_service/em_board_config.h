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

#ifndef EM_BOARD_CONFIG_H_
#define EM_BOARD_CONFIG_H_

#include <stdbool.h>
#include "os/os_types.h"
#include "em_service/power_supply_type.h"

/**
 *  Structure to handle power supply function(s)
 */
struct power_supply_fct
{
	bool (*init)(T_QUEUE,void*);
	void (*charge_enable)(void);
	void (*charge_disable)(void);
	void (*maintenance_enable)(void);
	void (*maintenance_disable)(void);
	batt_svc_chg_src_t source_type;
};

/**
 *  Structure to handle charger function(s)
 */
struct charger_fct
{
	bool (*init)(T_QUEUE,void*);
};

/**@brief Function to get structure pointer of power supply and charger.
 * @param[in]  source0 structure pointer
 * @param[in]  source1 structure pointer
 * @param[in]  source2 structure pointer
 * @param[in]  charger structure pointer
 */
void get_source(struct power_supply_fct **src0,
		struct power_supply_fct **src1,
		struct power_supply_fct **src2,
		struct charger_fct **chrg);

#endif
