# $Id: Makefile,v 3.2 2007/10/08 08:55:46 ichiro Exp $

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
Package = cscSV/test/LTC

Sources = \
	LTCControl.cc \
	LTCControlV.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	../../include

LibraryDirs = $(XDAQ_ROOT)/lib/

DependentLibraryDirs = $(BUILD_HOME)/$(XDAQ_PLATFORM)/lib
DependentLibraries = CSCSupervisor

UserSourcePath =
UserCFlags =
UserCCFlags =
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
DynamicLibrary = LTCControl
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules

# End of file
