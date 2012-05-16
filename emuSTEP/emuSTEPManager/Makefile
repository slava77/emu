##
#
# This is the TriDAS/emu/emuSTEP/emuSTEPManager Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# these variables will disappear soon ...

#
# Packages to be built
#
Project=emu
Package=emuSTEP/emuSTEPManager

Sources=\
	emuSTEPManager.cc \
	emuSTEPManagerV.cc 

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(DCSDIR)/include \
	$(BUILD_HOME)/emu/emuDAQ/emuUtil/include \
	$(BUILD_HOME)/emu/cscSV/include \
	$(BUILD_HOME)/emu/emuSTEP/emuSTEPApplication/include \
	$(BUILD_HOME)/emu/emuSTEP/emuSTEPConfigRead/include

LibraryDirs = $(XDAQ_ROOT)/lib

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib \
	$(DIMDIR)/$(XDAQ_OS) \
	$(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(BUILD_HOME)/emu/cscSV/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib

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
                mimetic xmlappender udpappender \

#
# Compile the source files and create a shared library
#
DynamicLibrary= emuSTEPManager
StaticLibrary= emuSTEPManager

Executables=
TestExecutables=  

include $(XDAQ_ROOT)/config/Makefile.rules
