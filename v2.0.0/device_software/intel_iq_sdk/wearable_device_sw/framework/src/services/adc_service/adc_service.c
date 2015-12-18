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
#include "cfw/cfw_internal.h"
#include "cfw/cfw_service.h"

#include "machine.h"
#include "drivers/ss_adc.h"
#include "drivers/data_type.h"
#include "infra/device.h"
#include "infra/log.h"

#include "services/adc_service/adc_service.h"

#define ADC_DEV        SS_ADC_ID
#define ADC_PANIC_CODE      -42
/****************************************************************************************
 ************************** SERVICE INITIALIZATION **************************************
 ****************************************************************************************/

static void handle_message(struct cfw_message * msg, void * param);
static void adc_client_connected(conn_handle_t * instance);
static void adc_client_disconnected(conn_handle_t * instance);
static struct device* dev;

static service_t adc_service = {
        .service_id = SS_ADC_SERVICE_ID,
        .client_connected = adc_client_connected,
        .client_disconnected = adc_client_disconnected,
};

uint32_t read_data = 0;
volatile uint8_t adc_in_service = 0;
void * adc_service_priv = NULL;

static void adc_rx(uint32_t dev_id, void *priv)
{
    if(adc_service_priv == NULL) {
        pr_error(LOG_MODULE_DRV, "adc_rx - context is NULL");
        panic(ADC_PANIC_CODE);
        return;
    }

    adc_get_val_rsp_msg_t * resp = (adc_get_val_rsp_msg_t * ) cfw_alloc_rsp_msg(adc_service_priv,
                MSG_ID_ADC_GET_VAL_RSP, sizeof(*resp));

    resp->rsp_header.status = DRV_RC_OK;
    resp->value = read_data;
    resp->reason = ADC_EVT_RX;
    cfw_send_message(resp);

    cfw_msg_free(adc_service_priv);
    adc_service_priv = NULL;
    adc_in_service = 0;
}

static void adc_err(uint32_t dev_id, void *priv)
{
    if(adc_service_priv == NULL) {
        pr_error(LOG_MODULE_DRV, "adc_err - context is NULL");
        panic(ADC_PANIC_CODE);
        return;
    }

    adc_get_val_rsp_msg_t * resp = (adc_get_val_rsp_msg_t * ) cfw_alloc_rsp_msg(adc_service_priv,
                    MSG_ID_ADC_GET_VAL_RSP, sizeof(*resp));

    resp->rsp_header.status = DRV_RC_OK;
    resp->reason = ADC_EVT_ERR;
    cfw_send_message(resp);

    cfw_msg_free(adc_service_priv);
    adc_service_priv = NULL;
    adc_in_service = 0;
}

static bool adc_enable(void)
{
    ss_adc_cfg_data_t config;
    config.in_mode         = SINGLED_ENDED;
    config.out_mode        = PARALLEL;
    config.serial_dly      = 1;
    config.capture_mode    = FALLING_EDGE;
    config.clock_ratio     = 1024;
    config.cb_rx           = adc_rx;
    config.cb_err          = adc_err;
    config.sample_width    = WIDTH_10_BIT;
    config.seq_mode        = SINGLESHOT;
    config.priv            = NULL;
    if (!ss_adc_set_config(dev, &config))
        return false;
    return true;
}

void adc_service_init(void * queue, int service_id)
{
    dev = &pf_device_ss_adc;
    adc_service.service_id = service_id;
    cfw_register_service(queue, &adc_service, handle_message, NULL);
}

/*******************************************************************************
 *********************** SERVICE IMPLEMENTATION ********************************
 ******************************************************************************/

static void adc_client_connected(conn_handle_t * instance) {
    pr_debug(LOG_MODULE_MAIN, "%s: \n", __func__);
}

static void adc_client_disconnected(conn_handle_t * instance) {
    pr_debug(LOG_MODULE_MAIN, "%s: \n", __func__);
}

static void handle_get_value(struct cfw_message *msg)
{
    struct device* dev = NULL;
    adc_get_val_req_msg_t * req = (adc_get_val_req_msg_t*) msg;
    adc_get_val_rsp_msg_t * resp;

    io_adc_seq_entry_t entrys = {14, 1};
    io_adc_seq_table_t seq_tbl;
    DRIVER_API_RC status = DRV_RC_FAIL;
    uint32_t data_len = 1;

    dev = &pf_device_ss_adc;

    if ((req->channel < ADC_MIN_CHANNEL) || (req->channel > ADC_MAX_CHANNEL)) {
        pr_debug(LOG_MODULE_MAIN, "Unknown channel number %d\n", req->channel);
        goto send;
    }

    uint32_t flags = interrupt_lock();
    if (adc_service_priv != NULL) {
        /* this variable must not be NULL here
         * there is already an adc request pending */
        interrupt_unlock(flags);
        goto send;
    }
    adc_service_priv = msg;
    interrupt_unlock(flags);

    if (!adc_enable())
        goto send;

    entrys.channel_id = req->channel;
    seq_tbl.entries = &entrys;
    seq_tbl.num_entries = data_len;

    status = ss_adc_read(dev, &seq_tbl, &read_data, data_len);
    if (DRV_RC_OK != status) {
        adc_service_priv = NULL;
        goto send;
    }

    return;

send:
    resp = (adc_get_val_rsp_msg_t * ) cfw_alloc_rsp_msg(msg,
            MSG_ID_ADC_GET_VAL_RSP, sizeof(*resp));
    resp->rsp_header.status = status;
    cfw_send_message(resp);
    cfw_msg_free(msg);
}

static void handle_message(struct cfw_message * msg, void * param)
{
    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_ADC_GET_VAL_REQ:
        handle_get_value(msg);
        break;
    default:
	cfw_print_default_handle_error_msg(LOG_MODULE_MAIN, CFW_MESSAGE_ID(msg));
        break;
    }
}
