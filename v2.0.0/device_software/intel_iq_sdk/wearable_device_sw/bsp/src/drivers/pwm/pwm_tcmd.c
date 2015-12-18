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
#include <stdbool.h>
#include <string.h>
#include "infra/tcmd/handler.h"
#include "drivers/intel_qrk_pwm.h"
#include "infra/device.h"
#include "machine.h"

/*
 * @addtogroup infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_pwm PWM Test Commands
 * Interfaces to support PWM Test Commands.
 * @{
 */

/*
 * Test command to configure a channel of PWM: pwm conf <channel> <frequency> <unit> <duty cycle>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name),
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The opaque context to pass to responses
 */
void pwm_conf(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    char* msg = "Invalid: pwm conf <ch> <freq> <unit(mHz/Hz/kHz)> <duty cycle>";

    if (argc == 6) {
        /* Retrieve channel, frequency (in mHz) and duty cycle from parameters */
        uint8_t channel = (uint8_t)(atoi(argv[2]));
        uint64_t frequency = (uint64_t)(atoi(argv[3]));
        int duty_cycle = atoi(argv[5]);
        uint32_t unit = 0;

        struct soc_pwm_channel_config config;

        if(!strcmp(argv[4],"mHz")) unit = 1;
        else if(!strcmp(argv[4],"Hz")) unit = 1000;
        else if(!strcmp(argv[4],"kHz")) unit = 1000000;
        else goto err;
        /* Complete config structure for configuration */
        /* Only PWM_MODE is needed for test command */
        config.mode = PWM_MODE;
        /* Frequency has no unit and should be multiplied by unit to be in mHz */
        frequency = frequency * unit;
        /* Frequency is in mHz so 10^12 shall be divided by frequency to have pwm period in ns */
        config.pwm_period_ns = (1000000000000 + (frequency >> 1)) / frequency;
        /* config.pwm_duty_cycle_ns = (duty_cycle * config.pwm_period_ns) / 100 */
        /* so config.pwm_duty_cycle_ns = (duty_cycle * 10^10) / frequency */
        config.pwm_duty_cycle_ns = ((duty_cycle * 10000000000) + (frequency >> 1)) / frequency;
        /* No interruption needed for test command */
        config.pwm_enable_interrupts = false;
        config.interrupt_fn = NULL;
        config.timer_timeout_ns = 0;

        if (soc_pwm_set_config(&pf_device_pwm, channel, &config) != DRV_RC_OK) {
            msg = "Conf KO";
            goto err;
        }

        /* SUCCESS: Test command is passed */
        TCMD_RSP_FINAL(ctx, NULL);
        return;
    }

err:
    /* Error cases:
     * Problem during initialization
     * Problem during configuration
     * Invalid number of parameters: pwm conf <channel> <frequency> <unit> <duty cycle>
    */
    TCMD_RSP_ERROR(ctx, msg);
}
DECLARE_TEST_COMMAND_ENG (pwm, conf, pwm_conf);

/*
 * Test command to start a channel of PWM: pwm start <channel>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name),
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The opaque context to pass to responses
 */
void pwm_start(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    struct device *dev = &pf_device_pwm;
    if (argc == 3) {
        uint8_t channel = (uint8_t)(atoi(argv[2]));
        soc_pwm_start(dev, channel);
        /* SUCCESS: Test command is passed */
        TCMD_RSP_FINAL(ctx, NULL);
        return;
    }
    TCMD_RSP_ERROR(ctx, "Invalid: pwm start <ch>");
}
DECLARE_TEST_COMMAND_ENG (pwm, start, pwm_start);

/*
 * Test command to stop a channel of PWM: pwm stop <channel>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name),
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The opaque context to pass to responses
 */
void pwm_stop(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
    struct device *dev = &pf_device_pwm;
    if (argc == 3) {
        uint8_t channel = (uint8_t)(atoi(argv[2]));
        soc_pwm_stop(dev, channel);
        /* SUCCESS: Test command is passed */
        TCMD_RSP_FINAL(ctx, NULL);
        return;
    }
    TCMD_RSP_ERROR(ctx, "Invalid: pwm stop <ch>");
}
DECLARE_TEST_COMMAND_ENG (pwm, stop, pwm_stop);

/*
 * @}
 *
 * @}
 */
