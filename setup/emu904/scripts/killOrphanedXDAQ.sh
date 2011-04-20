#!/bin/zsh

# This script kills the orphaned XDAQ processes that are listening on the same port that is given in its arguments.


# Usage:
#    killOrphanedXDAQ.sh <command to start xdaq>
# E.g.:
#    killOrphanedXDAQ.sh ~/xdaq.sh --daemon --stdout /tmp/test.stdout --stderr /tmp/test.stderr --pidfile /tmp/test.pid -u file:/tmp/test.log -h csc-daq.cms -p 7777


# To test, issue
# ~/xdaq.sh --daemon --stdout /tmp/test.stdout --stderr /tmp/test.stderr --pidfile /tmp/test.pid -u file:/tmp/test.log -h csc-daq.cms -p 7777
# and then
# killOrphanedXDAQ.sh ~/xdaq.sh --daemon --stdout /tmp/test.stdout --stderr /tmp/test.stderr --pidfile /tmp/test.pid -u file:/tmp/test.log -h csc-daq.cms -p 7777


# Get the port number from the arguments:
PORT=$(print "$@" | sed -e 's/^[[:print:]]*xdaq[[:print:]]*-p[[:blank:]]\+\([[:digit:]]\+\)/\1/')

# Loop over the process list to select the XDAQ processes listening on $PORT
PIDSTOKILL=""
print "Orphaned processes listening on port $PORT:"
print "  PID  PPID                  STARTED CMD"
ps -eo pid,ppid,lstart,cmd | \
    grep '^[[:print:]]*xdaq[[:print:]]*-p[[:blank:]]\+'$PORT | \
    grep -v $0 | \
    while read LINE; do
        WORDARRAY=( $(print $LINE) )
        # It must be orphaned (PPID=1):
	if [[ ${WORDARRAY[2]} -eq 1 ]]; then
	    print $LINE
	    PIDSTOKILL="$PIDSTOKILL ${WORDARRAY[1]}"
	fi
    done
if [[ ${#PIDSTOKILL} -gt 0 ]]; then
    print "Executing \"kill -9 $PIDSTOKILL\""
    eval "kill -9 $PIDSTOKILL"
fi
