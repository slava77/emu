##
#
# This is the TriDAS/emu/emuDCS/FEDApps Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# Testing
#CXX=g++4
#LD=g++4

#
# Packages to be built
#
Project=emu
Package=emuDCS/FEDApps
PackageName=FEDApps
Description="Applications to control and monitor the Front End Device (FED) crates for the CMS Emu (CSC) subdetector"
Summary="Emu (CSC) FED XDAQ Applications"
Authors="Phillip Killewald, Stan Durkin, Jason Gilmore, Jianhui Gu"
Link="http://physics.ohio-state.edu/~cms"
PACKAGE_VER_MAJOR=07
PACKAGE_VER_MINOR=06
PACKAGE_VER_PATCH=00

Sources=\
	AppsVersion.cc \
	Application.cc \
	Supervised.cc \
	Manager.cc \
	Communicator.cc \
	Monitor.cc \
	Commander.cc \
	EmuFCrateHyperDAQ.cc

IncludeDirs = \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/emuDCS/FEDCore/include \
	$(BUILD_HOME)/emu/emuDCS/FEDUtils/include \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS)

LibraryDirs = \
	$(XDAQ_ROOT)/lib

UserCFlags =
UserCCFlags = -pipe -DLINUX -g -Wall -pedantic-errors -Wno-long-long -isystem $(XDAQ_ROOT)/include
UserStaticLinkFlags =
UserDynamicLinkFlags = -lboost_filesystem
UserExecutableLinkFlags = -D_REENTRAN -pthread -pedantic-errors

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects =

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuFEDApps
StaticLibrary =

Executables =
TestExecutables =

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
