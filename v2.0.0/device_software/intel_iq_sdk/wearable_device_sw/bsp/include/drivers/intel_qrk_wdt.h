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

#ifndef INTEL_QRK_WDT_H_
#define INTEL_QRK_WDT_H_

#include "drivers/data_type.h"
#include "drivers/clk_system.h"

/**
 * @defgroup wdt Intel Quark WDT
 * Intel&reg; Quark&trade; WatchDog Timer driver API.
 *
 * The user can configure the watchdog into two ways:
 * - The Watchdog interrupt is routed as a non maskable interrupt (NMI) and a
 *   panic is triggered. This parameter is configurable by CONFIG_WDT_NMI.
 * - The Watchdog is set as normal interrupt and the user must provide a callback to
 *   implement for its own code.
 * The Watchdog routes as a NMI interrupt ensure that in a case of stall
 * the system goes into panic. Standard mode interrupt, the interrupt watchdog can 
 * not preempt another interrupt which can lead to a stall
 * @ingroup common_drivers
 * @{
 */

/*
 * Mode of operation.
 */
typedef enum{
    MODE_RESET = 0,
    MODE_INTERRUPT_RESET
}qrk_cxxxx_wdt_mode_t;

/**
 * Watchdog driver.
 */
extern struct driver watchdog_driver;

/*! watchdog Power management structure */
struct wdt_pm_data {
	uint32_t timeout_range_register;	/*!< Watchdog enable/disable and reset configuration */
	uint32_t control_register;			/*!< Watchdog timeout duration */
	struct clk_gate_info_s* clk_gate_info;  /*!< pointer to clock gate data */
};

/**
 *  WDT configuration.
 *  The user instantiates one of these with given parameters for the WDT,
 *  configured using the "qrk_cxxxx_pwm_set_config" function
 */
struct qrk_cxxxx_wdt_config
{
    uint32_t timeout;	/*!< Select watchdog expiration timeout. For 32MHZ clock timeout is from 2.048ms to 67.109s (power of 2).
						 * Value is rounded to the higher power of 2: 2^(16+i) * (1/clock speed)  i=timeout_multiplier(0-15)
						 */
    qrk_cxxxx_wdt_mode_t response_mode;	/*!< MODE0 = Generate a system reset MODE1 = First generate interrupt if not cleared generate reset  */
#ifndef CONFIG_WDT_NMI
    void (*interrupt_fn) (void);    	/*!< Pointer to function to call when WDT expires for the first time -Mode1(MODE_INTERRUPT_RESET) only */
#endif
};

/**
 *  Function to enable clock for the peripheral WDT
 */
void qrk_cxxxx_wdt_clk_enable(void);

/**
 *  Function to enable clock gating for the WDT
 *  @param  clk_gate_info   : pointer to the clock gate data
 */
void qrk_cxxxx_wdt_gating_enable(struct clk_gate_info_s* clk_gate_info);

/**
 *  Function to disable clock for the peripheral WDT
 */
void qrk_cxxxx_wdt_clk_disable(void);

/**
 *  Function to disable clock gating for WDT
 *  @param  clk_gate_info   : pointer to the clock gate data
 */
void qrk_cxxxx_wdt_gating_disable(struct clk_gate_info_s* clk_gate_info);

/**
 *  Function to configure the WDT.
 *
 *  @param config   : pointer to a WDT configuration structure
 *
 *  @return DRV_RC_OK on success\n
 *           DRV_RC_FAIL otherwise
 */
DRIVER_API_RC qrk_cxxxx_wdt_set_config(struct qrk_cxxxx_wdt_config *config);

/**
 *  Function to tickle WDT, preventing it from expiring
 */
void qrk_cxxxx_wdt_tickle(void);

/**
 *  Function to read WDT counter.
 *
 *  @return WDT counter
 */
uint32_t qrk_cxxxx_wdt_read_counter(void);

/**
 *  Function to read WDT timeout.
 *
 *  @return WDT timeout
 */
uint32_t qrk_cxxxx_wdt_timeout(void);

/** @} */

#endif /* INTEL_QRK_WDT_H_ */
