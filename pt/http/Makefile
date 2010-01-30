# $Id: Makefile,v 1.2 2010/01/30 16:21:30 banicz Exp $

#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2009, CERN.			                #
# All rights reserved.                                                  #
# Authors: J. Gutleber and L. Orsini					#
#                                                                       #
# For the licensing terms see LICENSE.		                        #
# For the list of contributors see CREDITS.   			        #
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


Project=emu
Package=pt/http

Sources=\
	version.cc \
	Address.cc \
	Channel.cc \
	ClientChannel.cc \
	PeerTransportReceiver.cc \
	PeerTransportSender.cc \
	ReceiverLoop.cc \
	Utils.cc \
	SOAPMessenger.cc \
	SOAPLoopbackMessenger.cc \
	PeerTransportHTTP.cc 

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
	$(PT_INCLUDE_PREFIX)

TestLibraryDirs = 

LibraryDirs = 


UserCFlags =
UserCCFlags =
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =


# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =

#
# Compile the source files and create a shared library
#
DynamicLibrary=emupthttp
StaticLibrary=
Executables=

TestExecutables= testlink.cc
TestLibraries = peer xoap toolbox log4cplus xerces-c xdata xdaq cgicc

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules

