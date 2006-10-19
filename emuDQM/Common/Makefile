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
	EmuDQM_AppParameters.cc \
	EmuDQM_SOAPCommands.cc \
	EmuMonitoringObject.cc \
	EmuMonitoringCanvas.cc 

Executables = 
ExternalObjects = $(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a

IncludeDirs = $(XdaqIncludeDirs) \
	$(XDAQ_ROOT)/$(Project)/emuDQM/EmuROOTDisplayServer/include \
	$(ROOTDIR)/include 

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
DynamicLibrary = emuDQMCommon
StaticLibrary = emuDQMCommon

MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
