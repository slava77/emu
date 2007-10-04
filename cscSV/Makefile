# $Id: Makefile,v 3.6 2007/10/04 07:22:48 ichiro Exp $

#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2004, CERN.			                #
# All rights reserved.                                                  #
# Authors: J. Gutleber and L. Orsini					#
#                                                                       #
# For the licensing terms see LICENSE.		                        #
# For the list of contributors see CREDITS.   			        #
#                                                                       #
# Modified for xdaqctl by I.Suzuki                                      #
#########################################################################

BUILD_HOME = .

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
Project =
Package = CSCSupervisor

Sources = \
        EmuApplication.cc\
	CSCSupervisor.cc \
	CSCSupervisorV.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include \
	$(XDAQ_ROOT)/include/$(XDAQ_OS)

LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath = src/common

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
DynamicLibrary = $(Package)
StaticLibrary =

TestLibraries=
TestExecutables=

#include $(XDAQ_ROOT)/config/Makefile.rules
include ./Makefile.rules

installcss:
	$(Install) -p html/cscsv.css $(LibInstallDir)/
