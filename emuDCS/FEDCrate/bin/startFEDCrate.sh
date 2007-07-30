#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi
# kill all currently running xdaq processes
#killproc xdaq.exe
killall -q $XDAQ_ROOT/daq/xdaq/bin/linux/x86/xdaq.exe

# cd to proper home directory
cd $XDAQ_ROOT/emu/emuDCS/FEDCrate/

# link images
ln -fs $XDAQ_ROOT/emu/emuDCS/FEDCrate/*.jpg $XDAQ_ROOT/daq/examples/images/
ln -fs $XDAQ_ROOT/emu/emuDCS/FEDCrate/*.gif $XDAQ_ROOT/daq/examples/images/

# start xdaq
nohup $XDAQ_ROOT/daq/xdaq/scripts/xdaq.sh -p 40100 -c xml/EmuFCluster.xml -l INFO >& ~/log/xdaq.log &
