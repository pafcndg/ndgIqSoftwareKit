obj-y += adc_service_test.o
obj-$(CONFIG_SERVICES_QUARK_SE_GPIO_IMPL) += gpio_service_test.o
obj-y += ui_svc_test.o
obj-$(CONFIG_PWRBTN_GPIO) += pwrbtn_service_test.o

ifeq ($(CONFIG_QUARK_DRIVER_TESTS),y)
obj-y += ll_storage_service_test.o
obj-y += battery_service_test.o
obj-y += properties_service_test.o
endif
obj-$(CONFIG_TOPIC_MANAGER) += itm_test.o
