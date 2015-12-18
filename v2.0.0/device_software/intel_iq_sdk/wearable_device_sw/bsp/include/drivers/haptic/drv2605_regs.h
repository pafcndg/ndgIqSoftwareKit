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

#ifndef DRV2605_REGS_H_
#define DRV2605_REGS_H_

/**
 * @defgroup common_driver_haptic Haptic Driver API
 * @ingroup common_drivers
 * @{
 */

#define DELAY_I2C_RSP            2000
/* to play 256 patterns (0=>255) */
#define MAX_REPEAT_COUNT         255

#define SPECIAL_EFFECT_5         8

#define SQUARE_T_ON_1            0
#define SQUARE_T_OFF_1           1
#define SQUARE_T_ON_2            2
#define SQUARE_T_OFF_2           3

/* According DRV2605 data sheet */
#define FIRST_LIBRARY_WAVE_NUM   1
#define LAST_LIBRARY_WAVE_NUM    123

/*
* DRV2605
*/
#define DEVICE_ADDR              0x5A  /*I2c device address */
#define STATUS_REG               0x00
#define MODE_REG                 0x01
#define RTPIN_REG                0x02
#define LIBRARY_SELECTION_REG    0x03
#define WAVEQ1_REG               0x04
#define WAVEQ2_REG               0x05
#define WAVEQ3_REG               0x06
#define WAVEQ4_REG               0x07
#define WAVEQ5_REG               0x08
#define WAVEQ6_REG               0x09
#define WAVEQ7_REG               0x0A
#define WAVEQ8_REG               0x0B
#define GO_REG                   0x0C
#define OVERDRIVE_REG            0x0D
#define SUSTAIN_POS_REG          0x0E
#define SUSTAIN_NEG_REG          0x0F
#define BREAK_REG                0x10
#define RATED_VOLTAGE_REG        0x16
#define OD_CLAMP_REG             0x17
#define FEEDBACK_REG             0x1A
#define CONTROL1_REG             0x1B
#define CONTROL2_REG             0x1C
#define CONTROL3_REG             0x1D
#define CONTROL4_REG             0x1E

#define LIBRARY_A                0x01
#define LIBRARY_B                0x02
#define LIBRARY_C                0x03
#define LIBRARY_D                0x04
#define LIBRARY_E                0x05
#define LIBRARY_LRA              0x06

#define AUTO_CALIBRATION         7
#define AUTOCAL_TIME_1000MS      (3 << 4)
#define DEV_ID_MASK              (7 << 5)
#define DRV2605                  (3 << 5)
#define DRV2604                  (4 << 5)
#define STANDBY_MODE             0x40
#define DEFAULT_CTRL_2           0xF5
#define DEFAULT_CTRL_3           0xA0
#define DEFAULT_RATED_VOLTAGE    0x3F
#define ERM_CLOSE_LOOP           0x88
#define UNIDIRECTIONAL_INPUT     0x55
#define RATED_VOLTAGE_STRONGER   0x7F


#define CURRENT_LIBRARY          LIBRARY_D

/** @} */

#endif
