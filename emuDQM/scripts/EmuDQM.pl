#!/usr/local/bin/perl -w

use strict;
use POSIX;

my $EMU_DATA_FOLDER  = $ARGV[0];
my $EMU_MONITOR_TEST = $ARGV[1]; 
my $EMU_ROOT_MACRO   = $ARGV[2];
my $CADAVER          = "~/bin/cadaver"; 
#"/home/slice/kotov/cadaver-0.22.3/cadaver";
my $WEB              = "https://cms-csc.web.cern.ch:444/cms-csc/";
#"https://test-emu-dqm.webtest.cern.ch:444/test-emu-dqm";

# Read content of the target derictory - analog of `/bin/find . -type f`
sub scan_dir {
	my ( $cwd ) = @_;
	my @file_list;
	opendir DIR, $cwd or die "Cannot read directory $cwd";
	my @catalogue = grep { $_!~/^\.$/ && $_!~/^\.\.$/ } readdir DIR;
	closedir DIR;
	push @file_list, map ( -d $_ ? scan_dir($_) : $_ , grep { -f $_ || -s $_ || -d } map ( "$cwd/$_" , @catalogue ) );
	return @file_list;
}

# Make a list of all files that belong to the same run and same RUI (i.e. csc_(run_number)_EmuRUIxx_Monitor_(part_number).raw)
sub combine_parts {
	my %runs;
	foreach my $file ( @_ ) {
		my $run = $file;
		$run =~ s/Monitor_\d+/Monitor/g;
		$run =~ s/Monitor-\d+/Monitor/g;
		$runs{$run} .= " ".$file;
	}
	return %runs;
}

