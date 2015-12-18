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


/* Check circular buffer type is valid */
#define IS_CBUFFER_STORAGE_TYPE(CBUFFER_STORAGE_TYPE) \
        (((CBUFFER_STORAGE_TYPE) == CBUFFER_STORAGE_FIXED_ELEMENT_SIZE) || \
        ((CBUFFER_STORAGE_TYPE) == CBUFFER_STORAGE_VARIABLE_ELEMENT_SIZE))


static uint32_t get_fixed_element_size(const cbuffer_storage_t * cbuffer_storage)
{
    return cbuffer_storage->cbuffer_element_size;
}

cbuffer_storage_err_t cbuffer_storage_init(cbuffer_storage_t * self)
{
    /* Check cbuffer_size is power of 2 */
    if (! IS_POWER_OF_TWO(self->cbuffer_size))
        return CBUFFER_STORAGE_BOUNDS_ERROR;

    /* Check r and w pointers are valid */
    if ((self->w > self->cbuffer_size) || (self->r > self->cbuffer_size))
        return CBUFFER_STORAGE_BOUNDS_ERROR;

    /* Check circular buffer type is valid */
    if (! IS_CBUFFER_STORAGE_TYPE(self->cbuffer_type))
        return CBUFFER_STORAGE_ERROR;

    if (self->cbuffer_type == CBUFFER_STORAGE_FIXED_ELEMENT_SIZE)
    {
        if ((self->cbuffer_element_size == 0) ||
            (self->cbuffer_element_size > self->cbuffer_size))
            return CBUFFER_STORAGE_BOUNDS_ERROR;

        self->_get_element_size = (*get_fixed_element_size);
    }

    /* Test if API functions have been overwritten.
     * If not, use default init/push/pop/peek functions
     */
    if (self->_push ==  NULL)
        self->_push = (*cbuffer_storage_push);

    if (self->_pop ==  NULL)
        self->_pop = (*cbuffer_storage_pop);

    if (self->_peek ==  NULL)
        self->_peek = (*cbuffer_storage_peek);

    /* Check if backend functions have been corectly provided user */
    if ((self->cb_backend.write ==  NULL) ||
            (self->cb_backend.read ==  NULL) ||
            (self->_get_element_size ==  NULL))
        return CBUFFER_STORAGE_ERROR;

    /* Circular buffer init done. */
    return CBUFFER_STORAGE_SUCCESS;
}
