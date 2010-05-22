##
#
# This is the TriDAS/emu/emuDCS/OnlineDB Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

# Packages to be built
#
Project=emu
Package=emuDCS/OnlineDB
PackageName=OnlineDB
Description="Hardware ID libraries for DMBs and CFEBs in Peripheral Crates"


Sources=\
	BoardsDB.cc \
        ConnectionsDB.cc \
        ConnectionsGUI.cc \
        TStoreRequest.cc \
        ConfigurationEditor.cc \
	OnlineDBVersion.cc 
	

IncludeDirs = \
        $(XDAQ_ROOT)/include \
        $(BUILD_HOME)/emu/emuDCS/OnlineDB/include \

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib 

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic logxmlappender logudpappender asyncresolv config \
                uuid
                
#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuOnlineDB
StaticLibrary= 

Executables=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
