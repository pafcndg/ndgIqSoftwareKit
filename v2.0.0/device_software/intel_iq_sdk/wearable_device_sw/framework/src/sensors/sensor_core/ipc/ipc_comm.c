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

#include <stdio.h>
#include <string.h>
#include "ipc_comm.h"
#include "os/os.h"
#include "cfw/cfw_client.h"
#include "cfw/cfw_service.h"
#include "services/sensor_service/sensor_svc_sensor_core.h"
#include "infra/log.h"
#include "infra/message.h"

void *IPC_2CORE_QUEUE;
#define CONVERT_OS_ERR_TYPE_TO_GENERIC_ERROR(err) ((int)0 | err)
#define ipc_log(fmt, arg...) pr_info(LOG_MODULE_SC_IPC,"[%s]:"fmt, __func__,## arg)

static const char *_get_name_err_type( IPC_ERR_TYPE id)
{
    switch ( id ){
        case 0:
        return "IPC_STS_OK";
        case -1:
        return "IPC_STS_ERR";
        case -2:
        return "IPC_STS_ERR_TIMEOUT";
        case -3:
        return "IPC_STS_ERR_BUSY";
        case -4:
        return "IPC_STS_ERR_OVERFLOW";
        case -5:
        return "IPC_STS_ERR_EMPTY";
        case -6:
        return "IPC_STS_ERR_NOT_ALLOWED";
        default:
        return "IPC_STS_ERR_UNKNOWN";
    }
}

/*
*\brief  Initalize 2 queue to connect the SVC and SENSOR_CORE
*
* */
IPC_ERR_TYPE ipc_svc_core_create( )
{
    OS_ERR_TYPE err = 0;
    IPC_2CORE_QUEUE = (void *)queue_create(MAX_IPC_2CORE_NUM_MSG, &err);
    if( err < 0 )
        ipc_log("Create IPC_2CORE ERR,err_id=%s\n",_get_name_err_type(err));
    return err;
}

/**
 * \brief Read a message from sensor_CORE
 *     Read and dequeue a message.
 *     This service may panic if err parameter is NULL and:
 *      -# queue parameter is invalid, or
 *      -# message parameter is NULL, or
 *      -# when called from an ISR.
 *
 *     Authorized execution levels:  task, fiber.
 *
 * \param timeout: maximum number of milliseconds to wait for the message. Special
 *                values OS_NO_WAIT and OS_WAIT_FOREVER may be used.
 * \return : return IPC_ERR_TYPE, we can use _get_name_err_type to locate the err reason.
 *           return 0 is sucessful.
 */
IPC_ERR_TYPE ipc_core_receive(struct ia_cmd **cmd, int timeout)
{
    OS_ERR_TYPE err = 0;
    if( !IPC_2CORE_QUEUE ){
        ipc_log("ERR: please create ipc first\n");
        return IPC_STS_ERR_EMPTY;
    }
    queue_get_message (IPC_2CORE_QUEUE, (void **)cmd, timeout,  &err);
    if(err < 0 && err != -2 ){
        ipc_log("Receive cmd err,err_id=%s\n",_get_name_err_type(err));
        panic(CONVERT_OS_ERR_TYPE_TO_GENERIC_ERROR(err));
    }
    return err;
}

/**
 * \brief SVC send a message  to sensor_core on a queue
 *     This service may panic if err parameter is NULL and:
 *      -# queue parameter is invalid, or
 *      -# the queue is already full, or
 *     Authorized execution levels:  task, fiber, ISR.
 *
 *\param ia_cmd : message to send, should not be NULL
 *\return 0:sucessed  <0:failed
 */
IPC_ERR_TYPE ipc_2core_send(struct ia_cmd *cmd)
{
    OS_ERR_TYPE err = 0;
    if( !cmd ){
        ipc_log("Message is NULL\n");
        return IPC_STS_ERR_EMPTY;
    }
    if( !IPC_2CORE_QUEUE ) {
        ipc_log("ERR: please create ipc first\n");
        return IPC_STS_ERR_EMPTY;
    }
    queue_send_message(IPC_2CORE_QUEUE, (void*)cmd, &err);
    if( err < 0 ){
        ipc_log("Ipc err[%s]\n", _get_name_err_type(err));
        panic(CONVERT_OS_ERR_TYPE_TO_GENERIC_ERROR(err));
    }
    return err;
}

/**
 * \brief Sensor-core send a message  to svc on a queue
 *     This service may panic if err parameter is NULL and:
 *      -# queue parameter is invalid, or
 *      -# the queue is already full, or
 *     Authorized execution levels:  task, fiber, ISR.
 *
 *\param ia_cmd : message to send, should not be NULL
 *\return 0:sucessed  <0:failed
 */
extern int ss_svc_port_id;

IPC_ERR_TYPE ipc_2svc_send(struct ia_cmd *cmd)
{
    sc_rsp_t *p_msg;
    OS_ERR_TYPE  err = 0;
    if( !cmd ){
        ipc_log("Message is NULL,err_type=%s\n",_get_name_err_type(IPC_STS_ERR_EMPTY));
        goto err_out;
    }
    if( !cmd->length ){
        ipc_log("Message's length is 0,err_type=%s\n",_get_name_err_type(IPC_STS_ERR_EMPTY));
        goto err_out;
    }
    /* Allocate message */
    p_msg = (sc_rsp_t*) message_alloc(sizeof(sc_rsp_t)+cmd->length, &err);
    if( err < 0 ){
        ipc_log("Allocate message err,err=%s\n",_get_name_err_type(err));
        panic(CONVERT_OS_ERR_TYPE_TO_GENERIC_ERROR(err));
    }
    /* Fill Parameters and send */
    if(cmd->cmd_id == SENSOR_DATA)
        CFW_MESSAGE_ID(&p_msg->head) = MSG_ID_SS_SC_EVT_MSG;
    else
        CFW_MESSAGE_ID(&p_msg->head) = MSG_ID_SS_SC_RSP_MSG;
    CFW_MESSAGE_LEN(&p_msg->head) = sizeof(sc_rsp_t)+cmd->length;
    CFW_MESSAGE_SRC(&p_msg->head) = ss_svc_port_id;
    CFW_MESSAGE_DST(&p_msg->head) = ss_svc_port_id;
    CFW_MESSAGE_TYPE(&p_msg->head) = TYPE_REQ;

    p_msg->msg_id = cmd->cmd_id;
    memcpy(p_msg->param, cmd, cmd->length);

    return  cfw_send_message( p_msg );
err_out:
    return IPC_STS_ERR;
}

