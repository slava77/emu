##
#
# This is the TriDAS/emu/emuDCS/PeripheralCore Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

#
# Packages to be built
#
Project=emu
Package=emuDCS/PeripheralCore
PackageName=PeripheralCore
Description="Core libraries for Peripheral Crates"

Sources=\
	CoreVersion.cc \
	ALCTController.cc \
	CCB.cc \
	RAT.cc \
	CFEB.cc \
	Crate.cc \
	DAQMB.cc \
	MPC.cc \
	TMBTester.cc \
	TMB.cc \
	VMECC.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	Chamber.cc \
	EMUjtag.cc \
	EmuEndcap.cc \
        EmuLogger.cc \
        PROBLEM.cc \
        DDU.cc \
	Unpacker.cc

IncludeDirs = 

LibraryDirs = 

TestLibraryDirs =

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
TestLibraries =

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralCore
StaticLibrary= 

Executables=
TestExecutables= 

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
