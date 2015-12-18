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

#include <string.h>

#include "util/cbuffer.h"
#include "util/cunit_test.h"

#define MSG_SIZE 10
#define NB_MSG_TST 10
#define MSG_SIZE_WRAP 200
#define NB_MSG_TST_WRAP (CONFIG_LOG_CBUFFER_SIZE/MSG_SIZE_WRAP+1) // Number of messages the buffer can contain +1

static uint8_t logbuf_test[CONFIG_LOG_CBUFFER_SIZE];
static cbuffer_t cbuffer_test = {.buf = logbuf_test, .buf_size=CONFIG_LOG_CBUFFER_SIZE};

static void cbuffer_addition_messages_tst(void);
static void cbuffer_wrap_tst(void);


static void cbuffer_addition_messages_tst(void) {
    uint8_t i;
    uint8_t msg_write[MSG_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t msg_read[MSG_SIZE];
    uint32_t rc;
    int32_t ret;
    int start_r = 0;   // Start of the next message index

    /* We write NB_MSG_TST messages in the cbuffer */
    for( i = 0; i < NB_MSG_TST; i++) {
        ret = cb_push(&cbuffer_test, msg_write, MSG_SIZE);
        CU_ASSERT("Wrong return of cb_push", (ret == 0));
    }

    /* We read the cbuffer and compare if write and read values are identical */
    for( i = 0; i < NB_MSG_TST; i++) {

        start_r = cb_find(0, &cbuffer_test, cbuffer_test.r, cbuffer_test.w, MSG_SIZE);
        CU_ASSERT("No message found", start_r >= 0);

        cb_pop(&cbuffer_test, start_r, msg_read, MSG_SIZE);
        rc = memcmp(msg_read, msg_write, MSG_SIZE);
        CU_ASSERT("Read message different than write message", (rc == 0));
    }
}


static void cbuffer_wrap_tst(void) {
    uint8_t i;
    uint8_t msg_write[MSG_SIZE_WRAP];
    uint8_t msg_read[MSG_SIZE_WRAP];
    uint32_t rc;
    int32_t ret;
    int start_r = 0;   // Start of the next message index

    /* We initialise msg_write */
    for( i = 0; i < MSG_SIZE_WRAP; i++) {
        msg_write[i] = i;
    }

    /* We write NB_MSG_TST_WRAP messages in the cbuffer */
    for( i = 0; i < NB_MSG_TST_WRAP-1; i++) {
        ret = cb_push(&cbuffer_test, msg_write, MSG_SIZE_WRAP);
        CU_ASSERT("Wrong return of cb_push", (ret == 0));
    }
    ret = cb_push(&cbuffer_test, msg_write, MSG_SIZE_WRAP);
    CU_ASSERT("Wrong return of cb_push", (ret == 0));
    CU_ASSERT("Wrong saturation state", (cbuffer_test.saturation_flag == 1));

    /* We read the cbuffer and compare if write and read values are identical. */
    /* We wrote more messages than the cbuffer can contain, so we just compare */
    /* the firsts elements in the buffer to verify that the write continue at the */
    /* beggining of the buffer when reached the end */
    for( i = 0; i < NB_MSG_TST_WRAP-1; i++) {

        start_r = cb_find(0, &cbuffer_test, cbuffer_test.r, cbuffer_test.w, MSG_SIZE_WRAP);
        CU_ASSERT("No message found", start_r >= 0);

        cb_pop(&cbuffer_test, start_r, msg_read, MSG_SIZE_WRAP);
        rc = memcmp(msg_read, msg_write, MSG_SIZE_WRAP);
        CU_ASSERT("Read message different than write message", (rc == 0));
    }
}


void cbuffer_tst(void) {
    int32_t ret;

    cu_print("########################################################\n");
    cu_print("# Purpose of Circular Buffer tests :                   #\n");
    cu_print("#            Addition of messages in the buffer        #\n");
    cu_print("#            Addition of messages to wrap the buffer   #\n");
    cu_print("########################################################\n");

    ret = cb_init(&cbuffer_test);
    CU_ASSERT("Buffer size should be power of two", (ret == 0));

    cbuffer_addition_messages_tst();
    cbuffer_wrap_tst();
}
