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

#include <sign.h>
#include <uECC.h>
#include <sha256.h>


#if defined(CONFIG_SECURE_BOOT) || defined(CONFIG_SECURE_UPDATE)
static const uint8_t pk[][uECC_BYTES * 2] = {
	/* boot */
	{
	  0xb5, 0x48, 0xf3, 0x9f, 0x33, 0xf1, 0xd5, 0x70, 0x69, 0x17, 0xcd, 0xd3,
	  0xf9, 0xed, 0x8a, 0x48, 0xf6, 0x12, 0x5c, 0x04, 0xa5, 0x30, 0x3e, 0x04,
	  0xd7, 0x7f, 0x52, 0xcc, 0x97, 0x84, 0x09, 0xe8, 0x89, 0x1d, 0x0d, 0xb5,
	  0x34, 0x02, 0x71, 0xe6, 0x92, 0x0b, 0xeb, 0x93, 0x33, 0xdd, 0x9f, 0x90,
	  0xc2, 0x19, 0xee, 0xa6, 0x64, 0x6e, 0x3a, 0x9a, 0xd4, 0x1a, 0xf8, 0x68,
	  0x36, 0x03, 0xb2, 0x05
	},
	/* ota */
	{
	  0x21, 0x41, 0xb3, 0xc6, 0x74, 0x02, 0xd9, 0x24, 0x50, 0x6a, 0x0a, 0xc7,
	  0xdb, 0x2f, 0x93, 0x32, 0xbe, 0xb3, 0xf2, 0x70, 0xae, 0x1a, 0x80, 0xad,
	  0x8d, 0x3c, 0xc3, 0xd5, 0xce, 0xbe, 0x3a, 0x77, 0xc3, 0x9a, 0x61, 0x58,
	  0x69, 0x6b, 0xcb, 0xa3, 0x1f, 0xbb, 0x36, 0x47, 0x94, 0x13, 0x88, 0x66,
	  0x36, 0xec, 0x9e, 0x6a, 0xac, 0x19, 0xc8, 0xa8, 0x73, 0xb2, 0x41, 0xc3,
	  0xbc, 0xcc, 0x1a, 0x2c
	 },
};

static int verify_binary(const uint8_t *key, const uint32_t key_size,
		   const uint8_t *data, const struct signature_header *sig)
{
	uint8_t *c_hash;

	if (memcmp(sig->magic, "$SIG", sizeof(sig->magic)) != 0)
		return 0;

	c_hash = sha256(data, sig->size, NULL);
	return (uECC_verify(key, c_hash, sig->signature) == 1);
}

int secure_boot(uint32_t address)
{
	struct signature_header *sig = (struct signature_header *)address;

	return verify_binary(pk[0], uECC_BYTES * 2, (const uint8_t *)sig +
				     CONFIG_SIGNATURE_HEADER_SIZE, sig);
}

#else

int secure_boot(uint32_t address)
{
	return true;
}

#endif
