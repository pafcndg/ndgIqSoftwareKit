obj-$(CONFIG_SERVICES_BLE_IASP) += iasp.o
CFLAGS_iasp.o = -I$(T)/framework/src/services/ble_service/
