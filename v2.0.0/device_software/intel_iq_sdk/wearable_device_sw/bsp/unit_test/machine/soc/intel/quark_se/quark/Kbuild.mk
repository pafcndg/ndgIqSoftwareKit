obj-$(CONFIG_INTEL_QRK_WDT) += wdt_tst.o
obj-$(CONFIG_INTEL_QRK_RTC) += rtc_test.o
obj-$(CONFIG_INTEL_QRK_PWM) += pwm_tst.o
obj-$(CONFIG_SOC_FLASH) += flash_tst.o
obj-$(CONFIG_INTEL_QRK_AON_PT) += aonpt_test.o
ifeq ($(CONFIG_SBA),y)
obj-$(CONFIG_SPI_FLASH) += spi_flash_tst.o
endif
obj-$(CONFIG_LOG_CBUFFER) += logger_test.o
obj-$(CONFIG_PM_PUPDR) += pm_tst.o quark_sleep_tst.o
obj-y += unit_tests.o
