obj-$(CONFIG_SENSOR_BUS_COMMON) += sensor_bus_common.o
obj-$(CONFIG_BMI160) += bmi160_gpio.o bmi160_bus.o bmi160_support.o bmi160_drv.o
obj-$(CONFIG_BMM150) += bmm150_support.o bmm150_drv.o
obj-$(CONFIG_APDS9190) += apds9190.o
obj-$(CONFIG_BME280) += bme280.o bme280_support.o bme280_bus.o bme280_drv.o
