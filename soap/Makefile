##
#
# This is the emusoap Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

#
# Packages to be built
#
Project=emu
Package=soap
PackageName=soap
Description="SOAP messenger for Emu (CSC) applications."
Summary="SOAP messenger for Emu (CSC) applications."
Authors="Karoly Banicz"
Link=""

Sources = \
	Attachment.cc \
	Attributes.cc \
	Parameters.cc \
        Messenger.cc \
        ToolBox.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS) \
	$(BUILD_HOME)/emu/soap/include


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
DynamicLibrary = emusoap
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
