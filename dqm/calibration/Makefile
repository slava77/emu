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
CFlags+= 
CCFlags= -g -O1 -Wall
#
# Packages to be built
#
Project=emu
Package=dqm/calibration

Sources=\
	TestCanvas_6gr1h.cc \
	TestCanvas_1h.cc \
	TestCanvas_userHisto.cc \
	Test_Generic.cc \
	Test_CFEB02.cc \
	Test_CFEB03.cc \
	Test_CFEB04.cc
	


Executables = runEmuCalibAnalysis.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	$(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a \
	$(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCRawToDigi.a \
        $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDigi.a 

IncludeDirs = $(XdaqIncludeDirs) \
	$(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(BUILD_HOME)/emu/dqm/common/include \
	$(BUILD_HOME)/daq/reader/include \
	$(CMSSW) \
	$(BOOST) \
	$(ORA_DIR)/include \
	$(SQLITE)/include


LibraryDirs = $(XdaqLibraryDirs) $(SQLITE)/lib


UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS)
UserDynamicLinkFlags = $(ROOTLIBS)
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTGLIBS)

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries) emudaqreader Minuit sqlite3


#
# Compile the source files and create a shared library
#
DynamicLibrary = emucalibanalysis 
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
