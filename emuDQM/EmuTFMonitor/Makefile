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
Package=emuDQM/EmuTFMonitor

Sources=\
	EmuTFbookkeeper.cc \
	EmuTFxmlParsing.cc \
	EmuTFfiller.cc     \
	EmuTFvalidator.cc  \
	RegEx.cc

Executables = EmuTFtest.cc 

ExternalObjects = 

IncludeDirs = $(XdaqIncludeDirs) \
        $(XDAQ_ROOT)/$(Project)/$(Package)/../Common/include/ \
	/home/slice/kotov/CMSSW_0_8_0_pre2/src \
	$(XDAQ_ROOT)/$(Project)

LibraryDirs = $(XdaqLibraryDirs) 

UserSourcePath = 

UserCFlags = $(ROOTCFLAGS)
UserCCFlags = $(ROOTCFLAGS) -DBASE="\"${HOME}/TriDAS/emu/emuDQM/\"" -I${HOME}/CMSSW/src 
UserDynamicLinkFlags = $(ROOTLIBS) 
UserStaticLinkFlags =
UserExecutableLinkFlags += $(ROOTCFLAGS) -Xlinker -rpath -Xlinker ${HOME}/TriDAS/emu/emuDQM/EmuTFMonitor/lib/linux/x86/ 

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = $(XdaqLibraries)  

#
# Compile the source files and create a shared library
#
DynamicLibrary = EmuTFMonitor
StaticLibrary =


MakeFilename=Makefile
include $(XDAQ_ROOT)/config/Makefile.rules
