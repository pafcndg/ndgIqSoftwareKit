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

#ifndef _UI_SVC_AUDIO_H
#define _UI_SVC_AUDIO_H

#include <stdint.h>

/**
 * @defgroup ui_service_audio UI Service Audio
 * User Interface service: Audio features.
 * @ingroup ui_service
 * @{
 */

/* Audio domain ID:
 * 0x0: No domain
 * 0x1: Voice Triggers
 * 0x2: Workout
 * 0x3: Music
 * 0x4: Glasses Control
 * 0x5: Phone Control
 * 0x6: Notification
 * ID > 0x01 is in ASR domain.
 */
#define AUDIO_LPAL_DOMAIN     0x1

#define RESP_ID_MASK_SENTENCE 0xFF

#define RESP_ID_MASK_DOMAIN   0x0F

#define RESP_ID_MASK_TYPE     0xF0

#define RESP_ID_MASK_TONE     0xFF

/**
 * Handle audio requests to play tone and/or sentence.
 *
 * @param  msg  Request message.
 * @return none
 */
void ui_handle_audio_req(message_t * msg);


/**
 * Open connection with the Audio service.
 *
 * The function opens the audio service and provides message
 * handler function pointer.
 * @param  queue Queue of messages.
 * @param  service Pointer to ui service.
 * @return none.
 */
void ui_audio_svc_open(void *queue, service_t *service);

/** @} */

#endif
