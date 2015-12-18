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
#include <stdio.h>
#include "drivers/soc_comparator.h"
#include "infra/tcmd/handler.h"
#include <os/os.h>
#include "scss_registers.h"
#include "string.h"
#include "machine.h"

#define LENGTH         80
#define POL_MAX_LEN    10

/*
 * @addtogroup infra_tcmd
 * @{
 */

/*
 * @defgroup infra_tcmd_adc ADC COMPARATOR Test Commands
 * Interfaces to support ADC COMPARATOR Test Commands.
 * @{
 */

struct comparator_data_s {
	char polarity[POL_MAX_LEN];
	uint8_t channel;
	struct tcmd_handler_ctx *ctx;
};

void comparator_callback_tcmd(void *priv)
{
	char *answer = balloc(LENGTH, NULL);
	struct comparator_data_s *tmp = (struct comparator_data_s *)priv;
	sprintf(answer, "%d %s\n", tmp->channel, tmp->polarity);

	TCMD_RSP_FINAL(tmp->ctx, answer);
	bfree(tmp);
	bfree(answer);
}

/*
 * Test command to configure a comparator: adc comp <channel> <vref> <polarity>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name),
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The opaque context to pass to responses
 */

void adc_comp(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	char *answer;
	DRIVER_API_RC ret;
	struct comparator_data_s *priv;
	uint8_t channel;
	struct device* comparator_dev = &pf_device_soc_comparator;

	/* Retrieve channel, vref and polarity from parameters */
	priv = balloc(sizeof(struct comparator_data_s), NULL);
	if (argc != 5)
		goto err;
	channel = (uint8_t) (atoi(argv[2]));
	if ((strcmp(argv[4], "above") && strcmp(argv[4], "under"))
	    || (strcmp(argv[3], "ref_a") && strcmp(argv[3], "ref_b")))
		goto err;
	memset(priv->polarity, 0, sizeof(priv->polarity));
	strncpy(priv->polarity, argv[4], POL_MAX_LEN-1);
	priv->channel = channel;
	priv->ctx = ctx;
	ret = comp_configure(comparator_dev, channel, argv[4][0] == 'u',
			     argv[3][4] == 'b',
			     comparator_callback_tcmd, priv);

	if (ret != DRV_RC_OK) {
		goto err;
	}
	return;
err:
	answer = balloc(LENGTH, NULL);
	bfree(priv);
	sprintf(answer,
		"invalid param: adc comp [0-%d] [ref_a/ref_b] [above/under]",
		CMP_COUNT);
	TCMD_RSP_ERROR(ctx, answer);
	bfree(answer);
}

DECLARE_TEST_COMMAND_ENG(adc, comp, adc_comp);

/*
 * @}
 *
 * @}
 */
