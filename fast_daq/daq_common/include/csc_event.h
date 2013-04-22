/* csc_event.h
 *
 * Author: David Chrisman
 * Date: 17 Aug 1999
 *
 * NOTE: csc_event.h (C) and csc_event.inc (FORTRAN) should be identical!
 *
*/
#ifndef __CSC_EVENT_H__
#define __CSC_EVENT_H__

#include "csc_parameters.h"     /* NLAYER, NSTRIP, etc... */

typedef struct {
/*
 * ALCT and CLCT raw hits data (must come first with the other long words)
*/
  unsigned long  alct_dump[NLAYER][NWIRE];  /* ALCT hits (bits are timebins) */
  unsigned long  clct_dump[NLAYER][NSTRIP/2]; /* CLCT triads (bits are tbins) */
  unsigned long  clct_dump_strips[NLAYER][NSTRIP]; /* CLCT triads (bits are tbins) */
  unsigned long  clct_dump_halfstrips[NLAYER][NSTRIP][2]; /* Comparator hit yes/no */

/*
 * Scaler counts (24-bit words)
*/
  unsigned long  num_ungated_triggers;  //used

/* 
 * Lecroy 2551 scaler
*/

/* unsigned long scaler[NSCALER_CHAN][2]; */

/*
 * Event header information (see $DAQHOME/readout_control/doc/data_format.txt)
*/
  unsigned long  event_number;//filled at begin of display - used in tests
  unsigned long  time_musec;//used
  unsigned long  trigger_number;//not used
  unsigned short run_number;
  unsigned short event_type;
  unsigned short spill_number;
  unsigned short time_YYMM; 
  unsigned short time_DDHH; 
  unsigned short time_MMSS; 
  unsigned short module_config_id;
  unsigned short cable_map_id;
  unsigned short trigger_setup_id;
  unsigned short afeb_config_id;//needed
  unsigned short anode_pattern_id;
  unsigned short chamber_type_id;
  unsigned short anode_hot_mask_id;
  unsigned short anode_threshold_id;
  unsigned short anode_delay_id;
  unsigned short cathode_hot_mask_id;
  unsigned short cathode_threshold_id;
  unsigned short version;
  unsigned short revision;
  char user_name[USER_LENGTH]; /* this is only possible because it is even */
  char site[SITE_LENGTH]; /* podobno */
/*
 * Anode wire data from 3377 TDC's
 */

  /* List of leading edge wire times    */
  unsigned short wire_times_lead[MAX_WIRE_HITS]; 

  /* List of trailing edge wire times   */
  unsigned short wire_times_trail[MAX_WIRE_HITS];

  /* ...corresponding wire groups       */
  unsigned short wire_groups[MAX_WIRE_HITS];

  /* ...corresponding wire layers       */
  unsigned short wire_layers[MAX_WIRE_HITS];

  /* Number of items in wire_ lists     */
  unsigned short num_wire_hits;

  /* Anode calibration current value of scan variable */
  /* (May be threshold, delay or test pulse amp, depending on scan mode.) */
  unsigned short alctcal_scan_mode;//used - set in test
  unsigned short alctcal_current_value;//used - set in test
  unsigned short alctcal_current_threshold;//used and set in test
  unsigned short alctcal_trigger_per_step;//used and set in test
  unsigned short alctcal_num_points;//used and set in test
  unsigned short alctcal_num_points_2nd_param;//used and set in test


/*
 * Trigger scintillator data (Cosmic ray trigger at FNAL)
*/

  /* Scintillator hits as recorded by Phillips discriminators (no time info).
   * The first index is: 0=east above, 1=west above, 2=east below, 3=west below
   * (at least, in Florida). The second index is the scintillator in that layer.
   */
  unsigned short scint_on[NSCINT_LAYERS][NSCINT_PER_LAYER];//not in

  /* List of trigger scintillator times from TDCs */
  unsigned short scint_times[MAX_SCINT_HITS];//not in

  /* ...corresponding scint labels      */
  unsigned short scint_labels[MAX_SCINT_HITS];//not in

  /* Number of items in scint_ lists    */
  unsigned short num_scint_hits;//not in

  /*/
 * Strip Calibration data
 * Current strip and current value of scan variable; may be threshold, 
 * delay or test pulse amp, depending on scan mode.
*/
  unsigned short stripcal_scan_mode;//set in test
  unsigned short stripcal_current_strip;//set in test
  unsigned short stripcal_current_value;//set in test
  unsigned short stripcal_current_value_2;//set in test
  unsigned short stripcal_trigger_per_step;//set in test
  unsigned short stripcal_num_points; //set in test
  unsigned short stripcal_num_points_turnoff;//set in test
/*
 * Cathode FEB data
*/

  /* Number of SCA samples */
  unsigned short nsca_sample;//done

  /* One bit for each CFEB that has data */
  unsigned short active_febs[NCFEB];//done

  /* Strip data from SCA   */
  unsigned short sca[NLAYER][NSTRIP][MAX_SCA_SAMPLE];//done

  /* SCA capacitor block */
  unsigned short sca_block[NLAYER][NSTRIP][MAX_SCA_SAMPLE];//done

  unsigned short sca_trig_time[NLAYER][NSTRIP][MAX_SCA_SAMPLE];//done
  unsigned short sca_clock_phase;//done

  /* Out of range flag */
  unsigned short adc_out_of_range[NLAYER][NSTRIP][MAX_SCA_SAMPLE];//done

/*
 * LCT99 data
*/
  unsigned short alct_active;            /* = 1 if ACLT data available       */ //done
  unsigned short alct_lct_id;            /* ALCT LCT Module ID               */ //only in 2006
  unsigned short alct_csc_id;            /* ALCT Chamber number              */ //only in 2006
  unsigned short alct_l1a_num;           /* ALCT Level 1 Accept counter      */ //done
  unsigned short alct_l1a_lct_match;     /* ALCT L1A*LCT Match               */ //only in 2006
  unsigned short alct_nbucket;           /* ALCT Num of 25ns time buckets    */ //done
  unsigned short alct_tr_mode;           /* ALCT No Dump, Full Dump, etc...  */
  unsigned short alct_full_bxn;          /* ALCT Full Bunch Crossing Number  */
  unsigned short alct_active_febs;       /* ALCT One bit per FEB with hits   */ //only in 2006
  unsigned short alct_febs_read;       /* ALCT One bit per FEB sent to DAQMB */ //only in 2006
  unsigned short alct_valid_patt[2];     /* ALCT valid pattern flag          */ //done
  unsigned short alct_patt_quality[2];   /* ALCT pattern quality (0-3)       */ //done
  unsigned short alct_accel_muon[2];     /* ALCT accelerator muon            */ //done
  unsigned short alct_wire_group[2];     /* ALCT Wire-Gang ID (0-111)        */ //done
  unsigned short alct_bxn[2];            /* ALCT bunch Crossing Number       */ //done 
  unsigned short alct_patb[2];           /* ALCT pattern B                   */ //done

  unsigned short clct_active;            /* = 1 if CLCT data available       */ //done
  unsigned short clct_lct_id;            /* CLCT LCT Module ID               */
  unsigned short clct_csc_id;            /* CLCT Chamber number              */
  unsigned short clct_l1a_num;           /* CLCT Level 1 Accept counter      */
  unsigned short clct_l1a_lct_match;     /* CLCT L1A*LCT Match               */
  unsigned short clct_nbucket;           /* CLCT Num of 25ns time buckets    */ //done
  unsigned short clct_tr_mode;           /* CLCT No Dump, Full Dump, etc...  */
  unsigned short clct_full_bxn;          /* CLCT Full Bunch Crossing Number  */
  unsigned short clct_active_febs;       /* CLCT One bit per FEB with hits   */ //done
  unsigned short clct_febs_read;       /* CLCT One bit per FEB sent to DAQMB */
  unsigned short clct_valid_patt[2];     /* CLCT valid pattern flag          */ //done
  unsigned short clct_patt_number[2];    /* CLCT pattern number (0-255)      */ //done
  unsigned short clct_bend[2];           /* CLCT bend Left/Right (0/1)       */ //done
  //unsigned short clct_half_strip[2];     /* CLCT half-strip ID (0-159)       */ //done
  unsigned short clct_half_strip[2];     /* CLCT half-strip ID (0-31) of CFEB       */ //done
  unsigned short clct_bxn[2];            /* CLCT bunch Crossing Number       */ //done
  unsigned short clct_key_strip[2];      /* CLCT key strip                   */ //done

/*
 * TMB data - not used in tests
*/
  unsigned short tmb_sync;               /* 1-bit TBM_SYNC read from DDU     */
  unsigned short tmb_stat;               /* 6-bit TMB_STAT read from DDU     */
  unsigned short tmb_bxn;                /* 5-bit TMB_BXN  read from DDU     */ //done
  unsigned short tmb_l1a_num;            /* 3-bit TMB_L1Acc count from DDU   */ //done

  unsigned short tmb_alct_valid_patt[2]; /* ALCT valid pattern flag          */
  unsigned short tmb_alct_patt_quality[2];/* ALCT pattern quality (0-3)      */
  unsigned short tmb_alct_accel_muon[2]; /* ALCT accelerator muon            */
  unsigned short tmb_alct_wire_group[2]; /* ALCT Wire-Gang ID (0-111)        */
  unsigned short tmb_alct_bxn[2];        /* ALCT bunch Crossing Number       */
  unsigned short tmb_alct_full_bxn[2];   /* ALCT Full bunch Crossing Number  */

  unsigned short tmb_clct_valid_patt[2]; /* CLCT valid pattern flag          */
  unsigned short tmb_clct_patt_number[2];/* CLCT pattern number (0-255)      */
  unsigned short tmb_clct_bend[2];       /* CLCT bend Left/Right (0/1)       */
  unsigned short tmb_clct_half_strip[2]; /* CLCT half-strip ID (0-159)       */
  unsigned short tmb_clct_bxn[2];        /* CLCT bunch Crossing Number       */

/*
 * RPC's at CERN 1999 beam test
*//*
 * unsigned short rpc_times_lead[NRPC_SECTOR*NRPC_STRIP*NRPC_HIT];
 * unsigned short rpc_strip[NRPC_SECTOR*NRPC_STRIP*NRPC_HIT];
 * unsigned short rpc_sector[NRPC_SECTOR*NRPC_STRIP*NRPC_HIT];
 * unsigned short num_rpc_hits;
*/

/*
 * Beam Chambers at CERN 1999 beam test
*/
  unsigned short beam_cham_times[NBEAM_CHAM_LAYER][2];

  /* someone had put long words here... please do not do this, put them 
     near the top of the structure.  any ensuant error is probably due to the
     optimizer adding stuff bytes so that longs fall on quadword boundaries...
     this creates a hole in the structure and may cause confusion with
     the FORTRAN code...   */

}csc_event_type ;

#ifdef CSC_DISPLAY
extern csc_event_type upevt_;
#else
csc_event_type upevt_;     /* A global variable accessed by FORTRAN */
#endif

/* Function prototypes */

int get_next_event(short *buf, int first_time);

#endif /* if not included already */


