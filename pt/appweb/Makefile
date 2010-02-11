#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2007, CERN.                                        #
# All rights reserved.                                                  #
# Authors: J. Gutleber, L. Orsini, R. Moser                             #
#                                                                       #
# For the licensing terms see LICENSE.                                  #
# For the list of contributors see CREDITS.                             #
#########################################################################

##
#
# Package level makefile
#
##
BUILD_HOME:=$(shell pwd)/../../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

include $(XDAQ_ROOT)/config/mfDefs.extern_coretools
include $(XDAQ_ROOT)/config/mfDefs.coretools
include $(XDAQ_ROOT)/config/mfDefs.extern_powerpack

Project=emu
Package=pt/appweb

Sources=\
	version.cc \
	Application.cc \
	Address.cc \
	PeerTransportReceiver.cc \
	PeerTransportSender.cc \
	SOAPMessenger.cc \
	SOAPLoopbackMessenger.cc \
	CGIHandler.cc \
	SOAPHandler.cc \
	AccessHandler.cc

IncludeDirs = \
	$(XERCES_INCLUDE_PREFIX) \
	$(LOG4CPLUS_INCLUDE_PREFIX) \
	$(CGICC_INCLUDE_PREFIX) \
	$(XCEPT_INCLUDE_PREFIX) \
	$(CONFIG_INCLUDE_PREFIX) \
	$(TOOLBOX_INCLUDE_PREFIX) \
	$(XDAQ_INCLUDE_PREFIX) \
	$(XDATA_INCLUDE_PREFIX) \
	$(XOAP_INCLUDE_PREFIX) \
	$(XGI_INCLUDE_PREFIX) \
	$(PT_INCLUDE_PREFIX) \
	$(APPWEB_INCLUDE_PREFIX)

TestLibraryDirs =

LibraryDirs =

UserCFlags =
UserCCFlags = -DLINUX
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =


# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries = 

DependentLibraries =

#
# Compile the source files and create a shared library
#
DynamicLibrary=emuptappweb
StaticLibrary=
Executables=

TestExecutables= testlink.cc
TestLibraries = peer xoap toolbox log4cplus xerces-c xdata xdaq cgicc

all: _all

rom:
	$(APPWEB_SBIN_PREFIX)/httpComp rom.files > src/common/rom.cc	

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules

