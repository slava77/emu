##
#
# This is the TriDAS/emu/emuDAQ/DDUReadout Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDAQ/emuReadout

Sources=\
	EmuReader.cc \
	EmuFileReader.cc \
	EmuSpyReader.cc
# 	EmuSLinkReader.cc

IncludeDirs =  \
        $(XDAQ_ROOT)/daq/toolbox/include \
        $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
        $(XDAQ_ROOT)/daq/toolbox/include/solaris \
        $(XDAQ_ROOT)/daq/xoap/include \
        $(XDAQ_ROOT)/daq/xdaq/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/emu/emuDAQ/drivers/e1000hook/eth_hook_2_nobigphysxxx \
	$(XDAQ_ROOT)/daq/itools/i2ocore-modules/fedkit

LibraryDirs =  \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags =
UserCCFlags = -DUSE_DDU2004 -g -Wall -Werror
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xdaq toolbox xoap
TestLibraries = EmuReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuReadout
StaticLibrary=

Executables=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
