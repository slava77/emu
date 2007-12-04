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
Package=emuDQM/emuDisplayClient

#
# TODO: Select version.cc or EmuDisplayClientV.cc depending on xdaq version
#	version.cc - xdaq 4 (>3.11)

Sources=\
	EmuBrowsing.cc \
	EmuDisplayClient.cc \
	version.cc
#	EmuDisplayClientV.cc

Executables = 
ExternalObjects = ../EmuROOTDisplayServer/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuROOTDisplayServer.a \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemuDQMCommon.a 
	

IncludeDirs = $(XdaqIncludeDirs) \
	../EmuROOTDisplayServer/include \
	../Common/include \
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
DynamicLibrary = emuDisplayClient
StaticLibrary = emuDisplayClient

MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
