ifeq ($(CONFIG_INTEL_QRK_WDT),y)
obj-y += intel_qrk_wdt.o
obj-$(CONFIG_TCMD_WDT) += wdt_tcmd.o
endif
