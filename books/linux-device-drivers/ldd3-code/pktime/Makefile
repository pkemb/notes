ifneq ($(KERNELRELEASE),)
	obj-m := pktime.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	./install.sh

uninstall:
	./uninstall.sh

clean:
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.ko.cmd .tmp_versions
