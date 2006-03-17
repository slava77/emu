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

Sources=\
	ALCTController.cc \
	AnodeChannel.cc \
        CalibDAQ.cc \
	CCB.cc \
	RAT.cc \
	CCBParser.cc \
	CFEB.cc \
	Crate.cc \
	Chamber.cc \
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
	PeripheralCrateController.cc \
	PeripheralCrateParser.cc \
	TMB.cc \
	TMBParser.cc \
	TMB_trgmode.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	VMEParser.cc \
	EmuController.cc \
	ChamberUtilities.cc \
	TMBTester.cc \
	EMUjtag.cc

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
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include 

LibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/soap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

TestLibraryDirs = \
        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags = 
UserCCFlags = -DOSUcc
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap
TestLibraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap \
	        EmuPeripheralCrate

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralCrate
StaticLibrary= EmuPeripheralCrate

Executables=
TestExecutables= \
        calibpulse.cpp \
	slicetestconfig.cpp \
	EmuConfig.cpp \
	CrateTiming.cpp \
	parsemain.cpp \
	mpc2sp.cpp \
	loadALCT.cpp \
	testtmbmain.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
