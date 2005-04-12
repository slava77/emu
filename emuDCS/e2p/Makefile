##
#
# This is the TriDAS/emu/emuDCS/e2p Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)


Project=emu
Package=emuDCS/e2p

Sources =\
         DcsDimService.cxx \
         DcsDimCommand.cxx \
         EmuDcs.cxx

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
	$(XDAQ_ROOT)/emu/extern/dim/$(XDAQ_OS)$(XDAQ_PLATFORM)/dim \
	$(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/include

LibraryDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/soap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

TestLibraryDirs = \
	$(XDAQ_ROOT)/emu/extern/dim/$(XDAQ_OS)$(XDAQ_PLATFORM)/$(XDAQ_OS) \
	$(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags = -DUSEDCS
UserCCFlags = -DUSEDCS
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap
TestLibraries = xerces-c xdaq log4cplus ptSOAP ptTCP toolbox xoap \
                dim EmuPeripheralCrate

#
# Compile the source files and create a shared library
#
DynamicLibrary= dcs_emu
StaticLibrary= dcs_emu

Executables=
TestExecutables= \
        main.cpp \
        EmuDcs_main.cxx

include $(XDAQ_ROOT)/config/Makefile.rules
