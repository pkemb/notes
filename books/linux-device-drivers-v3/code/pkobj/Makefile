ifneq ($(KERNELRELEASE),)
	obj-m := pkobj.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

i:
	insmod ./pkobj.ko

u:
	rmmod pkobj

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.ko.cmd .tmp_versions
