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
#include "util/cunit_test.h"
#include "machine/soc/intel/quark_se/quark/log_backend_uart.h"

void run_unit_tests(void)
{
	cu_set_log_backend(&log_backend_uart);

	CU_RUN_TEST(logger_test);

	CU_RUN_TEST(properties_storage_test);

#if defined(CONFIG_CONSOLE_MANAGER)
	CU_RUN_TEST(console_manager_test);
#endif
	CU_RUN_TEST(aonpt_test);
	CU_RUN_TEST(deep_sleep_test);
#if defined(CONFIG_WDT_NMI) || !defined(CONFIG_INTEL_QRK_WDT)
	CU_TEST_DISABLED(qrk_wdt_test);
#else
	CU_RUN_TEST(qrk_wdt_test);
#endif
	CU_RUN_TEST(rtc_test);
	CU_RUN_TEST(qrk_pwm_test);
#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SOC_GPIO_32)
	CU_RUN_TEST(gpio_test);
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SOC_GPIO_AON)
	CU_RUN_TEST(gpio_aon_test);
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SOC_COMPARATOR)
	CU_RUN_TEST(comparator_test);
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_INTEL_QRK_I2C)
	CU_RUN_TEST(sba_i2c_test);
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_INTEL_QRK_SPI)
	CU_RUN_TEST(sba_spi_test);
#endif

	CU_RUN_TEST(flash_test);

#if defined (CONFIG_SPI_FLASH_MX25U12835F) && defined (CONFIG_INTEL_QRK_SPI) && defined (CONFIG_SPI_FLASH_INTEL_QRK)
	CU_RUN_TEST(spi_flash_test);
#endif

	CU_RUN_TEST(cbuffer_tst);
	CU_RUN_TEST(wakelock_test);
	CU_RUN_TEST(list_test);
	CU_RUN_TEST(cbuffer_storage_test);
}
