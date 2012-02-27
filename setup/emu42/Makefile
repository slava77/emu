# $Id: Makefile,v 1.2 2012/02/27 19:57:47 banicz Exp $

#########################################################################
# XDAQ Components for Distributed Data Acquisition                      #
# Copyright (C) 2000-2004, CERN.			                #
# All rights reserved.                                                  #
# Authors: J. Gutleber and L. Orsini					#
#                                                                       #
# For the licensing terms see LICENSE.		                        #
# For the list of contributors see CREDITS.   			        #
#########################################################################

##
#
# 
# 
#
##


#
# Packages to be built
#
BUILD_HOME:=$(shell pwd)/../../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

PackageName=emu42
Project=emu
Package=setup/$(PackageName)

PACKAGE_VER_MAJOR=1
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=1

Summary=Setup data for the CMS ME-4/2 production

Description=CMS ME-4/2 production

Authors=Johannes Gutleber, Luciano Orsini, K. Banicz

Link=http://xdaqwiki.cern.ch

# This somewhat awkward construct makes sure that at the end the variable
# DirectoryAgentIP contains the IP address irrespectively if in the hosts.setup
# there is an IP address or a hostname.
# If the query return the string pointer then an IP address was already provided
ifndef DirectoryAgentIP 
DirectoryAgentHost=$(shell perl -n -e 'if ( /([^:]*).+directory-agent/ ) {print "$$1";}' hosts.setup )
DirectoryAgentIP=$(shell host $(DirectoryAgentHost) | cut -d' ' -f 4)
ifeq ($(DirectoryAgentIP), pointer)
DirectoryAgentIP=$(DirectoryAgentHost)
endif
endif


_all: all

default: all

all:
	echo $(PackageName)  >  etc/xdaq.zone
	perl -p -e 's#%ipnumber#$(DirectoryAgentIP)#' conf/da.conf.in > conf/slp.DA.conf
	scripts/instantiateXML.awk "argument=conf/xdaqd.conf.in" hosts.setup

_cleanall: clean

clean:
	-rm -f conf/xdaqd.*.conf
	-rm -f conf/slp.DA.conf
	-rm -f etc/xdaq.zone

include $(XDAQ_ROOT)/config/mfSetupRPM.rules
