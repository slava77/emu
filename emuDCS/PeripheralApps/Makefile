##
#
# This is the TriDAS/emu/emuDCS/PeripheralApps Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# Packages to be built
#
Project=emu
Package=emuDCS/PeripheralApps


Sources=\
        AppsVersion.cc \
        CalibDAQ.cc \
        ChamberUtilities.cc \
	CrateUtilities.cc \
        XMLParser.cc \
        TStoreRequest.cc \
        EmuPCrateConfigTStore.cc \
        EmuController.cc \
	EmuPeripheralCrateBroadcast.cc \
        EmuPeripheralCrateConfig.cc \
        EmuPeripheralCrateCommand.cc \
        EmuPeripheralCrateMonitor.cc \
        EmuPeripheralCrateManager.cc

IncludeDirs = \
        $(XDAQ_ROOT)/include \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/OnlineDB/include \
        $(BUILD_HOME)/emu/cscSV/include

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/emuDCS/PeripheralApps/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/cscSV/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

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
                mimetic logxmlappender logudpappender asyncresolv config \
                CSCSupervisor EmuPeripheralCore EmuPeripheralApps uuid
                
#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralApps
StaticLibrary= EmuPeripheralApps

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
