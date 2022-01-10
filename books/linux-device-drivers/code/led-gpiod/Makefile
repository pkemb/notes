KO_NAME=led
MODULE_NAME=led-gpiod
# $(KO_NAME)-objs := xxx.o

ifneq ($(KERNELRELEASE),)
# second run
include $(PWD)/../var.mk
else
# first run
include ../target.mk
endif