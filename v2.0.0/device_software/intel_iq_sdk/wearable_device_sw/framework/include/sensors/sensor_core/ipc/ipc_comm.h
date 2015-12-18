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

#ifndef __IPC_COMM_H
#define __IPC_COMM_H

#include "ipc_ia.h"
#include "infra/panic.h"

/**
 * @ingroup sensor_core_ipc
 * @{
 */
#define MAX_IPC_2CORE_NUM_MSG  100
#define MAX_IPC_2SVC_NUM_MSG  100

/** \brief Generic type for execution status */
typedef enum {
    IPC_STS_OK = 0,                  /** generic OK status */
    /* use negative values for errors */
    IPC_STS_ERR = -1,                /** generic error status */
    IPC_STS_ERR_TIMEOUT = -2,        /** timeout expired */
    IPC_STS_ERR_BUSY = -3,           /** resource is not available */
    IPC_STS_ERR_OVERFLOW = -4,       /** service would cause an overflow */
    IPC_STS_ERR_EMPTY = -5,          /** no data available (e.g.  queue is empty) */
    IPC_STS_ERR_NOT_ALLOWED = -6,    /** service is not allowed in current execution context */
    /* more error codes to be defined */
    IPC_STS_ERR_UNKNOWN = - 100,     /** invalid error code (bug?) */
} IPC_ERR_TYPE;

/**
 * @brief Create a IPC channel
 * @return  negative: failure, 0 success,
 */
extern IPC_ERR_TYPE ipc_svc_core_create();

/**
 * @brief sensor_core recieve message
 *        Authorized execution levels:  task, fiber.
 * @param[in] cmd       common struct to transmit
 * @param[in] timeout   maximum number of milliseconds to wait for the message.
 * @return  negative: failure, 0 success.
 */
extern IPC_ERR_TYPE ipc_core_receive(struct ia_cmd **cmd, int timeout);

/**
 * @brief SVC send a message  to sensor_core on a queue
 *        Authorized execution levels:  task, fiber, ISR.
 * @param[in] cmd       common struct to transmit
 * @return  negative: failure, 0 success,
 */
extern IPC_ERR_TYPE ipc_2core_send(struct ia_cmd *cmd);

/**
 * sensor_service recieve message
 * @param[in] cmd       common struct to transmit
 * @param[in] timeout   maximum number of milliseconds to wait for the message.
 *
 * @return  negative: failure, 0 success,
 */
extern IPC_ERR_TYPE ipc_svc_receive(struct ia_cmd **cmd, int timeout);

/**
 * @brief Sensor-core send a message  to svc on a queue
 *        Authorized execution levels:  task, fiber, ISR.
 * @param[in] cmd       common struct to transmit
 * @return  negative: failure, 0 success,
 */
extern IPC_ERR_TYPE ipc_2svc_send(struct ia_cmd *cmd);

/** @} */
#endif
