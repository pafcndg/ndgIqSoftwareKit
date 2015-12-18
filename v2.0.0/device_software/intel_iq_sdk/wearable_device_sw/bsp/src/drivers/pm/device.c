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

#include <stddef.h>
#include <errno.h>

#include "infra/device.h"
#include "infra/log.h"
#include "machine.h"

static struct device **all_devices = NULL;
static uint32_t all_devices_count = 0;

void init_devices(struct device **_all_devices, uint32_t _all_devices_count)
{
	if (all_devices != NULL)
		/* Devices already init */
		return;

	/* Link array with root device */
	all_devices = _all_devices;
	all_devices_count = _all_devices_count;

	uint32_t i;
	int ret = 0;

	for (i = 0; i < all_devices_count; ++i)
	{
		struct device* dev = all_devices[i];

		/* Make sure that a device's parent (usually the bus) is initialized
		 * before the device itself */
		if (dev->parent && dev->parent->powerstate != PM_RUNNING)
			panic(66);

		if (dev->driver->init && (ret = dev->driver->init(dev))) {
			dev->powerstate = PM_NOT_INIT;
			pr_error(LOG_MODULE_DRV, "failed to init device %d (%d)", dev->id, ret);
			log_flush();
			panic(66);
		}
		dev->powerstate = PM_RUNNING;
	}
}

static void resume_devices_from_index(uint32_t i)
{
	int ret = 0;
	struct device* dev = NULL;
	for (; i < all_devices_count; ++i)
	{
		dev = all_devices[i];

		pr_debug(LOG_MODULE_DRV, "resume device %d", dev->id);
		if (dev->powerstate <= PM_SHUTDOWN) {
			ret = -EINVAL;
			goto err_resume_device;
		}

		if (dev->powerstate == PM_RUNNING)
			/* Device already running */
			continue;

		/* Make sure that a device's parent (usually the bus) is resumed
		 * before the device itself */
		if (dev->parent && dev->parent->powerstate != PM_RUNNING)
		{
			ret = 66;
			goto err_resume_device;
		}

		if (dev->driver->resume && (ret = dev->driver->resume(dev)))
			goto err_resume_device;

		/* Current device resumed */
		dev->powerstate = PM_RUNNING;
	}

	return;

err_resume_device:
	pr_error(LOG_MODULE_DRV, "failed to resume device %d (%d)",dev->id, ret);
	log_flush();
	panic(66);
}

void resume_devices(void)
{
	resume_devices_from_index(0);
}

int suspend_devices(PM_POWERSTATE state)
{
	int32_t i;
	int ret = 0;

	/* Use the reverse order used for init, i.e. we suspend bus devices first,
	 * then buses, then top level devices */
	for (i = all_devices_count -1; i >= 0 ; --i)
	{
		struct device* dev = all_devices[i];

		// device already suspended
		if (dev->powerstate <= state)
			continue;

		pr_debug(LOG_MODULE_DRV, "suspend dev %d", dev->id);

		if (!dev->driver->suspend) {
			dev->powerstate = state;
			continue;
		}

		ret = dev->driver->suspend(dev, state);
		if (!ret) {
			dev->powerstate = state;
			continue;
		}

		break;
	}

	if (!ret)
		return 0;

	/* Suspend aborted, resume all devices starting from where we had
	 * an issue */
	if (state > PM_SHUTDOWN)
		resume_devices_from_index(i+1);

	return -1;
}
