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

#include <microkernel.h>
#include "cfw/cfw.h"
#include "cfw/cfw_messages.h"
#include "cfw/cfw_debug.h"

#include "service_queue.h"
#include "cfw/cproxy.h"
#include "services/adc_service/adc_service.h"
#include "util/cunit_test.h"
#include "service_tests.h"

#define TEST_CHANNEL    4

#define ADC_CU_ASSERT(msg, cdt) \
    do { CU_ASSERT(msg, cdt); \
    if(!(cdt)) { return; }} while(0)

static DRIVER_API_RC adc_read_resp = DRV_RC_INVALID_OPERATION;

static void adc_handle_msg(struct cfw_message * msg, void * data)
{
    switch (CFW_MESSAGE_ID(msg)) {
    case MSG_ID_ADC_GET_VAL_RSP:
        adc_read_resp = ((adc_get_val_rsp_msg_t*)msg)->rsp_header.status;
        if (adc_read_resp == DRV_RC_OK){
            if (((adc_get_val_rsp_msg_t*)msg)->reason  == ADC_EVT_RX) {
                CU_ASSERT("SS_ADC value is negative", ((adc_get_val_rsp_msg_t*)msg)->value >= 0);
            }
        }
        break;
    default:
        cu_print("default cfw handler\n");
        break;
    }
    cfw_msg_free(msg);
}

void adc_service_test()
{
    uint64_t timeout = 0;
    cfw_service_conn_t * adc_service_handle = NULL;

    cu_print("##################################################\n");
    cu_print("# Purpose of ADC service test :                  #\n");
    cu_print("#            Retrieve value sampled on channel X #\n");
    cu_print("##################################################\n");

    SRV_WAIT(!cfw_service_registered(SS_ADC_SERVICE_ID), 0xFFFFFF);
    ADC_CU_ASSERT("Timeout expired, cfw service is not registered",
            cfw_service_registered(SS_ADC_SERVICE_ID));

    cu_print("ADC Service registered, open it\n");
    adc_service_handle = cproxy_connect(SS_ADC_SERVICE_ID, adc_handle_msg, NULL);
    ADC_CU_ASSERT("Timeout expired, unable to open ADC service", adc_service_handle);

    adc_get_value(adc_service_handle, TEST_CHANNEL, NULL);
    while ((++timeout) <= 0xFFFFFF) {
        queue_process_message(get_test_queue());
        if ((adc_read_resp == DRV_RC_CONTROLLER_IN_USE) ||
            (adc_read_resp == DRV_RC_FAIL) ||
            (adc_read_resp == DRV_RC_OK))
            break;
    }
    ADC_CU_ASSERT("Timeout expired, not adc read response", timeout <= 0xFFFFFF);

}
