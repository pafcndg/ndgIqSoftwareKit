ifdef CONFIG_SERVICES_SENSOR_IMPL
obj-y += sensor_svc.o
obj-y += sensor_svc_list.o
obj-y += sensor_svc_utils.o
obj-y+= sensor_svc_calibration.o
obj-y += quark_se_platform/
endif
obj-$(CONFIG_SERVICES_SENSOR) += sensor_svc_api.o
ifeq ($(CONFIG_TCMD),y)
obj-$(CONFIG_SERVICES_SENSOR) += ss_tcmd_client.o
endif
