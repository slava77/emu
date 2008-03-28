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
	emuDQM/CMSSWLibs \
	emuDQM/Common \
	emuDQM/EmuPlotter \
	emuDQM/EmuMonitor \
	emuDQM/EmuDisplayClient
#	emuDQM/emuMonitor \
	emuDQM/emuDisplayClient \
	emuDQM/dduBinExaminer \
	emuDQM/EmuROOTDisplayServer
endif

ifeq ($(Set),emuMonitor)
Packages= emuDQM/emuMonitor
endif

ifeq ($(Set),emuDisplayClient)
Packages= emuDQM/emuDisplayClient
endif

ifeq ($(Set),Common)
Packages= emuDQM/Common
endif

ifeq ($(Set),CMSSWLibs)
Packages= emuDQM/CMSSWLibs
endif

ifeq ($(Set),dduBinExaminer)
Packages= emuDQM/dduBinExaminer
endif


include $(XDAQ_ROOT)/config/Makefile.rules
