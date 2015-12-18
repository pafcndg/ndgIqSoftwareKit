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

#ifndef _SENSOR_BIT_CFG_H
#define _SENSOR_BIT_CFG_H

#define SNR_COMN_BIT_CFG_MASK ((u16)0xFF00)
#define SNR_PRIV_BIT_CFG_MASK ((u16)(~SNR_COMN_BIT_CFG_MASK))

/* Sensor common configuration bits */
#define SNR_BIT_CFG_DATA_RAW ((u16)(0x1 << 15))
#define SNR_BIT_CFG_FIFO_ON  ((u16)(0x1 << 14))
#define SNR_BIT_CFG_AS_WAKEUP_SRC  ((u16)(0x1 << 13))
#define SNR_BIT_CFG_BUFF_DELAY_ON  ((u16)(0x1 << 12))

/* ACCEL Private configuration bits */
/* Full scale range settings */
#define ACCEL_BIT_CFG_FS_8G     ((u16)(0x1 << 0))
#define ACCEL_BIT_CFG_FS_16G    ((u16)(0x1 << 1))
/* To set it into Move Detection mode */
#define ACCEL_BIT_CFG_MDECT     ((u16)(0x1 << 2))
#define ACCEL_BIT_CFG_FS_2G     ((u16)(0x1 << 3))
#define ACCEL_BIT_CFG_FS_4G     ((u16)(0x1 << 4))

/* Compass private configuration bits */
#define COMPASS_BIT_CFG_GAIN_AUTO   ((u16)(0))
#define COMPASS_BIT_CFG_GAIN_INC    ((u16)(0x1 << 0))
#define COMPASS_BIT_CFG_GAIN_DEC    ((u16)(0x1 << 1))
#define COMPASS_BIT_CFG_GAIN_MASK   ((u16)(0x3))

#endif
