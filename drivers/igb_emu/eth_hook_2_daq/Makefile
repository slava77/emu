ifneq ($(KERNELRELEASE),)
obj-m	:= eth_hook_2_daq.o

else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -fr .tmp_versions *.o *.ko *.mod *.mod.c *.cmd .*.cmd Module.markers Module.symvers

endif


