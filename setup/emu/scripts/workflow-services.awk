#! /bin/awk -f
function printEntry(url, service) {
	print "  <cfg:service>" >> filename
	print "    <cfg:url>" url "</cfg:url>" >> filename
	print "    <cfg:type>" service "</cfg:type>" >> filename
	print "    <cfg:missingdep>false</cfg:missingdep>" >> filename
	print "  </cfg:service>" >> filename
}

function getPort(filename) {
	cmd = "cat " filename " | grep ^XDAQ_PORT | sed -r \"s/^XDAQ_PORT=//\""
	cmd | getline port;
	return port;
}

BEGIN {
	IGNORECASE=1;
	FS=":";
	"hostname -s" | getline hostname;
	#print "HOSTNAME IS: [",hostname, "]";
	pattern = "^" hostname;
	filename = "conf/workflow-services.conf";
	system ("rm -f " filename)
	print "Generate workflow-services.conf";
	print "<cfg:GetServiceListResponse xmlns:cfg=\"http://xdaq.web.cern.ch/xdaq/wsdl/2007/config.wsdl\">" > filename
}

#$0 ~ pattern { 
/^[^#]/ {
		# add jobcontrol entries
		if ( index($5,"jobcontrol") != 0) {
			service = "xdaq::jobcontrol";
			url = "http://" $1 ":" getPort("conf/jobcontrol.conf.in") "/urn:xdaq-application:service=jobcontrol";
			printEntry(url, service);
		}

		# add ws-eventing service for states
		if ( index($5,"ws-eventing-state") != 0) {
			service = "xdaq::ws::eventing/state";
			url = "http://" $1 ":" getPort("conf/ws-eventing-state.conf.in") "/urn:xdaq-application:service=ws-eventing";
			printEntry(url, service);
		}

		# add netflow services
		if ( index($4, "netflow") !=0 ) {
			port = 9910
			print "netflow services found"
			cmd = "cat conf/" $4 ".in | sed 's/%host/" $1 "/g' | sed 's/%port/" port "/g'";
			while ( (cmd | getline e) >0 )
				print e >> filename;
		}

		if ( index($4,"directory-agent") != 0) {
			printEntry("http://" $1 ":427", "daemon::slp/da");
		}
		else {
			printEntry("http://" $1 ":427", "daemon::slp/sa");
		}
}

END {
	print "</cfg:GetServiceListResponse>" >> filename
	print "Finished processing";
}

