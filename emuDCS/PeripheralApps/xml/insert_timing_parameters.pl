#!/usr/bin/perl
#################################################################################
# usage:
#    perl insert_timing_parameters.pl <parameter_file> <xml_filename_to_update>
#################################################################################
#
# Make sure we're called correctly 
if ((-e $ARGV[0]) && (-e $ARGV[1])) {

    $insertfilename = $ARGV[0];
    $xmlfilename = $ARGV[1];

    #make sure files exist
    open(OLDFILE, $insertfilename) || die "Cannot open file $insertfilename!\n";
    @insertparameters = readline(OLDFILE);
    close(OLDFILE);
    
    open(XMLFILE, $xmlfilename) || die "Cannot open file $xmlfilename!\n";
    @xmlparameters = readline(XMLFILE);
    close(XMLFILE);

    # The following code determines if the "slot" tag appears in the logfile; if not, use default values
    $useslots = 0;
    my @slotvals;
    
    for($i=0;$i<scalar(@insertparameters);$i++) {
	if($insertparameters[$i] =~ m/slot/) {
	    ($text,$slot,$tmb) = split ' ', $insertparameters[$i];
	    $slotvals[$slot]=$tmb;
	    $useslots++;
	}
    }

    if(!($useslots)) {
	# default values
	$slotvals[0]=2;
	$slotvals[1]=4;	
	$slotvals[2]=6;	
	$slotvals[3]=8;	
	$slotvals[4]=10;	
	$slotvals[5]=14;	
	$slotvals[6]=16;	
	$slotvals[7]=18;	
	$slotvals[8]=20;	
    }
    
    $section = -5;
    #
    #$numberofinsert = scalar(@insertparameters);
    #print "ready to insert $numberofinsert lines...";
    #$numberofxml = scalar(@xmlparameters);
    #print "into $numberofxml lines...\n";
    #
    foreach $insertparameter (@insertparameters) {
	#
	#print "input file:  line =  $insertparameter";
	#
	# omit blank lines
	if(!($insertparameter =~ m/^$/)) {
	    #
	    if ($insertparameter =~ m/VCC/) {
		($field1,$field2,$field3) = split ' ', $insertparameter;
		# check if the parameter file has a value for the VCC, otherwise keep the value in the xml file...
		if ($field3>0) {
		    $newVCCvalue = sprintf("%x", $field3); 
		    if($field3<10) {
			$newVCCvalue = "0" . $newVCCvalue;
		    }
		} else {
		    $newVCCvalue = -2;
		}
	    }
	    #
	    # The 'section' variable determines when we've reached the Timing Scans section of the logfile.
	    # It's set at -5 to start, and gets set to -2 when we see the text "Timing scans".
	    # Then when the actual data values appear, we have $section == 0. We reset this to 0 every time
	    # we see "Timing scans" again.
	    if ($section>-5) {
		$section++;
	    }
	    
	    if ($insertparameter =~ m/Timing scans/) {
		$section = -2;
	    };
	    
	    # Start over at a new block...
            # (N.B. in old summary file, it is possible to begin the section NOT with the TMB slot)
	    if(($useslots && $insertparameter =~ m/slot/) || (!($useslots) && $insertparameter =~ m/cfeb0delay/)) {
		$section = 0;
		$alreadyPrintedTMB=0;
	    }
 
	    if ($section >= 0) {
		#
		($insert1,$insert2,$insert3) = split ' ', $insertparameter;
		$insert1 =~ s/^\s+//;  #remove leading whitespace
		#
		$current_tmb=$slotvals[$insert2];
                #
		$in_current_tmb=0;
		#
		foreach $xmlparameter (@xmlparameters) {
		    #print "xml file: line $xmlparameter...";
		    ($parametername,$secondhalf) = split '=', $xmlparameter;
                    $parametername =~ s/^\s+//;  #remove leading whitespace
		    #print "has parametername=$parametername, in_current_tmb = $in_current_tmb\n";
		    #
		    # Check to make sure we're in section for the correct TMB
		    if($parametername =~ m/<TMB slot/) {
			($dontcare1,$slotnumber,$dontcare2) = split '\"', $secondhalf;
                        if ($slotnumber == $current_tmb) {
			    #print "xml file:  YES in correct TMB slot = $slotnumber, replace $insert1\n";
			    $in_current_tmb=1;
			} else {
			    $in_current_tmb=0;
			}
		    }
		    # 
		    if($insert1 eq "mpc_delay") {    #old value possible in old summary file
			$insert1="mpc_rx_delay";
		    } 
		    #
		    if( ($in_current_tmb) &&                  #update parameter for this TMB only 
			($parametername eq $insert1) &&       #update parameter with this specific name
			($insert3 >= 0) ) {                   #flag that this parameter should be updated
			if ($alreadyPrintedTMB == 0) {
			    print "For TMB slot $current_tmb...\n";
			    $alreadyPrintedTMB=1;
			}
			print "old value $xmlparameter";
			$xmlparameter =~ s/$insert1=\"[0-9]{1,}\"/$insert1=\"$insert3\"/;
			print "new value $xmlparameter";
		    }
		}
	    }
	}
    }  

    # Finally, update the MAC address
    foreach $xmlparameter (@xmlparameters) {
	if ($newVCCvalue>0) {
	    if ($xmlparameter =~ m/<PeripheralCrate/) {
		$xmlparameter =~ s/:[0-9A-F]{2}\"/:$newVCCvalue\"/;
	    }
	}
    }

    $xmlfilename =~ s/\./_new\./;
    open(XMLFILE, ">$xmlfilename") || die "Cannot open file $xmlfilename!\n";
    foreach $xmlparameter (@xmlparameters) {
	print XMLFILE $xmlparameter;
    }
    close(XMLFILE);
    
} else { 
    die "Usage: perl insert_timing_parameters.pl <parameter_file> <xml_filename_to_update>";
}
