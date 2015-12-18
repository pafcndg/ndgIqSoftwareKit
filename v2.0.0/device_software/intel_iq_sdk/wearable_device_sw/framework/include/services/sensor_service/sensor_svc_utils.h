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

#ifndef __SENSOR_SVC_UTILS_H__
#define __SENSOR_SVC_UTILS_H__

#include <stdint.h>

#define GET_SENSOR_TYPE(sensor_handle)  ((((uint32_t)(sensor_handle)) >> 24) & 0xFF)
#define GET_SENSOR_ID(sensor_handle)    (((uint32_t)(sensor_handle)) & 0xFFFFFF)

#define GET_SENSOR_HANDLE(type, id)     (void *)((((type) & 0xFF) << 24) | ((id) & 0xFFFFFF))

#define IS_ON_BOARD_SENSOR(type)  (type > ON_BOARD_SENSOR_TYPE_START && type < ON_BOARD_SENSOR_TYPE_END)
#define IS_ANT_SENSOR(type) (type > ANT_SENSOR_TYPE_START && type < ANT_SENSOR_TYPE_END)
#define IS_BLE_SENSOR(type) (type > BLE_SENSOR_TYPE_START && type < BLE_SENSOR_TYPE_END)

#define GET_CLIENT_HANDLE(p_msg) (((conn_handle_t *)(((struct cfw_message *)p_msg)->conn))->client_handle)

#define svc_foreach_list(p_list) for(; (p_list) != NULL; p_list = (__typeof__(*p_list) *)(((list_t *)(p_list))->next))

uint16_t common_multiple_cal(uint16_t num1, uint16_t num2);

int8_t data_cpy(uint8_t *dst, const uint8_t *src, uint32_t len);
char nibble_to_ascii(uint8_t nibble);
void multiple_byte_to_string(uint8_t *dst, const uint8_t *src, uint8_t byte_nb);
void single_byte_to_string(uint8_t *dst, uint8_t byte);
void two_bytes_to_string(uint8_t *dst, uint16_t bytes);
void four_bytes_to_string(uint8_t *dst, uint32_t bytes);

#endif
