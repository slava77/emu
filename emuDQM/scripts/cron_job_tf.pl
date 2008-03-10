#!/usr/local/bin/perl -w

use strict;
use Time::Local;
use POSIX;
use List::Util qw[min max];

######################## Configuration section #########################

# cscdqm@cmsusr0 :
my $WEB     = "~/cadaver https://cms-csc.web.cern.ch:444/cms-csc/";
#my $SOURCE  = "/data1/lookarea/";
##my $SOURCE  = "/cmssrv0/nfshome0/kkotov/data/";
my $SOURCE  = "/data/";
my $SCRATCH = "/nfshome0/cscdqm/scratch/";
my $LOGS    = "$SCRATCH/logs/";
my $DQMHOST = "csc-c2d07-02";
#my $DQMHOST = "srv-C2D05-17";
##my $DATAHOST= $DQMHOST;
my $DATAHOST= "csc-daq10";
#my $SU_DQM  = "sudo -H -u cscdqm bash -c";
my $SU_DQM  = "bash -c";
my $TFDQM   = "perl /nfshome0/cscdqm/TriDAS/emu/emuDQM/scripts/TFDQM.pl ./ /nfshome0/cscdqm/TriDAS/emu/emuDQM/EmuTFMonitor/bin/linux/x86/EmuTFtest.exe /nfshome0/cscdqm/TriDAS/emu/emuDQM/scripts/drawAllSP.C";

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
# 0. Find out time difference between local pc and pc with files (important for identifying "new" files):
my $delta_time = `ssh $DATAHOST 'date +\%s' 2>/dev/null` - `date +\%s`;

