ifneq ($(KERNELRELEASE),)
	obj-m := demo_chr_dev.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

test: default

.PHONY: clean cls
clean cls:
	-rm *.ko
	-rm .*.cmd
	-rm *.o
	-rm *.mod.c
	-rm *.order
	-rm *.symvers
	-rm -rf .tmp_versions	
	-rm *~
endif
