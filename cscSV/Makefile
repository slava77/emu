# $Id: Makefile,v 3.4 2007/06/06 16:48:00 banicz Exp $

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
Project = daq
Package = CSCSupervisor

Sources = \
        EmuApplication.cc\
	CSCSupervisor.cc \
	CSCSupervisorV.cc

IncludeDirs = \
	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include/ \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i2o/shared \
	$(XDAQ_ROOT)/daq/toolbox/include \
	$(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
	$(XDAQ_ROOT)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT)/daq/xdata/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/xoap/include \
	$(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/pt/include \
	$(XDAQ_ROOT)/daq/xgi/include \
	$(XDAQ_ROOT)/daq/i2o/include \
	$(XDAQ_ROOT)/daq/i2o/utils/include \
	$(XDAQ_ROOT)/emu/emuDAQ/emuUtil/include


LibraryDirs = \
	$(XDAQ_ROOT)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)


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
DynamicLibrary = $(Package)
StaticLibrary =

TestLibraries=
TestExecutables=

include ./Makefile.rules

installcss:
	$(Install) -p html/cscsv.css $(LibInstallDir)/
