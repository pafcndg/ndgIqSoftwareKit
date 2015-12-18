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
#include "util/cunit_test.h"
#include "infra/properties_storage.h"

void properties_storage_test(void)
{
	cu_print("##############################################################\n");
	cu_print("# Properties_storage test                                    #\n");
	cu_print("##############################################################\n");

	properties_storage_format_all();

	const uint8_t* data = "Random Test Data";
	const uint8_t* data2 = "132456789";
	uint8_t rdata[PROPERTIES_STORAGE_MAX_VALUE_LEN + 50];
	uint16_t readlen;
	properties_storage_status_t ret;

	/* Read from empty storage */
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Key correctly not found", ret == PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR);

	/* Basic read write test, non persistent */
	ret = properties_storage_set(42, data, 8, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 8);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 8) == 0);

	ret = properties_storage_set(66, data, 13, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(66, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	/* Check integrity of previous entry */
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 8);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 8) == 0);

	/* Overwrite value of property 42 */
	ret = properties_storage_set(42, data2, 3, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 3);
	CU_ASSERT("Read content correct", strncmp(data2, rdata, 3) == 0);

	/* Fill the partition with a lot of values so that we switch to a new block */
	for (int i = 0; i < 2048/13 + 15; ++i)
	{
		ret = properties_storage_set(42, data, 13, false);
		CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	}
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	/* Fill the partition even more so that we need to clear a previous block */
	for (int i = 0; i < 3*2048/13 + 15; ++i)
	{
		ret = properties_storage_set(42, data, 13, false);
		CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	}
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	ret = properties_storage_get(66, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	/* Reinit the storage to validate that we correctly end up with the same
	 * items */
	properties_storage_init();
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	ret = properties_storage_get(66, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	properties_storage_format_all();

	uint8_t large_buf[PROPERTIES_STORAGE_MAX_VALUE_LEN + 50];
	for (int i = 0; i < sizeof(large_buf) ; ++i)
		large_buf[i] = i % 50 + 'a';

	ret = properties_storage_set(42, data2, 3, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 3);
	CU_ASSERT("Read content correct", strncmp(data2, rdata, 3) == 0);

	/* Write a property with maximum property size */
	ret = properties_storage_set(42, large_buf, PROPERTIES_STORAGE_MAX_VALUE_LEN, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == PROPERTIES_STORAGE_MAX_VALUE_LEN);
	CU_ASSERT("Read content correct", strncmp(large_buf, rdata, PROPERTIES_STORAGE_MAX_VALUE_LEN) == 0);

	/* Check error when property is larger than maximum property size */
	ret = properties_storage_set(42, large_buf, PROPERTIES_STORAGE_MAX_VALUE_LEN + 1, false);
	CU_ASSERT("Write NOK", ret == PROPERTIES_STORAGE_INVALID_ARG);

	/* Fill a block with only different properties (not deprecated data) */
	for (int i = 0; i < 2048/PROPERTIES_STORAGE_MAX_VALUE_LEN+1; ++i)
	{
		ret = properties_storage_set(i, large_buf, PROPERTIES_STORAGE_MAX_VALUE_LEN, false);
		CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	}

	/* Then make sure the partition is fully written causing block shifts */
	for (int i = 0; i < 3*2048/13 + 15; ++i)
	{
		ret = properties_storage_set(999999, data, 13, false);
		CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	}

	for (int i = 0; i < 2048/PROPERTIES_STORAGE_MAX_VALUE_LEN+1; ++i)
	{
		ret = properties_storage_get(i, rdata, sizeof(rdata), &readlen);
		CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
		CU_ASSERT("Read Length OK", readlen == PROPERTIES_STORAGE_MAX_VALUE_LEN);
		CU_ASSERT("Read content correct", strncmp(large_buf, rdata, PROPERTIES_STORAGE_MAX_VALUE_LEN) == 0);
	}

	/* Test property delete */
	ret = properties_storage_delete(999999);
	CU_ASSERT("Delete OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(999999, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Get NOK", ret == PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR);

	ret = properties_storage_delete(42);
	CU_ASSERT("Delete OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Get NOK", ret == PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR);
	for (int i = 0; i < 2048/PROPERTIES_STORAGE_MAX_VALUE_LEN+1; ++i)
	{
		ret = properties_storage_delete(i);
		CU_ASSERT("Delete OK", ret == PROPERTIES_STORAGE_SUCCESS);
		ret = properties_storage_get(i, rdata, sizeof(rdata), &readlen);
		CU_ASSERT("Get NOK", ret == PROPERTIES_STORAGE_KEY_NOT_FOUND_ERROR);
	}

	/* Re-write a property after deleting many */
	ret = properties_storage_set(42, large_buf, PROPERTIES_STORAGE_MAX_VALUE_LEN, false);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(42, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == PROPERTIES_STORAGE_MAX_VALUE_LEN);
	CU_ASSERT("Read content correct", strncmp(large_buf, rdata, PROPERTIES_STORAGE_MAX_VALUE_LEN) == 0);

	/* Basic read write test, persistent */
	ret = properties_storage_set(142, data, 8, true);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(142, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 8);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 8) == 0);

	ret = properties_storage_set(166, data, 13, true);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(166, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 13);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 13) == 0);

	/* Check integrity of previous entry */
	ret = properties_storage_get(142, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 8);
	CU_ASSERT("Read content correct", strncmp(data, rdata, 8) == 0);

	/* Overwrite value of property 42 */
	ret = properties_storage_set(142, data2, 3, true);
	CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	ret = properties_storage_get(142, rdata, sizeof(rdata), &readlen);
	CU_ASSERT("Read OK", ret == PROPERTIES_STORAGE_SUCCESS);
	CU_ASSERT("Read Length OK", readlen == 3);
	CU_ASSERT("Read content correct", strncmp(data2, rdata, 3) == 0);

	properties_storage_format_all();

	/* Check the behaviour when the maximum number of properties is reached */
	for (int i = 0; i < PROPERTIES_STORAGE_MAX_NB_PROPERTIES; ++i)
	{
		ret = properties_storage_set(i, data, 13, false);
		CU_ASSERT("Write OK", ret == PROPERTIES_STORAGE_SUCCESS);
	}

	ret = properties_storage_set(999999, data, 13, false);
	CU_ASSERT("Write NOK", ret == PROPERTIES_STORAGE_BOUNDS_ERROR);

	/* Format the partition: later unit tests rely on it being not full.. */
	properties_storage_format_all();
}
