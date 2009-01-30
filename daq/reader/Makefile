##
#
# This is the emudaqreader test application Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project = emu
Package = daq/reader

Sources = \
	Base.cc \
	RawDataFile.cc \
	Spy.cc \
	Clock.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/daq/reader/include \
        $(BUILD_HOME)/emu/emuDAQ/drivers/e1000hook/eth_hook_2_nobigphysxxx


LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
#UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserCCFlags = -g -Wall -Wno-long-long
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
DynamicLibrary = emudaqreader
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
