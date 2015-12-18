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

#ifndef __PROPERTIES_SERVICE_INTERNAL_H__
#define __PROPERTIES_SERVICE_INTERNAL_H__

#include "cfw/cfw.h"

#define MSG_ID_PROP_SERVICE_ADD_PROP_REQ      (MSG_ID_PROP_SERVICE_BASE + 1)
#define MSG_ID_PROP_SERVICE_REMOVE_PROP_REQ   (MSG_ID_PROP_SERVICE_BASE + 2)
#define MSG_ID_PROP_SERVICE_READ_PROP_REQ     (MSG_ID_PROP_SERVICE_BASE + 3)
#define MSG_ID_PROP_SERVICE_WRITE_PROP_REQ    (MSG_ID_PROP_SERVICE_BASE + 4)

#define ADD_PROP_ACTION                       0x1
#define WRITE_PROP_ACTION                     0x2
#define REMOVE_PROP_ACTION                    0x3
#define READ_PROP_ACTION                      0x5

/**
 * Structure containing the request for:
 *  - @ref storage_service_properties_add
 */
typedef struct add_property_req_msg {
	struct cfw_message header;
	uint16_t service_id;
	uint16_t property_id;
	uint8_t factory_reset_persistent;
	uint16_t size;
	uint8_t value[];
} add_property_req_msg_t;

typedef struct write_property_req_msg {
	struct cfw_message header;
	uint16_t service_id;
	uint16_t property_id;
	uint16_t size;
	uint8_t value[];
} write_property_req_msg_t;

typedef struct remove_property_req_msg {
	struct  cfw_message header;
	uint16_t service_id;
	uint16_t property_id;
} remove_property_req_msg_t;

typedef struct read_property_req_msg {
	struct  cfw_message header;
	uint16_t service_id;
	uint16_t property_id;
} read_property_req_msg_t;


#endif /* __PROPERTIES_SERVICE_INTERNAL_H__ */
