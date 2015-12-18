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

#include <assert.h>
#include "cfw/cfw_client.h"
#include "machine.h"
#include "services/ll_storage_service/ll_storage_service.h"
#include "services/ll_storage_service/ll_storage_service_api.h"
#include "infra/log.h"
#include "storage.h"
#include "project_mapping.h"

/****************************************************************************************
 *********************** SERVICE API IMPLEMENATION **************************************
 ****************************************************************************************/

int ll_storage_service_erase_block(cfw_service_conn_t * conn, uint16_t partition_id, uint16_t start_block, uint16_t number_of_blocks, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_ERASE_BLOCK_REQ,
            sizeof(ll_storage_erase_block_req_msg_t), priv);
    ll_storage_erase_block_req_msg_t * req = (ll_storage_erase_block_req_msg_t *) msg;
    req->partition_id = partition_id;
    req->st_blk = start_block;
    req->no_blks = number_of_blocks;
    cfw_send_message(msg);
    return 0;
}

int ll_storage_service_erase_partition(cfw_service_conn_t * conn, uint16_t partition_id, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_WRITE_PARTITION_REQ,
                sizeof(ll_storage_write_partition_req_msg_t), priv);
    ll_storage_write_partition_req_msg_t * req = (ll_storage_write_partition_req_msg_t *) msg;
    req->write_type = ERASE_REQ;
    req->partition_id = partition_id;
    req->st_offset = 0;
    req->size = 0;
    req->buffer = NULL;
    cfw_send_message(msg);
    return 0;
}


int ll_storage_service_read(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t start_offset, uint32_t size, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_READ_PARTITION_REQ,
                sizeof(ll_storage_read_partition_req_msg_t), priv);
    ll_storage_read_partition_req_msg_t * req = (ll_storage_read_partition_req_msg_t *) msg;
    req->partition_id = partition_id;
    req->st_offset = start_offset;
    req->size = size;
    cfw_send_message(msg);
    return 0;
}


int ll_storage_service_write(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t start_offset, void * buffer, uint32_t size, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_WRITE_PARTITION_REQ,
                sizeof(ll_storage_write_partition_req_msg_t), priv);
    ll_storage_write_partition_req_msg_t * req = (ll_storage_write_partition_req_msg_t *) msg;
    req->write_type = WRITE_REQ;
    req->partition_id = partition_id;
    req->st_offset = start_offset;
    req->size = size;
    req->buffer = buffer;
    cfw_send_message(msg);
    return 0;
}

#ifdef CONFIG_SERVICES_QUARK_SE_STORAGE_CIRCULAR
int ll_storage_service_cir_stor_init(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t size, uint32_t block_start, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_CIR_STOR_INIT_REQ,
                sizeof(ll_storage_cir_stor_init_req_msg_t), priv);
    ll_storage_cir_stor_init_req_msg_t * req = (ll_storage_cir_stor_init_req_msg_t *) msg;
    req->partition_id = partition_id;
    req->size = size;
    req->block_start = block_start;
    cfw_send_message(msg);
    return 0;
}

int ll_storage_service_push(cfw_service_conn_t * conn, uint8_t * buffer, uint32_t size, void *storage, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_PUSH_REQ,
                sizeof(ll_storage_push_req_msg_t), priv);
    ll_storage_push_req_msg_t * req = (ll_storage_push_req_msg_t *) msg;
    req->size = size;
    req->buffer = buffer;
    req->storage = storage;
    cfw_send_message(msg);
    return 0;
}

int ll_storage_service_pop(cfw_service_conn_t * conn, uint32_t size, void *storage, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_POP_REQ,
                sizeof(ll_storage_pop_req_msg_t), priv);
    ll_storage_pop_req_msg_t * req = (ll_storage_pop_req_msg_t *) msg;
    req->size = size;
    req->storage = storage;
    cfw_send_message(msg);
    return 0;
}

int ll_storage_service_peek(cfw_service_conn_t * conn, uint32_t size, uint32_t offset, void *storage, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_PEEK_REQ,
                sizeof(ll_storage_peek_req_msg_t), priv);
    ll_storage_peek_req_msg_t * req = (ll_storage_peek_req_msg_t *) msg;
    req->offset = offset;
    req->size = size;
    req->storage = storage;
    cfw_send_message(msg);
    return 0;
}

int ll_storage_service_clear(cfw_service_conn_t * conn, void *storage, uint32_t size, void * priv)
{
    struct cfw_message * msg = cfw_alloc_message_for_service(conn, MSG_ID_LL_CLEAR_REQ,
                sizeof(ll_storage_clear_req_msg_t), priv);
    ll_storage_clear_req_msg_t * req = (ll_storage_clear_req_msg_t *) msg;
    req->size = size;
    req->storage = storage;
    cfw_send_message(msg);
    return 0;
}
#endif
