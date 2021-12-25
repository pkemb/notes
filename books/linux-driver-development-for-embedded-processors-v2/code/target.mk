# first run, in current dir
KERNELDIR ?= $(HOME)/linux-rpi-4.19.y
PWD := $(shell pwd)

USER=root
HOST=pi3b.inc

build:
	$(MAKE) -C $(KERNELDIR) \
		ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		M=$(PWD) modules

deploy:
	scp $(KO_NAME).ko $(USER)@$(HOST):/root

i: deploy
	@echo "install $(KO_NAME).ko"
	@ssh $(USER)@$(HOST) "echo 8 > /proc/sys/kernel/printk"
	@ssh $(USER)@$(HOST) "insmod /root/$(KO_NAME).ko"

u:
	@echo "uninstall $(KO_NAME).ko"
	@ssh $(USER)@$(HOST) "rmmod $(KO_NAME).ko"

clean:
	$(MAKE) -C $(KERNELDIR) \
		ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
		M=$(PWD) clean
