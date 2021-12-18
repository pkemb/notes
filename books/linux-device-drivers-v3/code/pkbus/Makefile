MODULE_NAME=pkbus
obj-m += $(MODULE_NAME).o


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

EXTRA_CFLAGS += -DMODULE_NAME=\"$(MODULE_NAME)\"

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

i:
	insmod ./pkbus.ko

u:
	rmmod pkbus

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.ko.cmd .tmp_versions
