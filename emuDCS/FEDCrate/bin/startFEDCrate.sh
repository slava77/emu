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
killall -q $XDAQ_ROOT/bin/xdaq.exe

# cd to proper home directory
cd $BUILD_HOME/emu/emuDCS/FEDCrate/

# link images
ln -fs $BUILD_HOME/emu/emuDCS/FEDCrate/*.jpg $BUILD_HOME/daq/examples/images/
ln -fs $BUILD_HOME/emu/emuDCS/FEDCrate/*.gif $BUILD_HOME/daq/examples/images/

# start xdaq
# old config location:  nohup $XDAQ_ROOT/daq/xdaq/scripts/xdaq.sh -p 40100 -c xml/EmuFCluster.xml -l INFO >& ~/log/xdaq.log &
<<<<<<< startFEDCrate.sh
nohup $XDAQ_ROOT/bin/xdaq.exe -p 40100 -c /home/cscdev/TriDAS/emu/emuDCS/FEDCrate/xml/EmuFCluster.xml -l INFO >& ~/log/xdaq.log &
=======
nohup $XDAQ_ROOT/bin/xdaq.exe -p 40100 -c $BUILD_HOME/emu/emuDCS/FEDCrate/xml/EmuFCluster.xml -l INFO >& ~/log/xdaq.log &
>>>>>>> 3.5
