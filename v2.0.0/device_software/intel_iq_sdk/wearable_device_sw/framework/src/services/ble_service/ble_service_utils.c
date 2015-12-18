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
#include "os/os.h"
#include "ble_protocol.h"
#include "services/ble_service/ble_service_gap_api.h"
#include "ble_service_int.h"
#include "ble_service_utils.h"

#if 0
static uint8_t ble_enc_adv_svc16(uint8_t *current_ptr, uint16_t uuid)
{
	*current_ptr++ = 0x03;
	*current_ptr++ = BLE_ADV_TYPE_COMP_16_UUID;
	UINT16_TO_LESTREAM(current_ptr, uuid);
	return 4;
}
#endif

uint8_t ble_enc_adv_flags(uint8_t *current_ptr, uint8_t flag)
{
	*current_ptr++ = 0x02;
	*current_ptr++ = BLE_ADV_TYPE_FLAGS;
	*current_ptr++ = flag;
	return 3;
}

uint8_t ble_enc_adv_appearance(uint8_t *current_ptr, uint16_t appearance)
{
	*current_ptr++ = 0x03;
	*current_ptr++ = BLE_ADV_TYPE_APPEARANCE;
	UINT16_TO_LESTREAM(current_ptr, appearance);
	return 4;
}

uint8_t ble_enc_adv_manuf(uint8_t *current_ptr, uint16_t manuf_id, uint8_t *p_data, uint8_t len)
{
	*current_ptr++ = 3 + len;
	*current_ptr++ = BLE_ADV_TYPE_MANUFACTURER;
	UINT16_TO_LESTREAM(current_ptr, manuf_id);

	if (len) {
		memcpy(current_ptr, p_data, len);
	}

	return 3 + len + 1;
}

uint8_t ble_enc_adv_name(uint8_t *current_ptr, uint8_t adv_data_len, const uint8_t *name, uint8_t len)
{
	uint8_t calculated_len;
	if (adv_data_len + len + 2 <= BLE_MAX_ADV_SIZE) {
		*current_ptr++ = len + 1;
		*current_ptr++ = BLE_ADV_TYPE_COMP_LOCAL_NAME;
		calculated_len = len;
	} else {
		*current_ptr++ = BLE_MAX_ADV_SIZE - adv_data_len - 1;
		*current_ptr++ = BLE_ADV_TYPE_SHORT_LOCAL_NAME;
		calculated_len = BLE_MAX_ADV_SIZE - adv_data_len - 2;
	}
	memcpy(current_ptr, name, calculated_len);
	return calculated_len + 2;
}
void uint8_to_ascii(uint8_t in, uint8_t *p)
{
	uint8_t hi = (in & 0xF0) >> 4;
	uint8_t lo = in & 0x0F;

	if (hi < 0x0A)
		*p = '0' + hi;
	else
		*p = 'A' + (hi - 0x0A);

	p++;

	if (lo < 10)
		*p = '0' + lo;
	else
		*p = 'A' + (lo - 0x0A);
}

void uint8buf_to_ascii(uint8_t * dst, const uint8_t * src, int len)
{
	int i;
	for (i = 0; i < len; ++i) {
		uint8_to_ascii(src[i], dst);
		dst += 2;
	}
}
