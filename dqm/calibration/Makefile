##
#
# This is the TriDAS/emu/dqm/calibration Package Makefile
#
##
ifeq ("$(USELOCALBUILD)", "yes")
INST_BUILD_HOME:=$(TRIDASDIST)
$(info using INST_BUILD_HOME=$(TRIDASDIST))
else 
INST_BUILD_HOME:=$(BUILD_HOME)
endif  
EMU_DQM_CONFIG = $(INST_BUILD_HOME)/emu/dqm/config
include $(EMU_DQM_CONFIG)/compdef.mk
include $(EMU_DQM_CONFIG)/xdaq.mk
include $(EMU_DQM_CONFIG)/site.mk


#OPT_FORMAT = METB_FORMAT
USE_DDUREADOUT = DDU


include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#DebugFlags=
#CFlags+= 
#CCFlags= -g -O1 -Wall
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
	Test_CFEB04.cc \
	Test_AFEB05.cc \
	Test_AFEB06.cc \
	Test_AFEB07.cc \
	Test_11_AFEBNoise.cc \
	Test_16_CFEBConnectivity.cc \
	Test_19_CFEBComparators.cc \
	Test_21_CFEBComparatorLogic.cc \
	Test_25_ALCTTrigger.cc \
	Test_30_PipelineDepth.cc \
	Test_GasGain.cc


Executables = runEmuCalibAnalysis.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	$(INST_BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a \
	$(INST_BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCRawToDigi.a \
        $(INST_BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDigi.a 
ifeq ("$(USELOCALBUILD)", "yes")
ExternalObjects =
endif

IncludeDirs = $(XdaqIncludeDirs)
ifeq ("$(USELOCALBUILD)", "yes")
#put the central install includes first (start from local, then look in central)
IncludeDirs += $(BUILD_HOME)/$(XDAQ_PLATFORM)/include
IncludeDirs += $(INST_BUILD_HOME)/$(XDAQ_PLATFORM)/include
endif
IncludeDirs += 	$(INST_BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
	$(INST_BUILD_HOME)/emu/dqm/common/include \
	$(INST_BUILD_HOME)/emu/daq/reader/include \
	$(CMSSW) \
	$(BOOST) \
	$(ORA_DIR)/include \
	$(SQLITE)/include


LibraryDirs = $(XdaqLibraryDirs) $(SQLITE)/lib 
ifeq ("$(USELOCALBUILD)", "yes")
LibraryDirs += $(INST_BUILD_HOME)/$(XDAQ_PLATFORM)/lib
endif


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
ifeq ("$(USELOCALBUILD)", "yes")
Libraries += emudqmcommon CSCRawToDigi CSCDigi
endif


#
# Compile the source files and create a shared library
#
DynamicLibrary = emucalibanalysis 
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
