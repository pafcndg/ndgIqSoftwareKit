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

#ifndef __CBUFFER_STORAGE_BACKEND_H
#define __CBUFFER_STORAGE_BACKEND_H

typedef struct _cbuffer_storage_t cbuffer_storage_t;
/**
 * A cbuffer_storage backends provide the final functions used to write/read buffer elements.
 * @note
 * offset is the relative address to the circular buffer. 0 <= offset <= cbuffer_size - 1.
 * buf to write/read can be considered contiguous at this level.
 */
typedef struct {
    /** byte write function, return len if success */
    uint32_t (*write)(cbuffer_storage_t *cbuffer_storage,
                        uint32_t offset,
                        const uint8_t *buf,
                        uint32_t len);

    /** byte read function. return len if success */
    uint32_t (*read)(cbuffer_storage_t *cbuffer_storage,
                        uint32_t offset,
                        uint8_t *buf,
                        uint32_t len);
} cbuffer_storage_backend_t;

#endif /* __CBUFFER_STORAGE_BACKEND_H */
