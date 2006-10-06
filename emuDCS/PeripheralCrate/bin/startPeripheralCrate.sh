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
#xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40011 -c xml/EmuCluster.xml -l INFO &
#xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40012 -c xml/EmuCluster.xml -l INFO &
#xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40013 -c xml/EmuCluster.xml -l INFO &
#xterm -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40014 -c xml/EmuCluster.xml -l INFO &
#xterm -geometry 200x20+20+0 -e $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40010 -c xml/EmuCluster.xml -l INFO &
#
nohup $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40011 -c xml/EmuCluster.xml -l INFO >& /tmp/xdaq40011.log &
nohup $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40012 -c xml/EmuCluster.xml -l INFO >& /tmp/xdaq40012.log &
nohup $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40013 -c xml/EmuCluster.xml -l INFO >& /tmp/xdaq40013.log &
nohup $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40014 -c xml/EmuCluster.xml -l INFO >& /tmp/xdaq40014.log &
nohup $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 40010 -c xml/EmuCluster.xml -l INFO >& /tmp/xdaq40010.log &
#
~/startLoggingServer.sh

