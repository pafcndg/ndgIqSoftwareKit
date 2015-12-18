ifeq ($(CONFIG_SBA),y)
obj-$(CONFIG_INTEL_QRK_I2C) += sba_i2c_tst.o
obj-$(CONFIG_INTEL_QRK_SPI) += sba_spi_tst.o
endif
obj-$(CONFIG_LOG_CBUFFER) += cbuffer_test.o
obj-$(CONFIG_WAKELOCK) += wakelock_tst.o
obj-$(CONFIG_LIST) += list_tst.o
obj-$(CONFIG_GPIO_DRIVER_TESTS) += gpio_tst.o
obj-$(CONFIG_SOC_COMPARATOR) += comparator_tst.o