# Windows has problems with underscores
foreach my $file ( grep { $_=~/\.raw$/ } scan_dir($EMU_DATA_FOLDER) ){
	my $top = "./";
	map {
		my $new_name = $_;
		$new_name =~ s/_/-/g;
		if( -e "$top/$_" && $_ ne $new_name ){
			rename $top."/".$_, $top."/".$new_name or die "Can't rename $top/$_ to $top/$new_name";
		}
		$top .= "/$new_name";
	} split(/\/*\.?\/+/,$file);
}

my %runs = combine_parts(grep { $_=~/\.raw$/ } scan_dir($EMU_DATA_FOLDER));

# Generate and upload DQM plots 
foreach my $run ( keys %runs ) { 
	# Generate root file
	if( $runs{$run} ne " $run" ){
		# Merge multiple files through named pipe and process it
		unlink "_fifo_" if( -e "_fifo_" );
		POSIX::mkfifo("_fifo_", 0666) or die "Can't create named pipe";
		my $pid = fork();
		die "Can't fork" unless( defined($pid) );
		if( $pid==0 ){
			exec "cat $runs{$run} > _fifo_";
		}
		die "Can't run EmuMonitorTest" if system("$EMU_MONITOR_TEST _fifo_ > dqm_results.log");
		unlink "_fifo_" or die "Can't delete named pipe";
		waitpid $pid, 0;
	} else {
		die "Can't run EmuMonitorTest" if system("$EMU_MONITOR_TEST $run > dqm_results.log");
	}
	# Prepare local files/folders
	$run =~ s/\.raw//g;
	$EMU_DATA_FOLDER =~ s/\./\\./g;
	$run =~ s/$EMU_DATA_FOLDER//g;
	$run =~ s/^\/*//g;
	my $basename = $run; 
	my $dirname  = $run;
	$basename =~ s/.*\///g;
	$dirname  =~ s/$basename//g;
	unless( -d "./$dirname" ){
		mkdir "./$dirname" or die "Can't create directory";
	}
	rename "dqm_results.root", "$run.root" or die "Can't move dqm_results.root to $run.root";
	rename "dqm_results.log",  "$run.log"  or die "Can't move dqm_results.log to $run.log";
	# Converting to png plots
	die "Can't convert plots" if system("root.exe -b $EMU_ROOT_MACRO'(\"$run.root\")' >> $run.log");
	#  Perform actual uploading (.png and .html files only)
	my %content;
	map {
		my $folder = $_;
		$folder =~ s/(.*\/).*/$1/g;
		my $file = $_;
		# Simplyfy file names for windows
		$file =~ s/:\s*/_/g;
		$file =~ s/\s+/_/g;
		$file =~ s/\.\././g;
		$file =~ s/\,/_/g;
		if( $file ne $_ ){
			rename $_, $file or die "Can't switch ':' in file names";
		}
		$file =~ s/.*\///g;
		#$file =~ s/ /\\ /g;
		$content{$folder} .= "  $file";
	} grep { $_=~/\.png/ || $_=~/\.html/ } scan_dir("plots/$run.plots");

	my $cadaver_script="";

	foreach my $folder ( sort keys %content ){
		#$cadaver_script .= "cd /test-emu-dqm/SliceTest/DAQ/\n";
		$cadaver_script .= "cd /cms-csc/DQM/DAQ/\n";
		my $depth;
		map { $cadaver_script.="mkdir $_\ncd $_\n"; $depth.="../"; } split(/\/*\.?\/+/,$folder);
		$cadaver_script .= "lcd $folder\n";
		$cadaver_script .= "mput *\n";
		$cadaver_script .= "lcd $depth\n";
	}
	#$cadaver_script .= "ls\nquit\n";

	die "Can't upload plots" if system("echo -e \"$cadaver_script\" | $CADAVER $WEB >> $run.log");

	# Generate navigation
	my %layout;

	map {
		if( $_=~/crate\d+\/slot\d+/ ){
			my $crate = $_;
			$crate =~ s/.*?\/(crate\d+)\/slot\d+.*/$1/g;
			my $slot  = $_;
			$slot  =~ s/.*?\/crate\d+\/(slot\d+).*/$1/g;
			if( $layout{$crate}!~/$slot/ ){
				$layout{$crate} .="'$slot',";
			}
		}
	} scan_dir("./plots/$run.plots");

	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime();
	my $ctime = ($hour<10?"0".$hour:$hour).":".($min<10?"0".$min:$min)." ".($mon<9?"0".$mon+1:$mon+1)."/".($mday<10?"0".$mday:$mday)."/".($year+1900);

	my $entry = "['$run.plots ($ctime)',";
	foreach my $crate (keys %layout){
		$entry .= "['$crate', [$layout{$crate}]],";
	}
	$entry .= "],";
	$entry =~ s/,\]/\]/g;
	
	#die "Can't run cadaver" if system("echo -e \"cd /test-emu-dqm/SliceTest/DAQ/plots/\nget tree_runs.js\nquit\n\" | $CADAVER $WEB >> $run.log");
	die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/DAQ/plots/\nget tree_runs.js tree_runs.js\n\" | $CADAVER $WEB >> $run.log");

	open (TREE_RUNS, "< tree_runs.js") or die("Can't open tree_runs.js");
	my @list = <TREE_RUNS>;
	close (TREE_RUNS);

	open (TREE_RUNS, "> tree_runs.js") or die("Can't open tree_runs.js");
	if( scalar(@list)==0 ){
		print TREE_RUNS "var RUNS = [\n"; 
		$entry =~ s/,$//g; 
		print TREE_RUNS $entry."\n];";
	} else {
		for my $row (0..$#list){
			print TREE_RUNS $entry."\n" if( $row == 1 );
			print TREE_RUNS $list[$row];
		}
	}
	close (TREE_RUNS);

	#die "Can't run cadaver" if system("echo -e \"cd /test-emu-dqm/SliceTest/DAQ/plots/\nput tree_runs.js\nquit\n\" | $CADAVER $WEB >> $run.log");
	die "Can't run cadaver" if system("echo -e \"cd /cms-csc/DQM/DAQ/plots/\nput tree_runs.js\n\" | $CADAVER $WEB >> $run.log");
}
