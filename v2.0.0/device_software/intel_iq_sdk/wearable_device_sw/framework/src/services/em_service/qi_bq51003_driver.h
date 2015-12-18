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

#ifndef QI_DRIVER_H_
#define QI_DRIVER_H_
#include "os/os_types.h"

/* Define bit mask */
#define CH_BIT_MASK_32		(uint32_t)(0x1)

/* Define output of GPIO on QRK */
/* GPIO[24] LOW = charger enable / HIGH = charger disable*/
#define SOC_GPIO_EN			(uint8_t)(24)

/* Define input of GPIO on AON */
/* GPIO_AON[3] LOW = QI plugged / HIGH = QI unplugged*/
#define AON_GPIO_QI_STATUS	(uint8_t)(3)


/**@brief Function to initialize qi driver
 * @param[in]  cb callback called when init done
 * @return   true if function success, false if function failed.
 */
bool qi_init(T_QUEUE parent_queue,void* call_back);

/**@brief Function to enable charging.
 */
void qi_enable(void);

/**@brief Function to disable charging.
 */
void qi_disable(void);

/**@brief Function to enable maintenance.
 */
void qi_maintenance_enable(void);

/**@brief Function to enable maintenance.
 */
void qi_maintenance_disable(void);

#endif /* QI_DRIVER_H_ */
