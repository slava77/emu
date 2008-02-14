##
#
# This is the TriDAS/emu/emuDCS/base Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)


Project=emu
Package=emuDCS/e2p

#DIMDIR=$(XDAQ_ROOT)/$(Project)/extern/dim/$(XDAQ_OS)$(XDAQ_PLATFORM)
#####DIMDIR=$(HOME)/dim
DCSDIR=$(BUILD_HOME)/emu/emuDCS/e2p
#-----
#DependentLibraryDirs = $(XDAQ_ROOT)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
#DependentLibraries = CSCSupervisor EmuPeripheralCore EmuPeripheralCrate
#----- 
Sources =\
         DcsDimService.cxx \
         DcsDimCommand.cxx \
         EmuDcs.cxx \
         DcsEmuController.cc



IncludeDirs = \
        $(XDAQ_ROOT)/include \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/include \
	$(BUILD_HOME)/emu/emuDCS/PeripheralCrate/include \
        $(BUILD_HOME)/emu/cscSV/include \
        $(DCSDIR)/include
#####        $(DIMDIR)/dim

#        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCore/include \
#        $(XDAQ_ROOT)/emu/cscSV/include \
#        $(DCSDIR)/include \
#        $(DIMDIR)/dim \
#	$(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/include



#LibraryDirs = \
#	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/extern/asyncresolv/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/pt/tcp/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/pt/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#        $(DIMDIR)/$(XDAQ_OS)

TestLibraryDirs = \
	$(XDAQ_ROOT)/lib \
        $(BUILD_HOME)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/emuDCS/PeripheralApps/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(BUILD_HOME)/emu/cscSV/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
#######        $(DIMDIR)/$(XDAQ_OS)


#TestLibraryDirs = \
#	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/xcept/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/xgi/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/extern/asyncresolv/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#	$(XDAQ_ROOT)/daq/extern/log4cplus/xmlappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#	$(XDAQ_ROOT)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
#        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCore/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#        $(XDAQ_ROOT)/emu/emuDCS/PeripheralCrate/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#        $(XDAQ_ROOT)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
#        $(DIMDIR)/$(XDAQ_OS) \
#        $(DCSDIR)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)


UserCFlags = -DUSEDCS -DDCS_PRINTING_1 -DDCS_PRINTING_0 -DOSUcc -DNEW_DIM_SERVICES
UserCCFlags = -DUSEDCS -DDCS_PRINTING_1 -DDCS_PRINTING_0 -DOSUcc -DNEW_DIM_SERVICES
#UserCFlags = -DUSEDCS -DDCS_PRINTING_1 -DDCS_PRINTING_0 -DOSUcc
#UserCCFlags = -DUSEDCS -DDCS_PRINTING_1 -DDCS_PRINTING_0 -DOSUcc
#UserCFlags =
#UserCCFlags =
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
#Libraries = xerces-c xdaq log4cplus toolbox xoap
#TestLibraries = xerces-c xdaq log4cplus toolbox xoap \
#                dim EmuPeripheralCrate

#Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer dim
#TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
#                mimetic xmlappender udpappender \
#                dim EmuPeripheralCrate

#Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
#TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
#                mimetic logxmlappender logudpappender asyncresolv\
#	        dim CSCSupervisor EmuPeripheralCore EmuPeripheralCrate dcs_emu

Libraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer
TestLibraries = xerces-c xdaq xdata log4cplus toolbox xoap cgicc xcept xgi peer \
                mimetic logxmlappender logudpappender asyncresolv config \
                CSCSupervisor EmuPeripheralCore EmuPeripheralApps uuid dim dcs_emu

#
# Compile the source files and create a shared library
#
DynamicLibrary= dcs_emu
StaticLibrary= dcs_emu

Executables=
TestExecutables=\
         DcsEmuConfig.cpp
#TestExecutables= \
#        dcscontrol.cpp \
#        lvget.cpp

include $(XDAQ_ROOT)/config/Makefile.rules
