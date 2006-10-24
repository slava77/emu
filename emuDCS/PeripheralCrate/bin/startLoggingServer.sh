#!/bin/sh
# Source function library.
if [ -f /etc/init.d/functions ] ; then
        . /etc/init.d/functions
else
    echo -n 'ERROR xdaqjc: Could not find /etc/init.d/functions, stopping'
    exit -1
fi
#
killall -q /home/cscpc/DAQkit/v3.9/TriDAS/daq/extern/log4cplus/linuxx86/src/.libs/lt-loggingserver
#killproc lt-loggingserver
#
cd $XDAQ_ROOT/daq/extern/log4cplus/linuxx86/src/
nohup ./loggingserver 2924 $XDAQ_ROOT/daq/extern/log4cplus/etc/loggingserver.config >& /tmp/logging.log &
