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

#include "util/cunit_test.h"
#include "os/os.h"
#include "scss_registers.h"

#include <misc/printk.h>
static void os_printk_puts(const char *s, uint16_t len)
{
    uint16_t i;
    char c[2];

    c[1] = '\0';

    for(i = 0; i < len ; i++) {
        c[0] = s[i];
        printk("%s", c);
    }
}
struct log_backend log_backend_os_printk = { os_printk_puts };

void run_arc_driver_tests(void)
{
    // Disable all interrupts on QRK core
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, INT_SPI_MST_0_MASK) |= ~(QRK_INT_UNMASK_IA);
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, INT_SPI_MST_1_MASK) |= ~(QRK_INT_UNMASK_IA);
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, INT_I2C_MST_0_MASK) |= ~(QRK_INT_UNMASK_IA);
    MMIO_REG_VAL_FROM_BASE(SCSS_REGISTER_BASE, INT_I2C_MST_1_MASK) |= ~(QRK_INT_UNMASK_IA);

    cu_set_log_backend(&log_backend_os_printk);

#if defined (CONFIG_SS_ADC)
    CU_RUN_TEST(adc_test);
#endif

    CU_RUN_TEST(cbuffer_tst);

    /* TODO: drop when KConfig implemented*/
#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_INTEL_QRK_SPI)
    CU_RUN_TEST(sba_spi_test);          //ARC SBA SOC SPI test
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_SPI)
    CU_RUN_TEST(sba_ss_unit_spi);       //ARC SBA SS SPI test
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_INTEL_QRK_I2C)
    CU_RUN_TEST(sba_i2c_test);          //ARC SBA SOC I2C test
#endif

#if defined (CONFIG_BOARD_CURIE_APP) && defined (CONFIG_SBA) && defined (CONFIG_I2C)
    CU_RUN_TEST(sba_ss_unit_i2c);
#endif

#if defined (CONFIG_GPIO_DRIVER_TESTS)
#if defined (CONFIG_SOC_GPIO_32)
    CU_RUN_TEST(gpio_test);
#endif

#if defined (CONFIG_SOC_GPIO_AON)
    CU_RUN_TEST(gpio_aon_test);
#endif

#if defined (CONFIG_SS_GPIO)
    CU_RUN_TEST(gpio_ss_test);
#endif
#endif

#ifdef CONFIG_SOC_COMPARATOR
    CU_RUN_TEST(comparator_test);
#endif

    CU_RUN_TEST(wakelock_test);

#if defined (CONFIG_BMI160)
    CU_RUN_TEST(bmi160_unit_test);
#endif

#if defined (CONFIG_BME280)
    CU_RUN_TEST(bme280_unit_test);
#endif

}
