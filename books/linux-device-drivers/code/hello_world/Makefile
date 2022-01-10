KO_NAME=pk
MODULE_NAME=hello
# $(KO_NAME)-objs := xxx.o

ifneq ($(KERNELRELEASE),)
# second run
include $(PWD)/../var.mk
else
# first run
include ../target.mk
endif