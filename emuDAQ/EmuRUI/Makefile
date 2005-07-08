##
#
# This is the TriDAS/emu/emuDAQ/EmuRUI Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDAQ/EmuRUI

Sources=\
	EmuRUI.cc \
	EmuRUISO.cc \
	EmuRUIV.cc \
	EmuConfigRUI.cc


IncludeDirs =  \
        $(XDAQ_ROOT)/daq/toolbox/include \
        $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
        $(XDAQ_ROOT)/daq/toolbox/include/solaris \
        $(XDAQ_ROOT)/daq/xoap/include \
        $(XDAQ_ROOT)/daq/xdaq/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/interface/shared/include \
	$(XDAQ_ROOT)/daq/interface/evb/include \
	$(XDAQ_ROOT)/emu/emuDAQ/GenericRUI/include \
	$(XDAQ_ROOT)/emu/emuDAQ/DDUReadout/include


LibraryDirs =  \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/pt/soap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/emu/emuDAQ/GenericRUI/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/emu/emuDAQ/DDUReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags =
UserCCFlags =
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xdaq toolbox xoap xerces-c GenericRUI DDUReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuRUI
StaticLibrary=


include $(XDAQ_ROOT)/config/Makefile.rules
