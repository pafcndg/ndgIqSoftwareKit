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

#include "ota.h"
#include "mtd/spi_flash.h"
#include "soc_flash.h"
#include "swd/swd.h"
#include "dfu_spi_flash.h"
#include "printk.h"
/*
 * liblzg header for _LZG_CalcChecksum definition
 */
#include "internal.h"

#define SIZE_WBUFF 4
#define SIZE_PATTERN_BUFF 4

extern struct ota_ops ota_ops;

uint32_t LZG_write_soc(unsigned char *address, uint32_t size ,uint32_t *value)
{
    unsigned int retlen;
    soc_flash_write((uint32_t)address, size>>2 , &retlen,value);
    return retlen ;
}

uint32_t LZG_write_swd(unsigned char *address, uint32_t size ,uint32_t *value)
{
    unsigned int ret;
    ret=swd_load_image((uint32_t)address&0xFFFFFFFC, value, size);
    return ret;
}

uint32_t LZG_read_soc(unsigned char *address, uint32_t size ,uint32_t *value)
{
    unsigned int retlen;
    soc_flash_read((uint32_t)address , size>>2, &retlen ,value);
    return retlen ;
}

uint32_t LZG_read_swd(unsigned char *address, uint32_t size ,uint32_t *value)
{
    unsigned int ret;
    ret=swd_dump_image((uint32_t)address&0xFFFFFFFC, value, size);
    return ret;
}


static uint32_t LZG_write(unsigned char *address, uint32_t size ,uint32_t *value)
{
    uint8_t ret =0 ;
#ifdef CONFIG_WATCHDOG
    WATCHDOG_RESET();

#endif
    if (ota_ops.write) {
        ret = ota_ops.write(address, size, value );
    }
    return ret ;
}

static unsigned char LZG_read_byte(const unsigned char *address,
                   unsigned int offset)
{
    unsigned int retlen, len = 1;
    uint8_t data;

#ifdef CONFIG_WATCHDOG
    WATCHDOG_RESET();
#endif
    spi_flash_info_t *spi_flash = get_spi_flash();
    spi_flash_read_byte(spi_flash, (uint32_t) (address + offset), len,
                &retlen, &data);
    return (unsigned char)data;

}

static void LZG_read_pattern(unsigned char *address,uint32_t size , uint32_t *pattern)
{
#ifdef CONFIG_WATCHDOG
    WATCHDOG_RESET();
#endif
    if (ota_ops.read) {
        ota_ops.read(address, size, pattern );
    }
}

/*-- PUBLIC ------------------------------------------------------------------*/
/* This file is part of liblzg and adapted for Quark_se and crb board . Quark_se
   have a internal flash which  can access only to 32 bytes . The compress file
   is located in SPI flash */

/* LUT for decoding the copy length parameter */
  const unsigned char _LZG_LENGTH_DECODE_LUT[32] = {
    2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
    18,19,20,21,22,23,24,25,26,27,28,29,35,48,72,128
};

/* Endian and alignment independent reader for 32-bit integers */
#define _LZG_GetUINT32(in, offs) \
    ((((lzg_uint32_t)LZG_read_byte(in, offs)) << 24) | \
     (((lzg_uint32_t)LZG_read_byte(in, offs+1) << 16) | \
     (((lzg_uint32_t)LZG_read_byte(in, offs+2) << 8) | \
     ((lzg_uint32_t)LZG_read_byte(in, offs+3)))))

/* This macro is used for out-of-bounds checks, to prevent invalid memory
   accesses. */
#ifndef LZG_UNSAFE
# define CHECK_BOUNDS(expr) if (UNLIKELY(!(expr))) return 0
#else
# define CHECK_BOUNDS(expr)
#endif

#define CHECKSUM_OP(ptr,i,a,b) do { \
    a += LZG_read_byte(ptr,i++) ;\
    b += a; \
} while(0)



