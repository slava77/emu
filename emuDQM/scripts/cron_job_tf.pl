#!/usr/local/bin/perl -w

use strict;
use Time::Local;

my $CADAVER = "~/bin/cadaver"; 
my $WEB     = "https://cms-csc.web.cern.ch:444/cms-csc/";
my $SOURCE  = "/data/";
my $LOGS    = "/net/data/dqm/logs";
my $TFDQM   = "perl /home/cscdqm/DAQKit/v3.9.2/TriDAS/emu/emuDQM/scripts/TFDQM.pl ./ /home/cscdqm/DAQKit/v3.9.2/TriDAS/emu/emuDQM/EmuTFMonitor/test/common/test /home/cscdqm/DAQKit/v3.9.2/TriDAS/emu/emuDQM/scripts/drawAllSP.C"; 

die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/TrackFinder/plots/\nget tree_runs.js tree_runs.js\n\" | $CADAVER $WEB >> cron_job_tf.log");

open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
my @runs_done = <RUNS>;
close RUNS;

my $data_list = `ssh -2 slice\@emutf 'find $SOURCE -type f -name "*.raw" -printf "%C@ %h/%f\n"' 2>/dev/null`;
my $delta_time = `ssh -2 slice\@emutf 'date +\%s' 2>/dev/null` - `date +\%s`; 

# Following procedure of combining different parts from the same run should be identical to what we have in TFDQM.pl
my %runs_todo;
foreach my $file ( split(/\n/,$data_list) ) {
	my $run = $file;
	$run =~ s/\.raw//g;
	$run =~ s/Evs.*//g;
	$run =~ s/Monitor_\d+/Monitor/g;
	$run =~ s/Monitor-\d+/Monitor/g;
	$run =~ s/^\d+\s*//g;
	$run =~ s/$SOURCE\/*//g;
	$runs_todo{$run} .= " $file";
}

foreach my $run ( keys %runs_todo ) {
#next if ( $run !~ /15_9_06/ );
	my $windows_name = $run;
	$windows_name =~ s/_/-/g;
	die "Another unfinisged process" if ( -d "./tf_scratch/" );
	my $process = 1;
	foreach my $entry ( @runs_done ){
		if( $entry =~ $windows_name ){
			my $latest_todo_time = 0;
			foreach my $time ( grep { $_=~/^\d+$/ } split(/ /,$runs_todo{$run}) ){
				if( $time > $latest_todo_time ){ $latest_todo_time = $time; }
			}
			$latest_todo_time -= $delta_time;
			my $last_process_time = $entry;
			if( $last_process_time =~ /.*\((\d+):(\d+)\s+(\d+)\/(\d+)\/(\d+)\).*/ ){
				$last_process_time = timelocal(0,$2,$1,$4,$3-1,$5);
			}
			if( $latest_todo_time <= $last_process_time ){ $process = 0; }
		}
	}
	if( $process ){
		open(RUNS,"> tree_runs.js") or die "Can't read tree_runs.js";
		foreach my $entry ( @runs_done ){
			next if( $entry =~ $windows_name );
			print RUNS $entry;
		}
		close RUNS;
		die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/TrackFinder/plots/\nput tree_runs.js\n\" | $CADAVER $WEB >> cron_job_tf.log");
		my $dirname = $run;
		$dirname =~ s/(.*\/).*/$1/g;
		die "Can't create $dirname" if system("mkdir -p ./tf_scratch/$dirname");
		my @files = split(/\s*\d+\s+/, $runs_todo{$run});
		shift @files;
		foreach my $file ( @files ) {
			die "Can't copy $file" if system("scp -2 -r 'slice\@emutf:$file' ./tf_scratch/$dirname/ 2>/dev/null");
		}
		die "Can't process $run" if system("cd ./tf_scratch/ && $TFDQM && cp tree_runs.js ../ && find . -name '*.log' -exec gzip {} \\; && find . -name '*.log.gz' -exec cp {} $LOGS \\; && find . -name '*.root' -exec cp {} $LOGS/ \\; && cd ../ && rm -rf ./tf_scratch");
		open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
		@runs_done = <RUNS>;
		close RUNS;
	}
}
