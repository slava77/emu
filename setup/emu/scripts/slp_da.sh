#!/bin/sh
FOUND=`grep -c "receive response from DA" /var/log/slpd.log`
echo Found $FOUND directory agent loss messages
if [ "$FOUND" -gt "0" ]; then
	/sbin/service slp stop
	rm /var/log/slpd.log
	/sbin/service slp start
fi
