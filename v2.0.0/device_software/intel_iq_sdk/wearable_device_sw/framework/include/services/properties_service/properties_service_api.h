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

#ifndef __PROPERTIES_SERVICE_API_H__
#define __PROPERTIES_SERVICE_API_H__

#include "cfw/cfw.h"
#include "drivers/data_type.h"
#include "infra/properties_storage.h"
#include "services/services_ids.h"

/**
 * @defgroup properties_service Properties Service
 * Properties Service (storage of named variables)
 * @ingroup services
 */

 /**
 * @defgroup properties_service_api Properties Service API
 * Define the interface of Properties Service
 * @ingroup properties_service
 * @{
 */

#define MSG_ID_PROP_SERVICE_ADD_PROP_RSP     ((MSG_ID_PROP_SERVICE_BASE + 1) | 0x40)
#define MSG_ID_PROP_SERVICE_READ_PROP_RSP    ((MSG_ID_PROP_SERVICE_BASE + 3) | 0x40)
#define MSG_ID_PROP_SERVICE_WRITE_PROP_RSP   ((MSG_ID_PROP_SERVICE_BASE + 4) | 0x40)
#define MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP  ((MSG_ID_PROP_SERVICE_BASE + 2) | 0x40)

/**
 * Structure containing the response to:
 *  - @ref properties_service_read
 */
typedef struct read_property_rsp_msg {
	struct cfw_rsp_message rsp_header;   /*!< message header */
	/** status of the read operation, use it to check for errors */
	properties_storage_status_t status;
	uint16_t property_size;              /*!< size of property value */
	uint8_t start_of_values;             /*!< property value */
} read_property_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref properties_service_add
 */
typedef struct add_property_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
} add_property_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref properties_service_remove
 */
typedef struct remove_property_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
} remove_property_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref properties_service_write
 */
typedef struct write_property_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
} write_property_rsp_msg_t;

/**
 * Init Properties Service.
 *
 * Must be called before any other API of this service.
 *
 * @param queue queue used to transmit messages between different storage services
 */
void property_service_init(void * queue);

/**
 * Read a property.
 *
 * This function returns immediately and the response will be sent through an
 * incoming cfw message with ID MSG_ID_PROP_SERVICE_READ_PROP_RSP of type
 * read_property_rsp_msg.
 *
 * @param conn service client connection pointer.
 * @param service_id service_id client is interested in
 * @param property_id property_id the client is interested in
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_PROP_SERVICE_READ_PROP_RSP
 *
 */
int properties_service_read(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * priv);

/**
 * Add a property.
 *
 * @param conn service client connection pointer.
 * @param service_id service_id client is interested in
 * @param property_id property_id the client is interested in
 * @param factory_rest_persistent set to true if the property needs to persist upon a factory reset
 * @param buffer buffer containing the value of the new property
 * @param size size of the new property's value
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_PROP_SERVICE_ADD_PROP_RSP
 *
 */
int properties_service_add(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, bool factory_rest_persistent, void * buffer, uint16_t size, void * priv);


/**
 * Write or add a property.
 *
 * @param conn service client connection pointer.
 * @param service_id service_id client is interested in
 * @param property_id property_id the client is interested in
 * @param buffer buffer containing the value of the new property
 * @param size size of the new property's value
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_PROP_SERVICE_WRITE_PROP_RSP
 *
 */
int properties_service_write(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * buffer, uint16_t size, void * priv);

/**
 * Remove property from non-volatile memory
 *
 * @param conn service client connection pointer.
 * @param service_id service_id client is interested in
 * @param property_id property_id the client is interested in
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *  0     - request successfully sent \n
 *  panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_PROP_SERVICE_REMOVE_PROP_RSP
 *
 */
int properties_service_remove(cfw_service_conn_t * conn, uint16_t service_id, uint16_t property_id, void * priv);

/** @} */

#endif /* __PROPERTIES_SERVICE_API_H__ */
