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
CCFlags+= -O3 -DLOCAL_UNPACK

# check emu/dqm/config/site.mk for NEW_UNPACKER2013 variable and comment it you need old unpacker code prior to CMSSW_6_X_X
ifdef NEW_UNPACKER2013
CCFlags+= -DUNPACKER2013
endif

#
# Packages to be built
#
Project=emu
Package=dqm/dcfebpipeline

Sources=\
	DCFEBSample.cc \
	PipelineDepthFinder.cc \
	PipelineAnalyzer.cc \
	analyzePipelineDepthScan.cc

Executables = analyzePipelineDepthScan.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCRawToDigi.a \
        $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDigi.a

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(BUILD_HOME)/emu/dqm/cmssw/src/common \
	$(BUILD_HOME)/emu/base/include \
	$(CMSSW)

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
Libraries = $(XdaqLibraries)

#
# Compile the source files and create a shared library
#
DynamicLibrary = dcfebpipeline
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
