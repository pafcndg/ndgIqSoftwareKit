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

#ifndef __INFRA_TCMD_MESSAGES_H_
#define __INFRA_TCMD_MESSAGES_H_

#include "infra/message.h"

/**
 * @addtogroup infra
 * @{
 * @defgroup infra_tcmd Test Commands
 * @}
 */

/**
 * @addtogroup infra_tcmd
 * @{
 */

/**
 * @defgroup infra_tcmd_multi Test Commands Engine Multi Nodes support
 * @brief Allow multiple engines to be organized in master/slave
 * @{
 */

/* Message ranges */
#define INFRA_MSG_TCMD_SLAVE_REQ   0x00 + INFRA_MSG_TCMD_BASE
#define INFRA_MSG_TCMD_SLAVE_RSP   0x40 + INFRA_MSG_TCMD_BASE
#define INFRA_MSG_TCMD_ASYNC       0x80 + INFRA_MSG_TCMD_BASE
#define INFRA_MSG_TCMD_SLAVE_REG   0x81 + INFRA_MSG_TCMD_BASE

#define TCMD_GROUP_MAX_LENGTH 8
#define TCMD_NAME_MAX_LENGTH  8
#define TCMD_SLAVE_MAX_LENGTH 8

/**
 * An message capable of holding a Test Command request to send it to a slave
 *
 * The command is the last field of the message and has a variable size
 */
struct tcmd_slave_request {
	struct message msg;
	void * ctx;
	struct tcmd_slave * slave;
	char command[];
};

/**
 * A message capable of holding a Test Command response to send it to master
 *
 * The last field of the message is the only one of variable size
 */
struct tcmd_slave_response {
	struct message msg;
	void * ctx;
	char group[TCMD_GROUP_MAX_LENGTH + 1];
	char name[TCMD_NAME_MAX_LENGTH + 1];
	unsigned int cii;
	int type;
	char buffer[];
};

/**
 * Handle remote Test Command messages
 *
 * @param msg the test command message
 * @param data a module-wide opaque data (unused)
 *
 */
void handle_tcmd_remote_messages(struct message * msg, void *data);

/** @} @} */
#endif /* __INFRA_TCMD_MESSAGES_H_ */
