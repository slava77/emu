##
#
# This is the emustep Makefile
#
##

BUILD_HOME:=$(shell pwd)/../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
PACKAGE_VER_MAJOR=04
PACKAGE_VER_MINOR=00
PACKAGE_VER_PATCH=04

#
# Packages to be built
#
Project = emu
Package = step
PackageName= step
Description="Emu (CSC) STEP (System Test of Endcap Peripheral crate and chamber electronics)"
Summary="Emu (CSC) STEP"
Authors="Karoly Banicz, Alex Madorsky"
Link=""

Sources = \
	Application.cc \
	Manager.cc \
	Tester.cc \
	Configuration.cc \
	TestParameters.cc	\
	Test.cc	\
	ChamberMap.cc	\
	version.cc
TestSources =
TestIncludeDirs =

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/emu/soap/include \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralApps/include \
	$(BUILD_HOME)/emu/step/include
LibraryDirs =

UserSourcePath =
UserCFlags = $(ROOTCFLAGS)
#UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserCCFlags = -O0 -g -Wall -Wno-long-long $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects =

#
# Compile the source files and create a shared library
#
DynamicLibrary = emustep
StaticLibrary =

# To build tests:
#     export BUILD_HOME=... 
#     make tests
# To run it:
#     LD_LIBRARY_PATH=$BUILD_HOME/emu/lib/$XDAQ_OS/$XDAQ_PLATFORM:$XDAQ_ROOT/lib $BUILD_HOME/emu/step/$XDAQ_OS/$XDAQ_PLATFORM/*.exe
TestLibraryDirs=
TestLibraries=xcept toolbox log4cplus xdata xerces-c xalan-c xdaq xalanMsg xoap xgi cgicc pthttp peer asyncresolv mimetic config logudpappender logxmlappender uuid emubase emusoap tstoreclient EmuOnlineDB EmuPeripheralCore EmuPeripheralApps
TestExecutables=configTest.cc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
