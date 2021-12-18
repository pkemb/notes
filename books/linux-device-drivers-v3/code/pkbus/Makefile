ifneq ($(KERNELRELEASE),)
	obj-m := pkbus.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

i:
	insmod ./pkbus.ko

u:
	rmmod pkbus

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.ko.cmd .tmp_versions
