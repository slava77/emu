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

CCFlags= -g  -Wall 

#
# Packages to be built
#
Project=emu
Package=dqm/cscmonitor

Sources=\
	EmuMonitor_SOAP.cc \
	EmuMonitor_WebInterface.cc \
	EmuMonitor.cc \
	version.cc


Executables = 

TestLibraries =  
TestExecutables = 

ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/daq/server/include \
        $(BUILD_HOME)/emu/daq/reader/include \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(BUILD_HOME)/emu/dqm/common/include \
	$(BUILD_HOME)/emu/dqm/cscanalyzer/include \
	$(CMSSW) \
	../.. \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) \
	$(BUILD_HOME)/emu/dqm/cscanalyzer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
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
Libraries = $(XdaqLibraries) emudaqreader emudqmcscanalyzer 

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmcscmonitor 
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules

