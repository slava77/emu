#/usr/local/bin/perl

opendir TOP, ".";
@catalogue = readdir TOP;
closedir TOP;

@RunList = reverse sort @catalogue;

$run_list = "var RUNS = [\n";

foreach $dir (@RunList){
        if( $dir !~ /plots/ ){ next; }
        $paths = `find $dir -type d`;


	%layout = ();
	map {
		if( $_=~/SP\d+\/F\d+\/CSC\d+/ ){
			$SP = $_;
			$SP =~ s/.*?\/(SP\d+)\/F\d+\/CSC\d+.*/$1/g;
			$FA = $_;
			$FA =~ s/.*?\/SP\d+\/(F\d+)\/CSC\d+.*/$1/g;
			$CSC = $_;
			$CSC =~ s/.*?\/SP\d+\/F\d+\/(CSC\d+).*/$1/g;
			if( $layout{$FA}!~/$CSC/ ){
				$layout{$FA} .="'$CSC',";
			}
		}
	} split('\n',$paths);

	$time = (stat( $dir ))[9];
	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($time);
	$ctime = ($hour<10?"0".$hour:$hour).":".($min<10?"0".$min:$min)." ".($mon<9?"0".$mon+1:$mon+1)."/".($mday<10?"0".$mday:$mday)."/".($year+1900)	;

	$run = "['$dir ($ctime)'";
	if( %layout ){
		$run .= ",['$SP', [";
		foreach $FA (keys %layout){
			$run .= "['$FA', [$layout{$FA}]],";
		}
		$run .= "]],"
	}
	$run .= "],";
	$entries{$time} = $run."\n";
}

foreach $time (reverse sort keys %entries){
	$run_list .= $entries{$time};
}


$run_list =~ s/,\]/\]/g;
$run_list =~ s/,\n$/\n/g;
$run_list .= "];";
print $run_list."\n"

