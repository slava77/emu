#!/bin/tcsh
cd $XDAQ_ROOT/emu/emuDCS/PeripheralCrate/
if( -test xdaq.log ) rm xdaq.log
$XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40010 -c xml/EmuCluster.xml -l INFO
