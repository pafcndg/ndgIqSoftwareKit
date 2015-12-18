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

#include "swd/swd_tst.h"
#include "printk.h"

static void swd_tst_erase(void);
static void swd_tst_load_ble_core_binary(uint32_t* const src);
static void swd_tst_verify_ble_core_binary(uint32_t* const src);

void swd_tst(void)
{
    uint32_t flashing_time = 0;
    uint32_t* src = (uint32_t*) 0x4005C000;

/*         *
 * Pin mux *
 *         */

    SET_PIN_MODE(NRF_SWCLK_PIN, QRK_PMUX_SEL_MODEA);
    SET_PIN_MODE(NRF_SWDIO_PIN, QRK_PMUX_SEL_MODEA);

/*          *
 * SwD test *
 *          */

    swd_init();
    swd_tst_erase();
    swd_tst_load_ble_core_binary(src);
    swd_tst_verify_ble_core_binary(src);
    swd_page_erase(6);
    pr_err("Erase finished \n");
    swd_run_target();

}

void swd_tst_erase(void)
{
    pr_err("\n\nErase image in ble_core: \n");
    if (swd_erase_all() == SWD_ERROR_OK) {
        pr_err("Erase OK \n");
    } else {
        pr_err("Erase failed \n");
    }
}

void swd_tst_load_ble_core_binary(uint32_t* const src)
{
    //load binary ble_core test in ble_core
    pr_err("\n\nLoad image to ble_core : \n");
    if (swd_load_image(0, src, BLE_CORE_BIN_SIZE) == SWD_ERROR_OK) {
        pr_err("load bin successful OK \n");
    } else {
        pr_err("load bin failed \n");
    }
}

void swd_tst_verify_ble_core_binary(uint32_t* const src)
{
    //verify binary ble_core test flashed
    pr_err("\n\nVerify image\n");
    if (swd_verify_image(0, src, BLE_CORE_BIN_SIZE) == SWD_ERROR_OK) {
        pr_err("Verify bin successful OK \n");
    } else {
        pr_err("Verify bin failed \n");
    }
}
