obj-$(CONFIG_UI_SERVICE_IMPL) += ui_svc.o
obj-$(CONFIG_UI_SERVICE_IMPL) += ui_svc_ctrl.o
ifeq ($(CONFIG_UI_SERVICE_IMPL),y)
obj-$(CONFIG_TCMD_UI_SVC) += ui_svc_tcmd.o
endif
obj-$(CONFIG_UI_SERVICE) += ui_svc_api.o
