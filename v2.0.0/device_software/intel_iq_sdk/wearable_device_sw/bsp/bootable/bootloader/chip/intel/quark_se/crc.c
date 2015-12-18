/*
* Copyright (c) 2014, Intel Corporation
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
*    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "crc.h"
#include <partition.h>
#include <ota.h>

/* CRC16-CCITT: x16 + x12 + x5 + x1 */
unsigned short calc_crc16_ccitt(unsigned char *data, unsigned short len)
{
	unsigned short tmp;
	unsigned short crc;
	int i, j;

	crc = 0xffff;

	for (i = 0; i < len; i++) {
		for (j = 0; j < 8; j++) {
			tmp = ((data[i] << j) & 0x80) ^ ((crc & 0x8000) >> 8);
			crc <<= 1;
			if (tmp)
				crc ^= 0x1021;
		}
	}

	return crc;
}

int check_crc(uint8_t * data, int len)
{
	uint16_t crc;

	crc = calc_crc16_ccitt(data, len - 2);

	if (((uint8_t) (crc & 0xff) == data[len - 2])
	    && ((uint8_t) ((crc & 0xff00) >> 8) == data[len - 1]))
		return 0;
	else
		return -1;
}

#define CRC32_POLYNOMIAL 0xedb88320UL

uint32_t calc_crc32(uint32_t crcinit, const void *buf, size_t size)
{
	int i;
	uint32_t crc, mask;
	const uint8_t *p;
	uint32_t byte = 0;
	p = buf;

	crc = crcinit ^ ~0U;
	while (size--)
	{
		/* check where is the data should be read RAM SOC, FLASH SOC
		SPI FLASH */
		if (((uint32_t)p & (~(CACHE_PARTITION_SIZE-1))) == CACHE_PARTITION_ADRESS ){
			ota_read_bytes((uint32_t)p++, (uint8_t*)&byte, 1);}
		else
			byte = *p++;
		crc = crc ^ byte;
		for (i = 8; i > 0; i--)
		{
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (CRC32_POLYNOMIAL & mask);
		}
	}
	return crc ^ ~0U;
}