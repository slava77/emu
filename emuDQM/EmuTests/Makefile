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
#CCFlags= -O2 -Wall
#
# Packages to be built
#
Project=emu
Package=emuDQM/EmuTests

Sources=\
	TestCanvas_6gr1h.cc \
	TestCanvas_1h.cc \
	TestCanvas_userHisto.cc \
	Test_Generic.cc \
	Test_CFEB04.c \
	Test_CFEB02.cc 
#	Test_CFEB03.cc \
	Test_CFEB04.cc
	
#	EmuTests.cc 


Executables = runEmuTests.cc

TestLibraries = 
TestExecutables =  

ExternalObjects = \
	../Common/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/libEmuDQMCommon.a \
	../CMSSWLibs/lib/libCSCRawToDigi.a \
        ../CMSSWLibs/lib/libCSCDigi.a 

IncludeDirs = $(XdaqIncludeDirs) \
	../CMSSWLibs \
	../Common/include \
	../../emuDAQ/emuReadout/include \
        ../../emuDAQ/emuReadout/ORCA \
	$(CMSSW) \
	$(BOOST) \
	$(ORA_DIR)/include


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
Libraries = $(XdaqLibraries) EmuReadout #cscmap1_p5 occi clntsh nnz10


#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuTests
StaticLibrary = 


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
