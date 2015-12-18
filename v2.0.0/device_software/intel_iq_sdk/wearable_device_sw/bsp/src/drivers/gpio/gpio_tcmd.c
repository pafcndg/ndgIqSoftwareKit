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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "infra/tcmd/handler.h"

#ifdef CONFIG_SS_GPIO
#include "drivers/ss_gpio_iface.h"
#endif
#ifdef CONFIG_SOC_GPIO
#include "drivers/soc_gpio.h"
#endif

#include "machine.h"

#define ANS_LENGTH    80

enum {
	TCMD_SS = 0,
	TCMD_SOC,
	TCMD_PORT_COUNT
};

struct args_index {
	uint8_t port;
	uint8_t index;
	uint8_t mode;
	uint8_t value;
};

struct _listen_args_index {
	uint8_t port;
	uint8_t op;
	uint8_t index;
	uint8_t mode;
	uint8_t deb;
};

struct args_number {
	uint8_t conf;
	uint8_t set;
	uint8_t get;
	uint8_t listen;
};

static const struct args_index args_indx = {
	.port = 2,
	.index = 3,
	.mode = 4,
	.value = 4,
};

static const struct args_number args_no = {
	.conf = 5,
	.set = 5,
	.get = 4,
	.listen = 7
};

static volatile uint8_t _counter = 0;

/*
 * @addtogroup infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_gpio GPIO Test Commands
 * Interfaces to support GPIO Test Commands.
 * @{
 */

static void check_result(uint32_t rc, struct tcmd_handler_ctx *ctx)
{
	switch (rc) {
	case DRV_RC_OK:
		TCMD_RSP_FINAL(ctx, NULL);
		break;
	default:
		TCMD_RSP_ERROR(ctx, NULL);
		break;
	}
}

/*
 * Returns used port(quark: soc, aon; arc: 8b0, 8b1)
 *
 * @param[in]   argc           Number of arguments in the Test Command (including group and name)
 * @param[in]   argv           Table of null-terminated buffers containing the arguments
 * @param[out]  ss_soc_select  Quark of Arc gpio select
 * @return      gpio_dev       gpio device to use
 */
static struct device* get_port(int argc, char *argv[], int32_t *ss_soc_select)
{
	struct device *gpio_dev = NULL;

	if (argc < 3)
		return gpio_dev;

#ifdef CONFIG_SS_GPIO
	if (!strcmp(argv[args_indx.port], "8b0")) {
		gpio_dev = &pf_device_ss_gpio_8b0;
		*ss_soc_select = TCMD_SS;
	} else if (!strcmp(argv[args_indx.port], "8b1")) {
		gpio_dev = &pf_device_ss_gpio_8b1;
		*ss_soc_select = TCMD_SS;
	}
#endif
#ifdef CONFIG_SOC_GPIO
	if (!strcmp(argv[args_indx.port], "soc")) {
#ifdef CONFIG_SOC_GPIO_32
		gpio_dev = &pf_device_soc_gpio_32;
		*ss_soc_select = TCMD_SOC;
#endif
	} else if (!strcmp(argv[args_indx.port], "aon")) {
#ifdef CONFIG_SOC_GPIO_AON
		gpio_dev = &pf_device_soc_gpio_aon;
		*ss_soc_select = TCMD_SOC;
#endif
	}
#endif
	return gpio_dev;
}

