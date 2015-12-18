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

#ifndef __BOOT_H__
#define __BOOT_H__

/**
 * @defgroup infra_boot Bootloader
 * Defines bootloader API for the application.
 * @ingroup infra
 * @{
 */

/**
 * List of all boot targets.
 */
enum boot_targets {
        TARGET_MAIN = 0x0,
        TARGET_CHARGING,
        TARGET_WIRELESS_CHARGING,
        TARGET_RECOVERY,
        TARGET_FLASHING,
        TARGET_FACTORY,
        TARGET_OTA,
        TARGET_DTM,
        TARGET_CERTIFICATION,
        TARGET_RESERVED_0,
        TARGET_APP_1,
        TARGET_APP_2,
        TARGET_RESERVED_1,
        TARGET_RESERVED_2,
        TARGET_RESERVED_3,
        TARGET_RESERVED_4
};

/*
 * main_task is the end user application entry point. It should be implemented
 * by the user.
 */
void main_task(void *param);

/*
 * recovery is the recovery application.
 */
void recovery(void *param);

/*
 * Retrieve current boot target information.
 *
 * @return boot_targets boot target information
 */
enum boot_targets get_boot_target(void);

/** @} */

#endif /* __BOOT_H__ */
