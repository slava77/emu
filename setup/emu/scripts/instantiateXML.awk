#! /bin/awk -f
BEGIN {
	IGNORECASE=1;
	FS=":";
	"hostname -s" | getline hostname;
	#print "HOSTNAME IS: [",hostname, "]";
	pattern = "^" hostname;
}

#$1 ~ pattern { 
/^[^#]/ { 
		split(argument, fname, ".");
		filename = fname[1] "." $1 "." fname[2];
		#print "Service ", fname[1];
		#print "List ", $5;
		split( fname[1],service, "/");

		# split services into an array and check if service to instantiate can be found
		split( $5, services, " ");
		found = 0;
		for ( i in services) {
			if( services[i] == service[2]) {
				found = 1;
			}
		}

		if ( (found != 0  ) || (service[2] == "default") || ( service[2] == "xdaqd")) {
			print "Creating ",filename;

			command = "host " $1;
			command | getline host;
			split (host, ipnumber, " ");

			system ("sed 's/%row/" $2 "/g' " argument "> " filename);
			system ("sed --in-place 's/%ipnumber/" ipnumber[4] "/g' " filename );
			system ("sed --in-place 's/%nodetype/" $3 "/g' " filename );
			system ("sed --in-place 's/%domain/" $4 "/g' " filename );
			system ("sed --in-place 's/%service/" $5 "/g' " filename );
		}
		
}

END {
	print "Finished processing";
}

