obj-y += board/
obj-y += chip/
obj-y += common/
obj-y += core/
obj-y += drivers/
# obj-y += tools/

obj-$(CONFIG_COMP_LZG) += $(T)/external/liblzg/src/lib/
subdir-cflags-$(CONFIG_COMP_LZG) += -I$(T)/external/liblzg/src/include/ \
-I$(T)/external/liblzg/src/lib

obj-$(CONFIG_CRYPTO_ECDSA) += $(T)/external/micro-ecc/
subdir-cflags-$(CONFIG_CRYPTO_ECDSA) += -I$(T)/external/micro-ecc/

obj-$(CONFIG_CRYPTO_SHA256) += $(T)/external/sha256/
subdir-cflags-$(CONFIG_CRYPTO_SHA256) += -I$(T)/external/sha256/

# Allow extending the build tree by specifying a path where a Kbuild.mk is expected
ifneq (${CONFIG_BOARD_DIRECTORY},)
obj-y += $(subst ",,${CONFIG_BOARD_DIRECTORY})
endif

