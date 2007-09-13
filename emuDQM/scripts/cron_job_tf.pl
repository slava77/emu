#!/usr/local/bin/perl -w

use strict;
use Time::Local;

######################## Configuration section #########################

# cscdqm@cmsusr0 :
my $WEB     = "~/cadaver https://cms-csc.web.cern.ch:444/cms-csc/";
my $SOURCE  = "/data/";
my $SCRATCH = "/nfshome0/cscdqm/scratch/";
my $LOGS    = "$SCRATCH/logs/";
my $DQMHOST = "csc-c2d07-10";
my $DATAHOST= $DQMHOST;
my $SU_DQM  = "sudo -H -u cscdqm bash -c";
my $TFDQM   = "perl /nfshome0/cscdqm/DAQKit/TriDAS/emu/emuDQM/scripts/TFDQM.pl ./ /nfshome0/cscdqm/DAQKit/TriDAS/emu/emuDQM/EmuTFMonitor/test/common/test /nfshome0/cscdqm/DAQKit/TriDAS/emu/emuDQM/scripts/drawAllSP.C";

# cscdqm@emudqm:
#my $WEB     = "~/bin/cadaver https://cms-csc.web.cern.ch:444/cms-csc/";
#my $SOURCE  = "/data/";
#my $SCRATCH = "/tmp/TrackFinder/";
#my $LOGS    = "/net/data/dqm/logs";
#my $DQMHOST = "localhost";
#my $DATAHOST= "slice\@emutf";
#my $SU_DQM  = "bash -c";
#my $TFDQM   = "perl /home/cscdqm/DAQKit/v4.2.1/TriDAS/emu/emuDQM/scripts/TFDQM.pl ./ /home/cscdqm/DAQKit/v4.2.1/TriDAS/emu/emuDQM/EmuTFMonitor/test/common/test /home/cscdqm/DAQKit/v4.2.1/TriDAS/emu/emuDQM/scripts/drawAllSP.C"; 

############################# The code ##################################
die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/TrackFinder/plots/\nget tree_runs.js tree_runs.js\n\" | $WEB >> cron_job_tf.log");

open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
my @runs_done = <RUNS>;
close RUNS;

my $data_list  = `ssh $DATAHOST 'find $SOURCE -type f -name "*RUI02*.raw" -printf "%C@ %h/%f\n" 2>/dev/null'`;
my $delta_time = `ssh $DATAHOST 'date +\%s' 2>/dev/null` - `date +\%s`;

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
	my $windows_name = $run;
	$windows_name =~ s/_/-/g;
	die "Another unfinished process" if ( -d "./tf_scratch/" );
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
		open(RUNS,"> tree_runs.js") or die "Can't write to tree_runs.js";
		foreach my $entry ( @runs_done ){
			next if( $entry =~ $windows_name );
			print RUNS $entry;
		}
		close RUNS;
		die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/TrackFinder/plots/\nput tree_runs.js\n\" | $WEB >> cron_job_tf.log");

		my $dirname = $run;
		if( $dirname =~ /\// ){
			$dirname =~ s/(.*\/).*/$1/g;
		} else {
			$dirname = "";
		}
		my $PREPARE = "$SU_DQM 'mkdir -p $SCRATCH/tf_scratch/$dirname";
		my @files = split(/\s*\d+\s+/, $runs_todo{$run});
		shift @files;
		foreach my $file ( @files ) {
			$PREPARE .= " && if [ -e $file ] ; then ln -s $file $SCRATCH/tf_scratch/$dirname 2>/dev/null; else scp -r '$DATAHOST:$file' $SCRATCH/tf_scratch/$dirname/ 2>/dev/null; fi";
		}
		$PREPARE .= " && cp tree_runs.js $SCRATCH/tf_scratch/'";
		die "Can't setup scratch folder" if system("$PREPARE");

		die "Can't process $run" if system("ssh $DQMHOST \"$SU_DQM 'source ~/.bash_profile && cd $SCRATCH/tf_scratch/ && $TFDQM'\"");
		die "Can't run cadaver"  if system("cat $SCRATCH/tf_scratch/$dirname/*.cadaver | awk 'BEGIN{print \"lcd $SCRATCH/tf_scratch/\\n\"} {print \$0}' | $WEB >> cron_job_tf.log && echo -e \"lcd $SCRATCH/tf_scratch/\ncd /cms-csc/DQM/TrackFinder/plots/\nput tree_runs.js\n\" | $WEB >> cron_job_tf.log");
		die "Can't clean up"     if system("$SU_DQM \"cd $SCRATCH/tf_scratch/ && find . -name '*.log' -exec gzip {} \\; && find . -name '*.log.gz' -exec cp {} $LOGS \\; && find . -name '*.root' -exec cp {} $LOGS/ \\; && cd ../ && rm -rf ./tf_scratch\"");

		open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
		@runs_done = <RUNS>;
		close RUNS;
	}
}
