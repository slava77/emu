XDAQ_BACK_TO_ROOT:=../../..

include $(XDAQ_BACK_TO_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=emuDAQ/emuRUI

Sources = \
	EmuRUI.cc \
	EmuRUIV.cc

IncludeDirs = \
	$(XDAQ_ROOT) \
	$(XDAQ_ROOT)/emu \
	$(XDAQ_ROOT)/emu/emuDAQ/DDUReadout/include \
	$(XDAQ_ROOT)/emu/emuDAQ/DDUReadout/ORCA \
	$(XDAQ_ROOT)/daq \
	$(XDAQ_ROOT)/daq/i2o/include \
	$(XDAQ_ROOT)/daq/toolbox/include \
	$(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include/ \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/sentinel/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/xdaq/include/solaris \
	$(XDAQ_ROOT)/daq/xdaq/include/linux \
	$(XDAQ_ROOT)/daq/xgi/include \
	$(XDAQ_ROOT)/daq/xdata/include \
	$(XDAQ_ROOT)/daq/xoap/include

LibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/pt/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/emu/emuDAQ/DDUReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)


UserCFlags =
UserCCFlags = -g -Wall -Werror -pedantic-errors -DUSE_DDU2004
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

Libraries = DDUReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuRUI

StaticLibrary=

ifdef Executable
Libraries=toolbox xoap xerces-c
Executables= $(Executable).cc
endif

include $(XDAQ_ROOT)/config/Makefile.rules
