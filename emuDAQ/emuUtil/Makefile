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

IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	$(BUILD_HOME) \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib


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
