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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "os/os.h"
#include "infra/tcmd/handler.h"

#include "machine.h"
#include "drivers/ss_adc.h"

#define MIN_CH          0
#define MAX_CH          18
#define CH_PARAM        2
#define TEST_DLY        50
#define TEST_RESOLUTION 12
#define TEST_CLK_RATIO  1024
#define LENGTH          80
#define TEST_ADC_DEV    SS_ADC_ID /* TODO: add dev as parameter */

struct _adc_tcmd_ctx {
	uint32_t			channel;
	struct tcmd_handler_ctx *	ctx;
	uint32_t			data;
};

static void rx_cbk(uint32_t rx_data, void *priv);
static void err_cbk(uint32_t err_data, void *priv);

/*
 * @addtop infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_adc ADC Test commands
 * Interfaces to support ADC Test Command
 * @{
 */

/*
 * Callback functions
 */
static void rx_cbk(uint32_t dev_id, void *priv)
{
	char answer[LENGTH];
	struct _adc_tcmd_ctx *adc_ctx = (struct _adc_tcmd_ctx *)priv;

	snprintf(answer, LENGTH, "%lu %lu", adc_ctx->channel, adc_ctx->data);
	TCMD_RSP_FINAL((adc_ctx->ctx), answer);
	bfree(adc_ctx);
}

static void err_cbk(uint32_t dev_id, void *priv)
{
	char answer[LENGTH];
	struct _adc_tcmd_ctx *adc_ctx = (struct _adc_tcmd_ctx *)priv;

	snprintf(answer, LENGTH, "Error");
	TCMD_RSP_ERROR(adc_ctx->ctx, answer);
	bfree(adc_ctx);
}

/*
 * Test command to get the channel value : adc get <channel>
 *
 * @param[in]	argc	Number of arguments in the test command
 * @param[in]	argv	Table of null-terminated buffers containing the arguments
 * @param[in]	ctx	The Test command response context
 *
 */
void adc_get(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	/* initialization */
	struct device *dev = &pf_device_ss_adc;
	struct _adc_tcmd_ctx *adc_ctx =
		balloc(sizeof(struct _adc_tcmd_ctx), NULL);

	adc_ctx->channel = 0;
	adc_ctx->ctx = ctx;
	adc_ctx->data = 0;
	if (argc == 3) {
		adc_ctx->channel = (uint32_t)(atoi(argv[CH_PARAM]));
	} else {
		TCMD_RSP_ERROR(ctx, "Invalid cmd: adc get <ch>");
		goto exit;
	}
	io_adc_seq_entry_t entrys = {
		.channel_id	= adc_ctx->channel,
		.sample_dly	= TEST_DLY,
	};
	ss_adc_cfg_data_t config = { 0 };
	io_adc_seq_table_t seq_tbl;
	DRIVER_API_RC status = DRV_RC_FAIL;
	uint32_t data_len = 1;
	uint32_t resolution = TEST_RESOLUTION;

	/* Check the parameter */
	if (!(isdigit((unsigned char)argv[CH_PARAM][0]))) {
		TCMD_RSP_ERROR(ctx, "Invalid cmd: adc get <ch>");
		goto exit;
	} else {
		if (adc_ctx->channel >= MIN_CH && adc_ctx->channel <= MAX_CH) {
			/* configure */
			config.in_mode = SINGLED_ENDED;
			config.out_mode = PARALLEL;
			config.serial_dly = 1;
			config.capture_mode = FALLING_EDGE;
			config.clock_ratio = TEST_CLK_RATIO;
			config.cb_rx = (adc_callback)rx_cbk;
			config.cb_err = (adc_callback)err_cbk;
			config.priv = adc_ctx;
			config.sample_width =
				ss_adc_res_to_sample_width(resolution);

			if (!ss_adc_set_config(dev, &config))
				goto exit;


			/* get the value */

			seq_tbl.entries = &entrys;
			seq_tbl.num_entries = data_len;

			status =
				ss_adc_read(dev, &seq_tbl, &(adc_ctx->data),
					    data_len);
			if (status != DRV_RC_OK) {
				TCMD_RSP_ERROR(ctx, "Read status: KO");
				goto exit;
			}
		} else {
			TCMD_RSP_ERROR(ctx, "Invalid num ch: 0_ch_18");
			goto exit;
		}
	}
	return;
exit:
	bfree(adc_ctx);
	return;
}

DECLARE_TEST_COMMAND(adc, get, adc_get);

/*
 * @}
 *
 * @}
 */
