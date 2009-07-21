BUILD_HOME:=$(shell pwd)/../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=daq
PackageName=daq
Description="The Emu (CSC) local DAQ"
Summary="The Emu (CSC) local DAQ"
Authors="Steven Murray, Karoly banicz"
Link=""
PACKAGE_VER_MAJOR=7
PACKAGE_VER_MINOR=7
PACKAGE_VER_PATCH=0

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules

Packages= \
          daq/reader \
          daq/writer \
          daq/server \
          daq/rui \
          daq/ta \
          daq/fu \
          daq/manager

include $(XDAQ_ROOT)/config/Makefile.rules

.PHONY: _rpmall
_rpmall: spec_update spec_local_update makerpm
	@echo "Building emudaq rpm: done"

.PHONY: spec_local_update
spec_local_update:
	perl -p -i -e 's#__packages__#$(Packages)#' $(PackagePath)/rpm/$(PackageName).spec


# componentsrpm:
# 	for x in $(Packages); do \
# 	  make -C ../$$x rpm; \
# 	done
# 
# installcomponentsrpm:
# 	for x in $(Packages); do \
# 	  make -C ../$$x installrpm; \
# 	done
# 
# cleancomponentsrpm:
# 	for x in $(Packages); do \
# 	  make -C ../$$x cleanrpm; \
# 	done
