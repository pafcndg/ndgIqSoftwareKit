obj-$(CONFIG_BMI160) += bmi160_test.o
obj-$(CONFIG_BME280) += bme280_test.o
obj-$(CONFIG_SS_I2C) += ss_i2c_test.o
obj-$(CONFIG_SS_SPI) += ss_spi_test.o
ifeq ($(CONFIG_SBA),y)
obj-$(CONFIG_SS_I2C) += sba_ss_i2c_tst.o
obj-$(CONFIG_SS_SPI) += sba_ss_spi_tst.o
obj-$(CONFIG_SS_GPIO) += ss_gpio_tst.o
obj-y += sba_function.o
endif
obj-$(CONFIG_SS_ADC) += ss_adc_test.o

obj-y += arc_unit_tests.o