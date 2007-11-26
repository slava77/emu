include $(XDAQ_ROOT)/config/mfAutoconf.rules

include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Package to be built
#
Project=emu
Package=emuDAQ/emuClient

#
# Source files
#
Sources=EmuClient.cc EmuClientV.cc EmuSOAPClient.cc EmuSOAPClientV.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	$(BUILD_HOME) \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib


UserCFlags =
# UserCCFlags =  -g -Wall -Werror -pedantic-errors
# UserCCFlags =  -g -Wall -pedantic-errors
UserCCFlags =  -g -Wall
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
