obj-y += engine.o
obj-$(CONFIG_TCMD_ASYNC) += async.o
obj-$(CONFIG_TCMD_MASTER) += master.o
obj-$(CONFIG_TCMD_SLAVE) += slave.o
obj-$(CONFIG_TCMD_CONSOLE) += console.o
obj-$(CONFIG_TCMD_CONSOLE_USB_ACM) += acm_tcmd_client.o
