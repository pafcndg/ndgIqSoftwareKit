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

#ifndef HAPTIC_H_
#define HAPTIC_H_

#include <stdint.h>

/**
 * @defgroup common_driver_haptic Haptic Driver API
 * @{
 * @ingroup common_drivers
 */

/**
* Vibration type.
* This type defines the pattern to perform.
*/
typedef enum vibration_type{
    VIBRATION_NONE,
    VIBRATION_SQUARE_X2,
    VIBRATION_SPECIAL_EFFECTS
}vibration_type;

/**
 * Vibration square_x2 pattern defines the way to switch on / switch off a vibrator according:
 * amplitude, time on, time off and repetition.
 * NB: time duration are in milli second.
 */
typedef struct vibration_square_x2_t{
    uint8_t     amplitude;              /*!< Pulse amplitude */
    uint16_t    duration_on_1;          /*!< First pulse duration on state */
    uint16_t    duration_off_1;         /*!< First pulse duration off state */
    uint16_t    duration_on_2;          /*!< Second pulse duration on state */
    uint16_t    duration_off_2;         /*!< Second pulse duration off state */
    uint8_t     repetition_count;
} vibration_square_x2_t;

/**
 * Special effect pattern defines the way to switch on / switch off a vibrator according:
 * effects and time between effect.
 * NB: time duration are in milli second.
 */
typedef struct vibration_special_effect_t{
    uint8_t     effect_1;                   /*!< First effect type */
    uint16_t    duration_off_1;             /*!< Millis off after effect */
    uint8_t     effect_2;                   /*!< Second effect type */
    uint16_t    duration_off_2;             /*!< Millis off after effect */
    uint8_t     effect_3;                   /*!< Third effect type */
    uint16_t    duration_off_3;             /*!< Millis off after effect */
    uint8_t     effect_4;                   /*!< Fourth effect type */
    uint16_t    duration_off_4;             /*!< Millis off after effect */
    uint8_t     effect_5;                   /*!< Fifth effect type */
} vibration_special_effect_t;

/**
 * Groups the vibration pattern available.
 */
typedef union vibration_u{
    vibration_square_x2_t       square_x2;
    vibration_special_effect_t  special_effect;
} vibration_u;

/** Set the callback to return result of played haptic pattern.
 *  pointer to the callback have to be provided to the driver at
 *  init of the service.
 *
 *  @param   evt_callback_func pointer to the callback function
 *  @return  none
*/
void haptic_init(void (*evt_callback_func) (int8_t));

/**
 * Set the haptic driver to a specific pattern.
 *
 * @param   type pattern selected type
 * @param   pattern pattern parameters
 * @return  error status
 */
int8_t haptic_play(vibration_type type, vibration_u *pattern);

/** @} */

#endif /* HAPTIC_H_ */
