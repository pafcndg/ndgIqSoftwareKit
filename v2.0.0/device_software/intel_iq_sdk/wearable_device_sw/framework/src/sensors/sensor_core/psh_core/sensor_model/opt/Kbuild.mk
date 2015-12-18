ifeq ($(CONFIG_PVP_TEST),y)
obj-y += atlas_options.o
else
obj-$(CONFIG_BMI160) += atlas_options.o
endif
