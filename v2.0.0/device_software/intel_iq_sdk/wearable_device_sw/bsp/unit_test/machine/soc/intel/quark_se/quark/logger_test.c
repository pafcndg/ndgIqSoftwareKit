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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "infra/log.h"
#include "machine/soc/intel/quark_se/quark/log_backend_uart.h"
#include "util/cunit_test.h"

#define LOG_HEADER_LEN   (31) /* According to "9+3+8+5 +5 + 1" length */

static const char test_header[] = "*********|***|     LOG| INFO| ";

/* Test backend simply stores the last logged msg in a buffer */
static char testbackend_str[LOG_MAX_MSG_LEN + 50];
static int testbackend_count;
static void testbackend_puts(const char *buffer, uint16_t len){
	testbackend_count+=snprintf(testbackend_str+testbackend_count,
				    sizeof(testbackend_str) - testbackend_count,
				    "%.*s", len, buffer);
}
static void testbackend_reset() {testbackend_count=0;}

static bool testbackend_compare_header(const char* header) {
	int i;
	for (i=0; i<strlen(header); ++i)
	{
		if (!(testbackend_str[i]==header[i] || header[i]=='*'))
			return false;
	}
	return true;
}
static bool testbackend_compare_body(const char* content) {
	int i;
	static const int headersize = sizeof(test_header)-1;
	for (i=headersize; i<headersize+LOG_MAX_MSG_LEN+3 &&
					   i-headersize<= strlen(content); ++i)
	{
		if (testbackend_str[i] == '\r' && testbackend_str[i+1] == '\n' && content[i-headersize]=='\0')
			return true;
		if (content[i-headersize]=='\0')
			return false;
		if (testbackend_str[i] != content[i-headersize])
			return false;
	}
	return true;
}
static struct log_backend testbackend = { testbackend_puts };

/* A special backend which save the fact that it was used */
static bool usedbackend_was_used = false;
static void usedbackend_puts(const char *buffer, uint16_t len) {
	usedbackend_was_used = true;
}
static struct log_backend usedbackend = { usedbackend_puts };

/* A special backend which search for a log content */
static bool searchbackend_found = false;
static char* searchbackend_searchstring = NULL;
static void searchbackend_puts(const char *buffer, uint16_t len) {
	if (strncmp(buffer + LOG_HEADER_LEN -1,
		searchbackend_searchstring,
		strlen(searchbackend_searchstring))==0)
		searchbackend_found = true;
}
static struct log_backend searchbackend = { searchbackend_puts };


static void send_logs(uint8_t level, int nb, const char* str)
{
	int i;
	for (i = 0; i < nb; i++) {
		log_printk(level, LOG_MODULE_LOG, str);
	}
	log_flush();
}

static void fill_test_str(char* str, uint32_t len) {
	char c = 'A';
	int i;
	for (i=0;i<len-1;++i,++c)
	{
		str[i]=c;
		if (c=='z')
			c = 'A'-1;
	}
	str[len-1]='\0';
}

void logger_test(void)
{
	/* This test is valid for the master in multi CPU mode */
#ifndef CONFIG_LOG_MULTI_CPU_SUPPORT
#error Test valid only in multi CPU
#endif

#ifndef CONFIG_LOG_MASTER
#error Test valid only on master
#endif

	int8_t ret;

	cu_print("##############################################################\n");
	cu_print("# Purpose of the logger test :                               #\n");
	cu_print("#     Test that log_printk are added in the circular buffer  #\n");
	cu_print("#     Test that buffer is flushed                            #\n");
	cu_print("#     Test error cases                                       #\n");
	cu_print("##############################################################\n");

	char str[50];
	fill_test_str(str, sizeof(str));

	/* Free the log buffer before the tests */
	log_flush();

	ret = log_set_global_level(LOG_LEVEL_ERROR);
	CU_ASSERT("Error on log_set_global_level", ret == 0);
	CU_ASSERT("Log level badly set", log_get_global_level() == LOG_LEVEL_ERROR);

	/* Following tests should not output anything on backend */
	log_set_backend(usedbackend);

	/* All those should not produce any logs */
	send_logs(LOG_LEVEL_DEBUG, 5, str);
	CU_ASSERT("Log msg not discarded", usedbackend_was_used == false);
	send_logs(LOG_LEVEL_INFO, 5, str);
	CU_ASSERT("Log msg not discarded", usedbackend_was_used == false);
	send_logs(LOG_LEVEL_WARNING, 5, str);
	CU_ASSERT("Log msg not discarded", usedbackend_was_used == false);

	/* Check with saturation */
	send_logs(LOG_LEVEL_WARNING, 100, str);
	CU_ASSERT("Log msg not discarded", usedbackend_was_used == false);

	/* Test that no logs are output when module not enabled */
	CU_ASSERT("Log level change failed", log_set_global_level(LOG_LEVEL_DEBUG) == 0);

	/* Test a simple log msg */
	log_flush();
	log_set_backend(testbackend);
	testbackend_reset();
	send_logs(LOG_LEVEL_INFO, 1, str);
	CU_ASSERT("Unexpected log header", testbackend_compare_header(test_header));
	CU_ASSERT("Unexpected log content", testbackend_compare_body(str));
	testbackend_reset();

	char longstr[LOG_MAX_MSG_LEN+10];
	fill_test_str(longstr, sizeof(longstr));

	/* Test a long log msg, which should be truncated */
	log_flush();
	send_logs(LOG_LEVEL_INFO, 1, longstr);
	longstr[LOG_MAX_MSG_LEN-1]='\0';
	CU_ASSERT("Unexpected log header", testbackend_compare_header(test_header));
	CU_ASSERT("Unexpected log content", testbackend_compare_body(longstr));
	testbackend_reset();

	log_flush();
	searchbackend_searchstring = "-- log saturation --";
	log_set_backend(searchbackend);
	send_logs(LOG_LEVEL_ERROR, 100, str);
	CU_ASSERT("Log should have saturated", searchbackend_found == true);

	log_flush();
	log_set_backend(log_backend_uart);
}
