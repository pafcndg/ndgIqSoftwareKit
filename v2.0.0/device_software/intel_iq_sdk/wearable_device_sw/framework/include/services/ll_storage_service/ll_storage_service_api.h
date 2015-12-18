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

#ifndef __LL_STORAGE_SERVICE_API_H__
#define __LL_STORAGE_SERVICE_API_H__

#include "ll_storage_service.h"
#include "storage.h"

/**
 * @defgroup ll_storage_service Low Level Storage Service
 * Perform operations on non-volatile memory.
 * @ingroup services
 */

/**
 * @defgroup ll_storage_service_api Low Level Storage Service API
 * Define the interface for Low Level Storage Service.
 * @ingroup ll_storage_service
 * @{
 */

/**
 * Flash memory devices array
 */
extern const flash_device_t flash_devices[];

/**
 * Flash memory partitioning array
 */
extern flash_partition_t storage_configuration[];

#define MSG_ID_LL_ERASE_BLOCK_RSP                       (MSG_ID_LL_ERASE_BLOCK_REQ | 0x40)
#define MSG_ID_LL_READ_PARTITION_RSP                    (MSG_ID_LL_READ_PARTITION_REQ | 0x40)
#define MSG_ID_LL_WRITE_PARTITION_RSP                   (MSG_ID_LL_WRITE_PARTITION_REQ | 0x40)
#define MSG_ID_LL_PUSH_RSP                              (MSG_ID_LL_PUSH_REQ | 0x40)
#define MSG_ID_LL_POP_RSP                               (MSG_ID_LL_POP_REQ | 0x40)
#define MSG_ID_LL_PEEK_RSP                              (MSG_ID_LL_PEEK_REQ | 0x40)
#define MSG_ID_LL_CLEAR_RSP                             (MSG_ID_LL_CLEAR_REQ | 0x40)
#define MSG_ID_LL_CIR_STOR_INIT_RSP                     (MSG_ID_LL_CIR_STOR_INIT_REQ | 0x40)

/*MX25U12835F is 128Mb bits serial Flash memory,

Program command is executed on byte basis, or page (256 bytes) basis, or word basis
Erase command is executed on sector (4K-byte), block (32K-byte), or large block (64K-byte), or whole chip basis.
The erase block size that will be considered in the sector size */

/* Quark SE embedded 384KB Flash memory,

Program command is executed on 32-bits double word basis
Erase command is executed on pages (2K-byte), whole flash basis.
The erase block size that will be considered is the page size */

#define ERASE_REQ                                       0
#define WRITE_REQ                                       1

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_erase_block
 */
typedef struct ll_storage_erase_block_rsp_msg {
    struct cfw_rsp_message rsp_header;  /*!< message header */
} ll_storage_erase_block_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_erase_partition
 *  - @ref ll_storage_service_write
 */
typedef struct ll_storage_write_partition_rsp_msg {
    struct cfw_rsp_message rsp_header; /*!< message header */
    uint8_t write_type;                 /*!< type of action: ERASE/WRITE */
    uint32_t actual_size;               /*!< actual size of written data */
} ll_storage_write_partition_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_read
 */
typedef struct ll_storage_read_partition_rsp_msg {
    struct cfw_rsp_message rsp_header; /*!< message header */
    uint32_t actual_read_size;          /*!< number of bytes read */
    void * buffer;                     /*!< buffer containing data */
} ll_storage_read_partition_rsp_msg_t;

#ifdef CONFIG_SERVICES_QUARK_SE_STORAGE_CIRCULAR
/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_cir_stor_init
 */
typedef struct ll_storage_cir_stor_init_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
	void *storage;            /*!< storage pointer */
} ll_storage_cir_stor_init_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_push
 */
typedef struct ll_storage_push_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
} ll_storage_push_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_pop
 */
typedef struct ll_storage_pop_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
	uint8_t *buffer;                   /*!< buffer containing data */
} ll_storage_pop_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_peek
 */
typedef struct ll_storage_peek_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
	uint8_t *buffer;                   /*!< buffer containing data */
} ll_storage_peek_rsp_msg_t;

/**
 * Structure containing the response to:
 *  - @ref ll_storage_service_clear
 */
typedef struct ll_storage_clear_rsp_msg {
	struct cfw_rsp_message rsp_header; /*!< message header */
} ll_storage_clear_rsp_msg_t;
#endif

/**
 * Init and Configure partitions seen by the Storage Service.
 *
 * @param queue message queue to be used
 * @param storage_partitions pointer to the array that contains the list of partitions low-level storage system manages
 * @param number_of_partitions number of partitions that are provided
 */
void ll_storage_service_init(void * queue, flash_partition_t* storage_partitions, uint8_t number_of_partitions);

/**
 * Low level partition erase.
 *
 * @msc
 *  Client,"Low Level Storage Service","Flash memory driver";
 *
 *  Client->"Low Level Storage Service" [label="erase request", URL="\ref ll_storage_write_partition_req_msg"];
 *  "Low Level Storage Service"=>"Flash memory driver" [label="erase \n function call"];
 *  "Low Level Storage Service"<<"Flash memory driver" [label="erase \n return status"];
 *  Client<-"Low Level Storage Service" [label="erase response \n message w/ status", URL="\ref ll_storage_write_partition_rsp_msg"];
 * @endmsc
 *
 * @param conn service client connection pointer.
 * @param partition_id ID of the partition
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_WRITE_PARTITION_RSP; write_type element - ERASE_REQ
 *
 */
