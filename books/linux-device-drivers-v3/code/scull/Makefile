ifneq ($(KERNELRELEASE),)
	obj-m := scull.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	@echo "please insert install command"

uninstall:
	@echo "please insert uninstall command"
	
clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers
