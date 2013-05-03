#!/bin/zsh
export XDAQ_OS=${XDAQ_OS:-linux}
export XDAQ_PLATFORM=${XDAQ_PLATFORM:-x86_64_slc5}

ROOTINST=( $(print /opt/cern/root_*(/) ) )
export ROOTSYS=${ROOTSYS:-$ROOTINST[-1]}

# export BUILD_HOME=${BUILD_HOME:-${1:h}/../../..}
# export LD_LIBRARY_PATH=$BUILD_HOME/emu/dqm/cmssw/lib/$XDAQ_OS/$XDAQ_PLATFORM:$ROOTSYS/lib
# export ICONLOCATION=$BUILD_HOME/emu/fast_daq/csc_display/src/icons
# export DAQHOME=$BUILD_HOME/emu/fast_daq
# export DATADIR=$BUILD_HOME/emu/fast_daq
# $BUILD_HOME/emu/fast_daq/csc_display/lib/csc_display $1


export XDAQ_ROOT=${XDAQ_ROOT:-/opt/xdaq}
export LD_LIBRARY_PATH=$XDAQ_ROOT/lib:$ROOTSYS/lib
export ICONLOCATION=$XDAQ_ROOT/htdocs/emu/cscdisplay/images
export DAQHOME=/tmp
export DATADIR=/tmp
print "Environment variables"
print "    ROOTSYS=$ROOTSYS"
print "    XDAQ_ROOT=$XDAQ_ROOT"
print "    LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
print "    ICONLOCATION=$ICONLOCATION"
print "    DAQHOME=$DAQHOME"
print "    DATADIR=$DATADIR"
$XDAQ_ROOT/bin/csc_display $1
