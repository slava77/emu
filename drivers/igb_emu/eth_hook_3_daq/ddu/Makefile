obj-m	:= eth_hook_3_ddu.o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	rm -f *.h *.c
	ln -s ../schar.h
	ln -s ../eth_hook_3_daq.h
	ln -s ../eth_hook_3_daq.c eth_hook_3_ddu.c
	$(MAKE) EXTRA_CFLAGS="-D_DDU_SPY_" -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -fr .tmp_versions *.o .*.o.d *.ko *.mod *.mod.c *.cmd .*.cmd Module.markers Module.symvers
