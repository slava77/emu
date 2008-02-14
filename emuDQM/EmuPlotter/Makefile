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
#CCFlags= -O2 -Wall
#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuPlotter

Sources=\
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
	ClusterLocalMax.cc 


Executables = runEmuPlotter.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuDQMCommon.a \
	../CMSSWLibs/lib/libCSCRawToDigi.a \
        ../CMSSWLibs/lib/libCSCDigi.a 

IncludeDirs = $(XdaqIncludeDirs) \
	../CMSSWLibs \
	../Common/include \
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
Libraries = $(XdaqLibraries) EmuReadout

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuPlotter
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
