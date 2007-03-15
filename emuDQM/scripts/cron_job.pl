#!/usr/local/bin/perl -w

use strict;

my $CADAVER = "~/bin/cadaver"; 
my $WEB     = "https://cms-csc.web.cern.ch:444/cms-csc/";
my $SOURCE  = "/net/data/dqm/";
my $REMOTE_SOURCE = "/data/cscdaq";
my $EMUDQM  = "perl /home/cscdqm/DAQKit/v.3.9.2/TriDAS/emu/emuDQM/scripts/EmuDQM.pl ./ /home/cscdqm/DAQKit/v.3.9.2/TriDAS/emu/emuDQM/EmuMonitor/src/linux/x86/EmuMonitorTest.exe /home/cscdqm/DAQKit/v.3.9.2/TriDAS/emu/emuDQM/scripts/drawAll.C"; 

die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/DAQ/plots/\nget tree_runs.js tree_runs.js\n\" | $CADAVER $WEB >> cron_job.log");

open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
my @runs_done = <RUNS>;
close RUNS;

#my $remote_data_list = `ssh -2 -f slice\@emuslice02 'find $REMOTE_SOURCE -type f -name "*EmuRUI*Monitor*.raw" -printf "%C@ %h/%f\n"' 2>/dev/null`;
#my $delta_time = `ssh -2 -f slice\@emuslice02 'date +\%s' 2>/dev/null` - `date +\%s`; 
#
# Synchronize emuslice02 and emuslice11
#my $local_file_list;
#foreach my $local_file ( glob("$SOURCE/*.raw") ){
#	$local_file_list .= " $local_file";
#}
#foreach my $remote_file ( split(/\n/,$remote_data_list) ){
#	$remote_file =~ s/^\d+\s*$REMOTE_SOURCE//g;
#	my $remote_windows_name =~ s/_/-/g;
#	if( $local_file_list !~ $remote_windows_name ){
#		system("scp -2 slice\@emuslice02:$REMOTE_SOURCE/$remote_file /csc_data/");
#	}
#}

my $local_runs_done = "";
my $global_runs_done = "";
foreach my $run ( @runs_done ){ 
	$run =~ s/\[\'(.*)\.plots.*$/$1/g;
	if( $run =~ /EmuRUI/ ){
		$local_runs_done .= " $run";
	} else {
		$global_runs_done .= " $run";
	}
}

# Following procedure of combining different parts from the same run should be identical to what we have in EmuDQM.pl
my %local_runs_todo;
foreach my $file ( glob("$SOURCE/*.raw") ) {
	my $run = $file;
	$run =~ s/\.raw//g;
	$run =~ s/Monitor_\d+/Monitor/g;
	$run =~ s/Monitor-\d+/Monitor/g;
	$run =~ s/$SOURCE\/*//g;
	$local_runs_todo{$run} .= " ".$file;
}

open(RUNS,"< $SOURCE/runs.txt") or die "Can't read $SOURCE/runs.txt";
my @runs_todo = <RUNS>;
close RUNS;

my @global_runs_todo;
my @remote_runs_todo;
foreach my $run ( @runs_todo ){
	if( $run =~ /^\d+$/ ){
		push @global_runs_todo, $run;
	} else {
		push @remote_runs_todo, $run;
	}
}

foreach my $remote_file ( @remote_runs_todo ){
	$remote_file =~ s/^\s*$REMOTE_SOURCE//g;
	$remote_file =~ s/\n//g;
	my $remote_windows_name = $remote_file;
	$remote_windows_name =~ s/_/-/g;
	foreach my $run ( @runs_done ){
		if( $run =~ $remote_windows_name ){
			$remote_windows_name = undef;
			last;
		}
	}
	if( defined($remote_windows_name) ){
		my $data_list1 = `ssh -2 slice\@emudaq01 'ls $REMOTE_SOURCE/$remote_file*' 2>/dev/null`;
		map {
			my $file_name = $_;
			$file_name =~ s/.*\///g ;
			unless( -e "$SOURCE/$file_name" || $file_name !~ /.raw/ ){
				system("scp -2 slice\@emudaq01:$REMOTE_SOURCE/$file_name $SOURCE/ 1>/dev/null");
			}
		} split (/\n/, $data_list1);

		my $data_list2 = `ssh -2 slice\@emudaq02 'ls $REMOTE_SOURCE/$remote_file*' 2>/dev/null`;
		map {
			my $file_name = $_;
			$file_name =~ s/.*\///g ;
			unless( -e "$SOURCE/$file_name" || $file_name !~ /.raw/ ){
				system("scp -2 slice\@emudaq02:$REMOTE_SOURCE/$file_name $SOURCE/ 1>/dev/null");
			}
		} split (/\n/, $data_list2);
		#system("for i in `ssh -2 slice\@emudaq02 'ls $REMOTE_SOURCE/$remote_file*' 2>/dev/null`; do if ( [ -e /csc_data/`basename \$i` ] || [ `echo \$i | sed -e 's|.*\\(\\.\\w\\+\\)\$|\\1|g'` != '.raw' ] ) ; then echo > /dev/null ; else scp -2 slice\@emudaq02:\$i /csc_data/ 1>/dev/null ; fi ; done");
	}
}

foreach my $run ( @global_runs_todo ){
	$run =~ s/\n//g;
	if ( $global_runs_done =~ $run ){
		if( -f "$SOURCE/$run.processing" ){ unlink "$SOURCE/$run.processing"; }
		next;
	}
	if( -f "$SOURCE/$run.processing" ){
		print "Global run $run is in progress\n"; 
		next;
	}
	#print "Processing Global Run: $run\n";
	#die "Can't connect to lxplus009" if system("ssh -f korytov\@lxplus009.cern.ch 'klog -principal korytov -password neuzheli! && ~/CMSSW_0_9_0/src/EventFilter/Dumper/convert.csh $run' 1>global_$run.log 2>&1 && touch /csc_data/$run.processing");
	die "Can't connect to lxplus009" if system("ssh -2 -f korytov\@lxplus009.cern.ch 'klog -principal korytov -password neuzheli! && ~/CMSSW_1_1_0/src/IORawData/CSCCommissioning/test/convert.csh $run' 1>global_$run.log 2>&1 && touch $SOURCE/$run.processing");
}

foreach my $run ( keys %local_runs_todo ){
	my $run_windows_name = $run;
	$run_windows_name =~ s/_/-/g;
	next if ( $local_runs_done =~ $run_windows_name || $global_runs_done =~ $run_windows_name );
	die "Copying file" if( -f "$SOURCE/*.copying" );
	#print "Processing Local Run: $run\n";
	die "Another unfinisged process" if ( -d "./scratch/" );
	if( system("mkdir -p ./scratch && cd ./scratch && cp $local_runs_todo{$run} ./ && $EMUDQM && find . -name '*.log' -exec gzip {} \\; && find . -name '*.root' -exec cp {} $SOURCE/logs \\; && find . -name '*.log.gz' -exec cp {} $SOURCE/logs \\; && cd ../ && rm -rf ./scratch && rm $local_runs_todo{$run}") ){
		system("rm -rf ./scratch");
		die "Can't process $run";
	}
}
