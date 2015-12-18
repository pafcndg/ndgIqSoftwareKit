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

#include "drivers/usb_api.h"
#include "drivers/usb_acm.h"
#include "machine/soc/intel/quark_se/quark/log_backend_usb.h"
#include "machine.h"
#include "infra/console_backend_usb_acm.h"
#include "infra/log.h"

static volatile int usb_line_state = 0;
static bool usb_backend_allow = false;
static bool is_usb_acm_backend_ready = false;

static void callback_usb_connect(int event, int param)
{
	if (event == ACM_SET_CONTROL_LINE_STATE) {
		usb_line_state = param;
		pr_info(LOG_MODULE_USB, "USB line state changed %d", usb_line_state);
		/* check if connected to ttyACM* end point */
		if (usb_line_state & 0x1) {
			is_usb_acm_backend_ready = true;
#ifdef CONFIG_TCMD_CONSOLE_USB_ACM
			extern void acm_tcmd_trigger_read(void);
			acm_tcmd_trigger_read();
#endif
		} else if (usb_backend_allow == true) {
			is_usb_acm_backend_ready = false;
			release_usb_backend_xfer();
		} else {
			is_usb_acm_backend_ready = false;
		}
		return;
	}
#ifdef CONFIG_TCMD_CONSOLE_USB_ACM
	if (event == ACM_EVENT_CONNECTED) {
		acm_set_comm_state(0, 0x3f);
	}
#endif
	/* Release ACM semaphore on USB disconnection */
	if (usb_backend_allow == true) {
		if (event == ACM_EVENT_DISCONNECTED && is_usb_acm_backend_ready == true) {
			is_usb_acm_backend_ready = false;
			release_usb_backend_xfer();
		}
	}
}

static void console_backend_set_usb_acm(void)
{
	usb_backend_allow = true;
	log_set_backend(log_backend_usb);
}

static void console_backend_unset_usb_acm(void)
{
	usb_backend_allow = false;
}

bool is_console_backend_usb_acm_ready(void)
{
	return is_usb_acm_backend_ready;
}

void console_backend_usb_acm_init(void)
{
	usb_register_function_driver(usb_acm_class_init, NULL);
	usb_driver_init(SOC_USB_BASE_ADDR);
	acm_init(0, callback_usb_connect);
}

const console_backend_t console_backend_usb_acm = {
	.name = "usb",
	.init = console_backend_usb_acm_init,
	.set_as_log_backend = console_backend_set_usb_acm,
	.unset_as_log_backend = console_backend_unset_usb_acm
};
