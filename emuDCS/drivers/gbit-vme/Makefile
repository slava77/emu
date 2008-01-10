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
