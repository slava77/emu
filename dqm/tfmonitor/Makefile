##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include ../config/compdef.mk
include ../config/xdaq.mk
include ../config/site.mk

#UserExecutableLinkFlags = $(ROOTGLIBS)

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

CCFlags= -g  -Wall -O1

#
# Packages to be built
#
Project=emu
Package=dqm/tfmonitor

Sources=\
	EmuTFMonitor_SOAP.cc \
	EmuTFMonitor_WebInterface.cc \
	EmuTFMonitor.cc \
	version.cc


Executables = 

TestLibraries =  
TestExecutables = 

ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/daq/server/include \
        $(BUILD_HOME)/emu/daq/reader/include \
	$(BUILD_HOME)/emu/dqm/cmssw/emu/dqm/cmssw \
	$(BUILD_HOME)/emu/dqm/common/include \
	$(CMSSW) \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) \
	$(BUILD_HOME)/emu/dqm/tfanalyzer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
	$(BUILD_HOME)/emu/daq/reader/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ 


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS) 
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTGLIBS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries) emudaqreader emudqmtfanalyzer

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmtfmonitor
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
