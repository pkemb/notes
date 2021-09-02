ifneq ($(KERNELRELEASE),)
	obj-m := pkchr.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	insmod pkchr.ko

uninstall:
	rmmod pkchr

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers
