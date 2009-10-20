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

Standalone = no

Sources = \
	Base.cc \
	RawDataFile.cc \
	Spy.cc \
	Clock.cc 

ifeq ($(Standalone),no)
	Sources += version.cc
endif

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/daq/reader/include \
        $(BUILD_HOME)/emu/drivers/e1000hook/eth_hook_2_nobigphysxxx


LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
#UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserCCFlags = -DUSE_DDU2004 -g -Wall -Wno-long-long
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
ifeq ($(Standalone),no)
	DynamicLibrary = emudaqreader
	StaticLibrary = 
else
	DynamicLibrary = 
	StaticLibrary = emudaqreader_sa
endif

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
