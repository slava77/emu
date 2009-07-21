##
#
# This is the TriDAS/emu/emuDCS Project Makefile
#  (this Makefile will soon become obsolete:
#   only project and package Makefiles should remain)
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

ifeq ($(Set),framework)
Packages=\
	emuDCS/PeripheralCore \
	emuDCS/PeripheralApps \
        emuDCS/PeripheralXtop \
        emuDCS/PCSwitches \
	emuDCS/OnlineDB \
	emuDCS/FEDCore \
	emuDCS/FEDUtils \
	emuDCS/FEDApps
endif

ifeq ($(Set),PeripheralApps)
Packages= \
	emuDCS/PeripheralCore \
	emuDCS/PeripheralApps \
        emuDCS/PeripheralXtop \
	emuDCS/OnlineDB \
endif

ifeq ($(Set),FEDCrate)
Packages= \
        emuDCS/FEDCore \
	emuDCS/FEDUtils \
	emuDCS/FEDApps
endif

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules