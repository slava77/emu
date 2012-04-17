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
Package=dqm/dbutils

Sources=

# Executables = sqlite_read_mappings.cc sqlite_write_mappings.cc init_report_db.cc
Executables = sqlite_read_mappings.cc init_report_db.cc

TestLibraries = 
TestExecutables = 

ExternalObjects = 

IncludeDirs = \
	${CMSSW} \
	$(SQLITE)/include

LibraryDirs =  $(SQLITE)/lib \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/

UserSourcePath = 

UserCFlags = 
UserCCFlags =
UserDynamicLinkFlags =
UoserStaticLinkFlags =
UserExecutableLinkFlags = 

# These libraries can be platform specific and
# potentially need conditional processing
#
# Libraries = cscmap1_32 occi nnz10 clntsh sqlite3 CSCDigi log4cplus
Libraries = occi nnz11 clntsh sqlite3 log4cplus CSCDigi


#
# Compile the source files and create a shared library
#
DynamicLibrary = 
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
