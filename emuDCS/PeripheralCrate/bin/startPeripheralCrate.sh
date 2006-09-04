#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi
#
killproc xdaq.exe
#
cd $XDAQ_ROOT/emu/emuDCS/PeripheralCrate/
#
xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 2924 -c xml/EmuCluster.xml -l INFO &
xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 2925 -c xml/EmuCluster.xml -l INFO &
xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 2926 -c xml/EmuCluster.xml -l INFO &
xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 2927 -c xml/EmuCluster.xml -l INFO &
xterm -geometry 200x20+20+0 -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 2923 -c xml/EmuCluster.xml -l INFO &
