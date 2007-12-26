##
#
# This is the TriDAS/emu/emuDCS/PeripheralCore Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDCS/PeripheralCore

Sources=\
	ALCTController.cc \
	AnodeChannel.cc \
	CCB.cc \
	RAT.cc \
	CFEB.cc \
	Crate.cc \
	CrateSelector.cc \
	CrateSetup.cc \
	DAQMB.cc \
	MPC.cc \
	TMBTester.cc \
	TMB.cc \
	VMEController.cc \
	VMEController_jtag.cc \
	VMEModule.cc \
	Chamber.cc \
	EMUjtag.cc \
	EmuSystem.cc \
	EmuEndcap.cc

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
StaticLibrary= EmuPeripheralCore

Executables=
TestExecutables= 

include $(XDAQ_ROOT)/config/Makefile.rules
