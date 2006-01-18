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
	emuDCS/drivers/modules1OSUcc\
	emuDCS/drivers/modules2OSUcc\
	emuDCS/drivers/OSUcc_and_DDU
endif

ifeq ($(Set),modules1OSUcc)
Packages= emuDCS/drivers/modules1OSUcc
endif

ifeq ($(Set),modules2OSUcc)
Packages= emuDCS/drivers/modules2OSUcc
endif

ifeq ($(Set),OSUcc_and_DDU)
Packages= emuDCS/drivers/OSUcc_and_DDU
endif

include $(XDAQ_ROOT)/config/Makefile.rules
