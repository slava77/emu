//-----------------------------------------------------------------------
// $Id: alct_registers.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: alct_registers.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------

/* Common registers */
  static int    idreg_first[] = { 0, 4, 8, 24, 32, 40 };
  static int    idreg_length = 40;

/* LCT registers */
//fg static int    control_first[] = { 0, 1, 3, 5, 6, 8 }; //myc++
//fg static int    control_length = 8;
//fg 
//fg static int    delayline_first[]  = { 0, 20, 40, 60, 80, 100, 120 } ;
//fg static int    delayline_length   = 120;
//fg 
//fg static int    parameter_first[] = { 0, 1, 2, 6 } ;
//fg static int    parameter_length   = 6;
//fg 
//fg static int    collmask_first[] = { 0, 28, 56, 84, 112, 140, 168, 196, 224 } ;
//fg static int    collmask_length  = 224;
//fg 
//fg static int    hotchan_first[] = { 0, 16, 32, 48, 64, 80, 96, 
//fg 				  112, 128, 144, 160, 176, 192,
//fg                                   208, 224, 240, 256, 272, 288, 304,
//fg                                   320, 336, 352, 368, 384 };
static int    hotchan_length = 384;
                 
//fg static int    testpat_first[] = { 0, 16, 32, 48, 64, 80, 96, 
//fg 				  112, 128, 144, 160, 176, 192,
//fg                                   208, 224, 240, 256, 272, 288, 304,
//fg                                   320, 336, 352, 368, 384 };
//fg 
//fg   static int    testpat_length = 384;

/* Concentrator registers */
//fg   static int    config_first[] = { 0, 2, 3, 4, 5, 6, 13, 16, 19, 21, 26, 31,
//fg                  33, 36, 44, 48, 52, 53, 56, 60, 61, 62, 64, 66, 67, 69 };
//fg  static int    config_length = 69;

//fg  static int    trig_first[] = { 0, 2 };
//fg  static int    trig_length = 2;
//fg 
//fg   static int    inputenable_first[] = { 0, 1 };
//fg   static int    inputenable_length  = 1;
//fg 
//fg   static int    inputdisable_first[] = { 0, 1 };
//fg   static int    inputdisable_length  = 1;
//fg 
//fg   static int    csc_id_first[] = { 0, 4 };
//fg   static int    csc_id_length = 4;
//fg 
//fg   static int    rhstat_first[] = { 0, 12, 13, 14 };
//fg   static int    rhstat_length = 14;

/* Slow Control registers */
  static int    tpdac_first[] = { -1, 7 };
  static int    tpdac_length = 9;

//fg   static int    thr_first[] = { 0, 8, 12 };
//fg   static int    thr_length = 12;

  static int    tpg_first[] = { 0, 7 };
  static int    tpg_length = 7;

  static int    tps_first[] = { 0, 6 };
  static int    tps_length = 6;

  static int    stdby_first[] = { 0, 6, 12, 18, 24, 30, 36, 42 };
  static int    stdby_length = 42;

  static int    tppu_first[] = { 0, 1 };
  static int    tppu_length = 1;

//  static int    asic_delay_first[] = { 0, 24 }; //myc++
//  static int    asic_delay_length = 24;





