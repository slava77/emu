##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include $(XDAQ_ROOT)/config/compdef.mk
include $(XDAQ_ROOT)/config/xdaq.mk
include $(XDAQ_ROOT)/config/site.mk

UserExecutableLinkFlags = $(ROOTGLIBS)

OPT_FORMAT = METB_FORMAT

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuMonitor

Sources=\
	EmuBaseMonitor.cc \
	EmuLocalPlotter.cc \
	EmuSaveHistoFile.cc \
	EmuBookCommon.cc \
	EmuBookChamber.cc \
	EmuFillCommon.cc \
	EmuFillChamber.cc \
	EmuBinCheck.cc \
	EmuMonitor.cc \
        CSCStripClusterFinder.cc \
        CSCStripClusterFitData.cc \
        CSCStripCluster.cc \
        ClusterLocalMax.cc 
#	EmuSPMonitor.cc

#	EmuFillTrackFinder.cc

Executables = EmuMonitorTest.cpp

TestLibraries = 
TestExecutables = dduBinExaminerTest.cpp

ExternalObjects = $(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libdduBinExaminer.a
#$(XDAQ_ROOT)/daq/i2o/utils/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libi2outils.a

IncludeDirs = $(XdaqIncludeDirs) \
	$(CMSSW)/src \
	$(XDAQ_ROOT)/$(Project)/$(Package)/../Common/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuDisplayServer/include \
	$(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/DDUReadout/include \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/GenericRUI/include


LibraryDirs = $(XdaqLibraryDirs) \
	$(CMSSW)/lib/slc3_ia32_gcc323/ \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
        $(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
        $(XDAQ_ROOT)/$(Project)/emuDQM/dduBinExaminer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
	$(XDAQ_ROOT)/$(Project)/emuDAQ/DDUReadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/ \
        $(CMSSW)/src/EventFilter/CSCRawToDigi/test \
        $(CMSSW)/src/DataFormats/CSCDigi/test 


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS) -D$(OPT_FORMAT) -DL1CSC_STANDALONE
UserCCFlags = $(ROOTCFLAGS) -D$(OPT_FORMAT) -DL1CSC_STANDALONE
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS) -D$(OPT_FORMAT) -DL1CSC_STANDALONE

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  DDUReadout EmuROOTDisplayServer EmuDisplayServer CSCRawToDigi CSCDigi

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuMonitor
StaticLibrary =


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
