# $Id: Makefile,v 3.8 2008/03/26 22:28:13 rredjimi Exp $

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
Package = cscSV

Sources = \
        EmuApplication.cc\
	CSCSupervisor.cc \
	CSCSupervisorV.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS)\
	/nfshome0/cscdev/TriDAS/emu/cscSV/xdaq2rc/include	

LibraryDirs = $(XDAQ_ROOT)/lib

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
DynamicLibrary = CSCSupervisor
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules

installcss:
	$(Install) -p html/cscsv.css $(LibInstallDir)/
