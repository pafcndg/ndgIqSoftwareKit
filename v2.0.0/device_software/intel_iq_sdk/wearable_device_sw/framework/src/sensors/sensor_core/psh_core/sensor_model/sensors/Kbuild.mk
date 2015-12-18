ifeq ($(CONFIG_PVP_TEST),y)
obj-y+= accel_phy_drv.o
obj-y+= gyro_phy_drv.o
obj-y += physical/
obj-y += abstract/
else
obj-$(CONFIG_BMI160) += accel_phy_drv.o
obj-$(CONFIG_BMI160) += gyro_phy_drv.o
obj-$(CONFIG_BMI160) += motion_det_drv.o
obj-$(CONFIG_BMI160) += physical/
obj-$(CONFIG_BMI160) += abstract/
endif
