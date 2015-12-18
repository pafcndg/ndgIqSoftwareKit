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

#ifndef __LL_STORAGE_SERVICE_H__
#define __LL_STORAGE_SERVICE_H__

#include <stdint.h>

#include "cfw/cfw.h"
#include "services/services_ids.h"
#include "util/cir_storage.h"

#define MSG_ID_LL_ERASE_BLOCK_REQ        (MSG_ID_LL_STORAGE_BASE + 2)
#define MSG_ID_LL_READ_PARTITION_REQ     (MSG_ID_LL_STORAGE_BASE + 3)
#define MSG_ID_LL_WRITE_PARTITION_REQ    (MSG_ID_LL_STORAGE_BASE + 4)
#define MSG_ID_LL_PUSH_REQ               (MSG_ID_LL_STORAGE_BASE + 5)
#define MSG_ID_LL_POP_REQ                (MSG_ID_LL_STORAGE_BASE + 6)
#define MSG_ID_LL_PEEK_REQ               (MSG_ID_LL_STORAGE_BASE + 7)
#define MSG_ID_LL_CLEAR_REQ              (MSG_ID_LL_STORAGE_BASE + 8)
#define MSG_ID_LL_CIR_STOR_INIT_REQ      (MSG_ID_LL_STORAGE_BASE + 9)

typedef struct ll_storage_erase_block_req_msg {
    struct cfw_message header;
    uint16_t partition_id;
    uint32_t st_blk;
    uint32_t no_blks;
} ll_storage_erase_block_req_msg_t;

typedef struct ll_storage_write_partition_req_msg {
    struct cfw_message header;
    uint16_t partition_id;
    uint32_t st_offset;
    uint32_t size;
    uint8_t write_type;
    void * buffer;
} ll_storage_write_partition_req_msg_t;

typedef struct ll_storage_read_partition_req_msg {
    struct cfw_message header;
    uint16_t partition_id;
    uint32_t st_offset;
    uint32_t size;
} ll_storage_read_partition_req_msg_t;

#ifdef CONFIG_SERVICES_QUARK_SE_STORAGE_CIRCULAR
typedef struct ll_storage_cir_stor_init_req_msg {
    struct cfw_message header;
    uint32_t size;
    uint32_t block_start;
    uint16_t partition_id;
} ll_storage_cir_stor_init_req_msg_t;

typedef struct ll_storage_push_req_msg {
    struct cfw_message header;
    void *storage;
    uint32_t size;
    uint8_t *buffer;
} ll_storage_push_req_msg_t;

typedef struct ll_storage_pop_req_msg {
    struct cfw_message header;
    void *storage;
    uint32_t size;
} ll_storage_pop_req_msg_t;

typedef struct ll_storage_peek_req_msg {
    struct cfw_message header;
    void *storage;
    uint32_t offset;
    uint32_t size;
} ll_storage_peek_req_msg_t;

typedef struct ll_storage_clear_req_msg {
    struct cfw_message header;
    uint32_t size;
    void *storage;
} ll_storage_clear_req_msg_t;
#endif

#endif /* __LL_STORAGE_SERVICE_H__ */
