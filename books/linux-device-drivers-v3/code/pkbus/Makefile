MODULE_NAME=pkbus
KO_NAME=pk

ifneq ($(KERNELRELEASE),)

obj-m := $(KO_NAME).o
$(KO_NAME)-objs := $(MODULE_NAME).o pkdevice.o

EXTRA_CFLAGS += -DMODULE_NAME=\"$(MODULE_NAME)\"
EXTRA_CFLAGS += -DDEBUG

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

i:
	insmod ./$(KO_NAME).ko

u:
	rmmod $(KO_NAME).ko

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.ko.cmd .tmp_versions

endif
