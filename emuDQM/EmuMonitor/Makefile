##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include ../config/compdef.mk
include ../config/xdaq.mk
include ../config/site.mk

UserExecutableLinkFlags = $(ROOTGLIBS)

#OPT_FORMAT = METB_FORMAT
USE_DDUREADOUT = DDU


include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuMonitor

Sources=\
	EmuMonitor.cc \
        EmuMonitorV.cc \
	EmuLocalPlotter.cc \
        EmuSaveHistoFile.cc \
	EmuSaveHistoImages.cc \
        EmuBookCommon.cc \
        EmuBookChamber.cc \
        EmuFillCommon.cc \
        EmuFillChamber.cc \
        EmuBinCheck.cc \
        CSCStripClusterFinder.cc \
        CSCStripClusterFitData.cc \
        CSCStripCluster.cc \
        ClusterLocalMax.cc


Executables = EmuMonitorTest.cpp MonitorTest.cpp

TestLibraries = xerces-c
TestExecutables = xmlExample.cpp 

ExternalObjects = \
	$(CMSSW)/src/EventFilter/CSCRawToDigi/test/libCSCRawToDigi.a \
        $(CMSSW)/src/DataFormats/CSCDigi/test/libCSCDigi.a \
	$(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libdduBinExaminer.a \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a 
	# $(XDAQ_ROOT)/$(Project)/emuDAQ/DDUReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libDDUReadout.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(XDAQ_ROOT)/$(Project)/$(Package)/../Common/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/ORCA \
	$(CMSSW)/src \
	$(XDAQ_ROOT)/$(Project) \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/DDUReadout/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/GenericRUI/include


LibraryDirs = $(XdaqLibraryDirs) \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/DDUReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  EmuReadout DDUReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuMonitor
StaticLibrary =


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
