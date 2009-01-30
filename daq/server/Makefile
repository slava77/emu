# $Id: Makefile,v 1.1 2009/01/30 16:36:01 banicz Exp $

##
#
# This is the xdaqctl test application Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project = emu
Package = daq/server

Sources = \
	Base.cc \
	I2O.cc \
	SOAP.cc \
	version.cc


IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/daq/server/include


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
DynamicLibrary = emudaqserver
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
