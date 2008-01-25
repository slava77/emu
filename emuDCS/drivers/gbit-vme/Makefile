##
#
# This is the TriDAS/emu/emuDCS/drivers/gbit-vme
#
##


XDAQ_BACK_TO_ROOT:=$(XDAQ_ROOT)
include $(XDAQ_BACK_TO_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=emuDCS/drivers/gbit-vme

include $(XDAQ_ROOT)/config/Makefile.rules

_all:
	cd ./dl2k_driver; \
	/bin/sh ./compile; \
	cd ../eth_hook_jtag_simple; \
	/bin/sh ./compile

_cleanall:
	cd ./dl2k_driver; \
	/bin/rm -rf *.o *.ko *mod.c .*.cmd .tmp_versions; \
	cd ../eth_hook_jtag_simple; \
	/bin/rm -rf *.o *.ko *mod.c .*.cmd .tmp_versions

install:
	sudo /usr/bin/install -D ./dl2k_driver/dl2k.ko  /usr/local/lib/modules/`/bin/uname -r`/kernel/drivers/pcrate/dl2k.ko; \
	sudo /usr/bin/install -D ./eth_hook_jtag_simple/eth_hook.ko  /usr/local/lib/modules/`/bin/uname -r`/kernel/drivers/pcrate/eth_hook.ko
	sudo /usr/bin/install -m 755 ./bin/pcratedriver /etc/rc.d/init.d/pcratedriver ;\
	sudo /sbin/chkconfig -add pcratedriver; \
	sudo /sbin/chkconfig pcratedriver on
