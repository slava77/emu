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
Package=emuDQM/emuMonitor

Sources=\
	EmuMonitor.cc \
	EmuMonitor_SOAP.cc \
        EmuMonitorV.cc \
	EmuPlotter.cc \
	EmuPlotter_bookHistograms.cc \
	EmuPlotter_bookCanvases.cc \
	EmuPlotter_loadXMLBookingInfo.cc \
	EmuPlotter_loadXMLCanvasesInfo.cc \
	EmuPlotter_loadFromROOTFile.cc \
	EmuPlotter_processEvent.cc \
	EmuPlotter_processChamber.cc \
	EmuPlotter_saveImages.cc \
	EmuPlotter_saveToROOTFile.cc \
	CSCStripClusterFinder.cc \
	CSCStripClusterFitData.cc \
	CSCStripCluster.cc \
	ClusterLocalMax.cc
#	EmuBinCheck.cc 


Executables = emuMonitorTest.cpp makePlots.cc
#EmuMonitorTest.cpp MonitorTest.cpp

TestLibraries = xerces-c
TestExecutables = xmlExample.cpp 

ExternalObjects = \
	$(XDAQ_ROOT)/$(Project)/emuDQM/Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemuDQMCommon.a \
	$(XDAQ_ROOT)/$(Project)/emuDQM/CMSSWLibs/lib/libCSCRawToDigi.a \
        $(XDAQ_ROOT)/$(Project)/emuDQM/CMSSWLibs/lib/libCSCDigi.a \
	$(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libdduBinExaminer.a \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(XDAQ_ROOT)/$(Project)/emuDQM/CMSSWLibs \
	$(XDAQ_ROOT)/$(Project)/emuDQM/Common/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/ORCA \
	$(CMSSW) \
	$(XDAQ_ROOT)/$(Project) \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/GenericRUI/include \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/emuReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ 


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  EmuReadout 

#
# Compile the source files and create a shared library
#
DynamicLibrary = emuMonitor
StaticLibrary =


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
