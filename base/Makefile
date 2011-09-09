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
	Cgi.cc \
	DOM.cc \
	String.cc \
	Xalan.cc \
	System.cc \
	SimpleTimer.cc \
	SerializableFactory.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/base/include


LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
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

TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
	mimetic logxmlappender logudpappender asyncresolv config \
	uuid xalan-c xalanMsg xoapfilter emubase

TestExecutables= \
	testSystem.cc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
