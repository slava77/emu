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
$XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.sh -p 1971 -c xml/EmuCluster.xml -l INFO &

~/startLoggingServer.sh
