##
#
# This is the TriDAS/emu/emuDCS/FEDCrate Package Makefile
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
	CrateSelector.cc \
	CrateSetup.cc \
	DDU.cc \
	DDUParser.cc \
	DCC.cc \
	DCCParser.cc \
	elong.cc \
	EmuParser.cc \
	TestBeamCrateController.cc \
	FEDCrateController.cc \
	FEDCrateParser.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	IRQThread.cc \
	VMEParser.cc \
	EmuFCrateHyperDAQ.cc \
	EmuFRunControlHyperDAQ.cc \
	EmuFCrate.cc \
	EmuFCrateManager.cc \
	EmuFController.cc \
	Chamber.cc \
	ChamberParser.cc


IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	$(BUILD_HOME)/emu/cscSV/include/ \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib

TestLibraryDirs = \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xcept/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xgi/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/xmlappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(DIMDIR)/$(XDAQ_OS) \
	$(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/emu/emuDCS/FEDCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/emu/cscSV/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib

UserCFlags  = 
UserCCFlags = -DOSUcc -fPIC -DLINUX  -s
//UserDynamicLinkFlags = -l CAENVME
UserStaticLinkFlags =
UserExecutableLinkFlags =-D_REENTRAN -pthread

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer CAENVME
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic xmlappender udpappender \
	        EmuFEDCrate CAENVME

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuFEDCrate
StaticLibrary= EmuFEDCrate

Executables=
TestExecutables=  

include $(XDAQ_ROOT)/config/Makefile.rules
