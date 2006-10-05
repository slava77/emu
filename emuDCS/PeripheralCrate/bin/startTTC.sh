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
cd 
#
~/bin/xdaq.sh -p 40110

