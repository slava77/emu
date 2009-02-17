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

#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuMonitor

Sources=\
	EmuMonitor_SOAP.cc \
	EmuMonitor_WebInterface.cc \
	EmuMonitor.cc \
	version.cc


Executables = 

TestLibraries =  
TestExecutables = 

ExternalObjects = \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuDQMCommon.a #\
	../CMSSWLibs/lib/libCSCRawToDigi.a \
	../CMSSWLibs/lib/libCSCDigi.a \
	../EmuPlotter/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuPlotter.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/daq/server/include \
        $(BUILD_HOME)/emu/daq/reader/include \
	../CMSSWLibs \
	../Common/include \
	../../emuDAQ/emuReadout/include \
	../../emuDAQ/emuReadout/ORCA \
	$(CMSSW) \
	../.. \
	../EmuPlotter/include \
	../../emuDAQ/GenericRUI/include \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) \
	../EmuPlotter/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
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
Libraries = $(XdaqLibraries) emudaqreader  EmuPlotter

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuMonitor
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
