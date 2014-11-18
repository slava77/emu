##
#
# This is the emubase Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

#
# Packages to be built
#
Project=emu
Package=base
PackageName=base
Description="The Emu (CSC) application base class"
Summary="The Emu (CSC) application base class"
Authors="Ichiro Suzuki, Karoly Banicz"
Link=""

Sources = \
	Supervised.cc \
	WebReporter.cc \
	WebReportItem.cc \
	Fact.cc \
	FactCollection.cc \
	FactRequest.cc \
	FactFinder.cc \
	Component.cc \
	ApplicationStatusFact.cc \
	Stopwatch.cc \
	utils/Cgi.cc \
	utils/DOM.cc \
	utils/String.cc \
	utils/Xalan.cc \
	utils/System.cc \
	utils/SimpleTimer.cc \
	utils/SerializableFactory.cc \
	utils/Chamber.cc \
	utils/Progress.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/base/include


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
DynamicLibrary = emubase
StaticLibrary =

TestLibraries = xerces-c xalan-c xdaq xdata mimetic log4cplus logxmlappender logudpappender toolbox xoap cgicc xcept xgi peer asyncresolv config uuid emubase numa

TestExecutables= \
	testSystem.cc \
	testException.cc \
	testDOM.cc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
