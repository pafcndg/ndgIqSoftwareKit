obj-$(CONFIG_QUARK_SE_QUARK_INT_STUBS) += int_stubs.o
obj-$(CONFIG_QUARK_SE_PANIC_DEFAULT) += panic_stubs.o panic.o
obj-y += reboot.o
obj-$(CONFIG_QUARK_SE_QUARK_SOC_SETUP) += soc_setup.o
obj-$(CONFIG_DEEPSLEEP) += low_power.o
obj-$(CONFIG_LOG_BACKEND_USB) += log_backend_usb.o
obj-$(CONFIG_QUARK_SE_QUARK_LOG_BACKEND_UART) += log_backend_uart.o
obj-$(CONFIG_TCMD) += uart_tcmd_client.o
obj-$(CONFIG_QUARK_SE_QUARK_SOC_CONFIG) += soc_config.o
obj-$(CONFIG_IPC) += ipc.o
obj-$(CONFIG_LOG) += log.o
obj-$(CONFIG_BSP) += bsp.o
obj-y += boot.o
obj-$(CONFIG_QUARK_SE_PROPERTIES_STORAGE) += properties_storage_soc_flash.o
ifeq ($(CONFIG_PM_PUPDR),y)
obj-y += pm_pupdr.o
obj-y += idle.o
obj-$(CONFIG_TCMD) += pm_pupdr_tcmd.o
endif