unsigned int LZG_Decode(const unsigned char *in, lzg_uint32_t insize,
    unsigned char *out, lzg_uint32_t outsize)
{
    unsigned char *src, *inEnd, *dst, *outEnd, *copy ,*pattern,symbol, b, b2,b3;
    unsigned char marker1, marker2, marker3, marker4, method;
    unsigned int roffset=0 , wi=0 ,ri=0;
    unsigned char wbuff[SIZE_WBUFF];
    unsigned char pattern_buff[SIZE_PATTERN_BUFF];
    uint32_t sizebuff,sizepattern;
    lzg_uint32_t  i, length, offset, encodedSize, decodedSize, checksum;
    char isMarkerSymbolLUT[256];
    sizebuff=sizeof(wbuff);
    sizepattern=sizeof(pattern_buff);

    /* Does the input buffer at least contain the header? */
    if (insize < LZG_HEADER_SIZE)
        return 0;

    /* Check magic number */
    if ((LZG_read_byte(in, 0) != 'L') || (LZG_read_byte(in, 1) != 'Z') || (LZG_read_byte(in, 2) != 'G'))
        return 0;

    /* Get & check output buffer size */
    decodedSize = _LZG_GetUINT32(in, 3);
    if (outsize < decodedSize)
        return 0;

    /* Get & check input buffer size */
    encodedSize = _LZG_GetUINT32(in, 7);
        if (encodedSize != (insize - LZG_HEADER_SIZE))
        return 0;
    /* Get & check checksum */
#ifndef LZG_UNSAFE
    checksum = _LZG_GetUINT32(in, 11);
    if (_LZG_CalcChecksum(&in[LZG_HEADER_SIZE], encodedSize) != checksum){
        return 0;}
#endif
    /* Check which method is used */
    method = LZG_read_byte(in, 15);
    if (method > LZG_METHOD_LZG1)
        return 0;
    /* Initialize the byte streams */
    src = (unsigned char *)in;
    inEnd = ((unsigned char *)in) + insize;
    dst = out;
    outEnd = out + outsize;

    /* Skip header information */
    roffset += LZG_HEADER_SIZE;

    /* Plain copy? */
    if (method == LZG_METHOD_COPY)
    {
        if (decodedSize != encodedSize)
            return 0;

        /* Copy 1:1, input buffer to output buffer */
        for (i = decodedSize; i > 0; --i)
            *dst++ = LZG_read_byte(src, roffset++);

        return decodedSize;
    }
    /* Get marker symbols from the input stream */
    CHECK_BOUNDS((src+roffset + 4) <= inEnd);
    marker1 = LZG_read_byte(src, roffset++);
    marker2 = LZG_read_byte(src, roffset++);
    marker3 = LZG_read_byte(src, roffset++);
    marker4 = LZG_read_byte(src, roffset++);

    /* Initialize marker symbol LUT */
    for (i = 0; i < 256; ++i)
        isMarkerSymbolLUT[i] = 0;
    isMarkerSymbolLUT[marker1] = 1;
    isMarkerSymbolLUT[marker2] = 1;
    isMarkerSymbolLUT[marker3] = 1;
    isMarkerSymbolLUT[marker4] = 1;
    /* Main decompression loop */
    while ((src+roffset) < inEnd)
    {
        /* Get the next symbol */

        symbol = LZG_read_byte(src, roffset++);
        /* Marker symbol? */
        if (LIKELY(!isMarkerSymbolLUT[symbol]))
        {
            /* Literal copy */
            wbuff[wi++]= symbol;
            dst++;
            if (wi == sizebuff){
                LZG_write(dst-sizebuff, wi, (uint32_t *)wbuff);
                wi=0;
            }
        }
        else
        {
            CHECK_BOUNDS(src+roffset < inEnd);
            b = LZG_read_byte(src, roffset++);
            if (LIKELY(b))
            {
                /* Decode offset / length parameters */
                if (LIKELY(symbol == marker1))
                {

                    /* Distant copy */
                    CHECK_BOUNDS((src +roffset+ 2) <= inEnd);
                    length = _LZG_LENGTH_DECODE_LUT[b & 0x1f];
                    b2 = LZG_read_byte(src, roffset++);
                    b3 = LZG_read_byte(src, roffset++);
                    offset = (((unsigned int)(b & 0xe0)) << 11) |
                              (((unsigned int)b2) << 8) |
                              (b3);
                    offset += 2056;
                }
                else if (LIKELY(symbol == marker2))
                {

                    /* Medium copy */
                    CHECK_BOUNDS(src+roffset < inEnd);
                    length = _LZG_LENGTH_DECODE_LUT[b & 0x1f];
                    b2 = LZG_read_byte(src, roffset++);
                    offset = (((unsigned int)(b & 0xe0)) << 3) | b2;
                    offset += 8;
                }
                else if (LIKELY(symbol == marker3))
                {
                    /* Short copy */
                    length = (b >> 6) + 3;
                    offset = (b & 0x3f) + 8;
                }
                else
                {
                    /* Near copy (including RLE) */
                    length = _LZG_LENGTH_DECODE_LUT[b & 0x1f];
                    offset = (b >> 5) + 1;
                }
                /* Copy corresponding data from history window */

                copy =dst - offset ;
                pattern =copy ;
                /* search if the pattern is in write buffer */
                if ((((uint32_t)dst& (~(sizebuff-1))) - ((uint32_t)copy& (~(sizebuff-1)))) == 0 ) {
                    pattern = wbuff ;}
                else {
                     LZG_read_pattern(copy, sizepattern, (uint32_t *)pattern_buff);
                     pattern= pattern_buff;}
                /* initialize the read index in the buffer  */
                ri = (uint32_t)copy & (sizepattern-1) ;
                CHECK_BOUNDS((copy >= out) && ((dst + length) <= outEnd));

                for (i = 0; i <= length-1; i++){
                    wbuff[wi++] = pattern[ri++] ;
                    dst++;
                    /* Write the buffer */
                    if (wi == sizebuff){
                        LZG_write(dst-sizebuff, wi, (uint32_t *)wbuff);
                        wi= 0;
                    }
                    /* reload the pattern according the read index(ri),write index(wi) and offset
                    and the current write buffer  */
                    if ( (wi==0 && ri==sizepattern)  || (ri== sizepattern && offset>sizebuff)){
                        ri = 0;
                        copy = copy + sizepattern;
                        LZG_read_pattern(copy, sizepattern, (uint32_t *)pattern_buff);
                        pattern= pattern_buff;
                    }
                    if (wi==0 && offset < sizebuff )
                    {
                        if (((unsigned int)dst- (unsigned int)copy) > sizebuff )
                        {
                            copy = copy +sizepattern;
                        }
                        LZG_read_pattern(copy, sizepattern, (uint32_t *)pattern_buff);
                        pattern= pattern_buff;
                    }
                    if ( (offset <=sizebuff && ri == sizepattern)) {
                        pattern = wbuff ;
                        ri = 0;
                    }
            }
        }

            else
            {
                /* Single occurance of a marker symbol... */
                CHECK_BOUNDS(dst < outEnd);
                wbuff[wi++]= symbol;
                dst++;

                if (wi == sizebuff){
                    LZG_write(dst-sizebuff, wi, (uint32_t *)wbuff);
                    wi= 0;
                }
            }
        }
    }


    /* Did we get the right number of output bytes? */
    if ((unsigned int)(dst - out) != decodedSize)
        return 0;

    /* Return size of decompressed buffer */
    return decodedSize;
}

lzg_uint32_t _LZG_CalcChecksum(const unsigned char *data, lzg_uint32_t size)
{
    unsigned short a = 1, b = 0;
    lzg_uint32_t size8, sizediv8,i=0;
    unsigned char *ptr, *end;

    ptr = (unsigned char*)data;

    /* Loop unrolling (modulo 8) */
    sizediv8 = size / 8;
    size8 = sizediv8 * 8;
    end = (unsigned char*)ptr + size8;
    while (ptr+i < end)
    {
        CHECKSUM_OP(ptr, i, a, b); CHECKSUM_OP(ptr, i, a, b);
        CHECKSUM_OP(ptr, i, a, b); CHECKSUM_OP(ptr, i, a, b);
        CHECKSUM_OP(ptr, i, a, b); CHECKSUM_OP(ptr, i, a, b);
        CHECKSUM_OP(ptr, i, a, b); CHECKSUM_OP(ptr, i, a, b);
    }
    /* Finish up remaining data */
    size -= size8;
    while (size--)
    {
        CHECKSUM_OP(ptr, i, a, b);
    }
    return (((lzg_uint32_t)b) << 16) | a;
}
