ifneq ($(KERNELRELEASE),)
	obj-m := pkchr_fifo.o
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
	rm -rf *.o *.order *.ko *.mod.c *.markers *.symvers .*.o.cmd .*.o.d .*.ko.cmd .tmp_versions
