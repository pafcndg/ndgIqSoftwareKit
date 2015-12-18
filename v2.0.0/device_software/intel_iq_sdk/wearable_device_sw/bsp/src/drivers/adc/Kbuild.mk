ifeq ($(CONFIG_SS_ADC),y)
obj-$(CONFIG_SS_ADC) += ss_adc.o
obj-$(CONFIG_TCMD) += adc_tcmd.o
endif

