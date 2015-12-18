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

#ifndef __CBUFFER_STORAGE_H
#define __CBUFFER_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "infra/cbuffer_storage_backend.h"

/**
 * @defgroup infra_cbuffer Circular buffer storage
 * Handle of the circular buffer used to log over UART or USB
 * @ingroup infra
 * @{
 */

/** Circular buffer error code. */
typedef enum {
    CBUFFER_STORAGE_SUCCESS = 0,    /*!< Operation succeed */
    CBUFFER_STORAGE_BOUNDS_ERROR,   /*!< Bounds error */
    CBUFFER_STORAGE_EMPTY_ERROR,    /*!< Empty circular buffer error */
    CBUFFER_STORAGE_WRITE_ERROR,    /*!< Writing error from backend function*/
    CBUFFER_STORAGE_READ_ERROR,     /*!< Reading error from backend function*/
    CBUFFER_STORAGE_ERROR           /*!< Default circular buffer error */
} cbuffer_storage_err_t;


/** Circular buffer type. */
typedef enum {
    /*!< Each element to store have the same size */
    CBUFFER_STORAGE_FIXED_ELEMENT_SIZE = 0,
    /*!< Each element size can be different */
    CBUFFER_STORAGE_VARIABLE_ELEMENT_SIZE
} cbuffer_storage_type_t;


/**
 * Handle of the circular buffer
 * @note all fields must be filled.
 * default recommended values for a new circular buffer: r=0, w=0, is_buffer_empty=1.
 */
typedef struct _cbuffer_storage_t {
    uint32_t cbuffer_size;    /*!< Circular buffer size, must be a power of 2 */
    uint32_t cbuffer_element_size;  /*!< Circular buffer element size (for fixed size element) */
    uint32_t r, w;            /*!< r points to the oldest element (read pointer) */
                              /*!< w points just after the most recent element (write pointer) */

    cbuffer_storage_err_t (*_push)(cbuffer_storage_t * self, uint8_t *buf, uint32_t len);
    cbuffer_storage_err_t (*_pop)(cbuffer_storage_t * self, uint8_t *buf, uint32_t buf_size);
    cbuffer_storage_err_t (*_peek)(cbuffer_storage_t * self, uint32_t offset, uint8_t *buf);
    /*!< get current read element size. */
    uint32_t (*_get_element_size)(const cbuffer_storage_t * self);

    cbuffer_storage_backend_t cb_backend;   /*!< user specific functions to be implemented */

    cbuffer_storage_type_t cbuffer_type;                   /*!< Circular buffer type */
} cbuffer_storage_t;


/**
 * Push an element in the circular buffer.
 * @param self the pointer on the circular buffer.
 * @param buf pointer to the data to push.
 * @param len length of the buffer to push.
 * @return cbuffer_storage_err_t error code.
 *  CBUFFER_STORAGE_BOUNDS_ERROR: buffer length to push is larger
 *                                  than buffer size.
 *  CBUFFER_STORAGE_WRITE_ERROR: Writing step failed, element is not pushed.
 *  CBUFFER_STORAGE_SUCCESS: circular buffer push succeed.
 */
cbuffer_storage_err_t cbuffer_storage_push(cbuffer_storage_t * self,
                                    uint8_t *buf,
                                    uint32_t len);

/**
 * Pop the oldest element from the circular buffer
 * Popped element is definitely removed from the circular buffer.
 * @param self the pointer on the circular buffer.
 * @param buf pointer to the buffer to fill.
 * @param buf_size size of the buffer buf.
 * @return cbuffer_storage_err_t error code.
 *  CBUFFER_STORAGE_BOUNDS_ERROR: element to pop is larger than buffer size.
 *  CBUFFER_STORAGE_EMPTY_ERROR: circular buffer is empty. Pop is not possible.
 *  CBUFFER_STORAGE_READ_ERROR: Reading step failed, element is not popped.
 *  CBUFFER_STORAGE_SUCCESS: circular buffer pop succeed.
 */
cbuffer_storage_err_t cbuffer_storage_pop(cbuffer_storage_t * self,
                                    uint8_t *buf,
                                    uint32_t buf_size);

/**
 * Read 1 byte from the circular buffer.
 * @param self the pointer on the circular buffer.
 * @param offset read starts at offset.
 * @param buf pointer to the buffer to fill.
 * @return cbuffer_storage_err_t error code.
 *  CBUFFER_STORAGE_READ_ERROR: Reading step failed, element is not peeked.
 *  CBUFFER_STORAGE_SUCCESS: circular buffer peek succeed.
 */
cbuffer_storage_err_t cbuffer_storage_peek(cbuffer_storage_t * self,
                                    uint32_t offset,
                                    uint8_t *buf);

/** @} */

#endif /* __CBUFFER_STORAGE_H */
