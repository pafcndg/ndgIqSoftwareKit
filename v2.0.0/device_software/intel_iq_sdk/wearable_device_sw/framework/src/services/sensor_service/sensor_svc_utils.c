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
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif
uint16_t common_multiple_cal(uint16_t num1, uint16_t num2)
{
    if (num1 == 0) {
        return num2;
    }
    if (num2 == 0) {
        return num1;
    }
    uint16_t tmp1, tmp2;

    tmp2 = num1 * num2;
    /* To ensure num1 is greater than or equal to num2 */
    if (num1 < num2) {
        tmp1 = num1;
        num1 = num2;
        num2 = tmp1;
    }
    while(num2 != 0) {
        tmp1 = num1 % num2;
        num1 = num2;
        num2 = tmp1;
    }
    return (tmp2/num1);
}

int8_t data_cpy(uint8_t *dst, const uint8_t *src, uint32_t len)
{
    if (dst == (void *)0) {
        return -1;
    }
    int i;

    for(i = 0; i < len; i++) {
        *dst++ = *src++;
    }
    return 0;
}

char nibble_to_ascii(uint8_t nibble)
{
    nibble &= 0xF;

    if(nibble < 10) {
        nibble += '0';
    } else {
        nibble += 55;
    }
    return nibble;
}

/* dst space is 2 * byte_nb + 1 at least */
void multiple_byte_to_string(uint8_t *dst, const uint8_t *src, uint8_t byte_nb)
{
    uint8_t i;

    for(i = 0; i < byte_nb; i++) {
        *dst++ = nibble_to_ascii(*src >> 4);
        *dst++ = nibble_to_ascii(*src);
        src++;
    }
    *dst = '\0';
}

void single_byte_to_string(uint8_t *dst, uint8_t byte)
{
    multiple_byte_to_string(dst, &byte, 1);
}
#ifdef LITTLE_ENDIAN
static uint16_t swap_uint16(uint16_t value)
{
    return (((value & 0xFF) << 8) |
            ((value & 0xFF00) >> 8));
}

static uint32_t swap_uint32(uint32_t value)
{
    return (((value & 0xFF) << 24)    |
            ((value & 0xFF00) << 8)   |
            ((value & 0xFF0000) >> 8) |
            ((value & 0xFF000000) >> 24));
}
#endif
void two_bytes_to_string(uint8_t *dst, uint16_t bytes)
{
#ifdef LITTLE_ENDIAN
    bytes = swap_uint16(bytes);
#endif
    multiple_byte_to_string(dst, (uint8_t *)&bytes, 2);
}

void four_bytes_to_string(uint8_t *dst, uint32_t bytes)
{
#ifdef LITTLE_ENDIAN
    bytes = swap_uint32(bytes);
#endif
    multiple_byte_to_string(dst, (uint8_t *)&bytes, 4);
}
