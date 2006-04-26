##
#
# This is the TriDAS/emu/emuDAQ/DDUReadout Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDAQ/DDUReadout

# Point to (minimal) ORCA distribution
ORCA_ROOT=$(XDAQ_ROOT)/$(Project)/$(Package)/ORCA

Sources=\
	BinaryEventStream.cc \
	DDUReader.cc \
	EventStream.cc \
	DDUUtilities.cc \
	DDUHyperDAQ.cc \
	FileReaderDDU.cc \
	HardwareDDU.cc

IncludeDirs =  \
        $(XDAQ_ROOT)/daq/toolbox/include \
        $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
        $(XDAQ_ROOT)/daq/toolbox/include/solaris \
        $(XDAQ_ROOT)/daq/xoap/include \
        $(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/xdata/include \
	$(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/emu/emuDAQ/GenericRUI/include \
	$(XDAQ_ROOT)/daq/xgi/include \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/emu/emuDAQ/drivers/bigphysarea_xxxx/eth_hook_mmap_v0_2 \
	$(ORCA_ROOT)

LibraryDirs = \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 

TestLibraryDirs = \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xcept/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xgi/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/xmlappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(DIMDIR)/$(XDAQ_OS) \
	$(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)


UserCFlags =
UserCCFlags = -DUSE_DDU2004
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic xmlappender udpappender \
	        DDUReadout
#
# Compile the source files and create a shared library
#
DynamicLibrary= DDUReadout
StaticLibrary= DDUReadout

Executables=
TestExecutables= HardwareDumper.cpp HardwareMonitor.cpp FileReaderDDU.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
