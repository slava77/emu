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
Description="Database related EMU code"


Sources=\
	BoardsDB.cc \
	ConnectionsDB.cc \
	ConnectionsGUI.cc \
	TStoreRequest.cc \
	ConfigurationEditor.cc \
	TableDefinitions.cc \
	ConfigHierarchy.cc \
	PCConfigHierarchy.cc \
	ConfigTable.cc \
	ConfigRow.cc \
	ConfigTree.cc \
	TStoreCommand.cc \
	TStoreAgent.cc \
	TStoreReadWriter.cc \
	TranslatorKludgeXMLvsDB.cc \
	XMLReadWriter.cc \
	ConfigEditor.cc \
	OnlineDBVersion.cc

#       TStoreFixer.cc \
#       DBID.cc \
#       PeripheralDBID.cc \


IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/soap/include \
	$(BUILD_HOME)/emu/emuDCS/OnlineDB/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralApps/include

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
                uuid xalan-c xalanMsg xoapfilter tstoreutils tstoreclient tstore \
                b2innub executive pthttp ptfifo xrelay hyperdaq occi nnz11 clntsh ociei \
                emubase emusoap EmuOnlineDB EmuPeripheralCore EmuPeripheralApps

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuOnlineDB
StaticLibrary= 

Executables=
TestExecutables= \
	testTableDefinitions.cc \
	testConfigRow.cc \
	testConfigTree.cc \
	testPCConfigHierarchy.cc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