int ll_storage_service_erase_partition(cfw_service_conn_t * conn, uint16_t partition_id, void * priv) ;

/**
 * Low level erase block(s).
 *
 * @msc
 *  Client,"Low Level Storage Service","Flash memory driver";
 *
 *  Client->"Low Level Storage Service" [label="erase request", URL="\ref ll_storage_erase_block_req_msg"];
 *  "Low Level Storage Service"=>"Flash memory driver" [label="erase block\n function call"];
 *  "Low Level Storage Service"<<"Flash memory driver" [label="erase block \n return status"];
 *  Client<-"Low Level Storage Service" [label="erase response \n message w/ status", URL="\ref ll_storage_erase_block_rsp_msg"];
 * @endmsc
 *
 * @param conn service client connection pointer.
 * @param partition_id ID of the partition
 * @param start_block first block to be erased (offset from the beginning of the partition)
 * @param number_of_blocks  number of blocks to be erased
 * @param priv private data pointer that will be passed sending answer
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_ERASE_BLOCK_RSP; write_type element - ERASE_REQ
 *
 */
int ll_storage_service_erase_block(cfw_service_conn_t * conn, uint16_t partition_id, uint16_t start_block, uint16_t number_of_blocks, void * priv) ;

/**
 * Low level data read.
 *
 * @msc
 *  Client,"Low Level Storage Service","Flash memory driver";
 *
 *  Client->"Low Level Storage Service" [label="read request", URL="\ref ll_storage_read_partition_req_msg"];
 *  "Low Level Storage Service"=>"Flash memory driver" [label="read \n function call"];
 *  "Low Level Storage Service"<<"Flash memory driver" [label="read return status \n and data"];
 *  Client<-"Low Level Storage Service" [label="read response \n message w/ status and data", URL="\ref ll_storage_read_partition_rsp_msg"];
 * @endmsc
 *
 * @param conn service client connection pointer.
 * @param partition_id ID of the partition
 * @param start_offset first data address to be read (offset from the beginning of the partition); \n must be 4bytes aligned
 * @param size  number of bytes to be read
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_READ_PARTITION_RSP
 *
 */
int ll_storage_service_read(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t start_offset, uint32_t size, void * priv) ;

/**
 * Low level data write.
 *
 * @msc
 *  Client,"Low Level Storage Service","Flash memory driver";
 *
 *  Client->"Low Level Storage Service" [label="write request", URL="\ref ll_storage_write_partition_req_msg"];
 *  "Low Level Storage Service"=>"Flash memory driver" [label="write \n function call"];
 *  "Low Level Storage Service"<<"Flash memory driver" [label="write \n return status"];
 *  Client<-"Low Level Storage Service" [label="write response \n message w/ status", URL="\ref ll_storage_write_partition_rsp_msg"];
 * @endmsc
 *
 * @param conn service client connection pointer.
 * @param partition_id ID of the partition
 * @param start_offset first address to be written (offset from the beginning of the partition); \n must be 4bytes aligned
 * @param buffer buffer containing data to be written
 * @param size  number of bytes to be written
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_WRITE_PARTITION_RSP; write_type element - WRITE_REQ
 *
 */
int ll_storage_service_write(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t start_offset, void * buffer, uint32_t size, void * priv) ;

#ifdef CONFIG_SERVICES_QUARK_SE_STORAGE_CIRCULAR
/**
 * Flash storage init.
 *
 * @param conn service client connection pointer.
 * @param partition_id ID of the partition
 * @param block_start  start block of circular buffer
 * @param size  number of bytes to be written
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_PUSH_REQ
 *
 */
int ll_storage_service_cir_stor_init(cfw_service_conn_t * conn, uint16_t partition_id, uint32_t block_start, uint32_t size, void * priv);

/**
 * Flash storage push.
 *
 * @param conn service client connection pointer.
 * @param buffer buffer containing data to be written
 * @param size  number of bytes to be written
 * @param storage  pointer on the storage struct
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_PUSH_REQ
 *
 */
int ll_storage_service_push(cfw_service_conn_t * conn, uint8_t * buffer, uint32_t size, void *storage, void * priv);

/**
 * Flash storage pop.
 *
 * @param conn service client connection pointer.
 * @param size  number of bytes to be read
 * @param storage  pointer on the storage struct
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_POP_REQ
 *
 */
int ll_storage_service_pop(cfw_service_conn_t * conn, uint32_t size, void *storage, void * priv);

/**
 * Flash storage peek.
 *
 * @param conn service client connection pointer.
 * @param size  number of bytes to be read
 * @param offset read starts at offset.
 * @param storage  pointer on the storage struct
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_PEEK_REQ
 *
 */
int ll_storage_service_peek(cfw_service_conn_t * conn, uint32_t size, uint32_t offset, void *storage, void * priv);


/**
 * Flash storage clear data.
 *
 * @param conn service client connection pointer.
 * @param storage  pointer on the storage struct
 * @param size  size to clear, 0 to clear all
 * @param priv private data pointer that will be passed sending answer.
 *
 * @return
 *     0     - request successfully sent \n
 *     panic - issue during allocation of request message
 *
 * @b Response: message ID - MSG_ID_LL_ERASE_REQ
 *
 */
int ll_storage_service_clear(cfw_service_conn_t * conn, void *storage, uint32_t size, void * priv);

#endif

/** @} */

#endif /* __LL_STORAGE_SERVICE_H__ */
