BUILD_HOME:=$(shell pwd)/../..
CMSSW=$(HOME)/CMSSW/src

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
PACKAGE_VER_MAJOR=01
PACKAGE_VER_MINOR=00
PACKAGE_VER_PATCH=00

# ROOT Config
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)
ROOTGLIBS=$(shell root-config --glibs)

Project=emu
Package=web_csc_display

Sources = \
    JSON.cc \
    unpacker.cc \
    graph.cc \
    utils.cc \
    SimpleWeb.cc \
    version.cc

IncludeDirs= \
    $(ROOTSYS)/include \
    $(BUILD_HOME)/emu/daq/reader/include \
    $(BUILD_HOME)/emu/dqm/cmssw/include/emu/dqm/cmssw \
    $(CMSSW) \
    $(BUILD_HOME)/emu/dqm/common/include \
    $(XDAQ_ROOT)/include

LibraryDirs=$(ROOTSYS)/lib

UserCFlags=$(ROOTCFLAGS)
UserCCFlags=-O0 -g -Wall -Wno-long-long $(ROOTCFLAGS)
UserDynamicLinkFlags=$(ROOTLIBS)

DynamicLibrary=csc_display

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
