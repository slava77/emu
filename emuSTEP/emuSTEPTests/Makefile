##
#
# This is the TriDAS.emulib_V9_11/emu/emuSTEP/emuSTEPManager Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include /home/cscdev/xdaq10/TriDAS/emu/emuSTEP/emuSTEPTests/mfDefs.$(XDAQ_OS)

# these variables will disappear soon ...

#
# Packages to be built
#
Project=emu
Package=emuSTEP/emuSTEPTests

Sources=\
	emuSTEPTests.cc \
	emuSTEPTestsV.cc 

IncludeDirs = \
	$(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralApps/include \
	$(BUILD_HOME)/emu/emuSTEP/emuSTEPApplication/include \
	$(BUILD_HOME)/emu/emuSTEP/emuSTEPConfigRead/include \
	$(BUILD_HOME)/emu/emuDAQ/emuUtil/include \
	$(BUILD_HOME)/emu/cscSV/include \
	$(XDAQ_ROOT)/include \
	$(DCSDIR)/include \



LibraryDirs = $(XDAQ_ROOT)/lib

TestLibraryDirs = \
	$(BUILD_HOME)/emu/cscSV/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/emuDCS/PeripheralApps/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \      	$(DIMDIR)/$(XDAQ_OS) \
	$(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \  $(BUILD_HOME)/emu/base/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)	$(XDAQ_ROOT)/lib/ \

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
                mimetic xmlappender udpappender 

#
# Compile the source files and create a shared library
#
DynamicLibrary= emuSTEPTests
StaticLibrary= emuSTEPTests

Executables=
TestExecutables=  

include $(XDAQ_ROOT)/config/Makefile.rules
