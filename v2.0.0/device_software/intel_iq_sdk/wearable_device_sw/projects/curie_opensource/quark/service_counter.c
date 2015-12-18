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

#include "os/os.h"
#include "infra/log.h"
#include "cfw/cfw.h"
#include "cfw/cfw_client.h"
#include "services/services_ids.h"

static cfw_client_t * services_client = NULL;

void service_counter_handler(int service_id, uint16_t* services, int services_sz)
{
	int i;

	for (i = 0; i < services_sz; i++)
		if (services[i] == service_id) {
			if (i == services_sz-1)
				/* It was the last service we were waiting for */
				pr_info(LOG_MODULE_MAIN,
						"Services boot completed");
			else if (i != services_sz-1)
				/* If it's not the last service, then ask to be
				 * notified when the next service in the array
				 * is available */
				cfw_register_svc_available(services_client,
						services[i+1], NULL);
			return;

		}

	/* service wasn't expected */
	pr_warning(LOG_MODULE_MAIN, "Unknown service available: %d", service_id);
}

void service_counter_start(cfw_client_t * client, uint16_t* services)
{
	services_client = client;
	/* Ask to be notified when first service is available */
	cfw_register_svc_available(client, *services, NULL);
}
