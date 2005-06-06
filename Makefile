##
#
# This is the TriDAS/emu Project Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

### this will include other packages, like DAQ and DQM
ifeq ($(Set),framework)
Packages=\
        extern/dim \
	emuDCS/PeripheralCrate \
	emuDCS/e2p \
	emuDAQ/DDUReadout \
	emuDAQ/GenericRUI \
	emuDAQ/EmuRUI
endif

### specific package target builds
ifeq ($(Set),emuDCS)
Packages= \
        extern/dim \
        emuDCS/PeripheralCrate \
	emuDCS/e2p
endif

ifeq ($(Set),emuDAQ)
Packages= \
	emuDAQ/DDUReadout \
	emuDAQ/GenericRUI \
	emuDAQ/EmuRUI
endif

ifeq ($(Set),emuDQM)
Packages=
endif


include $(XDAQ_ROOT)/config/Makefile.rules
