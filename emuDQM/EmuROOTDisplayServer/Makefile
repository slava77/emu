##
#
# This is the emu/emuDQM/EmuDisplayServer Package Makefile
#
##
#include $(XDAQ_ROOT)/emu/emuDQM/config/xdaq.mk
#include $(XDAQ_ROOT)/emu/emuDQM/config/site.mk
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
Package=emuDQM/EmuROOTDisplayServer

Sources=\
	TConsumerInfo.cc \
	ConsumerList.cc \
	ConsumerListBox.cc \
	ConsumerExport.cc \
	ConsumerCanvas.cc \
	MessageStorage.cc \
	HistoDisplay.cc \
	HistoDisplayHelpText.cc \
	CHistogram.cc \
	CObject.cc \
	SocketUtils.cc \
	DisplayServer.cc 

ROOTDicts = \
	TConsumerInfo.hh \
	CHistogram.hh \
	CObject.hh \
	HistoDisplay.hh


IncludeDirs = $(XdaqIncludeDirs) \
	$(XDAQ_ROOT)/$(Project)/$(Package)/../Common/include \
	$(ROOTDIR)/include

LibraryDirs = $(XdaqLibraryDirs) 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = -O -fPIC $(ROOTCFLAGS) -DCONSUMER_STANDALONE -DLITTLE_ENDIAN__
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =  
UserExecutableLinkFlags = -g -O -fPIC $(ROOTGLIBS) 

Libraries= $(XdaqLibraries)

Executables = HistoDisplayMain.cc Server.cc 
ExternalObjects =  
#------------------------------------------------------------------------------
DynamicLibrary = EmuROOTDisplayServer 
StaticLibrary = EmuROOTDisplayServer

MakeFilename=Makefile
include $(XDAQ_ROOT)/$(Project)/$(Package)/../config/Makefile_root.rules
include $(XDAQ_ROOT)/config/Makefile.rules

