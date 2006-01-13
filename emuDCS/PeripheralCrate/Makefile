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
Package=emuDCS/PeripheralCrate

Sources=\
	ALCTController.cc \
	AnodeChannel.cc \
        CalibDAQ.cc \
	CCB.cc \
	CCBParser.cc \
	CFEB.cc \
	Crate.cc \
	CrateSelector.cc \
	CrateSetup.cc \
	DAQMB.cc \
	DAQMBParser.cc \
	DAQMBTester.cc \
	DDU.cc \
	DDUParser.cc \
	elong.cc \
	EmuParser.cc \
	MPC.cc \
	MPCParser.cc \
	PCDriverI2oListener.cpp \
	PeripheralCrateController.cc \
	PeripheralCrateParser.cc \
	TestBeamCrateController.cc \
	TMB.cc \
	TMBParser.cc \
	TMB_trgmode.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	VMEParser.cc \
	EmuController.cc \
	ChamberUtilities.cc \
	EmuPeripheralCrate.cc \
	EmuPeripheralCrateSO.cc \
	EmuPeripheralCrateV.cc \
	TMBTester.cc

IncludeDirs = \
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
	$(DCSDIR)/include \
	$(DIMDIR)/dim

LibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/soap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

TestLibraryDirs = \
	$(DIMDIR)/$(XDAQ_OS) \
	$(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags = -DUSEDSC
UserCCFlags = -DUSEDCS 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap
TestLibraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap \
	        dim dcs_emu EmuPeripheralCrate

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralCrate
StaticLibrary= EmuPeripheralCrate

Executables=
TestExecutables= \
        calibpulse.cpp \
	slicetestconfig.cpp \
	testbeamcontrol.cpp \
	tmbtiming.cpp \
        CrateScan.cpp \
	parsemain.cpp \
	alctmain.cpp \
	contrmain.cpp \
	mpc2sp.cpp \
	loadALCT.cpp \
	testtmbmain.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
