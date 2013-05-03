export XDAQ_OS=${XDAQ_OS:-linux}
export XDAQ_PLATFORM=${XDAQ_PLATFORM:-x86_64_slc5}
export LD_LIBRARY_PATH=$BUILD_HOME/emu/dqm/cmssw/lib/$XDAQ_OS/$XDAQ_PLATFORM:$LD_LIBRARY_PATH
export ICONLOCATION=$BUILD_HOME/emu/fast_daq/csc_display/src/icons
export DAQHOME=$BUILD_HOME/emu/fast_daq
export DATADIR=$BUILD_HOME/emu/fast_daq
