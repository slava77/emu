##
#
# This is the TriDAS/emu/extern/dim Makefile
#
# To Do:
#    - separate out various test builds, more explicit make
#    - introduce add'l default targets (i.e. _testall, _installall)
##


#
# Packages to be built
#
#XDAQ_BACK_TO_ROOT:=../../..
XDAQ_BACK_TO_ROOT:=$(XDAQ_ROOT)
include $(XDAQ_BACK_TO_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=extern/dim

## explicitly set DIM variables
export OS=$(shell uname -s)
export DIMDIR=$(XDAQ_ROOT)/$(Project)/$(Package)/$(XDAQ_OS)$(XDAQ_PLATFORM)
export ODIR=$(XDAQ_OS)

include $(XDAQ_ROOT)/config/Makefile.rules

_buildall:
	-rm -rf $(XDAQ_OS)$(XDAQ_PLATFORM);
	unzip -q dim_v15r8.zip; \
	mv dim $(XDAQ_OS)$(XDAQ_PLATFORM); \
	cd ./$(XDAQ_OS)$(XDAQ_PLATFORM); \
	chmod +x ./setup.sh; \
        ./setup.sh; \
	make

_cleanall:
	cd ./$(XDAQ_OS)$(XDAQ_PLATFORM); \
	make clean

_releaseall:
	$(MakeDir) $(ReleaseDir)/TriDAS/$(Project)/$(Package)/$(XDAQ_OS)$(XDAQ_PLATFORM); \
	$(Copy) $(XDAQ_ROOT)/$(Project)/$(Package)/Makefile  $(ReleaseDir)/TriDAS/$(Project)/$(Package); \
	$(Copy) $(XDAQ_ROOT)/$(Project)/$(Package)/dim_v15r8.zip  $(ReleaseDir)/TriDAS/$(Project)/$(Package); \
	$(CopyDir) $(XERCESCROOT) $(ReleaseDir)/TriDAS/$(Project)/$(Package)

