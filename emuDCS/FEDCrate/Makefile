##
#
# This is the TriDAS/emu/emuDCS/PeripheralCrate Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# these variables will disappear soon ...
DIMDIR=$(XDAQ_ROOT)/$(Project)/extern/dim/$(XDAQ_OS)$(XDAQ_PLATFORM)
DCSDIR=$(XDAQ_ROOT)/emu/emuDCS/e2p

#
# Packages to be built
#
Project=emu
Package=emuDCS/FEDCrate

Sources=\
	Crate.cc \
	CrateSelector.c \
	CrateSetup.cc \
	DCC.cc \
	DCCParser.cc \
	DDU.cc \
	DDUTester.cc \
	DDUParser.cc \
	elong.cc \
	EmuParser.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	VMEParser.cc \
	FEDCrateController.cc \
	FEDCrateParser.cc

IncludeDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include

LibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib 

TestLibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib 

UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c EmuFEDCrate
TestLibraries = xerces-c EmuFEDCrate

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuFEDCrate
StaticLibrary= EmuFEDCrate

Executables=
TestExecutables= \
        daqtest.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
