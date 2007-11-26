include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=emuDAQ/emuRUI

Sources = \
	EmuRUI.cc \
	EmuRUIV.cc \
	STEPEventCounter.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	$(BUILD_HOME) \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib


UserCFlags =
#UserCCFlags = -g -Wall -Werror -pedantic-errors -DUSE_DDU2004
UserCCFlags = -g -Wall -DUSE_DDU2004
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

Libraries = DDUReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuRUI

StaticLibrary=

ifdef Executable
Libraries=toolbox xoap xerces-c
Executables= $(Executable).cc
endif

include $(XDAQ_ROOT)/config/Makefile.rules
