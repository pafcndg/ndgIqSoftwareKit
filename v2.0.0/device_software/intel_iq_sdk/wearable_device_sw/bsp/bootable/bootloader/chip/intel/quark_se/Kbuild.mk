obj-y +=  quark_se.o
obj-y +=  usb_setup.o
obj-y +=  soc_flash.o
obj-y +=  factory_boot.o
obj-$(CONFIG_PANIC_DUMP) += panic_boot.o
obj-y +=  boot_x86.o
obj-$(CONFIG_HARDWARE_CHARGING) += cos.o hardware_charging.o
obj-$(CONFIG_OTA) += ota_partition.o
obj-$(CONFIG_OTA) += crc.o

# CFLAGS_dfu_desc.o += -Wno-error=format -Wno-error -w
obj-$(CONFIG_USB_DFU) += dfu_desc.o

# TODO: REMOVEME
cflags-y += -Wno-error=format
cflags-y += -Wno-error -w

