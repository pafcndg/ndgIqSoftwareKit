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

#ifndef BUTTON_GPIO_SVC_H
#define BUTTON_GPIO_SVC_H

#include <stdint.h>
#include <stdbool.h>
#include "cfw/cfw.h"
#include "lib/button/button.h"

/**
 * @defgroup button_impl_gpio_svc UI button implementation for gpio service
 * @{
 * @ingroup button
 */

/**
 * GPIO service wrapper priv structure for button module
 */
struct button_gpio_svc_priv {
	uint8_t              gpio_service_id;        /*!< the gpio service to open */
	uint8_t              pin;                    /*!< the gpio pin listened */
	bool                 active_low;             /*!< button polarity (active low if true) */
	/* Internal fields */
	cfw_service_conn_t *gpio_service_conn; /*!< gpio service handler */
};

/**
 * Init GPIO svc button.
 * @param  button button instance to use
 * @return none
 */
int button_gpio_svc_init(struct button *button);

/**
 * Stop GPIO svc button.
 * @param  button button instance to use
 * @return none
 */
void button_gpio_svc_shutdown(struct button *button);

/** @} */
#endif /* BUTTON_GPIO_SVC_H */
