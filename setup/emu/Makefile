# $Id: Makefile,v 1.3 2008/08/06 14:56:47 rredjimi Exp $

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

PackageName = emu
Project = emu
Package=setup/$(PackageName)


PACKAGE_VER_MAJOR=1
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=0

Summary=emu job-control setup package

Description=This package is to set emu JC package

Authors=Laria Redjimi

Link=http://cern.ch/emudoc/

include $(XDAQ_ROOT)/config/mfSetupRPM.rules

