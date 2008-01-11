##
#
# This is the TriDAS/emu/emuDCS/drivers Project Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

ifeq ($(Set),framework)
Packages=\
	emuDCS/drivers/gbit-vme
endif

ifeq ($(Set),gbit-vme)
Packages= emuDCS/drivers/gbit-vme
endif

include $(XDAQ_ROOT)/config/Makefile.rules
