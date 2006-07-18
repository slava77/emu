#!/bin/tcsh
cd $XDAQ_ROOT/emu/emuDCS/FEDCrate/
if( -test xdaq.log ) rm xdaq.log
$XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40100 -c xml/EmuFCluster.xml -l INFO
