# This package contains Intel proprietary Connectivity code

# IASP

# Add path to source code
obj-y += $(T)/packages/intel/connectivity/iasp/src/

# Export include path for client code
subdir-cflags-$(CONFIG_SERVICES_BLE_IASP) += \
	-I$(T)/packages/intel/connectivity/iasp/include/
