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

#ifndef _INFRA_TCMD_MASTER_H
#define _INFRA_TCMD_MASTER_H

#include "infra/tcmd/engine.h"

/**
 * @addtogroup infra_tcmd_multi
 * @{
 */

/**
 * Send a test command to the active engine
 *
 * @param command a null-terminated buffer containing the command
 * @param callback the function to call repeatedly until command is complete
 * @param data: opaque data to be passed back to the caller on each response
 *
 */
void tcmd_dispatch(char *command, tcmd_rsp_cb_t callback, void *data);

/**
 * Called when a TCMD IPC synchronous request is received
 *
 * Inter-processors TCMD request handler called in the context of an interrupt
 *
 * @param cpu_id the cpu this request comes from
 * @param param1 a first int parameter
 * @param param2 a second int parameter
 * @param ptr    a last pointer parameter
 *
 * @return 0 if success, -1 otherwise
 */
int tcmd_ipc_sync_callback(int cpu_id, int request, int param1, int param2
		, void *ptr);

/** @} */

#endif /* _INFRA_TCMD_MASTER_H */
