##
#
# This is the TriDAS/daq/xoap Package Makefile
#
##
include $(XDAQ_ROOT)/config/compdef.mk
include $(XDAQ_ROOT)/config/xdaq.mk
include $(XDAQ_ROOT)/config/site.mk

UserExecutableLinkFlags = $(ROOTGLIBS)

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDQM/dduBinExaminer

Sources=\
	dduBinExaminer.cc 

Executables = 

TestLibraries = 
TestExecutables = dduBinExaminerTest.cpp

ExternalObjects = 

IncludeDirs = 
LibraryDirs = 

UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = 

#
# Compile the source files and create a shared library
#
DynamicLibrary = dduBinExaminer 
StaticLibrary = dduBinExaminer


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
