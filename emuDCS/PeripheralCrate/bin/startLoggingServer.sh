#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi
#
killproc lt-loggingserver
#
cd $XDAQ_ROOT/daq/extern/log4cplus/linuxx86/src/
./loggingserver 2924 $XDAQ_ROOT/daq/extern/log4cplus/etc/loggingserver.config
