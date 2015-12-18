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
#include <stdlib.h>
#include <stdio.h>
#include "util/compiler.h"

#include "infra/version.h"
#include "infra/tcmd/handler.h"
#include "infra/log.h"

#define TEXT_SIZE       (uint8_t) (22)

/* The content of this struct is overwritten in a post-build script */
const struct version_header version_header
        __section(".version_header") __used
= {
    .magic = {'$', 'B', '!', 'N'},
    .version = 0x01,
    .reserved_1 = {0, 0, 0, 0},
    .reserved_2 = {0, 0, 0, 0},
};

static volatile struct version_header const * const p_version = &version_header;

void version_tcmd(int argc, char **argv, struct tcmd_handler_ctx *ctx)
{
    char buf[TEXT_SIZE];

    snprintf(buf, TEXT_SIZE, "%d.%d.%d", p_version->major, p_version->minor,
		    p_version->patch);
    TCMD_RSP_PROVISIONAL(ctx,buf);

    snprintf(buf, TEXT_SIZE, "%.20s", p_version->version_string);
    TCMD_RSP_PROVISIONAL(ctx,buf);

    snprintf(buf, TEXT_SIZE, "Micro-sha1 : %02x%02x%02x%02x", p_version->hash[0],
		    p_version->hash[1], p_version->hash[2], p_version->hash[3]);
    TCMD_RSP_PROVISIONAL(ctx,buf);

    TCMD_RSP_FINAL(ctx, NULL);
}
DECLARE_TEST_COMMAND(version, get, version_tcmd);
