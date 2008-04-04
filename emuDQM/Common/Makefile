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

#
# Packages to be built
#
Project=emu
Package=emuDQM/Common

Sources=\
	ConsumerCanvas.cc \
	EmuDQM_AppParameters.cc \
	EmuDQM_SOAPCommands.cc \
	EmuMonitoringObject.cc \
	EmuMonitoringCanvas.cc \
	CSCReadoutMappingFromFile.cc

Executables =
ExternalObjects =
# ../EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a

IncludeDirs = $(XdaqIncludeDirs) \
	../CMSSWLibs \
	$(ROOTDIR)/include \
	$(CMSSW)

#../EmuROOTDisplayServer/include \

LibraryDirs = $(XdaqLibraryDirs) 



UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags = 
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  

#
# Compile the source files and create a shared library
#
DynamicLibrary = 
StaticLibrary = EmuDQMCommon

MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
