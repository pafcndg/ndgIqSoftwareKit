obj-y += main.o
obj-y += crt_init.o
obj-y += startup.o
obj-$(CONFIG_BOARD_CURIE_CRB) += crb.o
