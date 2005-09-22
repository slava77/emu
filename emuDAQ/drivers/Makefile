##
#
# This is the TriDAS/emu/emuDAQ/drivers Project Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

ifeq ($(Set),framework)
Packages=\
	emuDAQ/drivers/bigphysarea_xxxx
endif

ifeq ($(Set),bigphysarea_xxxx)
Packages= emuDAQ/drivers/bigphysarea_xxxx
endif

include $(XDAQ_ROOT)/config/Makefile.rules
