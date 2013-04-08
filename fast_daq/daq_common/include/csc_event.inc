C csc_event.h
C
C Author: David Chrisman
C Date: 17 Aug 1999
C
C NOTE: csc_event.h (C) and csc_event.inc (FORTRAN) should be identical!
C       Also, the order is indices in multi-dimensional arrays are
C       swapped w.r.t. C code.
C
C
      include 'csc_parameters.inc'    ! NSTRIP, NLAYER etc...
 
      COMMON/UPEVT/ 
     & alct_dump, clct_dump, num_ungated_triggers,
     & event_number, time_musec, trigger_number,
     & run_number, event_type, spill_number, time_YYMM, time_MMSS, 
     & time_DDHH, module_config_id, cable_map_id, trigger_setup_id,
     & afeb_config_id, anode_pattern_id,
     & chamber_type_id, anode_hot_mask_id, anode_threshold_id, 
     & anode_delay_id, cathode_hot_mask_id,  cathode_threshold_id,
     & version, revision, user_name, site, 
     & wire_times_lead, wire_times_trail, 
     & wire_groups, wire_layers, num_wire_hits, 
     & alctcal_scan_mode, alctcal_current_value,
     & alctcal_current_threshold, 
     & alctcal_trigger_per_step, alctcal_num_points,
     & alctcal_num_points_2nd_param,   
     & anode_threshold,
     & scint_on, scint_times, scint_labels, num_scint_hits,
     & stripcal_scan_mode, stripcal_current_strip, 
     & stripcal_current_value, stripcal_current_value_2,
     & stripcal_trigger_per_step,
     & stripcal_num_points,
     & stripcal_num_points_turnoff,
     & nsca_sample, active_febs, sca, sca_block, sca_trig_time, 
     & sca_clock_phase,
     & adc_out_of_range, alct_active,
     & alct_lct_id, alct_csc_id, alct_l1a_num, alct_l1a_lct_match,
     & alct_nbucket, alct_tr_mode,
     & alct_full_bxn, alct_active_febs, alct_febs_read,
     & alct_valid_patt,alct_patt_quality, alct_accel_muon, 
     & alct_wire_group, alct_bxn, alct_patb, clct_active,
     & clct_lct_id, clct_csc_id, clct_l1a_num, clct_l1a_lct_match,
     & clct_nbucket, clct_tr_mode,
     & clct_full_bxn, clct_active_febs, clct_febs_read,
     & clct_valid_patt, clct_patt_number,    
     & clct_bend, clct_half_strip, clct_bxn,
     & tmb_sync, tmb_stat, tmb_bxn, tmb_l1a_num,
     & tmb_alct_valid_patt, tmb_alct_patt_quality, tmb_alct_accel_muon,
     & tmb_alct_wire_group, tmb_alct_bxn, tmb_alct_full_bxn,
     & tmb_clct_valid_patt, tmb_clct_patt_number,
     & tmb_clct_bend, tmb_clct_half_strip, tmb_clct_bxn,
*    & rpc_times_lead, rpc_strip, rpc_sector, num_rpc_hits,
     & beam_cham_times

C
C  ALCT and CLCT raw hits data (must be here with the other longwords)
C
      integer*4 alct_dump(NWIRE, NLAYER)
      integer*4 clct_dump(NSTRIP/2, NLAYER)
C
C  Scaler counts (24-bit words)
C
      integer*4 num_ungated_triggers

C
C  Event header information (see $DAQHOME/readout_control/doc/data_format.txt)
C
      integer*4 event_number
      integer*4 time_musec
      integer*4 trigger_number
      integer*2 run_number
      integer*2 event_type
      integer*2 spill_number
      integer*2 time_YYMM 
      integer*2 time_DDHH 
      integer*2 time_MMSS  
      integer*2 module_config_id
      integer*2 cable_map_id
      integer*2 trigger_setup_id
      integer*2 afeb_config_id
      integer*2 anode_pattern_id
      integer*2 chamber_type_id
      integer*2 anode_hot_mask_id
      integer*2 anode_threshold_id
      integer*2 anode_delay_id 
      integer*2 cathode_hot_mask_id
      integer*2 cathode_threshold_id
      integer*2 version
      integer*2 revision
      integer*2 user_name(USER_LENGTH)
      integer*2 site(SITE_LENGTH)

