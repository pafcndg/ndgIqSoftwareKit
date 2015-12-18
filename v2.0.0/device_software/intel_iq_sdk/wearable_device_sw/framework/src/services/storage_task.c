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

#include <assert.h>
#include "project_mapping.h"

#include "infra/log.h"
#include "services/properties_service/properties_service_api.h"
#include "services/ll_storage_service/ll_storage_service_api.h"

extern void _cfw_loop(void *);

T_QUEUE storage_queue;

void storage_task (void)
{
    storage_queue = queue_create(10, NULL);
    assert(storage_queue);

#ifdef CONFIG_SERVICES_QUARK_SE_STORAGE_IMPL
    ll_storage_service_init(storage_queue, storage_configuration, NUMBER_OF_PARTITIONS);
    pr_debug(LOG_MODULE_MAIN, "Low Level STORAGE service init done");
#endif

#ifdef CONFIG_SERVICES_QUARK_SE_PROPERTIES_IMPL
    property_service_init(storage_queue);
    pr_debug(LOG_MODULE_MAIN, "Properties service init done");
#endif

    _cfw_loop (storage_queue);
}
