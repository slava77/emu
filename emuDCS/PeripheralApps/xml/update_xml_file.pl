#!/usr/local/bin/perl
################################################################################
# update_xml_file.pl = changes old variable names in xml files into new ones
#
# usage:
#    update_xml_file.pl <old_xml_filename> <new_xml_filename>
################################################################################
#
if ($#ARGV != 1) {
 print "usage: update_xml_file.pl <old_xml_filename> <new_xml_filename>\n";
 exit;
}
#
$oldfilename = $ARGV[0];
$newfilename = $ARGV[1];
#
open(OLDFILE, $oldfilename) || die "Cannot open file $oldfilename!\n";
@parameters = readline(OLDFILE);                      # read OLDFILE into the vector parameters
close(OLDFILE);
#
# replace old names with new names
foreach $parameter (@parameters) {
#    print "old:  $parameter";
    chomp($parameter);
#
# which section are you in?
    if ($parameter =~ m/TMB slot/) {                   # does the line match "TMB slot" anywhere?
	$section = "tmb";
#	print "in $section section....\n";
    }
    if ($parameter =~ m/ALCT chamber/) {
	$section = "alct";
#	print "in $section section....\n";
    }
    if ($parameter =~ m/DAQMB slot/) {                   # does the line match "TMB slot" anywhere?
	$section = "dmb";
#	print "in $section section....\n";
    }
#
# TMB parameters...
    if ($section eq "tmb") {
	$parameter =~ s/l1adelay/tmb_l1a_delay/;     #replace "l1adelay" with "tmb_l1a_delay"
#
	if (($parameter =~ m/l1a_window_size/) && 
	    !($parameter =~ m/tmb_l1a_window_size/) ) {  # so we don't double replace something 
	    $parameter =~ s/l1a_window_size/tmb_l1a_window_size/;
	}
#
	$parameter =~ s/mpc_delay/mpc_rx_delay/;
#
	$parameter =~ s/alct_match_window_size/match_trig_window_size/;
#
	$parameter =~ s/alct_vpf_delay/match_trig_alct_delay/;
#
	$parameter =~ s/hs_pretrig_thresh/clct_halfstrip_pretrig_thresh/;
#
	$parameter =~ s/ds_pretrig_thresh/clct_distrip_pretrig_thresh/;
#
	$parameter =~ s/min_hits_pattern/clct_pattern_thresh/;
#
	$parameter =~ s/ALCT_input/enable_alct_rx/;
#
	$parameter =~ s/layer0_distrip_hot_channel_mask/layer0_distrip_hot_chann_mask/;
	$parameter =~ s/layer1_distrip_hot_channel_mask/layer1_distrip_hot_chann_mask/;
	$parameter =~ s/layer2_distrip_hot_channel_mask/layer2_distrip_hot_chann_mask/;
	$parameter =~ s/layer3_distrip_hot_channel_mask/layer3_distrip_hot_chann_mask/;
	$parameter =~ s/layer4_distrip_hot_channel_mask/layer4_distrip_hot_chann_mask/;
	$parameter =~ s/layer5_distrip_hot_channel_mask/layer5_distrip_hot_chann_mask/;
#
	if (($parameter =~ m/bxn_offset/) && 
	    !($parameter =~ m/rpc_bxn_offset/) && 
	    !($parameter =~ m/tmb_bxn_offset/) ) {
	    $parameter =~ s/bxn_offset/tmb_bxn_offset/;
	}
#
	if (($parameter =~ m/enableCLCTInputs/) && 
	    !($parameter =~ m/enableCLCTInputs_reg42/) &&
	    !($parameter =~ m/enableCLCTInputs_reg68/) ) { 
	    $parameter =~ s/enableCLCTInputs/enableCLCTInputs_reg42/;
	}
#
	if (($parameter =~ m/fifo_mode/) && 
	    !($parameter =~ m/tmb_fifo_mode/) ) { 
	    $parameter =~ s/fifo_mode/tmb_fifo_mode/;
	}
#
	if (($parameter =~ m/fifo_pretrig/) && 
	    !($parameter =~ m/tmb_fifo_pretrig/) ) { 
	    $parameter =~ s/fifo_pretrig/tmb_fifo_pretrig/;
	}
#
	if (($parameter =~ m/fifo_tbins/) && 
	    !($parameter =~ m/tmb_fifo_tbins/) ) { 
	    $parameter =~ s/fifo_tbins/tmb_fifo_tbins/;
	}
#
	if (($parameter =~ m/l1a_offset/) && 
	    !($parameter =~ m/tmb_l1a_offset/) ) { 
	    $parameter =~ s/l1a_offset/tmb_l1a_offset/;
	}
#
# buried TMB bits in xml "trgmode" parameter...
	$parameter =~ s/trgmode=\"5\"/clct_pretrig_enable=\"0\"\n          alct_pretrig_enable=\"0\"\n          match_pretrig_enable=\"1\"\n          clct_trig_enable=\"0\"\n          alct_trig_enable=\"0\"\n          match_trig_enable=\"1\"/;
#
	$parameter =~ s/trgmode=\"1\"/clct_pretrig_enable=\"1\"\n          alct_pretrig_enable=\"0\"\n          match_pretrig_enable=\"0\"\n          clct_trig_enable=\"1\"\n          alct_trig_enable=\"0\"\n          match_trig_enable=\"1\"/;
#
	$parameter =~ s/trgmode=\"2\"/clct_pretrig_enable=\"0\"\n          alct_pretrig_enable=\"1\"\n          match_pretrig_enable=\"0\"\n          clct_trig_enable=\"0\"\n          alct_trig_enable=\"1\"\n          match_trig_enable=\"1\"/;
#
    }
#
# ALCT parameters...
# doing it in an if statement prevents names which have just been replaced in the
# TMB section from being replaced again in the ALCT section...
    if ($section eq "alct") {
#
	if (($parameter =~ m/trig_mode/) && 
	    !($parameter =~ m/alct_trig_mode/) ) { 
	    $parameter =~ s/trig_mode/alct_trig_mode/;
	}
#
	if (($parameter =~ m/ext_trig_en/) && 
	    !($parameter =~ m/alct_ext_trig_enable/) ) { 
	    $parameter =~ s/ext_trig_en/alct_ext_trig_enable/;
	}
#
	if (($parameter =~ m/trig_info_en/) && 
	    !($parameter =~ m/alct_trig_info_en/) ) { 
	    $parameter =~ s/trig_info_en/alct_trig_info_en/;
	}
#
	if (($parameter =~ m/l1a_internal/) && 
	    !($parameter =~ m/alct_l1a_internal/) ) { 
	    $parameter =~ s/l1a_internal/alct_l1a_internal/;
	}
#
	if (($parameter =~ m/l1a_window/) && 
	    !($parameter =~ m/alct_l1a_window_size/) ) { 
	    $parameter =~ s/l1a_window/alct_l1a_window_size/;
	}
#
	$parameter =~ s/nph_thresh/alct_pretrig_thresh/;
#
	$parameter =~ s/nph_pattern/alct_pattern_thresh/;
#
	if (($parameter =~ m/ccb_enable/) && 
	    !($parameter =~ m/alct_ccb_enable/) ) { 
	    $parameter =~ s/ccb_enable/alct_ccb_enable/;
	}
#
	$parameter =~ s/amode/alct_accel_mode/;
#
	$parameter =~ s/bxc_offset/alct_bxn_offset/;
#
	if (($parameter =~ m/l1a_delay/) && 
	    !($parameter =~ m/alct_l1a_delay/) ) { 
	    $parameter =~ s/l1a_delay/alct_l1a_delay/;
	}
#
	if (($parameter =~ m/fifo_pretrig/) && 
	    !($parameter =~ m/alct_fifo_pretrig/) ) { 
	    $parameter =~ s/fifo_pretrig/alct_fifo_pretrig/;
	}
#
	if (($parameter =~ m/fifo_tbins/) && 
	    !($parameter =~ m/alct_fifo_tbins/) ) { 
	    $parameter =~ s/fifo_tbins/alct_fifo_tbins/;
	}
#
	if (($parameter =~ m/l1a_offset/) && 
	    !($parameter =~ m/alct_l1a_offset/) ) { 
	    $parameter =~ s/l1a_offset/alct_l1a_offset/;
	}
#
	if (($parameter =~ m/delay/) && 
	    !($parameter =~ m/rpc0_rat_delay/) && 
	    !($parameter =~ m/alct_drift_delay/) && 
	    !($parameter =~ m/alct_l1a_delay/) ) { 
	    $parameter =~ s/delay/afeb_fine_delay/;
	}
#
	if (($parameter =~ m/threshold/) && 
	    !($parameter =~ m/afeb_threshold/) ) { 
	    $parameter =~ s/threshold/afeb_threshold/;
	}
#
	$parameter =~ s/alct_firmware_negative_positive/alct_firmware_negat_posit/;
    }
#
# DMB parameters...
    if ($section eq "dmb") {
	if (($parameter =~ m/cable_delay/) && 
	    !($parameter =~ m/cfeb_cable_delay/) &&
	    !($parameter =~ m/tmb_lct_cable_delay/) &&
	    !($parameter =~ m/cfeb_dav_cable_delay/) &&
	    !($parameter =~ m/alct_dav_cable_delay/) ) { 
	    $parameter =~ s/cable_delay=..[0-9]{1,}\"/cfeb_cable_delay=\"0\"\n            tmb_lct_cable_delay=\"2\"\n            cfeb_dav_cable_delay=\"1\"\n            alct_dav_cable_delay=\"2\"/;     
	}
    }
#
    $parameter="$parameter\n";
#    print "new:  $parameter\n";
}
#
#print "@parameters";
#
open(NEWFILE,">$newfilename") || die "Cannot open file $newfilename\n" ;
# print it out line by line to avoid a " " at the beginning of each line:
foreach $parameter (@parameters) {
    print NEWFILE $parameter;
}
close(NEWFILE);
#
