##
#
# This is the TriDAS/emu/emuDCS/FEDCrate Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# these variables will disappear soon ...
#DIMDIR=$(XDAQ_ROOT)/$(Project)/extern/dim/$(XDAQ_OS)$(XDAQ_PLATFORM)
#DCSDIR=$(XDAQ_ROOT)/emu/emuDCS/e2p

#
# Packages to be built
#
Project=emu
Package=emuDCS/FEDCrate

Sources=\
	FEDVersion.cc \
	EmuFEDLoggable.cc \
	VMEModule.cc \
	Chamber.cc \
	DDU.cc \
	DCC.cc \
	VMEController.cc \
	FEDCrate.cc \
	EmuParser.cc \
	ChamberParser.cc \
	DDUParser.cc \
	DCCParser.cc \
	VMEControllerParser.cc \
	FEDCrateParser.cc \
	EmuFEDApplication.cc \
	EmuFCrateManager.cc \
	IRQThreadManager.cc \
	EmuFCrate.cc \
	DDUDebugger.cc \
	DCCDebugger.cc \
	DataTable.cc \
	EmuFCrateHyperDAQ.cc


IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/xdaq \
	$(BUILD_HOME)/emu/cscSV/include \
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
UserCCFlags = -march=nocona -pipe -fomit-frame-pointer -O2 -DOSUcc -fPIC -DLINUX  -s
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
