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

#ifndef __ADC_SERVICE_H__
#define __ADC_SERVICE_H__

#include <stdint.h>

#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "services/services_ids.h"

#include "drivers/data_type.h"

/**
 * @defgroup services Services
 * Component framework services definition.
 */


/**
 * @defgroup adc_service ADC Service
 * Analog to Digital Converter (ADC).
 * @ingroup services
 */

 /**
 * @defgroup adc_service_api ADC Service API
 * Define the interface for ADC service.
 * @ingroup adc_service
 * @{
 */

/** service internal message ID */
#define MSG_ID_ADC_GET_VAL_REQ      (MSG_ID_ADC_SERVICE_BASE + 5)
/** message ID for service response */
#define MSG_ID_ADC_GET_VAL_RSP      (MSG_ID_ADC_GET_VAL_REQ | 0x40)

/** ID for ADC RX interrupt */
#define ADC_EVT_RX                  0
/** ID for ADC ERR interrupt */
#define ADC_EVT_ERR                 1

/** min channel ID */
#define ADC_MIN_CHANNEL             0
 /** max channel ID */
#define ADC_MAX_CHANNEL             18

/**
 * @ref adc_get_value request message structure
 */
typedef struct adc_get_val_req_msg {
    struct cfw_message header; /*!< header message */
    uint32_t channel;           /*!< index of ADC channel */
} adc_get_val_req_msg_t;

/**
 * @ref adc_get_value response message structure
 */
typedef struct adc_get_val_rsp_msg {
    struct cfw_rsp_message rsp_header; /*!< response header message */
    uint32_t value;                     /*!< value read by ADC */
    uint8_t reason;                     /*!< reason of event (ADC_EVT_RX or ADC_EVT_ERR). */
} adc_get_val_rsp_msg_t;

/**
 * Get ADC sample value for a channel.
 *
 * @msc
 * Client,"ADC Service","ADC Driver";
 *
 * Client->"ADC Service" [label="ADC get value request", URL="\ref adc_get_val_req_msg"];
 * "ADC Service"=>"ADC Driver" [label="ADC read"];
 * "ADC Service"<<="ADC Driver" [label="ADC Value/Error"];
 * Client<-"ADC Service" [label="ADC get value response", URL="\ref adc_get_val_rsp_msg"];
 *
 * @endmsc
 *
 * @param service_conn the service connection handle.
 * @param channel specific channel [0..18].
 * @param priv the private data passed back in the response message.
 *
 * @return
 *     0 if request successfully sent, panic if issue is encountered during allocation of message
 *
 * ADC value will be available in the @ref adc_get_val_rsp_msg structure with MSG_ID_ADC_GET_VAL_RSP as message ID
 */
uint32_t adc_get_value(cfw_service_conn_t * service_conn, uint32_t channel, void * priv);

/**
 * Intialize/register the ADC service.
 *
 * @param queue the queue this service will use for processing its messages
 * @param service_id the id this service is assigned
 */
void adc_service_init(T_QUEUE queue, int service_id);

/** @} */

#endif /* __ADC_SERVICE_H__ */
