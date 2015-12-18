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

/* workaround: this shall be fixed together with Makefile begin */
#ifdef CONFIG_QUARK
/* workaround: this shall be fixed together with Makefile end */

#include <ctype.h>

#include "infra/tcmd/handler.h"

#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"

#include "services/adc_service/adc_service.h"

#include "service_queue.h"
#include "cfw/cproxy.h"

#define BUFFER_LENGTH   80
#define CHANNEL_ARG     2

#define _STRINGIFY(s)   #s
#define STRINGIFY(s)    _STRINGIFY(s)

typedef struct {
	cfw_service_conn_t *adc_service_conn;
	uint32_t adc_channel;
	struct tcmd_handler_ctx *context;
} adc_data_t;

char message[BUFFER_LENGTH];

/**@brief adc handle message: Callback function for adc test command
 *
 * @param[in]   msg        Message
 * @param[in]   data       Data
 */
static void adc_svc_handle_msg(struct cfw_message *msg, void *data)
{
	adc_data_t *priv = CFW_MESSAGE_PRIV(msg);
	struct tcmd_handler_ctx *ctxt = (priv->context);

	if (CFW_MESSAGE_ID(msg) == MSG_ID_ADC_GET_VAL_RSP) {
		if (((adc_get_val_rsp_msg_t *) msg)->rsp_header.status ==
		    DRV_RC_OK) {
			if (((adc_get_val_rsp_msg_t *) msg)->reason ==
			    ADC_EVT_RX) {
				snprintf(message, BUFFER_LENGTH, "%d %d",
					 (uint32_t) (priv->adc_channel),
					 ((adc_get_val_rsp_msg_t *)
					  msg)->value);
				TCMD_RSP_FINAL(ctxt, message);
			} else {	/* ADC_EVT_ERR */
				TCMD_RSP_ERROR(ctxt, "ADC_EVT_ERR");
			}
		} else {
			snprintf(message, BUFFER_LENGTH,
				 "Wrong status returned: %d",
				 ((adc_get_val_rsp_msg_t *) msg)->
				 rsp_header.status);
			TCMD_RSP_ERROR(ctxt, message);
		}
	} else {
		/* default cfw handler */
		snprintf(message, BUFFER_LENGTH,
			 "NOT MSG_ID_ADC_GET_VAL_RSP, msg: %x",
			 CFW_MESSAGE_ID(msg));
		TCMD_RSP_ERROR(ctxt, message);
	}
	cproxy_disconnect(priv->adc_service_conn);
	cfw_msg_free(msg);
	bfree(priv);
}

/**@brief Test command to get value of ADC corresponding at input channel: adc get <channel>
 *
 * @param[in]   argc        Number of arguments in the Test Command (including group and name),
 * @param[in]   argv        Table of null-terminated buffers containing the arguments
 * @param[in]   ctx         The Test command response context
 */
void adc_get(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	if (argc != 3) {
		snprintf(message, BUFFER_LENGTH,
			 "Wrong number of parameter, usage: adc get <channel>");
		goto err;
	}
	if (!isdigit(argv[CHANNEL_ARG][0])) {
		snprintf(message, BUFFER_LENGTH,
			 "Wrong parameter, please enter digit for <channel>");
		goto err;
	}

	/* Retrieve channel from user input */
	uint32_t channel = (uint32_t) (atoi(argv[CHANNEL_ARG]));

	if ((channel < ADC_MIN_CHANNEL) || (channel > ADC_MAX_CHANNEL)) {
		TCMD_RSP_ERROR(ctx,
				     "Invalid channel number, must be between "
				     STRINGIFY(ADC_MIN_CHANNEL) " and "
				     STRINGIFY(ADC_MAX_CHANNEL));
		return;
	} else {
		/* Initialization of variables */
		adc_data_t *adc_values = balloc(sizeof(adc_data_t), NULL);
		adc_values->adc_channel = channel;
		adc_values->context = ctx;

		adc_values->adc_service_conn =
		    cproxy_connect(SS_ADC_SERVICE_ID, adc_svc_handle_msg, NULL);

		if (adc_values->adc_service_conn == NULL) {
			snprintf(message, BUFFER_LENGTH,
				 "Connection to ADC service has failed");
			goto err;
		}

		/* ADC driver initialization done */
		adc_get_value(adc_values->adc_service_conn,
			      adc_values->adc_channel, adc_values);
		return;
	}

err:
	TCMD_RSP_ERROR(ctx, message);
}

DECLARE_TEST_COMMAND(adc, get, adc_get);

/**@} @}*/
#endif
