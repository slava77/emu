#!/bin/sh

#DA="1.2.3.4 10.176.37.230"
. $1

#
# Replace hostname IP address in the net.slp.interfaces property
#
HOSTNAME=`hostname -i`
sed -i.bak 's/\(;\)*\(net.slp.interfaces = \).*/\2'"$HOSTNAME"'/' /etc/slp.conf
#sed -i.bak 's/;net.slp.interfaces = 192.168.0.1/net.slp.interfaces = '"$HOSTNAME"'/' /etc/slp.conf

ZONE_SETUP_DIR=/opt/xdaq/share/`cat /etc/xdaq.zone`/scripts

#
# loop over all DA addresses
# If this host is a DA, set the relevant properties
# otherwise se the DA ip addresses for communication
#

isDA="false"
for i in $DA; do
	if [ $i = $HOSTNAME ]; then
		isDA="true"
		sed -i.back 's/\(;\)*\(net.slp.isDA\).*$/net.slp.isDA = true/' /etc/slp.conf
		sed -i.back 's/\(;\)*\(net.slp.passiveDADetection\).*$/net.slp.passiveDADetection = true/' /etc/slp.conf
		sed -i.back 's/\(;\)*\(net.slp.activeDADetection\).*$/net.slp.activeDADetection = true/' /etc/slp.conf
		sed -i.back 's/\(;\)*\(net.slp.DAActiveDiscoveryInterval\).*$/net.slp.DAActiveDiscoveryInterval = 1/' /etc/slp.conf
	fi
done

if [ $isDA = "false" ]; then
	DAList=${DA// /, }
	LINE="net.slp.DAAddresses = "$DAList
	sed -i.bak 's/;*net.slp.DAAddresses =.*/'"$LINE"'/' /etc/slp.conf

# Configure discovery timeouts to be less aggressive on the directory agent
#
	sed -i.back 's/\(;\)*\(net.slp.multicastMaximumWait\).*$/net.slp.multicastMaximumWait = 60000/' /etc/slp.conf
	sed -i.back 's/\(;\)*\(net.slp.multicastTimeouts\).*$/net.slp.multicastTimeouts  = 10000,10000,10000,10000,10000,10000/' /etc/slp.conf
	sed -i.back 's/\(;\)*\(net.slp.unicastMaximumWait\).*$/net.slp.unicastMaximumWait = 30000/' /etc/slp.conf
	sed -i.back 's/\(;\)*\(net.slp.unicastTimeouts\).*$/net.slp.unicastTimeouts  = 5000,5000,5000,5000,5000,5000/' /etc/slp.conf

# Install the directory agent checking script
#
#	cp $ZONE_SETUP_DIR/slp_da.cron /etc/cron.d/
#	chmod 644 /etc/cron.d/slp_da.cron
#	/sbin/service crond restart
fi
