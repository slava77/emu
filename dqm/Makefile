##
#
# This is the TriDAS/emu/emuDAQ Project Makefile
#  (this Makefile will soon become obsolete:
#   only project and package Makefiles should remain)
#
##

BUILD_HOME:=$(shell pwd)/../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)


Project=emu
Package=dqm
PackageName=dqm
Description="The Emu (CSC) Local DQM"
Summary="The Emu (CSC) Local DQM"
Authors="Victor Barashko, Khristian Kotov"
Link="http://www.phys.ufl.edu/emu/csc/dqm"
PACKAGE_VER_MAJOR=1
PACKAGE_VER_MINOR=0
PACKAGE_VER_PATCH=0

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules


Packages=\
	dqm/cmssw \
	dqm/common \
	dqm/utils \
	dqm/cscanalyzer \
	dqm/cscmonitor \
	dqm/cscdisplay \
	dqm/tfanalyzer \
        dqm/tfmonitor \
        dqm/tfdisplay 

include $(XDAQ_ROOT)/config/Makefile.rules
#include $(XDAQ_ROOT)/config/mfRPM.rules


.PHONY: _rpmall
_rpmall: spec_update spec_local_update makerpm
	@echo "Building emudqm rpm: done"

.PHONY: spec_local_update
spec_local_update:
	perl -p -i -e 's#__packages__#$(Packages)#' $(PackagePath)/rpm/$(PackageName).spec


