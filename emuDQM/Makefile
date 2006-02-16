##
#
# This is the TriDAS/emu/emuDAQ Project Makefile
#  (this Makefile will soon become obsolete:
#   only project and package Makefiles should remain)
#
##


include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

ifeq ($(Set),framework)
Packages=\
	emuDQM/dduBinExaminer \
	emuDQM/EmuROOTDisplayServer \
	emuDQM/EmuDisplayServer \
	emuDQM/EmuMonitor 
endif

ifeq ($(Set),dduBinExaminer)
Packages= emuDQM/dduBinExaminer
endif

ifeq ($(Set),GenericRUI)
Packages= emuDQM/EmuROOTDisplayServer
endif

ifeq ($(Set),EmuRUI)
Packages= emuDQM/EmuDisplayServer
endif

ifeq ($(Set),TriggerAdapter)
Packages= emuDQM/EmuMonitor
endif


include $(XDAQ_ROOT)/config/Makefile.rules
