##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include ../config/compdef.mk
include ../config/xdaq.mk
include ../config/site.mk

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#DebugFlags=
#CFlags+= -O2
CCFlags= -O2 -Wall -DDQMLOCAL -g
#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuDatabase

Sources=\
	DBImporter.cc \
	DBImporter_generate.cc \
	CSCDQM_DBWorker.cc

Executables = runEmuDBImport.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuDQMCommon.a \
	../CMSSWLibs/lib/libCSCRawToDigi.a \
        ../CMSSWLibs/lib/libCSCDigi.a \
	../CMSSWLibs/lib/libCSCDQMCommon.a 

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
Libraries = $(XdaqLibraries) occi ociei

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuDatabase
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
