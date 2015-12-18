# This package contains Intel proprietary Connectivity code

# ISPP

# Add path to source code
obj-y += $(T)/packages/intel/connectivity/ispp/src/

# Export include path for client code
subdir-cflags-$(CONFIG_SERVICES_BLE_ISPP) += \
	-I$(T)/packages/intel/connectivity/ispp/include/
