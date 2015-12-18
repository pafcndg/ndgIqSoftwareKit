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

#ifndef INTEL_QRK_PWM_H_
#define INTEL_QRK_PWM_H_

#include "drivers/data_type.h"
#include "infra/device.h"
#include "drivers/clk_system.h"

/**
 * @defgroup pwm Intel Quark PWM
 * Intel Quark Pulse Width Modulation driver API.
 * @ingroup common_drivers
 * @{
 */

/**
 * Number of available PWM channels available on the
 * FST Quark platform
 */
#define QRK_PWM_NPWM    4

/** Max duty cycle at slowest PWM pulse frequency 100% at 1/60th of a hertz */
#define DUTY_CYCLE_MAX_NS               60000000000
/**  Slowest supported PWM pulse freqency - 1/60th of a hertz */
#define PERIOD_MAX_NS                   60000000000
/**  Max timer timeout - 60 seconds */
#define TIMER_TIMEOUT_MAX_NS            60000000000

/**
 * Mode of operation
 */
typedef enum{
    PWM_MODE = 0,
    TIMER_MODE
}soc_pwm_mode_t;

/**
 *  PWM driver.
 */
extern struct driver pwm_driver;

/**
 * PWM channel configuration.
 *
 * The user instantiates one of these with given parameters for each PWM
 * channel, configured using the "soc_pwm_set_config" function
 */
struct soc_pwm_channel_config{
    soc_pwm_mode_t mode;            /*!< Operation Mode- PWM or timer mode */
    uint64_t pwm_period_ns;         /*!< PWM period in nanoseconds - PWM mode only  */
    uint64_t pwm_duty_cycle_ns;     /*!< PWM duty cycle in nanoseconds (time high) - PWM mode only */
    bool pwm_enable_interrupts;     /*!< Enable all edge interrupts - PWM mode only */
    uint64_t timer_timeout_ns;      /*!< Timer timeout in nanoseconds - timer mode only*/
    bool timer_enable_oneshot;      /*!< Enable one shot timer mode - timer mode only */
    void (*interrupt_fn) (void);    /*!< Pointer to function to call when a channel specific PWM interrupt fires or a timer expires */
};

/**
 * Timer data saved at suspend.
 */
struct timer_info_s {
    uint32_t timer_load_count;    /*!< Timer load count register */
    uint32_t timer_current_value; /*!< Timer current value register */
    uint32_t timer_control_reg;   /*!< Timer Control register */
    uint32_t timer_load_count_2;  /*!< Timer Load count 2 register */
};

/**
 * PWM Power management structure.
 */
struct pwm_pm_info {
    uint32_t timers_comp_version;           /*!< Timer component version */
    uint8_t running_pwm;                    /*!< Indicate if a pwm running (0 otherwise) */
    struct timer_info_s* timer_info;       /*!< Pointer to timer data */
    struct clk_gate_info_s* clk_gate_info; /*!< Pointer to clock gate data */
};

/**
 * Configure a specified PWM channel.
 *
 * @param  dev       Pointer to device
 * @param  channel   Channel number
 * @param  config    Pointer to a channel configuration structure
 *
 * @return
 *          - RC_OK on success
 *          - RC_FAIL otherwise
 */
DRIVER_API_RC soc_pwm_set_config(struct device *dev, uint8_t channel, struct soc_pwm_channel_config *config);

/**
 * Start a pwm/timer channel.
 *
 * @param  dev        Pointer to device
 * @param  channel    Channel number
 */
void soc_pwm_start(struct device *dev, uint8_t channel);

/**
 * Stop a pwm/timer channel.
 *
 * @param  dev        Pointer to device
 * @param  channel    Channel number
 */
void soc_pwm_stop(struct device *dev, uint8_t channel);

/**
 * PWM ISR, if specified calls a user defined callback.
 *
 * @param  dev       Pointer to device
 */
void pwm_isr(struct device *dev);

/** @} */

#endif /* INTEL_QRK_PWM_H_ */
