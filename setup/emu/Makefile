# $Id: Makefile,v 1.1 2008/05/28 21:23:07 rredjimi Exp $

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
##BUILD_HOME:=$(shell pwd)/../../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project = emu
Package=setup/emu
PackageName = jc

PACKAGE_VER_MAJOR=1
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=0

Summary=emu job-control setup package

Description=This package is to set emu JC package

Authors=Laria Redjimi

Link=http://cms-emu.cern.ch/

include $(XDAQ_ROOT)/config/mfSetupRPM.rules

