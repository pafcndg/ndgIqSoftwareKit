# This package contains Intel proprietary sensor code

ifneq ($(CONFIG_SENSOR_CORE),)

ifneq ($(wildcard $(T)/packages/intel/sensors/prebuilt/.),)
lib-y += $(T)/packages/intel/sensors/prebuilt/built-in.a
else
obj-y += $(T)/packages/intel/sensors/
endif

# Export include paths that need to be seen from client code

subdir-cflags-$(CONFIG_SENSOR_CORE_ALGO_STEPCOUNTER_ACTIVITY) += \
	-I$(T)/packages/intel/sensors/algo/activity_algos/include

subdir-cflags-$(CONFIG_SENSOR_CORE) += \
	-I$(T)/packages/intel/sensors/algo/inc_api

endif