C
C  Anode wire data
C
      integer*2 wire_times_lead(MAX_WIRE_HITS)
      integer*2 wire_times_trail(MAX_WIRE_HITS)
      integer*2 wire_groups(MAX_WIRE_HITS)
      integer*2 wire_layers(MAX_WIRE_HITS)
      integer*2 num_wire_hits
      integer*2 alctcal_scan_mode
      integer*2 alctcal_current_value
      integer*2 alctcal_current_threshold
      integer*2 alctcal_trigger_per_step
      integer*2 alctcal_num_points
      integer*2 alctcal_num_points_2nd_param
      integer*2 anode_threshold(NAFEB_CHIP, NAFEB)

C
C  Trigger scintillator data
C
      integer*2 scint_on(NSCINT_PER_LAYER,NSCINT_LAYERS)
      integer*2 scint_times(MAX_SCINT_HITS)
      integer*2 scint_labels(MAX_SCINT_HITS)
      integer*2 num_scint_hits
C
C  Strip Calibration data
C
      integer*2 stripcal_scan_mode
      integer*2 stripcal_current_strip
      integer*2 stripcal_current_value
      integer*2 stripcal_current_value_2
      integer*2 stripcal_trigger_per_step
      integer*2 stripcal_num_points
      integer*2 stripcal_num_points_turnoff
C
C  Cathode FEB data
C
      integer*2 nsca_sample
      integer*2 active_febs(NCFEB)
      integer*2 sca(MAX_SCA_SAMPLE, NSTRIP, NLAYER)
      integer*2 sca_block(MAX_SCA_SAMPLE, NSTRIP, NLAYER)
      integer*2 sca_trig_time(MAX_SCA_SAMPLE, NSTRIP, NLAYER)
      integer*2 sca_clock_phase
      integer*2 adc_out_of_range(MAX_SCA_SAMPLE, NSTRIP, NLAYER)
C
C  LCT99 data
C
      integer*2 alct_active
      integer*2 alct_lct_id
      integer*2 alct_csc_id
      integer*2 alct_l1a_num
      integer*2 alct_l1a_lct_match
      integer*2 alct_nbucket
      integer*2 alct_tr_mode
      integer*2 alct_full_bxn
      integer*2 alct_active_febs
      integer*2 alct_febs_read
      integer*2 alct_valid_patt(2)             
      integer*2 alct_patt_quality(2)          
      integer*2 alct_accel_muon(2)              
      integer*2 alct_wire_group(2)              
      integer*2 alct_bxn(2)                  
C
C  ALCT2 data
C
      INTEGER*2 ALCT_PATB(2)   

      integer*2 clct_active
      integer*2 clct_lct_id
      integer*2 clct_csc_id
      integer*2 clct_l1a_num
      integer*2 clct_l1a_lct_match
      integer*2 clct_nbucket
      integer*2 clct_tr_mode
      integer*2 clct_full_bxn
      integer*2 clct_active_febs
      integer*2 clct_febs_read
      integer*2 clct_valid_patt(2)              
      integer*2 clct_patt_number(2)             
      integer*2 clct_bend(2)                    
      integer*2 clct_half_strip(2)             
      integer*2 clct_bxn(2)                     
C
C TMB data
C
      integer*2 tmb_sync
      integer*2 tmb_stat
      integer*2 tmb_bxn
      integer*2 tmb_l1a_num

      integer*2 tmb_alct_valid_patt(2)
      integer*2 tmb_alct_patt_quality(2)
      integer*2 tmb_alct_accel_muon(2)
      integer*2 tmb_alct_wire_group(2)
      integer*2 tmb_alct_bxn(2)
      integer*2 tmb_alct_full_bxn(2)

      integer*2 tmb_clct_valid_patt(2)
      integer*2 tmb_clct_patt_number(2)
      integer*2 tmb_clct_bend(2)
      integer*2 tmb_clct_half_strip(2)
      integer*2 tmb_clct_bxn(2)
C
C RPC's at CERN 1999 beam test
C
*     integer*2 rpc_times_lead(NRPC_HIT*NRPC_STRIP*NRPC_SECTOR)
*     integer*2 rpc_strip(NRPC_HIT*NRPC_STRIP*NRPC_SECTOR)
*     integer*2 rpc_sector(NRPC_HIT*NRPC_STRIP*NRPC_SECTOR)
*     integer*2 num_rpc_hits 
C
C Beam Chambers at CERN 1999 beam test
C
      integer*2 beam_cham_times(2, NBEAM_CHAM_LAYER)
C
C Lecroy 2551 scaler
C
*     integer   scaler(2, NSCALER_CHAN)

      save/upevt/
