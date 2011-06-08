#!/bin/zsh
# This script kills the orphaned XDAQ process that is listening on the port given as its argument.

# Example:
# USER=cscdaq killOrphanedXDAQ.sh 20200

PORT=$1
LOGFILE=/tmp/killOrphanedXDAQ_${USER-unknownUser}.log

# Loop over the process list to select the XDAQ process listening on $PORT
{
    echo "-------------------------------------------"
    date
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
	date
	print "Executing \"kill -9 $PIDSTOKILL\""
	eval "kill -9 $PIDSTOKILL"
    fi
} >&1 >> $LOGFILE 2>&1
