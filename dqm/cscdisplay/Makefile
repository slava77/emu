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

CCFlags=-g -Wall -O3

#
# Packages to be built
#
Project=emu
Package=dqm/cscdisplay

#
# TODO: Select version.cc or EmuDisplayClientV.cc depending on xdaq version
#	version.cc - xdaq 4 (>3.11)

Sources=\
	EmuDisplayClient.cc \
	EmuDisplayClient_collections.cc \
	EmuDisplayClient_loadXMLBookingInfo.cc \
        EmuDisplayClient_loadXMLCanvasesInfo.cc \
	EmuDisplayClient_generateReport.cc \
	version.cc

Executables = 
ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDigi.a \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDQMCommon.a
	

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/dqm/common/include \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(ROOTDIR)/include \
	$(CMSSW) \
        $(BOOST)


LibraryDirs = $(XdaqLibraryDirs) 



UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags = 
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries) emubase 

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmcscdisplay
StaticLibrary = 

MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
