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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "util/cunit_test.h"
#include "infra/log.h"
#include "os/os.h"

static volatile bool test_fail;
static volatile uint32_t nb_tests_disabled;
static volatile uint32_t nb_tests_failed;
static volatile uint32_t nb_tests_passed;
static struct log_backend* backend = NULL;

void cu_init()
{
	nb_tests_disabled = 0;
	nb_tests_failed = 0;
	nb_tests_passed = 0;
}

void cu_set_log_backend(struct log_backend* b)
{
	backend = b;
}

void cu_print_report()
{
	cu_print("======================\n");
	if (nb_tests_failed == 0)
		cu_print("ALL TESTS PASSED.\n");
	else
		cu_print("TEST FAILED.\n");
	cu_print("%d test(s) passed.\n", nb_tests_passed);
	cu_print("%d test(s) failed.\n", nb_tests_failed);
	cu_print("%d test(s) disabled.\n", nb_tests_disabled);
	cu_print("Tests execution complete\n");
}

void cu_print(const char *format, ...)
{
	static char line[128];
	uint32_t saved = interrupt_lock();
	va_list args;
	va_start(args, format);
	int len = vsnprintf(line, sizeof(line), format, args);
	if (len > sizeof(line)-1)
	    len = sizeof(line)-1;
	else if (len <= 0){
	    interrupt_unlock(saved);
	    return;
	}
	va_end(args);
	if (backend)
	{
		backend->put_one_msg(line, len);
		if (line[len-1]=='\n') {
			static const char carriage[] = "\r";
			backend->put_one_msg(carriage, sizeof(carriage) -1);
			};
	}
	else
	{
		if (line[len-1]=='\n')
			line[len-1] = '\0';
		pr_info(LOG_MODULE_CUNIT, line);
	}
	interrupt_unlock(saved);
}

void _cu_fail()
{
	test_fail = true;
}

void _cu_disabled()
{
	nb_tests_disabled++;
}

void _cu_runtest(void (*test_func)(void))
{
	test_fail = false;
	test_func();
	if (test_fail) {
		cu_print("TEST FAILED\n");
		nb_tests_failed++;
	}
	else {
		cu_print("TEST PASSED\n");
		nb_tests_passed++;
	}
}
