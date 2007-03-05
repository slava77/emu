include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDAQ/emuUtil


Sources=\
	EmuServer.cc \
	EmuI2OServer.cc \
	EmuSOAPServer.cc

# IncludeDirs =  \
#         $(XDAQ_ROOT)/daq/toolbox/include \
#         $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
#         $(XDAQ_ROOT)/daq/toolbox/include/solaris \
#         $(XDAQ_ROOT)/daq/xcept/include \
#         $(XDAQ_ROOT)/daq/i2o/include \
#         $(XDAQ_ROOT)/daq/i2o/utils/include \
#         $(XDAQ_ROOT)/daq/extern/i2o/include \
#         $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
#         $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
# 	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
# 	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
# 	$(XDAQ_ROOT)/daq/xdaq/include \
# 	$(XDAQ_ROOT)/daq/xdaq/include/solaris \
# 	$(XDAQ_ROOT)/daq/xdaq/include/linux \
# 	$(XDAQ_ROOT)/daq/xdata/include \
#         $(XDAQ_ROOT)/daq/pt/include \
# 	$(XDAQ_ROOT)/daq/xoap/include \
# 	$(XDAQ_ROOT)/daq/xgi/include \
#         $(XDAQ_ROOT)/emu/emuDAQ/emuClient/include \
#         $(XDAQ_ROOT)

IncludeDirs =  \
        $(XDAQ_ROOT) \
        $(XDAQ_ROOT)/daq \
        $(XDAQ_ROOT)/daq/toolbox/include \
        $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
        $(XDAQ_ROOT)/daq/toolbox/include/solaris \
        $(XDAQ_ROOT)/daq/xcept/include \
        $(XDAQ_ROOT)/daq/i2o/include \
        $(XDAQ_ROOT)/daq/i2o/utils/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/xdaq/include/solaris \
	$(XDAQ_ROOT)/daq/xdaq/include/linux \
	$(XDAQ_ROOT)/daq/xdata/include \
        $(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/xoap/include \
	$(XDAQ_ROOT)/daq/xgi/include \
        $(XDAQ_ROOT)/emu/emuDAQ/emuClient/include

LibraryDirs =  \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserCFlags =
#UserCCFlags = -g -Wall -Werror -pedantic-errors
#UserCCFlags = -g -Wall -pedantic-errors
UserCCFlags = -g -Wall
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xdaq toolbox xoap
TestLibraries = EmuServer

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuServer
StaticLibrary=

Executables=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
