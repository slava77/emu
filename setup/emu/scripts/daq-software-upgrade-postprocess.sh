#!/bin/sh
#
# description: starts all daq related programs, daemons after a software upgrade (RedHat Linux)	

LOGFILE="/var/log/daq-software-upgrade.log"
DATE=`date`
HOSTNAME=`hostname --long`

log() {
	echo $DATE", "$HOSTNAME": "$* >> $LOGFILE
}

log "Postprocessing after Quattor managed software upgrade"

#
# Step 1, Start slp daemon
#


if [ -e /etc/init.d/slp ]; then
	log "Starting slp daemon"
	/sbin/service slp start >> $LOGFILE
else
	log "ERROR - slp daemon script not found, cannot continue."
	exit 1
fi

#
# Step 2, Check if slp is running
#
PIDLIST=`ps --no-headers -C slpd -o pid`
if [ -n "$PIDLIST" ]; then
	log "slp daemon is running, pid: " $PIDLIST
else
	log "ERROR - slp daemon failed to start, cannot continue."
	exit 1
fi

#
# Step 3, start xdaqd deamon
#
if [ -e /etc/init.d/xdaqd ]; then
	/sbin/service xdaqd start >> $LOGFILE
else
	log "ERROR - xdaqd daemon script not found, cannot continue"
fi

#
# Step 4, check if an xdaq.exe process is running
#
PIDLIST=`ps --no-headers -C xdaq.exe -o pid`

if [ -n "$PIDLIST" ]; then

	log "Found running xdaq.exe processes: " $PIDLIST
else
	log "No xdaq.exe processes running"
fi

exit 0
