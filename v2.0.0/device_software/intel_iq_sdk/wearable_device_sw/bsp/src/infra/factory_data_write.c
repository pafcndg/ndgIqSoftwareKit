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

#include <string.h>
#include <stdio.h>
#include <infra/factory_data.h>
#include "machine.h"
#include <infra/tcmd/handler.h>
#include <os/os.h>
#include "drivers/soc_rom.h"

int valid_factory_header(struct tcmd_handler_ctx *ctx);

void factory_mode(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	unsigned int retlen;
	uint32_t offset;
	uint32_t ret;
	uint32_t field;
	if (argc != 3){
		goto print_help;
	}
	if (strcmp(argv[2], "oem") == 0)
	{
		offset = offsetof(struct factory_data, oem_data.production_mode_oem);
		ret = soc_rom_read(FACTORY_DATA_ROM_ADDR +  (offset & ~0x3), 1, &retlen, &field);
		if (ret)
			goto print_error;

		*((uint8_t*) ((uint32_t)(&field) + (offset & 0x3))) = 0x1;
		ret = soc_rom_write(FACTORY_DATA_ROM_ADDR +  (offset & ~0x3), 1, &retlen, &field);
		if (ret)
			goto print_error;
	}
	else if (strcmp(argv[2], "customer") == 0)
	{
		offset = offsetof(struct factory_data, oem_data.production_mode_customer);
		ret = soc_rom_read(FACTORY_DATA_ROM_ADDR +  (offset & ~0x3), 1, &retlen, &field);
		if (ret)
			goto print_error;

		*((uint8_t*) ((uint32_t)(&field) + (offset & 0x3))) = 0x1;
		ret = soc_rom_write(FACTORY_DATA_ROM_ADDR +  (offset & ~0x3), 1, &retlen, &field);
		if (ret)
			goto print_error;
	}
	else
	{
		goto print_help;
	}
	TCMD_RSP_FINAL(ctx, NULL);
	return;
print_error:
	TCMD_RSP_ERROR(ctx, "write fails");
	return;
print_help:
	TCMD_RSP_ERROR(ctx, "Usage: factory end <oem/customer>");
}

DECLARE_TEST_COMMAND_ENG(factory, end, factory_mode);

void factory_lock(int argc, char *argv[], struct tcmd_handler_ctx *ctx)
{
	uint32_t ret;
	uint32_t field = 0x0;
	unsigned int retlen;
	if (argc != 3){
		goto print_help;
	}

	if (!valid_factory_header(ctx))
		return;

	if (global_factory_data->oem_data.production_mode_oem != 0x1
		|| global_factory_data->oem_data.production_mode_customer != 0x1){
		TCMD_RSP_ERROR(ctx, "permission denied");
		return;
	}

	if (strncmp((char*)global_factory_data->oem_data.uuid, argv[2],
		sizeof(global_factory_data->oem_data.uuid)) != 0){
		TCMD_RSP_ERROR(ctx, "wrong uuid");
		return;
	}

	ret = soc_rom_write(FACTORY_DATA_ROM_ADDR, 1, &retlen, &field);
	if (ret)
		goto print_error;
	TCMD_RSP_FINAL(ctx, NULL);
	return;
print_error:
	TCMD_RSP_ERROR(ctx, "write fails");
	return;
print_help:
	TCMD_RSP_ERROR(ctx, "Usage: factory lock $uuid");
}

DECLARE_TEST_COMMAND_ENG(factory, lock, factory_lock);
