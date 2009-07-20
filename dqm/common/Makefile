##
#
# This is the emu/emuDQM/EmuDisplayServer Package Makefile
#
##
include ../config/xdaq.mk
include ../config/site.mk

# ROOT Config
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)


include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

CCFlags= -g -Wall -O3 -march=prescott -mtune=prescott

#
# Packages to be built
#
Project=emu
Package=dqm/common

Sources=\
	EmuDQM_Utils.cc \
	ConsumerCanvas.cc \
	EmuDQM_AppParameters.cc \
	EmuMonitoringObject.cc \
	EmuMonitoringCanvas.cc \
	CSCReadoutMappingFromFile.cc 


Executables =
ExternalObjects = $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDQMCommon.a

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw/ \
	$(ROOTDIR)/include \
	$(CMSSW)


LibraryDirs = $(XdaqLibraryDirs) 



UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags = 
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmcommon
StaticLibrary = emudqmcommon

MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
