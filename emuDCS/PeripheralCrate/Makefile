##
#
# This is the TriDAS/emu/emuDCS/PeripheralCrate Package Makefile
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
Package=emuDCS/PeripheralCrate

DependentLibraryDirs = $(XDAQ_ROOT)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
DependentLibraries = CSCSupervisor EmuPeripheralCore

Sources=\
        CCBParser.cc \
        ChamberUtilities.cc \
        DAQMBParser.cc \
        EmuSystemParser.cc \
        EmuParser.cc \
        MPCParser.cc \
        PeripheralCrateParser.cc \
        EMUParser.cc \
        EmuController.cc \
        TMBParser.cc \
        VMEParser.cc \
        CalibDAQ.cc \
	CrateUtils.cc \
	CrateUtilities.cc \
        EmuPeripheralCrate.cc \
        EmuPeripheralCrateManager.cc \
	EmuPeripheralCrateBroadcast.cc \
        EmuRunControlHyperDAQ.cc \
        CSCParser.cc \
        TStoreParser.cc \
        TStoreVMEParser.cc \
        TStoreCCBParser.cc \
        TStoreMPCParser.cc \
        TStoreTMBParser.cc \
        TStoreDMBParser.cc \
        TStoreCSCParser.cc \
        TStoreALCTParser.cc \
        TStoreAFEBParser.cc \
        TStoreCFEBParser.cc \
        TStorePeriphParser.cc \


IncludeDirs = \
	$(XDAQ_ROOT)/daq/toolbox/include \
	$(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT)/daq/xoap/include \
	$(XDAQ_ROOT)/daq/xdata/include \
	$(XDAQ_ROOT)/daq/xdata/include/xdata \
	$(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/xgi/include \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCore/include \
        $(XDAQ_ROOT)/emu/cscSV/include

LibraryDirs = \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/asyncresolv/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
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
	$(XDAQ_ROOT)/daq/extern/asyncresolv/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/xmlappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic xmlappender udpappender asyncresolv\
	        CSCSupervisor EmuPeripheralCore EmuPeripheralCrate

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralCrate
StaticLibrary= EmuPeripheralCrate

Executables=
TestExecutables= \
	slicetestconfig.cpp \
	EmuConfig.cpp \
	CrateTiming.cpp \
	parsemain.cpp \
	mpc2sp.cpp \
	ParserTester.cpp \
	loadALCT.cpp \
	loadTMB.cpp \
        resetcc.cpp \
        scancrate.cpp \
	loadMPC.cpp \
	loadCCB.cpp\
	loadDMB.cpp \
	testtmbmain.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
