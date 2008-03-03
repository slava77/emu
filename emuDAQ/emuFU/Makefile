include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=emuDAQ/emuFU

Sources= \
	EmuFU.cc \
	EmuFUV.cc \
	EmuEventHeaderTrailer.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	$(BUILD_HOME) \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib

UserCFlags =
#UserCCFlags = -g -Wall -Werror -pedantic-errors
UserCCFlags = -g -Wall
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

Libraries = 

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuFU

StaticLibrary=

ifdef Executable
Libraries=toolbox xoap xerces-c
Executables= $(Executable).cc
endif

include $(XDAQ_ROOT)/config/Makefile.rules
