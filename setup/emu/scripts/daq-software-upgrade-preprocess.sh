#!/bin/sh
#
# description: stops all daq related programs, daemons and prepares for software upgrade (RedHat Linux)	

LOGFILE="/var/log/daq-software-upgrade.log"
DATE=`date`
HOSTNAME=`hostname --long`

log() {
	echo $DATE", "$HOSTNAME": "$* >> $LOGFILE
}

log "Preprocessing before Quattor managed software upgrade" 

#
# Step 1, stop xdaqd deamon
#
if [ -e /etc/init.d/xdaqd ]; then
	log "Stopping xdaqd programs" 
	/sbin/service xdaqd stop >> $LOGFILE
else
	log "xdaqd daemon script not found, killing processes manually" 
fi

#
# Step 2, if there are still xdaq.exe processes running, kill them
#
PIDLIST=`ps --no-headers -C xdaq.exe -o pid`

if [ -n "$PIDLIST" ]; then

	log "Found running xdaq.exe processes: " $PIDLIST 

	for i in $PIDLIST; do 
		kill -s 9 $i >> $LOGFILE
	done

fi

#
# Step 3, Stop slp daemon
#

PIDLIST=`ps --no-headers -C slpd -o pid`

if [ -e /etc/init.d/slp ]; then
	log "Stopping slp daemon"
	/sbin/service slp stop >> $LOGFILE
else
	log "slp daemon script not found, killing processes manually"

	for i in $PIDLIST; do 
		kill -s 9 $i >> $LOGFILE
	done
fi

exit 0
