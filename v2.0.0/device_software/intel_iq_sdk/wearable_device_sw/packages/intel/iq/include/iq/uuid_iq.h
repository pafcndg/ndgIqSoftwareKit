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

#ifndef __UUID_IQ_H__
#define __UUID_IQ_H__

#define UUID(...) {__VA_ARGS__, 0x00}

#define INTEL_UUID    0x49
#define CORE_UUID     0x43
#define DEVICE_UUID   0x44
#define BODY_UUID     0x42
#define EVENTS_UUID   0x45
#define FIRMWARE_UUID 0x46

/* intel/core/device/datetime */
#define DATETIME_UUID       UUID(INTEL_UUID, CORE_UUID, DEVICE_UUID, 0x44)
/* intel/core/device/factory_reset */
#define FACTORY_RESET_UUID  UUID(INTEL_UUID, CORE_UUID, DEVICE_UUID, 0x46)
/* intel/core/device/name */
#define NAME_UUID           UUID(INTEL_UUID, CORE_UUID, DEVICE_UUID, 0x4E)
/* intel/core/events/user */
#define USER_EVENT_UUID     UUID(INTEL_UUID, CORE_UUID, EVENTS_UUID, 0x55)
/* intel/core/events/system */
#define SYSTEM_EVENT_UUID   UUID(INTEL_UUID, CORE_UUID, EVENTS_UUID, 0x53)
/* intel/core/notification */
#define NOTIFICATION_UUID   UUID(INTEL_UUID, CORE_UUID, 0x4E)
/* intel/core/firmwareupdate */
#define FIRMWAREUPDATE_UUID UUID(INTEL_UUID, CORE_UUID, FIRMWARE_UUID)
/* intel/core/firmwareupdate/state */
#define FIRMWAREUPDATESTATE_UUID UUID(INTEL_UUID, CORE_UUID, FIRMWARE_UUID, 0x53)
/* intel/body/activity */
#define ACTIVITY_UUID       UUID(INTEL_UUID, BODY_UUID, 0x41)
/* intel/body/activity/settings */
#define ACTIVITY_SETTINGS_UUID       UUID(INTEL_UUID, BODY_UUID, 0x41, 0x53)

/* Decoding Failure error message */
#define ERROR_MSG_DECODING_FAILURE_FOR_UUID "Unable to decode message for UUID [%s]"

/* Encoding Failure error message */
#define ERROR_MSG_ENCODING_FAILURE_FOR_UUID "Unable to encode message for UUID [%s]"

#endif
