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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "infra/cbuffer_storage.h"
#include "infra/cbuffer_storage_backend.h"
#include "infra/cbuffer_storage_impl.h"

#include "util/cunit_test.h"

static uint8_t cbuffer_storage_init_tests(void);
static uint8_t cbuffer_storage_init_push_pop_tests(void);
static uint8_t cbuffer_storage_ram_init(cbuffer_storage_t *cbuffer_storage);

/* Circular buffer */
static cbuffer_storage_backend_t cb_backend;
static cbuffer_storage_t cbuffer_storage_ram;

/** INIT TESTS */
    /* Data use cases for cbuffer_storage_init_tests */
uint32_t cbuffer_size[4] = {0, 1, 15, 16};
uint32_t cbuffer_element_size[4] = {0, 1, 3, 17};
    /* Expected result from cbuffer_storage_init_tests */
#define CBUFFER_STORAGE_INIT_TEST_RESULT (0b1111101111111001 << 1)


/** PUSH/POP TESTS */
    /* memset */
#define SAMPLE_CBUFFER_SIZE 16
#define SAMPLE_ELEMENT_SIZE 3
uint8_t cb[SAMPLE_CBUFFER_SIZE] = {0};

    /* Sample of fixed size data struct to be stored in the circular buffer */
typedef struct __packed__{
    uint8_t event_id;
    uint8_t event_val;
    uint8_t event_val2;
} sample_event_t;

sample_event_t sample_event[6] = {
    {0x00, 0xAA, 0xAA},
    {0x01, 0xBB, 0xBB},
    {0x02, 0xCC, 0xCC},
    {0x03, 0xDD, 0xDD},
    {0x04, 0xEE, 0xEE},
    {0x05, 0xFF, 0xFF}
};
    /* Expected result from cbuffer_storage_push_pop_tests */
#define CBUFFER_STORAGE_PUSH_POP_TEST_RESULT (0x05)



/* Backend functions for a RAM+byte cbuffer access */
static uint32_t read(cbuffer_storage_t *cbuffer_storage, uint32_t offset, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    for(i = 0; i < len; i++)
        *(buf + i) = *(cb + offset + i);

    return len;
}


static uint32_t write(cbuffer_storage_t *cbuffer_storage, uint32_t offset, const uint8_t *buf, uint32_t len)
{
    uint32_t i;
    for(i = 0; i < len; i++)
        *(cb + offset + i) = *(buf + i);

    return len;
}


static uint8_t cbuffer_storage_ram_init(cbuffer_storage_t * cbuffer_storage)
{
    return cbuffer_storage_init(cbuffer_storage);
}


/*
 * Tests performed:
 * Initialization with (buffer_size, element_size) couples.
 *  Many of them are incorrect and have to failed.
 * Push and pop on a valid intialized circular buffer.
 *
 * Tests passed condition:
 * Tests results match to the already known expected result defined at compile time
 */
void cbuffer_storage_test(void)
{
    CU_ASSERT("cbuffer_storage_test",
        ! (cbuffer_storage_init_tests() || cbuffer_storage_init_push_pop_tests()));
}

static uint8_t cbuffer_storage_init_tests(void)
{
    uint8_t i,j;
    cbuffer_storage_err_t err;
    uint32_t init_test_results = 0;

    /* cbuffer common init */
    cbuffer_storage_ram.cbuffer_type = CBUFFER_STORAGE_FIXED_ELEMENT_SIZE;

    cb_backend.read = (*read);
    cb_backend.write = (*write);
    cbuffer_storage_ram.cb_backend = cb_backend;

    /* Circular buffer init tests */
    for (i=0; i<(sizeof(cbuffer_size)/sizeof(cbuffer_size[0])); i++)
        for (j=0; j<sizeof(cbuffer_element_size)/sizeof(cbuffer_element_size[0]); j++)
        {
            cbuffer_storage_ram.r = 0;
            cbuffer_storage_ram.w = 0;
            cbuffer_storage_ram.cbuffer_size = cbuffer_size[i];
            cbuffer_storage_ram.cbuffer_element_size = cbuffer_element_size[j];

            err = cbuffer_storage_ram_init(&cbuffer_storage_ram);
            if (err)
                init_test_results |= 0x01;
            init_test_results <<= 1;
        }

    /* Only (1,1), (3,1), (3,2) couples should lead to cbuffer init success */
    if (init_test_results != CBUFFER_STORAGE_INIT_TEST_RESULT)
        return CBUFFER_STORAGE_ERROR;

    return CBUFFER_STORAGE_SUCCESS;
}

static uint8_t cbuffer_storage_init_push_pop_tests(void)
{
    uint8_t i;
    cbuffer_storage_err_t err;
    uint8_t buf[SAMPLE_ELEMENT_SIZE];

    /* Circular buffer push/pop tests on a valid circular buffer*/
    cbuffer_storage_ram.cbuffer_type = CBUFFER_STORAGE_FIXED_ELEMENT_SIZE;
    cb_backend.read = (*read);
    cb_backend.write = (*write);
    cbuffer_storage_ram.cb_backend = cb_backend;
    cbuffer_storage_ram.r = 0;
    cbuffer_storage_ram.w = 0;
    cbuffer_storage_ram.cbuffer_size = SAMPLE_CBUFFER_SIZE;
    cbuffer_storage_ram.cbuffer_element_size = SAMPLE_ELEMENT_SIZE;

    err = cbuffer_storage_ram_init(&cbuffer_storage_ram);
    if (err)
        return CBUFFER_STORAGE_ERROR;

    /* Fill the buffer pushing many element and saturate the buffer*/
    for (i = 0; i < 6; i++)
    {
        err = cbuffer_storage_push(&cbuffer_storage_ram, (uint8_t *) (sample_event + (i % SAMPLE_CBUFFER_SIZE)), sizeof(sample_event[0]));
        if (err)
            return CBUFFER_STORAGE_ERROR;

        i = i % SAMPLE_CBUFFER_SIZE;
    }

    /* Empty the buffer popping all elements */
    for (i = 0; i < 5; i++)
    {
        err = cbuffer_storage_pop(&cbuffer_storage_ram, buf, sizeof(buf));
        if (err)
            return CBUFFER_STORAGE_ERROR;
    }

    /* Push again a element */
    err = cbuffer_storage_push(&cbuffer_storage_ram, (uint8_t *) (sample_event + 0), sizeof(sample_event[0]));
    if (err)
        return CBUFFER_STORAGE_ERROR;

    /* Pop the oldest element (which have just been pushed) */
    err = cbuffer_storage_pop(&cbuffer_storage_ram, buf, sizeof(buf));
    if (err)
        return CBUFFER_STORAGE_ERROR;

    /* Test r and w pointer value with the expected value */
    if((cbuffer_storage_ram.r != CBUFFER_STORAGE_PUSH_POP_TEST_RESULT) ||
       (cbuffer_storage_ram.w != CBUFFER_STORAGE_PUSH_POP_TEST_RESULT))
       return CBUFFER_STORAGE_ERROR;

    return CBUFFER_STORAGE_SUCCESS;
}
