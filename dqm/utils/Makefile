##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include ../config/compdef.mk
include ../config/xdaq.mk
include ../config/site.mk

UserExecutableLinkFlags = $(ROOTGLIBS)

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=dqm/utils

Sources=dduBinExaminer.cc

Executables = mergeCalibRootFiles.cpp mergeDQMRootFiles.cpp dduEvtDump.cpp

TestLibraries = 
TestExecutables = 

ExternalObjects = 

IncludeDirs = 
LibraryDirs = 

UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS) -fPIC
UserDynamicLinkFlags = $(ROOTLIBS)
UoserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS) -lProof -lProofPlayer -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = 

#
# Compile the source files and create a shared library
#
DynamicLibrary = 
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
