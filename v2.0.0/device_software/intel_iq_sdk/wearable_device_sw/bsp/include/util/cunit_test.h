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

#include "infra/log_backend.h"

/**
 * Initialize the cunit test framework.
 *
 * This function needs to be called before running the first test.
 */
void cu_init();

/**
 * Set the log backend to be used by the cu_print function.
 *
 * @param b the log_backend to use for printing test-related message. It is used
 * to by-pass the infra log system which is necessary in some cases. Keep this
 * to NULL to use the default log backend.
 */
void cu_set_log_backend(struct log_backend* b);

/**
 * Print a test report.
 */
void cu_print_report();

/**
 * A printk style function for unit tests.
 */
void cu_print(const char *format, ...);

/* Macros used to unit test the code */
#define CU_ASSERT(message, test) \
do { \
	if (!(test)) { \
		cu_print("CU_FAIL: %s - file:%s:%d\n", message, __FILE__, __LINE__); \
		_cu_fail(); \
	} \
} while (0)

#define CU_RUN_TEST(test) \
do { \
	extern void test(void); \
	cu_print("\n"); \
	cu_print("----------------------\n"); \
	cu_print("%s\n", #test); \
	_cu_runtest(test); \
} while (0)

#define CU_TEST_DISABLED(test) \
do { \
	extern void test(void); \
	cu_print("----------------------\n"); \
	cu_print("%s\n", #test); \
	cu_print("TEST DISABLED\n"); \
	_cu_disabled(test); \
} while (0)

/* Private functions used in macros */
void _cu_fail();
void _cu_disabled();
void _cu_runtest(void (*test_func)(void));
