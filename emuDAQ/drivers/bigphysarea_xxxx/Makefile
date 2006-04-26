##
#
# This is the TriDAS/emu/emuDAQ/drivers/bigphysarea_xxxx 
#
##


XDAQ_BACK_TO_ROOT:=$(XDAQ_ROOT)
include $(XDAQ_BACK_TO_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
 
Project=emu
Package=emuDAQ/drivers/bigphysarea_xxxx
  
include $(XDAQ_ROOT)/config/Makefile.rules
 
_buildall:
	cd d550; make
	cd eth_hook_mmap_v0_2; make
	install -D d550/dl2k.o lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/dl2k.o
	install -D eth_hook_mmap_v0_2/eth_hook_2.o lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/eth_hook_2.o
	install -D eth_hook_mmap_v0_3/eth_hook_3.o lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/eth_hook_3.o
                                                                                                                  
_cleanall:
	cd d550; make clean
	cd eth_hook_mmap_v0_2; make clean
	cd eth_hook_mmap_v0_3; make clean
	-rm -rf lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
