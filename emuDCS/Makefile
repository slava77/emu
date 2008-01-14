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
	emuDCS/e2p \
	emuDCS/FEDCrate
endif

ifeq ($(Set),PeripheralApps)
Packages= \
	emuDCS/PeripheralCore \
	emuDCS/PeripheralApps
endif

ifeq ($(Set),e2p)
Packages= emuDCS/e2p
endif

ifeq ($(Set),FEDCrate)
Packages= emuDCS/FEDCrate
endif

include $(XDAQ_ROOT)/config/Makefile.rules
