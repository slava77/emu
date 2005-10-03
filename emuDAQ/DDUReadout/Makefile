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
	FileReader.cc \
	FileReaderDCC.cc \
	FileReaderDDU.cc \
	HardwareDDU.cc

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
        $(XDAQ_ROOT)/emu/emuDAQ/GenericRUI/include \
        $(XDAQ_ROOT)/emu/emuDAQ/drivers/bigphysarea_xxxx/eth_hook_mmap_v0_2 \
	$(ORCA_ROOT)

LibraryDirs =  \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags =
UserCCFlags =
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xdaq toolbox xoap
TestLibraries = DDUReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary=
StaticLibrary= DDUReadout

Executables=
TestExecutables= HardwareDumper.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
