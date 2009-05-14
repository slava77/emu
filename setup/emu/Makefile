# $Id: Makefile,v 1.5 2009/05/14 12:49:11 gutleber Exp $

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

PackageName=emu
Project=emu
Package=setup/$(PackageName)

PACKAGE_VER_MAJOR=4
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=0

Summary=Setup data for the Emu (CSC) cluster at Cessy

Description=Emu (CSC) setup

Authors=Johannes Gutleber, Luciano Orsini, K. Banicz

Link=http://xdaqwiki.cern.ch

_all: all

default: all

all:
	echo $(PackageName)  >  etc/xdaq.zone
	scripts/instantiateXML.awk "argument=conf/xdaqd.conf.in" hosts.setup

_cleanall: clean

clean:
	-rm -f conf/xdaqd.*.conf
	-rm -f etc/xdaq.zone

include $(XDAQ_ROOT)/config/mfSetupRPM.rules
