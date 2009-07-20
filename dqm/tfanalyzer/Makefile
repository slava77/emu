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
Package=dqm/tfanalyzer

Sources=\
	EmuTFbookkeeper.cc \
	EmuTFxmlParsing.cc \
	EmuTFfiller.cc     \
	EmuTFvalidator.cc  \
	RegEx.cc           \
	EmuTFPlotter.cc   

Executables = runEmuTFAnalyzer.cc 

ExternalObjects = $(BUILD_HOME)/emu/dqm/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libemudqmcommon.a \
                  $(BUILD_HOME)/emu/dqm/cmssw/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libCSCDQMCommon.a 

IncludeDirs = $(XdaqIncludeDirs) \
        $(BUILD_HOME)/emu/dqm/common/include/ \
	$(BUILD_HOME)/emu \
	${CMSSW}

LibraryDirs = $(XdaqLibraryDirs) \
	$(BUILD_HOME)/common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS) -DBASE="\"$(BUILD_HOME)/${Project}/${Package}/\"" 
UserDynamicLinkFlags = $(ROOTLIBS) 
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS) -Xlinker -rpath -Xlinker $(BUILD_HOME)/${Project}/${Package}/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries) emudaqreader

#
# Compile the source files and create a shared library
#
DynamicLibrary = emudqmtfanalyzer
StaticLibrary =


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
