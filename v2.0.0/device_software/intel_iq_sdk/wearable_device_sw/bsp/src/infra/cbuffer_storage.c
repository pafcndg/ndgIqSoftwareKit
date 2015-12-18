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

#include <stdbool.h>
#include <string.h>

#include "util/misc.h"
#include "infra/cbuffer_storage.h"


/* Bounds management macro for "power of 2" buffer_size only */
#define CBUFFER_MODULO(A) ((A) & (self->cbuffer_size - 1))

static bool cbuffer_storage_is_empty(const cbuffer_storage_t * self)
{
    return (self->r == self->w);
}


cbuffer_storage_err_t cbuffer_storage_push(cbuffer_storage_t * self,
                                    uint8_t *buf,
                                    uint32_t len)
{
    /*****************************/
    /***** BUFFER MANAGEMENT *****/
    /*****************************/

    /* Invalid buffer len */
    if (len > self->cbuffer_size)
        return CBUFFER_STORAGE_BOUNDS_ERROR;

    if (!cbuffer_storage_is_empty(self))
    {
        /* Update r pointer to always point to a valid element */
        /* (w pointer must not "catching up" r pointer) */
        while (CBUFFER_MODULO(self->r - self->w) < len)
            self->r = CBUFFER_MODULO(self->r + self->_get_element_size(self));
    }


    /******************************/
    /***** BUFFER DATA UPDATE *****/
    /******************************/

    /* Is buf to write a single continuous block ? */
    /* Yes. */
    if (CBUFFER_MODULO(self->w + len) > self->w)
    {
        if (self->cb_backend.write(self, self->w, buf, len) == len)
            self->w = CBUFFER_MODULO(self->w + len);
        else
            return CBUFFER_STORAGE_WRITE_ERROR;
    }
    /* No, 2 write operations needed. */
    else
    {
        uint32_t cond1 = self->cb_backend.write(self,
                                                self->w,
                                                buf,
                                                self->cbuffer_size - self->w);
        uint32_t cond2 = self->cbuffer_size - self->w;
        uint32_t cond3 = self->cb_backend.write(self,
                                              0,
                                              &buf[self->cbuffer_size - self->w],
                                              len - cond2);
        uint32_t cond4 = len - cond2;
        if (((cond1) == (cond2)) && ((cond3) == (cond4)))
            self->w = CBUFFER_MODULO(self->w + len);
        else
            return CBUFFER_STORAGE_WRITE_ERROR;
    }

    /* PUSH correctly done. */
    return CBUFFER_STORAGE_SUCCESS;
}

cbuffer_storage_err_t cbuffer_storage_pop(cbuffer_storage_t * self,
                                  uint8_t *buf,
                                  uint32_t buf_size)
{
    /*****************************/
    /***** BUFFER MANAGEMENT *****/
    /*****************************/
    /* buffer empty, can't pop element */
    if (cbuffer_storage_is_empty(self))
        return CBUFFER_STORAGE_EMPTY_ERROR;

    uint32_t element_size = self->_get_element_size(self);
    uint32_t next_r = CBUFFER_MODULO(self->r + element_size);

    /* Invalid element size */
    if (element_size > buf_size)
        return CBUFFER_STORAGE_BOUNDS_ERROR;

    /* Is popped element too long ? Is next r pointer exceed w pointer ?*/
    if (((self->r < self->w) && (next_r > self->w)) ||
        ((self->r > self->w) && (next_r > self->w) && (next_r < self->r)) )
        return CBUFFER_STORAGE_ERROR;


    /******************************/
    /***** BUFFER DATA UPDATE *****/
    /******************************/

    /* Is buf to read a single continuous block ? */
    /* Yes. */
    if (CBUFFER_MODULO(self->r + element_size) > self->r)
    {
        if (self->cb_backend.read(self, self->r, buf, element_size) != element_size)
            return CBUFFER_STORAGE_READ_ERROR;
    }
    /* No, 2 read operations needed. */
    else
    {
        uint32_t cond1 = self->cb_backend.read(self,
                                              self->r,
                                              buf,
                                              self->cbuffer_size - self->r);
        uint32_t cond2 = self->cbuffer_size - self->r;
        uint32_t cond3 = self->cb_backend.read(self,
                                              0,
                                              &buf[self->cbuffer_size - self->r],
                                              element_size - cond2);
        uint32_t cond4 = element_size - cond2;

        if (((cond1) != (cond2)) || ((cond3) != (cond4)))
            return CBUFFER_STORAGE_READ_ERROR;
    }

    /* Read succeed, update r pointer */
    self->r = next_r;

    return CBUFFER_STORAGE_SUCCESS;
}

cbuffer_storage_err_t cbuffer_storage_peek(cbuffer_storage_t * self,
                                    uint32_t offset,
                                    uint8_t *buf)
{
    /* Invalid offset */
    if (offset > self->cbuffer_size)
        return CBUFFER_STORAGE_BOUNDS_ERROR;

    if(self->cb_backend.read(self, self->r, buf, 1) != 1);
        return CBUFFER_STORAGE_READ_ERROR;

    return CBUFFER_STORAGE_SUCCESS;
}
