XDAQ_BACK_TO_ROOT:=../../..

include $(XDAQ_BACK_TO_ROOT)/config/mfAutoconf.rules

include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Package to be built
#
Project=emu
Package=emuDAQ/emuClient

#
# Source files
#
# Sources=MStreamIO.cc Client.cc Server.cc MStreamIOV.cc
Sources=EmuClient.cc EmuClientV.cc EmuSOAPClient.cc EmuSOAPClientV.cc

#
# Include directories
#
IncludeDirs = \
	$(XDAQ_ROOT)/emu/emuDAQ \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/toolbox/include \
	$(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT)/daq/xoap/include \
	$(XDAQ_ROOT)/daq/i2o/include \
	$(XDAQ_ROOT)/daq/i2o/utils/include \
	$(XDAQ_ROOT)/daq \
	$(XDAQ_ROOT)/daq/xdata/include \
	$(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include/ \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared 

LibraryDirs = 

UserSourcePath =

UserCFlags =
UserCCFlags =  -g -Wall -Werror -pedantic-errors
UserDynamicLinkFlags =
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
#DynamicLibrary=MStreamIO
DynamicLibrary=EmuClient
StaticLibrary=

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
