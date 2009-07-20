##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include ../config/compdef.mk
include ../config/xdaq.mk
include ../config/site.mk


#OPT_FORMAT = METB_FORMAT
USE_DDUREADOUT = DDU


include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#DebugFlags=
#CFlags+= -O2
CCFlags= -g -Wall -O3 -march=prescott
#
# Packages to be built
#
Project=emu
Package=dqm/cscanalyzer

Sources=\
	EmuPlotter.cc \
	EmuPlotter_generateLayout.cc \
	EmuPlotter_loadXMLBookingInfo.cc \
	EmuPlotter_loadXMLCanvasesInfo.cc \
	EmuPlotter_loadFromROOTFile.cc \
	EmuPlotter_convertROOTToImages.cc \
	EmuPlotter_processEvent.cc \
	EmuPlotter_processChamber.cc \
	EmuPlotter_saveImages.cc \
	EmuPlotter_saveToROOTFile.cc \
	EmuPlotter_generateReport.cc \
	EmuPlotter_generateOnlineReport.cc \
	CSCStripClusterFinder.cc \
	CSCStripClusterFitData.cc \
	CSCStripCluster.cc \
	ClusterLocalMax.cc 


Executables = runEmuCSCAnalyzer.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCRawToDigi.a \
        $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDigi.a \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDQMCommon.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(BUILD_HOME)/emu/dqm/common/include \
	$(BUILD_HOME)/emu/dqm/common/include/emu/dqm/common \
	$(CMSSW) \
	$(BOOST)


LibraryDirs = $(XdaqLibraryDirs) 


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTGLIBS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries) emudaqreader

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmcscanalyzer
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
