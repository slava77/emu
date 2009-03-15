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
	base \
        supervisor \
	emuDCS/PeripheralCore \
	emuDCS/PeripheralApps \
	emuDCS/PeripheralXtop \
	emuDCS/OnlineDB \
	emuDCS/drivers/gbit-vme \
	emuDCS/FEDCore \
	emuDCS/FEDUtils \
	emuDCS/FEDApps \
endif

### specific package target builds
ifeq ($(Set),emuDCS)
Packages= \
        emuDCS/PeripheralCore \
        emuDCS/PeripheralApps \
	emuDCS/PeripheralXtop \
	emuDCS/OnlineDB \
	emuDCS/drivers/gbit-vme \
	emuDCS/FEDCore \
	emuDCS/FEDUtils \
	emuDCS/FEDApps
endif

ifeq ($(Set),emuDAQ)
Packages= \
	emuDAQ/emuReadout \
	emuDAQ/emuFU \
	emuDAQ/emuRUI \
	emuDAQ/emuTA \
	emuDAQ/emuDAQManager \
	emuDAQ/emuClient \
	emuDAQ/emuUtil \
	emuDAQ/drivers
endif

ifeq ($(Set),emuDQM)
Packages=
endif

ifeq ($(Set),supervisor) 
Packages= \
          base \
          supervisor
endif 

ifeq ($(Set),drivers)
Packages = \
	emuDAQ/drivers \
	emuDCS/drivers
endif

include $(XDAQ_ROOT)/config/Makefile.rules
