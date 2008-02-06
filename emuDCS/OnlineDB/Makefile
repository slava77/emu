##
#
# This is the TriDAS/emu/emuDCS/PeripheralApps Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

# Packages to be built
#
Project=emu
Package=emuDCS/OnlineDB


Sources=\
        ConnectionsDB.cc \
        ConnectionsGUI.cc 

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
StaticLibrary= EmuOnlineDB

Executables=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
