##
#
# This is the emusupervisor Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

#
# Packages to be built
#
Project = emu
Package = supervisor
PackageName=supervisor
Description="Emu (CSC) Supervisor"
Summary="Emu (CSC) Supervisor"
Authors="Ichiro Suzuki, Karoly Banicz, Jinghua Liu"
Link=""

Sources = \
	Application.cc \
	TCDSControl.cc \
	CIControl.cc \
	PIControl.cc \
	PMControl.cc \
	TCDSHardwareLease.cc \
	RegDumpPreprocessor.cc \
	RunInfo.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/base/include \
	$(BUILD_HOME)/emu/soap/include

LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long -Wno-variadic-macros
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects = 

#
# Compile the source files and create a shared library
#
DynamicLibrary = emusupervisor
StaticLibrary =

TestLibraries=xcept toolbox log4cplus xdata xerces-c xalan-c xdaq xdaq2rc xalanMsg xoap xgi cgicc pthttp peer asyncresolv mimetic config logudpappender logxmlappender uuid sentinelutils emubase emusoap emusupervisor
TestExecutables=testRegDumpPreprocessor.cc
# To execute:
# LD_LIBRARY_PATH=$BUILD_HOME/x86_64_slc6/lib:/opt/xdaq/lib $BUILD_HOME/emu/supervisor/bin/linux/x86_64_slc6/testRegDumpPreprocessor.exe <register_dump>

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