/*
 * Test command to configure SOC/AON port GPIO: [arc.]gpio conf <soc|aon|8b0|8b1> <index> <mode>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void gpio_conf(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint8_t index = 0;
	struct device *gpio_dev = NULL;
	int32_t rc = -1;
	int32_t ss_soc_select = -1;

	gpio_cfg_data_t pin_cfg = {
		.gpio_type = GPIO_INPUT,
		.int_type = LEVEL,
		.int_polarity = ACTIVE_LOW,
		.int_debounce = DEBOUNCE_OFF,
		.int_ls_sync = LS_SYNC_OFF,
		.gpio_cb = NULL
	};

	if ((gpio_dev = get_port(argc, argv, &ss_soc_select)) == NULL) {
		TCMD_RSP_ERROR(ctx, "Port not available or command syntax not ok.");
		return;
	}

	if (argc == args_no.conf &&
	    isdigit((unsigned char)argv[args_indx.index][0]) &&
	    isalpha((unsigned char)argv[args_indx.mode][0])) {

		index = (uint8_t) atoi(argv[args_indx.index]);
		if (!strcmp(argv[args_indx.mode], "in")) {
			pin_cfg.gpio_type = GPIO_INPUT;
		} else if (!strcmp(argv[args_indx.mode], "out")) {
			pin_cfg.gpio_type = GPIO_OUTPUT;
		} else
			goto print_help;

		switch (ss_soc_select) {
#ifdef CONFIG_SS_GPIO
		case TCMD_SS:
			rc = ss_gpio_set_config(gpio_dev, index, &pin_cfg);
			break;
#endif
#ifdef CONFIG_SOC_GPIO
		case TCMD_SOC:
			rc = soc_gpio_set_config(gpio_dev, index, &pin_cfg);
			break;
#endif
		default:
			TCMD_RSP_ERROR(ctx, "Port not available on this processor.");
			return;
		}
	} else
		goto print_help;

	check_result(rc, ctx);
	return;

print_help:
	TCMD_RSP_ERROR(ctx,
		       "Usage: [arc.]gpio conf <soc|aon|8b0|8b1> <index> <mode>");
}

DECLARE_TEST_COMMAND(gpio, conf, gpio_conf);

/*
 * Test command to get SOC/AON port GPIO: [arc.]gpio get <soc|aon|8b0|8b1> <index>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void gpio_get(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint8_t index = 0;
	bool value = 0;
	struct device *gpio_dev = NULL;
	int32_t rc = -1;
	int32_t ss_soc_select = -1;
	char answer[ANS_LENGTH];

	if ((gpio_dev = get_port(argc, argv, &ss_soc_select)) == NULL) {
		TCMD_RSP_ERROR(ctx, "Port not available or command syntax not ok.");
		return;
	}

	if (argc == args_no.get && isdigit((unsigned char)argv[args_indx.index][0])) {
		index = (uint8_t) atoi(argv[args_indx.index]);
		switch (ss_soc_select) {
#ifdef CONFIG_SS_GPIO
		case TCMD_SS:
			rc = ss_gpio_read(gpio_dev, index, &value);
			break;
#endif
#ifdef CONFIG_SOC_GPIO
		case TCMD_SOC:
			rc = soc_gpio_read(gpio_dev, index, &value);
			break;
#endif
		default:
			TCMD_RSP_ERROR(ctx, "Port not available on this processor.");
			return;
		}
	} else
		goto print_help;

	if (rc == DRV_RC_OK) {
		snprintf(answer, ANS_LENGTH, "%d %d", index,
			 value);
		TCMD_RSP_FINAL(ctx, answer);
	} else {
		TCMD_RSP_ERROR(ctx, NULL);
	}
	return;

print_help:
	TCMD_RSP_ERROR(ctx, "Usage: [arc.]gpio get <soc|aon|8b0|8b1> <index>");
}

DECLARE_TEST_COMMAND(gpio, get, gpio_get);

/*
 * Test command to set SOC/AON port GPIO: [arc.]gpio set <soc|aon|8b0|8b1> <index> <value>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name)
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The context to pass back to responses
 */
void gpio_set(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint8_t index = 0;
	uint8_t value = 0;
	struct device *gpio_dev = NULL;
	int32_t rc = -1;
	int32_t ss_soc_select = -1;

	if ((gpio_dev = get_port(argc, argv, &ss_soc_select)) == NULL) {
		TCMD_RSP_ERROR(ctx, "Port not available or command syntax not ok.");
		return;
	}

	if (argc == args_no.set &&
	    isdigit((unsigned char)argv[args_indx.index][0]) &&
	    isdigit((unsigned char)argv[args_indx.value][0])) {

		index = (uint8_t) atoi(argv[args_indx.index]);
		value = (uint8_t) atoi(argv[args_indx.value]);
		switch (ss_soc_select) {
#ifdef CONFIG_SS_GPIO
		case TCMD_SS:
			rc = ss_gpio_write(gpio_dev, index, value);
			break;
#endif
#ifdef CONFIG_SOC_GPIO
		case TCMD_SOC:
			rc = soc_gpio_write(gpio_dev, index, value);
			break;
#endif
		default:
			TCMD_RSP_ERROR(ctx, "Port not available on this processor.");
			return;
		}
	} else
		goto print_help;
	check_result(rc, ctx);
	return;

print_help:
	TCMD_RSP_ERROR(ctx,
		       "Usage: [arc.]gpio set <soc|aon|8b0|8b1> <index> <value>");
}

DECLARE_TEST_COMMAND(gpio, set, gpio_set);

/*
 * @}
 *
 * @}
 */
