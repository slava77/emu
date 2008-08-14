#!/usr/local/bin/perl

use Net::Telnet; 
$IP = shift @ARGV;
$ARGV = shift @ARGV;
$port = shift @ARGV;
$telnet = new Net::Telnet (Port=>23, Timeout=>10, Errmode=>'die', Telnetmode=>0); 
$telnet->open($IP); 
$telnet->waitfor('/User:$/'); 
$telnet->print('admin'); 
$telnet->waitfor('/Password:$/'); 
$telnet->print(''); 
$telnet->waitfor('/\(GSM7212\) \>/'); 

$|++;

if ($ARGV eq "status") {
	$telnet->print('show port status ',$port); 
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
} elsif ($ARGV eq "interface") {
	$telnet->print('show interface ',$port); 
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
} elsif ($ARGV eq "interfaceall") {
    print '(GSM7212) >';
	$telnet->print('show interface 0/1');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/2');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
 	$telnet->print('show interface 0/3');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/4');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/5');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/6');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
 	$telnet->print('show interface 0/7');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/8');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/9');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/10');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
 	$telnet->print('show interface 0/11');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
	$telnet->print('show interface 0/12');
	@lines = $telnet->waitfor('/\(GSM7212\) \>/'); 
	print @lines;
} elsif ($ARGV eq "mac-addr-table"){
    $telnet->print('show mac-addr-table');
#	      $done=0;
#	      while ( $done <= 0  ) { 
#		  $line = $telnet->getline;
#		  $remove=0;
#		  if ( $line =~ /\-\-More\-\- or \(q\)uit/ ) {
#		      $telnet->print('');
#		      $remove=1;
#		  }
#		  if ( $line =~ /\(GSM7212\) \>/ ) {
#		      $done=1;
#		  }
#		  if ( $done <= 0 ) {
#		      if ( $remove <= 0){
#			  print $line;
#		      }
#		  }
#	      }

	my $line = 0;
	while ($line = $telnet->get()) {
	    print $line;
	    if ($line =~ /More/) {
		$telnet->print('');
	    }
	    if ($line =~ /\(GSM7212\)/) {
		last;
	    }
	}
	#my $line = 0;
	#while ($line = $telnet->get()) {
	#    #print $line;
	#    if ($line =~ /More/) {
	#	$telnet->print('');
	#    } else {
        #      print $line
        #    }
	#    if ($line =~ /\(GSM7212\)/) {
	#	last;
	#    }
	#}
	#@lines = $telnet->getlines(All => 0);
	#print @lines;
	#@lines = $telnet->getlines(All => 0);
	#print @lines;
	####
	#my $newline = $telnet->getline();
	#until($newline=~/\(GSM7212\) \>/){
	#    print $newline;
	#    if($newline=~/--More-- or \(q\)uit/){
	#        $telnet->print('');
	#    }
	#    $newline = $telnet->getline();
	#}
} elsif ($ARGV eq "reload") {
	$telnet->print('enable'); 
	$telnet->waitfor('/Password:$/'); 
	$telnet->print('');
        $telnet->waitfor('/\(GSM7212\) \#/');
        $telnet->print('reload');
	$telnet->waitfor('/Would you like to save them now\? \(y\/n\) /');
	$telnet->print('y');
        $telnet->waitfor('/System will now restart\!/');
} elsif ($ARGV eq "clrcounters") {
        $telnet->print('enable');
        $telnet->waitfor('/Password:$/');
        $telnet->print('');
        $telnet->waitfor('/\(GSM7212\) \#/');
        $telnet->print('clear counters all');
        $telnet->waitfor('/Are you sure you want to clear ALL port stats\? \(y\/n\)/');
        $telnet->print('y');
        $telnet->waitfor('/\(GSM7212\) \#/');
        $telnet->print('exit');
        $telnet->waitfor('/\(GSM7212\) \>/');        
    } elsif ($ARGV eq "problems"){
	$telnet->print('show interface ethernet 0/1');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
         }
        $telnet->print('show interface ethernet 0/2');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
	}
        $telnet->print('show interface ethernet 0/3');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
	}
        $telnet->print('show interface ethernet 0/4');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
	}
        $telnet->print('show interface ethernet 0/5');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }
        $telnet->print('show interface ethernet 0/6');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }
        $telnet->print('show interface ethernet 0/7');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
	}
        $telnet->print('show interface ethernet 0/8');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }
        $telnet->print('show interface ethernet 0/9');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }
        $telnet->print('show interface ethernet 0/10');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
	}
        $telnet->print('show interface ethernet 0/11');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }
        $telnet->print('show interface ethernet 0/12');
        my $line = 0;
        while ($line = $telnet->get()) {
            print $line;
            if ($line =~ /More/) {
                $telnet->print('');
            }
            if ($line =~ /\(GSM7212\)/) {
                last;
            }
        }

     }

