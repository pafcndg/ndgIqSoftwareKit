ifeq ($(CONFIG_INTEL_QRK_PWM),y)
obj-y += intel_qrk_pwm.o
obj-$(CONFIG_TCMD) += pwm_tcmd.o
endif
