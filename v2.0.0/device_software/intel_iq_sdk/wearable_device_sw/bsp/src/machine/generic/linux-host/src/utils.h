/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material is
 * protected by worldwide copyright and trade secret laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, modified, published,
 * uploaded, posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.
 *
 * Any license under such intellectual property rights must be express and
 * approved by Intel in writing
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include "util/compiler.h"

#ifndef min
#define min(a, b) ((a)<(b))?(a):(b)
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#define BUILD_BUG_ON_ZERO(e) (sizeof(char[1 - 2 * !!(e)]) - 1)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

#ifndef container_of
// for offsetof
#include <stddef.h>
#define container_of(ptr, type, member) ({\
    (type *)( (char *)(ptr) - offsetof(type,member) );})
#endif

#define die() while(1)

/**@brief Represent an unsigned 8 bit value in ASCII hexadecimal
 *
 * @param[in] in The value to represent
 * @param[inout] p Pointer to the buffer to fill
 *
 * @note The representation requires 2 bytes
 */
void uint8_to_ascii(uint8_t in, uint8_t * p);

/**@brief Represent an unsigned 16 bit value in ASCII decimal
 *
 * @param[in] in The value to represent
 * @param[inout] p Pointer to the buffer to fill
 *
 * @note The representation requires 5 bytes
 */
void dec16_to_ascii(uint16_t in, uint8_t *p);

#endif
