##
#
# This is the emubase Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

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
PACKAGE_VER_MAJOR=07
PACKAGE_VER_MINOR=06
PACKAGE_VER_PATCH=00

Sources = \
        Supervised.cc \
	WebReporter.cc \
	WebReportItem.cc \
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

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
