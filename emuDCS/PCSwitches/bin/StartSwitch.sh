#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi

#killproc xdaq.exe
#killall -q $XDAQ_ROOT/bin/xdaq.exe

# go to FED Crate home directory
cd $BUILD_HOME/emu/emuDCS/PCSwitches/

# link images
#ln -fs $BUILD_HOME/emu/emuDCS/FEDCrate/*.jpg /tmp
#ln -fs $BUILD_HOME/emu/emuDCS/FEDCrate/*.gif /tmp
#ln -fs $BUILD_HOME/emu/emuDCS/FEDCrate/*.png /tmp

# start xdaq
if [ $HOSTNAME = "vmepcS2G18-08" ]; then
  export XMLFILE="xml/Switch_GUI-pc2.xml"
  export LOGFILE="~/log/Switchpc2.log.40024"
else
  export XMLFILE="xml/Switch_GUI-pc1.xml"
  export LOGFILE="~/log/Switchpc1.log.40024"
fi
nohup $XDAQ_ROOT/bin/xdaq.exe -p 40024 -c $XMLFILE -l INFO >& $LOGFILE &
#echo $XMLFILE
