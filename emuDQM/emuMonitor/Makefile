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
	EmuPlotter.cc \
	EmuPlotter_generateLayout.cc \
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
	ClusterLocalMax.cc \
	version.cc
#	EmuMonitorV.cc
#	EmuBinCheck.cc 


Executables = emuMonitorTest.cpp 
#EmuMonitorTest.cpp MonitorTest.cpp

TestLibraries = xerces-c
TestExecutables = xmlExample.cpp 

ExternalObjects = \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemuDQMCommon.a \
	../CMSSWLibs/lib/libCSCRawToDigi.a \
        ../CMSSWLibs/lib/libCSCDigi.a \
	../EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a 

IncludeDirs = $(XdaqIncludeDirs) \
	../CMSSWLibs \
	../Common/include \
	../../emuDAQ/emuReadout/include \
	../../emuDAQ/emuReadout/ORCA \
	$(CMSSW) \
	../.. \
	../EmuROOTDisplayServer/include \
	../../emuDAQ/GenericRUI/include \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) \
	../../emuDAQ/emuReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ 


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