# 1. Create list of processed runs:
# 1.1 Get list of processed runs from the web
die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/TrackFinder/plots/\nget tree_runs.js tree_runs.js\n\" | $WEB >> cron_job_tf.log");
# 1.2 Convert this list to array
open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
my @runs_done = <RUNS>;
close RUNS;
# 1.3 Split it to 2 categories (i.e. local and global runs):
my $local_runs_done  = "";
my $global_runs_done = "";
foreach my $entry ( @runs_done ){
	my $run = $entry;
	$run =~ s/\[\'(.*)\.plots.*$/$1/g;
	if( $run =~ /EmuRUI/ ){
		$local_runs_done  .= " $run";
	}
	if( $run =~ /Global/ ){
		$global_runs_done .= " $run";
	}
}
# 2. Find new global runs and convert them to .raw files:
# 2.1 Find all global runs (and provide timestamps for them):
my $global_runs= `ssh $DATAHOST 'find $SOURCE -type f -name "Global*.A.*0000.dat" -printf "%T@ %h/%f\n" 2>/dev/null'`;
# 2.2 Convert only new global daq files to local daq format
my %file_collection;
my %time_collection;
foreach my $file ( split(/\n/,$global_runs) ){
	my $timestamp = $file;
	$timestamp =~ s/(\d+)\s+.*/$1/g;
	$file =~ s/\d+\s*$SOURCE(.*).dat/$1/g;
	my $run = $file;
	$run  =~ s/(\w+\.\d+)\..*/$1/g;
	my $part = $file;
	$part =~ s/\w+\.\d+\.(\d+)\..*/$1/g;

	my $number = $run;
	$number =~ s/\w+\.0*(\d+)/$1/g;
	if($number<34148){ next; }

	$file_collection{$run} .= "$SCRATCH/$file.raw_760\n";
	$time_collection{$run} .= "$timestamp\n";

	map {
		if( $_ =~ $run && $_ =~ $part ){
			if( -f "$SCRATCH/$file.processing" ){ unlink "$SCRATCH/$file.processing"; }
			next;
		}
	} split(/\s+/,$global_runs_done);

	if( -f "$SCRATCH/$file.processing" ){
		print "Global run file $SOURCE/$file.dat is in progress\n";
		next;
	}
	#	die "Can't connect to srv-C2D05-17" if
			system("ssh $DQMHOST 'cd ~/CMSSW_1_7_1/src/IORawData/CSCCommissioning/test/ && ./convert.sh $SOURCE/$file.dat $SCRATCH/$file.raw' 1>$LOGS/$file.log 2>&1 && touch $SCRATCH/$file.processing");
}
# 2.3 Create list of only runs with new files
my %runs_todo;
foreach my $run ( keys %file_collection ) {
	my $run_name = $run;
	my $files    = "";
	my $process  = 1;
	foreach my $file ( split(/\n/, $file_collection{$run}) ){
		$files .= " $file";
		my $part = $file;
		$part =~ s/.*\/\w+\.\d+\.(\d+)\..*/$1/g;
		$run_name .= ".$part";
		map {
			if( $_ =~ $run && $_ =~ $part ){
				$process = 0;
			}
		} split(/\s+/,$global_runs_done);
	}
	if( $process ){
#		unlink "$SCRATCH/$run_name.raw" if( -e "$SCRATCH/$run_name.raw" );
#		POSIX::mkfifo("$SCRATCH/$run_name.raw", 0666) or die "Can't create named pipe";
#		my $pid = fork();
#		die "Can't fork" unless( defined($pid) );
#		if( $pid==0 ){
#			exec "cat $files > $SCRATCH/$run_name.raw";
#		}
		system("cat $files > $SCRATCH/$run_name.raw");
		$runs_todo{$run_name}  = max split(/\n/,$time_collection{$run});
		$runs_todo{$run_name} .= " $SCRATCH/$run_name.raw";
	}
}

# 3. Find new local daq files (i.e. local runs and converted global runs) and process them:
# 3.1 Find all local runs (and provide timestamps for them):
#my $local_runs = `ssh $DATAHOST 'find $SOURCE -type f -name "*RUI02*.raw" -printf "%C@ %h/%f\n" 2>/dev/null'`;
my $local_runs = `ssh $DATAHOST 'find $SOURCE -type f -name "csc_00000000*RUI00_Monitor*08030*.raw" -printf "%C@ %h/%f\n" 2>/dev/null'`;
print "Local runs: $local_runs\n";
# 3.2 Following procedure of combining different parts from the same run should be identical to what we have in TFDQM.pl
foreach my $file ( split(/\n/,$local_runs) ) {
	my $run = $file;
	$run =~ s/\.raw//g;
	$run =~ s/Evs.*//g;
	$run =~ s/Monitor_\d+/Monitor/g;
	$run =~ s/Monitor-\d+/Monitor/g;
	$run =~ s/^\d+\s*//g;
	$run =~ s/$SOURCE\/*//g;
	$runs_todo{$run} .= " $file";
}

foreach my $run ( sort keys %runs_todo ) {
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
		my $PREPARE = "$SU_DQM 'umask 000 && mkdir -p $SCRATCH/tf_scratch/$dirname";
		my @files = split(/\s*\d+\s+/, $runs_todo{$run});
		shift @files;
		foreach my $file ( @files ) {
			$PREPARE .= " && if [ -e $file ] ; then ln -s $file $SCRATCH/tf_scratch/$dirname 2>/dev/null; else scp -r '$DATAHOST:$file' $SCRATCH/tf_scratch/$dirname/ 2>/dev/null; fi";
		}
		$PREPARE .= " && cp tree_runs.js $SCRATCH/tf_scratch/'";
		die "Can't setup scratch folder" if system("$PREPARE");

#		die "Can't process $run" if system("ssh $DQMHOST \"$SU_DQM 'export HOME=/nfshome0/cscdqm/ && source /nfshome0/cscdqm/.bash_profile && cd $SCRATCH/tf_scratch/ && nice -19 $TFDQM'\"");
		while( system("ssh $DQMHOST \"$SU_DQM 'export HOME=/nfshome0/cscdqm/ && source /nfshome0/cscdqm/.bash_profile && cd $SCRATCH/tf_scratch/ && nice -19 $TFDQM'\"") ){
#print "Can't process $run. Trying again\n";
		}
		die "Can't run cadaver"  if system("cat $SCRATCH/tf_scratch/$dirname/*.cadaver | awk 'BEGIN{print \"lcd $SCRATCH/tf_scratch/\\n\"} {print \$0}' | $WEB >> cron_job_tf.log && echo -e \"lcd $SCRATCH/tf_scratch/\ncd /cms-csc/DQM/TrackFinder/plots/\nput tree_runs.js\n\" | $WEB >> cron_job_tf.log");
		die "Can't clean up"     if system("$SU_DQM \"cp $SCRATCH/tf_scratch/tree_runs.js ./ && cd $SCRATCH/tf_scratch/ && find . -name '*.log' -exec gzip {} \\; && find . -name '*.log.gz' -exec cp {} $LOGS \\; && find . -name '*.root' -exec cp {} $LOGS/ \\; && cd ../ && rm -rf ./tf_scratch\"");

		open(RUNS,"< tree_runs.js") or die "Can't read tree_runs.js";
		@runs_done = <RUNS>;
		close RUNS;
	}
}
