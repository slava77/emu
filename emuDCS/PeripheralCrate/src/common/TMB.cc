//-----------------------------------------------------------------------
// $Id: TMB.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: TMB.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "TMB.h"
#include "JTAG_constants.h"
#include "VMEController.h"
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include "TMB_constants.h"
#include "ALCTController.h"

// the VME addresses here are defined in
// http://www-collider.physics.ucla.edu/cms/trigger/tmb2001/tmb2001_spec.pdf

TMB::TMB(int newcrate, int slot) :
  VMEModule(newcrate, slot),
  ucla_ldev(1),
  cfeb0delay_(7),
  cfeb1delay_(7),
  cfeb2delay_(7),
  cfeb3delay_(7),
  cfeb4delay_(7),
  alct_tx_clock_delay_(0x00),
  alct_rx_clock_delay_(0x0a),
  l1a_window_size_(5),
  l1adelay_(128),
  alct_match_window_size_(3),
  alct_vpf_delay_(8),
  mpc_delay_(7),
  ALCT_input_(1),
  rpc_exists_(0xf),
  fifo_mode_(1),
  fifo_tbins_(7),
  fifo_pretrig_(2),
  alct_clear_(0),
  mpc_tx_delay_(0),
  l1a_offset_(0),
  bxn_offset_(0)
{
  std::cout << "TMB: crate=" << this->crate() << " slot=" << this->slot() << std::endl;
} 


TMB::~TMB() {
  std::cout << "destructing ALCTController" << std::endl; 
  delete alctController_; 
  std::cout << "destructing TMB" << std::endl;
}


void TMB::configure() {
  printf("Setting up trgmode CLCT TMB slot %2d\n", theSlot) ;
  trgmode(1);
  printf("***Set TMB CSC ID to Slot_ID/2 = %d\n", theSlot/2);
  load_cscid();
  std::cout << "Resetting counters" << std::endl;
  ResetCounters();
}


void TMB::lctrequestdelay(int dword)
{
  cmd[0]=VTX2_USR1;
  sndbuf[0]=33;
  theController->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,LATER);
  cmd[0]=VTX2_USR2;
  sndbuf[0]=dword&0XFF;
  sndbuf[1]=(dword>>8)&0xFF;
  theController->devdo(MCTRL,6,cmd,16,sndbuf,rcvbuf,LATER);
  cmd[0]=VTX2_USR1;
  sndbuf[0]=0;
  theController->devdo(MCTRL,6,cmd,8,sndbuf,rcvbuf,LATER);
  cmd[0]=VTX2_BYPASS;
  sndbuf[0]=0;
  theController->devdo(MCTRL,6,cmd,0,sndbuf,rcvbuf,NOW);

  printf("LCT Request  Delay are Set to %04x (Hex). \n",dword&0xFFFF);
}


void TMB::clear_i2c() {
  printf(" done so unstart state machine \n");
  sndbuf[0]=0x8c;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);
}

void TMB::DecodeALCT(){
   //
   std::cout << std::endl;
   printf("DecodeALCT.Read %x \n",alct_alct0_adr);
   tmb_vme(VME_READ,alct_alct0_adr,sndbuf,rcvbuf,NOW);
   printf("DecodeALCT.Done %x \n",alct_alct0_adr);
   //
   int data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
   //
   alct0_valid_     = data&0x1;
   alct0_quality_   = (data>>1)&0x3 ;
   alct0_amu_       = (data>>3)&0x1 ;
   alct0_first_key_ = (data>>4)&0x7f;
   alct0_first_bxn_ = (data>>11)&0x3 ;
   //
   printf(" alct0      = %8x \n",data);
   printf(" valid      = %d  \n",alct0_valid_);
   printf(" quality    = %d  \n",alct0_quality_);
   printf(" amu        = %d  \n",alct0_amu_);
   printf(" first_key  = %d  \n",alct0_first_key_);
   printf(" first_bxn  = %d  \n",alct0_first_bxn_);
   std::cout << std::endl;
   //
   tmb_vme(VME_READ,alct_alct1_adr,sndbuf,rcvbuf,NOW);
   //
   data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff);
   //
   alct1_valid_     = data&0x1;
   alct1_quality_   = (data>>1)&0x3 ;
   alct1_amu_       = (data>>3)&0x1 ;
   alct1_second_key_ = (data>>4)&0x7f;
   alct1_second_bxn_ = (data>>11)&0x3 ;
   //
   printf(" alct1      = %8x \n",data);
   printf(" valid      = %d  \n",alct1_valid_);
   printf(" quality    = %d  \n",alct1_quality_);
   printf(" amu        = %d  \n",alct1_amu_);
   printf(" second_key = %d  \n",alct1_second_key_);
   printf(" second_bxn = %d  \n",alct1_second_bxn_);
   //
}

void TMB::DecodeCLCT(){
   //
   std::cout << std::endl;
   tmb_vme(VME_READ,seq_clctm_adr,sndbuf,rcvbuf2,NOW);
   tmb_vme(VME_READ,seq_clct0_adr,sndbuf,rcvbuf,NOW);
   //
   int data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff | (rcvbuf2[1]&0x1f)<<16 ) ;
   printf(" data             = %8x \n",data);
   CLCT0_cfeb_ =  ((data>>14) & 0x7);
   CLCT0_nhit_ =  ((data>>1)  & 0x7);
   CLCT0_keyHalfStrip_ =  ((data>>9)  & 0x1f) ;
   std::cout << "CLCT0.Valid      = " << ((data)     & 0x1)  << std::endl ;
   std::cout << "CLCT0.Key HStrip = " << CLCT0_keyHalfStrip_ << std::endl;
   std::cout << "CLCT0.Key CFEB   = " << CLCT0_cfeb_  << std::endl ;
   std::cout << "CLCT0.Key nhit   = " << CLCT0_nhit_  << std::endl;
   std::cout << "CLCT0.Key PatD   = " << ((data>>7)  & 0x1)  << std::endl ;
   std::cout << "CLCT0.BXN        = " << ((data>>17) & 0x3)  << std::endl ;
   std::cout << std::endl;
   //
   tmb_vme(VME_READ,seq_clct1_adr,sndbuf,rcvbuf,NOW);
   data = (((rcvbuf[0]&0xff)<<8) | rcvbuf[1]&0xff ) | ((rcvbuf2[1]>>5)&0x7)<<16 | (rcvbuf2[0]&0x3)<<19 ;
   printf(" data             = %8x \n",data);
   CLCT1_cfeb_ =  ((data>>14) & 0x7);
   CLCT1_nhit_ =  ((data>>1)  & 0x7);
   CLCT1_keyHalfStrip_ =  ((data>>9)  & 0x1f) ;
   std::cout << "CLCT1.Valid      = " << ((data)     & 0x1)  << std::endl;
   std::cout << "CLCT1.Key HStrip = " << CLCT1_keyHalfStrip_ << std::endl;
   std::cout << "CLCT1.Key CFEB   = " << CLCT1_cfeb_  << std::endl ;
   std::cout << "CLCT1.Key nhit   = " << CLCT1_nhit_  << std::endl;
   std::cout << "CLCT1.Key PatD   = " << ((data>>7)  & 0x1)  << std::endl ;
   std::cout << "CLCT1.BXN        = " << ((data>>17) & 0x3)  << std::endl ;
   std::cout << std::endl;
   //
}
//
int TMB::FmState(){
  //
  tmb_vme(VME_READ,ccb_cmd_adr,sndbuf,rcvbuf,NOW);
  //
  int fm_state = (rcvbuf[1]&0xf0)>>4;
  //
}
//
void TMB::PrintCounters(){
//
   std::cout << std::endl;
   std::cout << "--------------------------------------------------------" << std::endl;
   std::cout << "---              Counters                             --" << std::endl;
   std::cout << "--------------------------------------------------------" << std::endl;
   std::cout << "ALCT: CRC error                                  " << FinalCounter[0] <<std::endl ;
   std::cout << "ALCT: LCT sent to TMB                            " << FinalCounter[1] <<std::endl ;
   std::cout << "ALCT: LCT error (alct debug firmware)            " << FinalCounter[2] <<std::endl ;
   std::cout << "ALCT: L1A readout                                " << FinalCounter[3] <<std::endl ;
   std::cout << "CLCT: Pretrigger                                 " << FinalCounter[4] <<std::endl ;
   std::cout << "CLCT: Pretrig but no wbuf available              " << FinalCounter[5] <<std::endl ;
   std::cout << "CLCT: Invalid pattern after drift                " << FinalCounter[6] <<std::endl ;
   std::cout << "CLCT: TMB matching rejected event                " << FinalCounter[7] <<std::endl ;
   std::cout << "TMB:  CLCT or  ALCT or both triggered            " << FinalCounter[8] <<std::endl ;
   std::cout << "TMB:  CLCT or  ALCT or both triggered xmit MPC   " << FinalCounter[9] <<std::endl ;
   std::cout << "TMB:  CLCT and ALCT matched in time              " << FinalCounter[10] <<std::endl ;
   std::cout << "TMB:  ALCT-only trigger                          " << FinalCounter[11] <<std::endl ;
   std::cout << "TMB:  CLCT-only trigger                          " << FinalCounter[12] <<std::endl ;
   std::cout << "TMB:  No trig pulse response                     " << FinalCounter[13] <<std::endl ;
   std::cout << "TMB:  No MPC transmission                        " << FinalCounter[14] <<std::endl ;
   std::cout << "TMB:  No MPC response FF pulse                   " << FinalCounter[15] <<std::endl ;
   std::cout << "TMB:  MPC accepted LCT0                          " << FinalCounter[16] <<std::endl ;
   std::cout << "TMB:  MPC accepted LCT1                          " << FinalCounter[17] <<std::endl ;
   std::cout << "L1A:  L1A received                               " << FinalCounter[18] <<std::endl ;
   std::cout << "L1A:  TMB triggered, TMB in L1A window           " << FinalCounter[19] <<std::endl ;
   std::cout << "L1A:  L1A received, no TMB in window             " << FinalCounter[20] <<std::endl ;
   std::cout << "L1A:  TMB triggered, no L1A received             " << FinalCounter[21] <<std::endl ;
   std::cout << "L1A:  TMB readout                                " << FinalCounter[22] <<std::endl;
   std::cout << std::endl;
//
}

void TMB::ResetCounters(){
   //
   unsigned long int adr;
   unsigned long int rd_data ;
   unsigned long int wr_data;
   //
   // Clear counters
   //
   adr = cnt_ctrl_adr ;
   wr_data= 0x1; //clear
   sndbuf[0] = (wr_data>>8)&0xff;
   sndbuf[1] = (wr_data&0xff);
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data= 0x0; //unclear
   sndbuf[0] = (wr_data>>8)&0xff;
   sndbuf[1] = (wr_data&0xff);
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);   
//
}

void TMB::GetCounters(){
   //
   unsigned long int adr;
   unsigned long int rd_data ;
   unsigned long int wr_data;
   //
   // Take snapshot of current counter state
   //
   adr = cnt_ctrl_adr ;
   wr_data= 0x2; //snap
   sndbuf[0] = (wr_data>>8)&0xff;
   sndbuf[1] = (wr_data&0xff);
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data= 0x0; //unsnap
   sndbuf[0] = (wr_data>>8)&0xff;
   sndbuf[1] = (wr_data&0xff);
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);   
   //
//
// Read counters
   //   
   for (int counter=0; counter < MaxCounter; counter++){
      adr = cnt_ctrl_adr;
      wr_data= counter << 8 ;
      sndbuf[0] = (wr_data>>8)&0xff;
      sndbuf[1] = (wr_data&0xff);
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      adr = cnt_rdata_adr;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
//
// Combine lsbs+msbs
      //
      int cnt_lsb, cnt_msb;
      long int cnt_full;
      //
      if( counter%2 ==0 ) {          //even addresses contain counter LSBs
         cnt_lsb = rd_data;
      }
      else                           //odd addresses contain counter MSBs
      {	 
         cnt_msb  = rd_data;
	 cnt_full = cnt_lsb | (cnt_msb<<16) ;
	 FinalCounter[counter/2] = cnt_full ;     //assembled counter MSB,LSB	 
      }
   }   
   //
}

void TMB::old_clk_delays(unsigned short int time,int cfeb_id)
{
 // for old PHOS4 chips
int iloop;
 iloop=0;
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  }
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,LATER);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  // send delay to dynatem
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,LATER);
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);


LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
}


void TMB::scope(int scp_arm,int scp_readout) {

  unsigned int nchannels = 128;
  unsigned int nrams     = nchannels/16;
  unsigned int nbits     = nchannels*16;

  static const char* scope_tag[128] = {"ch00 sequencer pretrig  ",
				      "ch01 active_feb_flag    ",
				      "ch02 any_cfeb_hit       ",
				      "ch03 any_cfeb_hsds      ",
				      "ch04 wr_buf_busy        ",
				      "ch05 wr_buf_ready       ",
				      "ch06 clct_ext_trig_os   ",
				      "ch07 alct_active_feb    ",
				      "ch08 alct_pretrig_win   ",
				      "ch09 first_really_valid ",
				      "ch10 clct_sm==xtmb      ",
				      "ch11 tmb_discard        ",
				      "ch12 discard_nobuf      ",
				      "ch13 discard_invp       ",
				      "ch14 discard_tmbreject  ",
				      "ch15 0(no dmb readout)  ",
				      "ch16 first_nhit[0]      ",
				      "ch17 first_nhit[1]      ",
				      "ch18 first_nhit[2]      ",
				      "ch19 first_pat[3]hsds   ",
				      "ch20 second_nhit[0]     ",
				      "ch21 second_nhit[1]     ",
				      "ch22 second_nhit[2]     ",
				      "ch23 second_pat[3]hsds  ",
				      "ch24 latch_clct0        ",
				      "ch25 latch_clct1        ",
				      "ch26 alct_1st_valid     ",
				      "ch27 alct_2nd_valid     ",
				      "ch28 alct_vpf_tp        ",
				      "ch29 clct_vpf_tp        ",
				      "ch30 clct_window_tp     ",
				      "ch31 0(no dmb readout)  ",
				      "ch32 sequencer pretrig  ",
				      "ch33 mpc_frame_ff       ",
				      "ch34 mpc_response_ff    ",
				      "ch35 mpc_accept_tp[0]   ",
				      "ch36 mpc_accept_tp[1]   ",
				      "ch37 l1a_pulse_dsp      ",
				      "ch38 l1a_window_dsp     ",
				      "ch39 dmb_dav_mux        ",
				      "ch40 dmb_busy           ",
				      "ch41 hs_thresh[0]       ",
				      "ch42 hs_thresh[1]       ",
				      "ch43 hs_thresh[2]       ",
				      "ch44 ds_thresh[0]       ",
				      "ch45 ds_thresh[1]       ",
				      "ch46 ds_thresh[2]       ",
				      "ch47 0(no dmb readout)  ",
				      "ch48 sequencer pretrig  ",
				      "ch49 valid_clct_required",
				      "ch50 buf_nbusy[0]       ",
				      "ch51 buf_nbusy[1]       ",
				      "ch52 buf_nbusy[2]       ",
				      "ch53 buf_nbusy[3]       ",
				      "ch54 0                  ",
				      "ch55 0                  ",
				      "ch56 0                  ",
				      "ch57 0                  ",
				      "ch58 0                  ",
				      "ch59 l1a_rx_counter[0]  ",
				      "ch60 l1a_rx_counter[0]  ",
				      "ch61 l1a_rx_counter[0]  ",
				      "ch62 l1a_rx_counter[0]  ",
				      "ch63 0(no dmb readout)  ",
				      "ch64 sequencer pretrig  ",
				      "ch65 bxn_counter[ 0]    ",
				      "ch66 bxn_counter[ 1]    ",
				      "ch67 bxn_counter[ 2]    ",
				      "ch68 bxn_counter[ 3]    ",
				      "ch69 bxn_counter[ 4]    ",
				      "ch70 bxn_counter[ 5]    ",
				      "ch71 bxn_counter[ 6]    ",
				      "ch72 bxn_counter[ 7]    ",
				      "ch73 bxn_counter[ 8]    ",
				      "ch74 bxn_counter[ 9]    ",
				      "ch75 bxn_counter[10]    ",
				      "ch76 bxn_counter[11]    ",
				      "ch77 0                  ",
				      "ch78 0                  ",
				      "ch79 0(no dmb readout)  ",
				      "ch80 dmb seq_wdata[0]   ",
				      "ch81 dmb seq_wdata[1]   ",
				      "ch82 dmb seq_wdata[2]   ",
				      "ch83 dmb seq_wdata[3]   ",
				      "ch84 dmb seq_wdata[4]   ",
				      "ch85 dmb seq_wdata[5]   ",
				      "ch86 dmb seq_wdata[6]   ",
				      "ch87 dmb seq_wdata[7]   ",
				      "ch88 dmb seq_wdata[8]   ",
				      "ch89 dmb seq_wdata[9]   ",
				      "ch90 dmb seq_wdata[10]  ",
				      "ch91 dmb seq_wdata[11]  ",
				      "ch92 dmb seq_wdata[12]  ",
				      "ch93 dmb seq_wdata[13]  ",
				      "ch94 dmb seq_wdata[14]  ",
				      "ch95 dmb seq_wdata[15]  ",
				      "ch96  rpc0_bxn[0]       ",
				      "ch97  rpc0_bxn[1]       ",
				      "ch98  rpc0_bxn[2]       ",
				      "ch99  rpc1_bxn[0]        ",
				      "ch100 rpc1_bxn[1]        ",
				      "ch101 rpc1_bxn[2]        ",
				      "ch102 rpc2bxn[0]         ",
				      "ch103 rpc2bxn[1]         ",
				      "ch104 rpc2bxn[2]         ",
				      "ch105 rpc3bxn[0]         ",
				      "ch106 rpc3bxn[1]         ",
				      "ch107 rpc3bxn[2]         ",
				      "ch108 0                  ",
				      "ch109 0                  ",
				      "ch110 0                  ",
				      "ch111 0(no dmb readout)  ",
				      "ch112 rpc0_nhits[0]      ",
				      "ch113 rpc0_nhits[1]      ",
				      "ch114 rpc0_nhits[2]      ",
				      "ch115 rpc0_nhits[3]      ",
				      "ch116 rpc1_nhits[0]      ",
				      "ch117 rpc1_nhits[1]      ",
				      "ch118 rpc1_nhits[2]      ",
				      "ch119 rpc1_nhits[3]      ",
				      "ch120 rpc2_nhits[0]      ",
				      "ch121 rpc2_nhits[1]      ",
				      "ch122 rpc2_nhits[2]      ",
				      "ch123 rpc2_nhits[3]      ",
				      "ch124 rpc3_nhits[0]      ",
				      "ch125 rpc3_nhits[1]      ",
				      "ch126 rpc3_nhits[2]      ",
				      "ch127 rpc3_nhits[3](no d)"
  } ;

  unsigned long int scope_ram[256][nrams];
  int scp_raw_data[nbits];
  char *scope_ch[256];
  int scope_raw_decode = 1;

  unsigned long int runstop;
  //unsigned long int force_trig;
  //unsigned long int ch_source;
  unsigned long int ram_sel;
  unsigned long int Auto;
  //unsigned long int nowrite;
  //unsigned long int ready;
  //unsigned long int trig_done;

  unsigned long int adr;
  unsigned long int rd_data = 0;
  unsigned long int wr_data;
  //unsigned long int status;
  unsigned long int i;
  unsigned long int itbin;
  unsigned long int ich;
  unsigned long int ibit;
  unsigned long int iram;
  //unsigned long int idigit;
  unsigned long int iline;
  unsigned long int iadr;

  unsigned long int first_nhit[256];
  unsigned long int second_nhit[256];
  unsigned long int hs_thresh[256] ;
  unsigned long int ds_thresh[256] ;
  unsigned long int bxn[256] ;
  unsigned long int dmb[256] ;
  unsigned long int l1a_rx_cnt[256] ;
  unsigned long int buf_nbusy[256] ;

  unsigned long int	rpc0_bxn[256] ;
  unsigned long int	rpc1_bxn[256] ;
  unsigned long int	rpc2_bxn[256] ;
  unsigned long int	rpc3_bxn[256] ;
  unsigned long int	rpc0_nhits[256] ;
  unsigned long int	rpc1_nhits[256] ;
  unsigned long int	rpc2_nhits[256] ;
  unsigned long int	rpc3_nhits[256] ;


  rd_data = 0; //temp

  /**** scp_arm ****************************************************/
  if(scp_arm) {

    //Get current scope control state, exit if in Auto mode
    adr = scp_ctrl_adr;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    Auto = ((rcvbuf[0]&0xff) << 8) & (rcvbuf[1]&0xff);
    Auto = (Auto >> 5) & 0x1;
    if(Auto == 1) return;

    //Reset scope state to idle
    runstop = 0;
    ram_sel = 0;

    adr = scp_ctrl_adr ;
    wr_data = (Auto << 5) | (ram_sel << 2) | runstop;
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    //status = tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    //printf("Scope status %04x",rd_data);

    //Select channel
    sndbuf[0] = 0x00;
    sndbuf[1] = 0x1d;
    tmb_vme(VME_WRITE,0xce,sndbuf,rcvbuf,NOW);

    //Arm scope for triggering, send scope state to wait_trig
    runstop = 1;
    adr = scp_ctrl_adr ;
    wr_data = (Auto << 5) | (ram_sel << 2) | runstop;
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    return;
  }

  /**** scp_readout *******************************************************/

  //Exit if in Auto mode
  if(Auto == 1) return;

  //Wait for scope to trigger
  if(scp_readout) {
     for(i=1;i<100;i++) {                              //Give it time to store 255 words
      adr = scp_ctrl_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);              //read scope status
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      printf("Scope status %04x\n",rd_data);
      if((rd_data & 0x0080) != 0) goto TRIGGERED;                    //triggered and done
      printf("Waiting for scope to trigger %ld\n",i);
    }
    printf("Scope never triggered\n");
    goto END;

    //Read back embedded scope data
    TRIGGERED:
      printf("Scope triggered\n");

	for(itbin=0;itbin<256;itbin++) {                    //loop over ram addresses
	  for(iram=0;iram<nrams;iram++) {
	    adr = scp_ctrl_adr ;
	    wr_data = 0x0000 | (itbin << 8) | (iram << 2) | (Auto << 5) ;             //write scope ram address
	    sndbuf[0] = (wr_data & 0xff00)>>8 ;
	    sndbuf[1] = wr_data & 0x00ff ;
	    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	    
	    adr = scp_rdata_adr ;
	    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);              //read scope data at this address
	    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	    scope_ram[itbin][iram] = rd_data;                  //store 16 parallel in local array
	    //printf(" 1 %3d %1d %4x %4x \n", itbin,iram,scope_ram[itbin][iram] ) ;
	  }
	}
	goto DISPLAY;
	
	//for(itbin=0;itbin<256;itbin++) {               //loop over ram addresses
	//for(iram=0;iram<4;iram++) {
	//  printf(" 2 %3d %1d %4x %4x \n", itbin,iram,scope_ram[itbin][iram] ) ;
	//}
	//}

  }

  /**** scp_raw_decode *************************************************/

  //Exit if not Auto mode
  if(Auto == 1) return;

  //load scope_ram from raw-hits format readout
  if(scope_raw_decode) {
    iline = 0;
    for(iram=0;iram<nrams;iram++){
      for(iadr=0;iadr<256;iadr++) {
	itbin = ram_sel*256 + iadr;
	scope_ram[itbin][iram] = scp_raw_data[iline];
	iline++;
      }
    }
  }
  goto DISPLAY;


  /**** Display *********************************************************/

  //Construct waveform
  DISPLAY:
  printf("Displaying \n");
  if (!pfile) pfile = fopen("tmb_scope.txt","w");
    for(ich=0;ich<128;ich++) {                      //loop over 128 scope channels
      iram=ich/16;
      fprintf(pfile,"%s ",scope_tag[ich]);
      for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	ibit = ((scope_ram[itbin][iram]) >> (ich%16) ) & 1;      //logic levels vs tbin for this chan	      
	if(ibit == 0) scope_ch[itbin] = "_";       //display symbol for logic 0
	if(ibit == 1) scope_ch[itbin] = "-";       //display symbol for logic 1
	fprintf(pfile,"%s",scope_ch[itbin]);
	      
	// Construct integer for special channel groups
	if (ich == 16) first_nhit[itbin]=ibit ;
	if (ich == 17) first_nhit[itbin]=first_nhit[itbin] | (ibit<<1);
	if (ich == 18) first_nhit[itbin]=first_nhit[itbin] | (ibit<<2);
	//
	if (ich == 20) second_nhit[itbin]=ibit;
	if (ich == 21) second_nhit[itbin]=second_nhit[itbin] | (ibit<<1);
	if (ich == 22) second_nhit[itbin]=second_nhit[itbin] | (ibit<<2);
	//
	if (ich == 41) hs_thresh[itbin]=ibit ;
	if (ich == 42) hs_thresh[itbin]=hs_thresh[itbin] | (ibit<<1) ;
	if (ich == 43) hs_thresh[itbin]=hs_thresh[itbin] | (ibit<<2) ;
	//
	if (ich == 44) ds_thresh[itbin]=ibit;
	if (ich == 45) ds_thresh[itbin]=ds_thresh[itbin] | (ibit<<1);
	if (ich == 46) ds_thresh[itbin]=ds_thresh[itbin] | (ibit<<2);
	//
	if (ich == 50) buf_nbusy[itbin]=ibit ;
	if (ich == 51) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<1) ;
	if (ich == 52) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<2) ;
	if (ich == 53) buf_nbusy[itbin]=buf_nbusy[itbin] | (ibit<<3) ;
	//									  //
	if (ich == 59) l1a_rx_cnt[itbin]=ibit;
	if (ich == 60) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<1);
	if (ich == 61) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<2);
	if (ich == 62) l1a_rx_cnt[itbin]=l1a_rx_cnt[itbin] | (ibit<<3);
	//
	if (ich == 65) bxn[itbin]=ibit ;
	if (ich >= 66 && ich <= 76) bxn[itbin]=bxn[itbin] | (ibit<<(ich-65)) ;
	// 
	if (ich == 80) dmb[itbin] = ibit ;
	if (ich >= 81 && ich <= 95) dmb[itbin]=dmb[itbin] | (ibit<<(ich-80)) ;
	//
	if (ich == 96) rpc0_bxn[itbin]=ibit ;
	if (ich == 97) rpc0_bxn[itbin]=rpc0_bxn[itbin] | (ibit << 1);
	if (ich == 98) rpc0_bxn[itbin]=rpc0_bxn[itbin] | (ibit << 2);
	//
	if (ich == 99 )rpc1_bxn[itbin]=ibit;
	if (ich == 100)rpc1_bxn[itbin]=rpc1_bxn[itbin] | (ibit << 1);
	if (ich == 101)rpc1_bxn[itbin]=rpc1_bxn[itbin] | (ibit << 2);
	//
	if (ich == 102)rpc2_bxn[itbin]=ibit;
	if (ich == 103)rpc2_bxn[itbin]=rpc2_bxn[itbin] | (ibit << 1);
	if (ich == 104)rpc2_bxn[itbin]=rpc2_bxn[itbin] | (ibit << 2);
	//
	if (ich == 105)rpc3_bxn[itbin]=ibit;
	if (ich == 106)rpc3_bxn[itbin]=rpc3_bxn[itbin] | (ibit << 1);
	if (ich == 107)rpc3_bxn[itbin]=rpc3_bxn[itbin] | (ibit << 2);
	//
	if(ich == 112)rpc0_nhits[itbin]=ibit;
	if(ich == 113)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 1);
	if(ich == 114)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 2);
	if(ich == 115)rpc0_nhits[itbin]=rpc0_nhits[itbin] | (ibit << 3);
	//
	if(ich == 116)rpc1_nhits[itbin]=ibit;
	if(ich == 117)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 1);
	if(ich == 118)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 2);
	if(ich == 119)rpc1_nhits[itbin]=rpc1_nhits[itbin] | (ibit << 3);
	//
	if(ich == 120)rpc2_nhits[itbin]=ibit;
	if(ich == 121)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 1);
	if(ich == 122)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 2);
	if(ich == 123)rpc2_nhits[itbin]=rpc2_nhits[itbin] | (ibit << 3);
	//
	if(ich == 124)rpc3_nhits[itbin]=ibit;
	if(ich == 125)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 1);
	if(ich == 126)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 2);
	if(ich == 127)rpc3_nhits[itbin]=rpc3_nhits[itbin] | (ibit << 3);
	//
      }  //close itbin

      if (ich == 18) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1d",first_nhit[itbin] ) ;
	}
      }
      if (ich == 22) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1d",second_nhit[itbin] ) ;
	}
      }
      if (ich == 43) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1d",hs_thresh[itbin] ) ;
	}
      }
      if (ich == 46) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1d",ds_thresh[itbin] ) ;
	}
      }
      if (ich == 53) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1d",buf_nbusy[itbin] ) ;
	}
      }
      if (ich == 62) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",l1a_rx_cnt[itbin] ) ;
	}
      }
      if (ich == 76) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(bxn[itbin] >> 8) & 0xf ) ;
	}
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(bxn[itbin] >> 4) & 0xf) ;
	}
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(bxn[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 95) {
	fprintf(pfile,"\n%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(dmb[itbin] >> 12) & 0xf ) ;
	}
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(dmb[itbin] >> 8) & 0xf ) ;
	}
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(dmb[itbin] >> 4) & 0xf) ;
	}
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(dmb[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 98 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc0_bxn[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 101 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc1_bxn[itbin]) & 0xf ) ;
	}
      }
      //      
      if (ich == 104 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc2_bxn[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 107 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc3_bxn[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 115 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc0_nhits[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 119 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc1_nhits[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 123 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc2_nhits[itbin]) & 0xf ) ;
	}
      }
      //
      if (ich == 127 ) {
	fprintf(pfile,"\n");    
	fprintf(pfile,"%s ",scope_tag[ich]);
	for(itbin=0;itbin<256;itbin++) {             //256 time bins per channel
	  fprintf(pfile,"%1x",(rpc3_nhits[itbin]) & 0xf ) ;
	}
      }
      //
      fprintf(pfile,"\n");
    }

  END:
    return;
} //scope

std::bitset<22> TMB::calCRC22(const std::vector< std::bitset<16> >& datain){
  std::bitset<22> CRC;
  CRC.reset();
  for(int i=0;i<datain.size()-4;i++){
    printf("Taking %d %x \n",i,datain[i].to_ulong());
    CRC=nextCRC22_D16(datain[i],CRC);
  }
  return CRC;
}


std::bitset<22> TMB::nextCRC22_D16(const std::bitset<16>& D, 
				       const std::bitset<22>& C){
  std::bitset<22> NewCRC;
  
  NewCRC[ 0] = D[ 0] ^ C[ 6];
  NewCRC[ 1] = D[ 1] ^ D[ 0] ^ C[ 6] ^ C[ 7];
  NewCRC[ 2] = D[ 2] ^ D[ 1] ^ C[ 7] ^ C[ 8];
  NewCRC[ 3] = D[ 3] ^ D[ 2] ^ C[ 8] ^ C[ 9];
  NewCRC[ 4] = D[ 4] ^ D[ 3] ^ C[ 9] ^ C[10];
  NewCRC[ 5] = D[ 5] ^ D[ 4] ^ C[10] ^ C[11];
  NewCRC[ 6] = D[ 6] ^ D[ 5] ^ C[11] ^ C[12];
  NewCRC[ 7] = D[ 7] ^ D[ 6] ^ C[12] ^ C[13];
  NewCRC[ 8] = D[ 8] ^ D[ 7] ^ C[13] ^ C[14];
  NewCRC[ 9] = D[ 9] ^ D[ 8] ^ C[14] ^ C[15];
  NewCRC[10] = D[10] ^ D[ 9] ^ C[15] ^ C[16];
  NewCRC[11] = D[11] ^ D[10] ^ C[16] ^ C[17];
  NewCRC[12] = D[12] ^ D[11] ^ C[17] ^ C[18];
  NewCRC[13] = D[13] ^ D[12] ^ C[18] ^ C[19];
  NewCRC[14] = D[14] ^ D[13] ^ C[19] ^ C[20];
  NewCRC[15] = D[15] ^ D[14] ^ C[20] ^ C[21];
  NewCRC[16] = D[15] ^ C[ 0] ^ C[21];
  NewCRC[17] = C[ 1];
  NewCRC[18] = C[ 2];
  NewCRC[19] = C[ 3];
  NewCRC[20] = C[ 4];
  NewCRC[21] = C[ 5];

  return NewCRC;
}


int TMB::TestArray(){
   std::cout << "In TestArray" << std::endl;
   int data[] = {      
 0x6b0c 
 ,0x13e7 
 ,0x1291 
 ,0x135f 
 ,0x5ba 
 ,0x13 
 ,0x3ec 
 ,0x12e9 
 ,0x402d 
 ,0x157 
 ,0x515 
 ,0x29d1 
 ,0x5125 
 ,0x1660 
 ,0x3aa5 
 ,0x1700 
 ,0x243f 
 ,0x5d 
 ,0x21 
 ,0xbb9 
 ,0x0 
 ,0x53 
 ,0xcff 
 ,0x23 
 ,0x24 
 ,0x38ca 
 ,0x6e0b 
 ,0x0 
 ,0x0 
 ,0x0 
 ,0x0 
 ,0x0 
 ,0x0 
 ,0x100 
 ,0x100 
 ,0x100 
 ,0x100 
 ,0x100 
 ,0x100 
 ,0x200 
 ,0x200 
 ,0x200 
 ,0x200 
 ,0x200 
 ,0x200 
 ,0x300 
 ,0x300 
 ,0x300 
 ,0x300 
 ,0x300 
 ,0x300 
 ,0x400 
 ,0x400 
 ,0x400 
 ,0x400 
 ,0x400 
 ,0x400 
 ,0x500 
 ,0x500 
 ,0x500 
 ,0x500 
 ,0x500 
 ,0x500 
 ,0x600 
 ,0x600 
 ,0x600 
 ,0x600 
 ,0x600 
 ,0x600 
 ,0x1000 
 ,0x1000 
 ,0x1000 
 ,0x1000 
 ,0x1000 
 ,0x1000 
 ,0x1100 
 ,0x1100 
 ,0x1100 
 ,0x1100 
 ,0x1100 
 ,0x1100 
 ,0x1200 
 ,0x1200 
 ,0x1200 
 ,0x1200 
 ,0x1200 
 ,0x1200 
 ,0x1300 
 ,0x1300 
 ,0x1300 
 ,0x1300 
 ,0x1300 
 ,0x1300 
 ,0x1400 
 ,0x1400 
 ,0x1400 
 ,0x1400 
 ,0x1400 
 ,0x1400 
 ,0x1500 
 ,0x1500 
 ,0x1500 
 ,0x1500 
 ,0x1500 
 ,0x1500 
 ,0x1600 
 ,0x1600 
 ,0x1600 
 ,0x1600 
 ,0x1600 
 ,0x1600 
 ,0x2000 
 ,0x2000 
 ,0x2000 
 ,0x2000 
 ,0x2000 
 ,0x2000 
 ,0x2100 
 ,0x2180 
 ,0x2100 
 ,0x2100 
 ,0x2100 
 ,0x2100 
 ,0x2280 
 ,0x2280 
 ,0x2280 
 ,0x2200 
 ,0x2200 
 ,0x2200 
 ,0x2300 
 ,0x2300 
 ,0x2380 
 ,0x2300 
 ,0x2300 
 ,0x2300 
 ,0x2400 
 ,0x2400 
 ,0x2400 
 ,0x2400 
 ,0x2400 
 ,0x2400 
 ,0x2500 
 ,0x2500 
 ,0x2500 
 ,0x2500 
 ,0x2500 
 ,0x2500 
 ,0x2600 
 ,0x2600 
 ,0x2600 
 ,0x2600 
 ,0x2600 
 ,0x2600 
 ,0x3000 
 ,0x3000 
 ,0x3000 
 ,0x3000 
 ,0x3000 
 ,0x3000 
 ,0x3100 
 ,0x3100 
 ,0x3100 
 ,0x3101 
 ,0x3100 
 ,0x3100 
 ,0x3200 
 ,0x3200 
 ,0x3200 
 ,0x3200 
 ,0x3200 
 ,0x3200 
 ,0x3300 
 ,0x3300 
 ,0x3300 
 ,0x3301 
 ,0x3300 
 ,0x3302 
 ,0x3400 
 ,0x3400 
 ,0x3400 
 ,0x3400 
 ,0x3401 
 ,0x3400 
 ,0x3500 
 ,0x3500 
 ,0x3500 
 ,0x3500 
 ,0x3500 
 ,0x3500 
 ,0x3600 
 ,0x3600 
 ,0x3600 
 ,0x3600 
 ,0x3601 
 ,0x3600 
 ,0x4000 
 ,0x4000 
 ,0x4000 
 ,0x4000 
 ,0x4000 
 ,0x4000 
 ,0x4100 
 ,0x4100 
 ,0x4100 
 ,0x4100 
 ,0x4100 
 ,0x4100 
 ,0x4200 
 ,0x4200 
 ,0x4200 
 ,0x4200 
 ,0x4200 
 ,0x4200 
 ,0x4300 
 ,0x4300 
 ,0x4300 
 ,0x4300 
 ,0x4300 
 ,0x4300 
 ,0x4400 
 ,0x4400 
 ,0x4400 
 ,0x4400 
 ,0x4400 
 ,0x4400 
 ,0x4500 
 ,0x4500 
 ,0x4500 
 ,0x4500 
 ,0x4500 
 ,0x4500 
 ,0x4600 
 ,0x4600 
 ,0x4600 
 ,0x4600 
 ,0x4600 
 ,0x4600 
 ,0x6b04 
 ,0xff 
 ,0xf7 
 ,0x1ff 
 ,0xf7 
 ,0x2ff 
 ,0xf7 
 ,0x3ff 
 ,0xf7 
 ,0x4ff 
 ,0xf7 
 ,0x5ff 
 ,0xf7 
 ,0x6ff 
 ,0xf7 
 ,0x10ff 
 ,0x17ff 
 ,0x11ff 
 ,0x17ff 
 ,0x12ff 
 ,0x17ff 
 ,0x13ff 
 ,0x17ff 
 ,0x14ff 
 ,0x17ff 
 ,0x15ff 
 ,0x17ff 
 ,0x16ff 
 ,0x17ff 
 ,0x20ff 
 ,0x20f7 
 ,0x21ff 
 ,0x20f7 
 ,0x22ff 
 ,0x20f7 
 ,0x23ff 
 ,0x20f7 
 ,0x24ff 
 ,0x20f7 
 ,0x25ff 
 ,0x20f7 
 ,0x26ff 
 ,0x20f7 
 ,0x30ff 
 ,0x37ff 
 ,0x31ff 
 ,0x37ff 
 ,0x32ff 
 ,0x37ff 
 ,0x33ff 
 ,0x37ff 
 ,0x34ff 
 ,0x37ff 
 ,0x35ff 
 ,0x37ff 
 ,0x36ff 
 ,0x37ff 
 ,0x6e04 
 ,0x6e0c 
 ,0xdf1a 
 ,0xdb92 
 ,0xde0f 
 ,-1
   };
   std::vector < std::bitset<16> > alct_data;
   for (int i=0; data[i] != -1; i++ ) {
	 alct_data.push_back((std::bitset<16>)data[i]);
   }
   int CRC_end  = alct_data.size();
   printf(" Size = %d \n",CRC_end);
   int CRC_low  = (alct_data[CRC_end-3].to_ulong()) &0x7ff ;
   int CRC_high = (alct_data[CRC_end-2].to_ulong()) &0x7ff ;
   int CRCdata  = (CRC_high<<11) | CRC_low ;
   printf(" CRC in data stream %x %x %x \n",alct_data[CRC_end-3].to_ulong(),alct_data[CRC_end-2].to_ulong(),CRCdata);
   printf(" CRC %x \n",TMBCRCcalc(alct_data));
   return 0;
}

int TMB::TMBCRCcalc(std::vector<std::bitset <16> >& TMBData) {
   std::bitset<22> CRC=calCRC22(TMBData);
   std::cout << " Test here " << CRC.to_ulong() << std::endl ;
   return CRC.to_ulong();
}


void TMB::DisableCLCTInputs(){
//
   int adr, alct_wdcnt, alct_busy, rd_data, wr_data, alct_rdata;
   int tmb_state, halt_state;
   adr = cfeb_inj_adr ;
   tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
   rd_data   = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
   sndbuf[0] = rcvbuf[0];
   sndbuf[1] = (rcvbuf[1] & 0xe0) ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
//
}

void TMB::EnableCLCTInputs(int CLCTInputs = 0x1f){
//
   int adr, alct_wdcnt, alct_busy, rd_data, wr_data, alct_rdata;
   int tmb_state, halt_state;
   adr = cfeb_inj_adr ;
   tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
   rd_data   = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
   sndbuf[0] = rcvbuf[0];
   sndbuf[1] = (rcvbuf[1] & 0xde) | CLCTInputs ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
//
}


void TMB::ALCTRawhits(){
//   
   int adr, alct_wdcnt, alct_busy, rd_data, wr_data, alct_rdata;
   int tmb_state, halt_state;
   std::vector < std::bitset<16> > alct_data;

   //Clear RAM address for next event and set sync bit
   adr = alctfifo1_adr ;
   wr_data  = 0x1; //reset RAM write address
   wr_data |= 0x1000;  // Set sync mode
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data  = 0x0; //unreset
   wr_data |= 0x1000;  // Set sync mode
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
   sleep(2);
   //
   while ( 1 < 2 ) {
      
      // Pretrigger halt
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      printf(" Start loop tmb_state machine  = %4ld\n",(rd_data>>15)&0x1);
      sndbuf[0] = (rcvbuf[0] & 0x7f) | 0x80 ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      
      adr = seqsm_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      tmb_state     = (rd_data & 0x7);
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      halt_state = ((rd_data>>15)&0x1);
      
      adr = alct_fifo_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      alct_wdcnt = (rd_data >> 2 ) & 0x1ff;
      alct_busy  = rd_data & 0x0001;
      
      printf("   word count         = %4ld\n",alct_wdcnt);
      printf("   busy               = %4ld\n",alct_busy);
      printf("   tmb_state machine  = %4ld\n",tmb_state);
      printf("   halt_state         = %4ld\n",halt_state);   

      while((alct_busy) || (tmb_state != 7 )) {
	 //
	 adr = seqsm_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 tmb_state     = (rd_data & 0x7);
	 //
	 adr = seq_clct_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 halt_state = ((rd_data>>15)&0x1);
	 //
	 adr = alct_fifo_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 alct_wdcnt = (rd_data >> 2 ) & 0x1ff;
	 alct_busy  = rd_data & 0x0001;
	 //
	 printf("   word count = %4ld\n",alct_wdcnt);
	 printf("   busy       = %4ld\n",alct_busy);
	 printf("   tmb_state machine  = %4ld\n",tmb_state);
	 printf("   halt_state         = %4ld\n",halt_state);   
	 //
      }
      //Write RAM read address to TMB
      for(int i=0;i<alct_wdcnt;i++) {
	 adr = alctfifo1_adr ;
	 wr_data = (i & 0x7FFF) << 1;
	 wr_data |= 0x1000;  // Set sync mode
	 sndbuf[0] = (wr_data & 0xff00)>>8 ;
	 sndbuf[1] = wr_data & 0x00ff ;
	 tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	 
	 //Read RAM data from TMB
	 adr = alctfifo2_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 alct_rdata = rd_data;
	 
	 printf("Adr=%4ld, Data=%5lx\n",i,alct_rdata);
	 alct_data.push_back((std::bitset<16>)alct_rdata);
      }

      if ( alct_wdcnt > 0 ) {
      
	 printf("The size is %d\n",alct_data.size());
	 
	 int CRC_end  = alct_data.size();
	 int CRC_low  = (alct_data[CRC_end-4].to_ulong()) &0x7ff ;
	 int CRC_high = (alct_data[CRC_end-3].to_ulong()) &0x7ff ;
	 int CRCdata  = (CRC_high<<11) | CRC_low ;
	 
	 int CRCcalc = TMBCRCcalc(alct_data) ;
	 
	 printf(" CRC %x \n",CRCcalc);
	 printf(" CRC in data stream %x %x %x \n",alct_data[CRC_end-4].to_ulong(),alct_data[CRC_end-3].to_ulong(),CRCdata);
	 
	 if ( CRCcalc != CRCdata ) {
	    printf("ALCT CRC doesn't agree \n");
	 } else {
	    printf("ALCT CRC does    agree \n");
	 }
	 
      }
      
      alct_data.clear();
      
      //Clear RAM address for next event
      adr = alctfifo1_adr ;
      wr_data = 0x1; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      //

      // Pretrigger unhalt
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      sndbuf[0] = (rcvbuf[0] & 0x7f) ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      
   }
}      
void TMB::ResetALCTRAMAddress(){
   //
   //Clear RAM address for next event
   //
   int adr, wr_data, rd_data;
   //
   adr = alctfifo1_adr ;
   wr_data = 0x1; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
}

void TMB::ResetRAMAddress(){
   //
   //Clear RAM address for next event
   //
   int adr, wr_data, rd_data;
   //
   adr = dmb_ram_adr ;
   wr_data = 0x2000; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
}

void TMB::TMBRawhits(){
   //
   int adr, dmb_wdcnt, dmb_busy, rd_data, wr_data, dmb_rdata, tmb_state, halt_state, tmb_state_comp;
   std::vector < std::bitset<16> > tmb_data;
   //
   //Clear RAM address for next event
   //
   adr = dmb_ram_adr ;
   wr_data = 0x2000; //reset RAM write address
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   wr_data = 0x0; //unreset
   sndbuf[0] = (wr_data & 0xff00)>>8 ;
   sndbuf[1] = wr_data & 0x00ff ;
   tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
   //
   sleep(2);
   //
   // Read forever
   //
   while ( 1 < 2 ) {
      //      
      // Pretrigger halt
      //
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      sndbuf[0] = (rcvbuf[0] & 0x7f) | 0x80 ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      //
      // Check state machine
      //
      adr = seqsm_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      tmb_state     = (rd_data & 0x7);
      tmb_state_comp=  rd_data ;
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      halt_state = ((rd_data>>15)&0x1);
      
      adr = dmb_wdcnt_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      dmb_wdcnt = rd_data & 0x0fff;
      dmb_busy  = (rd_data >> 14 ) & 0x0001;
    
      printf("   word count              = %4ld\n",dmb_wdcnt);
      printf("   busy                    = %4ld\n",dmb_busy);
      printf("   tmb_state machine       = %4ld\n",tmb_state);
      printf("   tmb_state machine comp  = %4ld\n",tmb_state_comp);
      printf("   halt_state              = %4ld\n",halt_state);
      
      while((dmb_busy) || (dmb_wdcnt <= 0) || (tmb_state != 7) ) {
	 
	 adr = seqsm_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 tmb_state     = (rd_data & 0x7);
	 
	 adr = dmb_wdcnt_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_wdcnt = rd_data & 0x0fff;
	 dmb_busy  = (rd_data >> 14 ) & 0x0001;
      
	 printf("   word count              = %4ld\n",dmb_wdcnt);
	 printf("   busy                    = %4ld\n",dmb_busy);
	 printf("   tmb_state machine       = %4ld\n",tmb_state);

      }
      //
      //Write RAM read address to TMB
      //
      for(int i=0;i<dmb_wdcnt;i++) {
	 adr = dmb_ram_adr ;
	 wr_data = (i & 0xFFFF);
	 sndbuf[0] = (wr_data & 0xff00)>>8 ;
	 sndbuf[1] = wr_data & 0x00ff ;
	 tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	 
	 //Read RAM data from TMB
	 adr = dmb_rdata_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_rdata = rd_data;
	 
	 printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
	 tmb_data.push_back((std::bitset<16>)dmb_rdata);
      }

      printf("The size is %d\n",tmb_data.size());

      int CRC_end  = tmb_data.size();
      int CRC_low  = (tmb_data[CRC_end-4].to_ulong()) &0x7ff ;
      int CRC_high = (tmb_data[CRC_end-3].to_ulong()) &0x7ff ;
      int CRCdata  = (CRC_high<<11) | CRC_low ;

      int CRCcalc = TMBCRCcalc(tmb_data) ;

      printf(" CRC %x \n",CRCcalc);
      printf(" CRC in data stream %x %x %x \n",tmb_data[CRC_end-4].to_ulong(),
	     tmb_data[CRC_end-3].to_ulong(),CRCdata);
      
      if ( CRCcalc != CRCdata ) {
	 printf("TMB CRC doesn't agree \n");
      } else {
	 printf("TMB CRC does    agree \n");
      }

      tmb_data.clear();
      //
      //Clear RAM address for next event
      //
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      //
      // Pretrigger unhalt
      //
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      sndbuf[0] = (rcvbuf[0] & 0x7f) ;
      sndbuf[1] = rcvbuf[1];
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
            
   }

}

void TMB::OnlyReadTMBRawhits(){
   //
   int adr, dmb_wdcnt, dmb_busy, rd_data, wr_data, dmb_rdata, tmb_state, halt_state, tmb_state_comp;
   std::vector < std::bitset<16> > tmb_data;
      //
      // Check state machine
      //
      adr = seqsm_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      tmb_state     = (rd_data & 0x7);
      tmb_state_comp=  rd_data ;
      
      adr = seq_clct_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      halt_state = ((rd_data>>15)&0x1);
      
      adr = dmb_wdcnt_adr ;
      tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      dmb_wdcnt = rd_data & 0x0fff;
      dmb_busy  = (rd_data >> 14 ) & 0x0001;
    
      printf("   word count              = %4ld\n",dmb_wdcnt);
      printf("   busy                    = %4ld\n",dmb_busy);
      printf("   tmb_state machine       = %4ld\n",tmb_state);
      printf("   tmb_state machine comp  = %4ld\n",tmb_state_comp);
      printf("   halt_state              = %4ld\n",halt_state);
      
      while((dmb_busy) || (dmb_wdcnt <= 0) ) {
	 
	 adr = seqsm_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 tmb_state     = (rd_data & 0x7);
	 
	 adr = dmb_wdcnt_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_wdcnt = rd_data & 0x0fff;
	 dmb_busy  = (rd_data >> 14 ) & 0x0001;
      
	 printf("   word count              = %4ld\n",dmb_wdcnt);
	 printf("   busy                    = %4ld\n",dmb_busy);
	 printf("   tmb_state machine       = %4ld\n",tmb_state);

      }
      //
      //Write RAM read address to TMB
      //
      for(int i=0;i<dmb_wdcnt;i++) {
	 adr = dmb_ram_adr ;
	 wr_data = (i & 0xFFFF);
	 sndbuf[0] = (wr_data & 0xff00)>>8 ;
	 sndbuf[1] = wr_data & 0x00ff ;
	 tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
	 
	 //Read RAM data from TMB
	 adr = dmb_rdata_adr ;
	 tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
	 rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
	 dmb_rdata = rd_data;
	 
	 printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
	 tmb_data.push_back((std::bitset<16>)dmb_rdata);
      }

      printf("The size is %d\n",tmb_data.size());

      int CRC_end  = tmb_data.size();
      int CRC_low  = (tmb_data[CRC_end-4].to_ulong()) &0x7ff ;
      int CRC_high = (tmb_data[CRC_end-3].to_ulong()) &0x7ff ;
      int CRCdata  = (CRC_high<<11) | CRC_low ;

      int CRCcalc = TMBCRCcalc(tmb_data) ;

      printf(" CRC %x \n",CRCcalc);
      printf(" CRC in data stream %x %x %x \n",tmb_data[CRC_end-4].to_ulong(),
	     tmb_data[CRC_end-3].to_ulong(),CRCdata);
      
      if ( CRCcalc != CRCdata ) {
	 printf("TMB CRC doesn't agree \n");
      } else {
	 printf("TMB CRC does    agree \n");
      }

      tmb_data.clear();
      //
      //Clear RAM address for next event
      //
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

}

void TMB::decode() {


  unsigned long int base_adr;
  unsigned long int boot_adr;
  unsigned long int adr;
  unsigned long int wr_data;
  unsigned long int rd_data;

  //  static const unsigned long int tmb_global_slot     = 26;
  //  static const unsigned long int tmb_brcst_slot      = 27;
  //  static const unsigned long int tmb_boot_adr        = 0x070000;
  static const unsigned long int islot               = 6;


  unsigned long int firmware_type;
  unsigned long int firmware_series;
  char *firmware_name;

  int rdscope;      //Boolean vars
  int scp_arm;
  int scp_readout;
  int disp_dump;

  unsigned long int clct0_vme;
  unsigned long int clct1_vme;
  unsigned long int clct0_msbs;
  unsigned long int clct1_msbs;

  unsigned long int clct0_vpf;
  unsigned long int clct0_pat;
  unsigned long int clct0_bend;
  unsigned long int clct0_key;
  unsigned long int clct0_cfeb;
  unsigned long int clct0_bxn;
  unsigned long int clct0_nhit;
  unsigned long int clct0_hsds;
  unsigned long int clct0_sync;
  unsigned long int clct0_bx0;

  unsigned long int clct1_vpf;
  unsigned long int clct1_pat;
  unsigned long int clct1_bend;
  unsigned long int clct1_key;
  unsigned long int clct1_cfeb;
  unsigned long int clct1_bxn;
  unsigned long int clct1_nhit;
  unsigned long int clct1_hsds;
  unsigned long int clct1_sync;
  unsigned long int clct1_bx0;

  unsigned long int dmb_wdcnt;
  //  unsigned long int dmb_adr;
  unsigned long int dmb_rdata;
  unsigned long int dmb_busy;

  char ians;
  unsigned long int i;
  int iloop;
  int nloop;
  //unsigned int kill_loop2 = 0;

  rd_data = 0; //temp
  ians = 'n'; //temp

  /*** Open VME Interface ***/


  //start(1) ;
  boot_adr = base_adr|0x070000;

  pfile = fopen("tmb_scope.txt","w");
  //pfile2 = fopen("TMBOUT.dat","w");

  /*** Get firmware type code ***/

  adr = base_adr;
  tmb_vme(VME_READ,0x0,sndbuf,rcvbuf,NOW);
  rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
  firmware_type = rd_data & 0xF;
  firmware_series = (rd_data >> 4) & 0xF;

  firmware_name = "TMB2001 type unknown ... beware";
  if((firmware_type == 0xC) && (firmware_series == 0xE)) firmware_name = "TMB2001E";

  printf(" TMB firmware %s\n Slot %02ld Adr %06lx\n",firmware_name,islot,base_adr);

  /*** Inject ALCT+CLCT then readout raw hits ***/

  int kill_loop=0;
  nloop = 0;

  //Single cycle or loop
  printf(" Loop? (y/n): ");
  scanf("%c",&ians);
  iloop = false;
  if((ians == 'y') || (ians == 'Y')) {
    iloop = true;
    printf(" How many events? (1-100): ");
    scanf("%d",&nloop);
  }
  else kill_loop = nloop;

  ians = 'n';
  printf("Display cathode dump on screen? (y/n): ");
  scanf("%c",&ians);
  if((ians == 'y') || (ians == 'Y')) disp_dump = true;
  else disp_dump = false;

  rdscope = true;
  //if(iloop) rdscope = false;
  //
  printf("\n");
  //
  //while(((iloop) || ((kill_loop++) <= 20))) {
  while( ((kill_loop++) <= nloop) )  {
    //
    while(FmState() == 1 ) {
      printf("%c7", '\033');
      printf("%c[01;37m",'\033');
      printf("%c8", '\033');
      printf("Waiting to get out of StopTrigger\n");
      printf("%c8", '\033');
    }
    printf("%c[0m", '\033');
    //
    printf("Event Number: %d\n",kill_loop);
    //
    //Wait for PHOS4 to initialize
    /*
    unsigned long int  phos4_lock = 0;
    while(!((phos4_lock != 0) || ((kill_loop2++) == 10000))) {
      adr = vme_phos4sm_adr ;
      status = tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
      rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
      phos4_lock = rd_data & 0x8000;
      if(phos4_lock == 0) printf("Waiting for PHOS4 lock");
    }
    */
    //Clear DMB RAM write-address
    adr = dmb_ram_adr ;
    wr_data = 0x2000;   //reset RAM write address
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    wr_data = 0x0000;   //unreset
    sndbuf[0] = (wr_data & 0xff00)>>8 ;
    sndbuf[1] = wr_data & 0x00ff ;
    tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

    //Arm scope trigger
    scp_arm = true;
    scp_readout = false;
    if(rdscope) scope(scp_arm,scp_readout);


    //Get latched CLCT words
    adr = seq_clct0_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct0_vme = rd_data;

    adr = seq_clct1_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct1_vme = rd_data;

    adr = seq_clctm_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    clct0_msbs = rd_data & 0x001F;
    clct1_msbs = (rd_data >> 5) & 0x001F;

    clct0_vme = clct0_vme | (clct0_msbs << 16);
    clct1_vme = clct1_vme | (clct1_msbs << 16);

    //if(!iloop) {
    //if(kill_loop == 0) {
    fprintf(pfile,"\n\n*********************************************\n");
    fprintf(pfile,"* Event Number: %d                          *\n",kill_loop);
    fprintf(pfile,"*********************************************\n");
      fprintf(pfile,"clct0_vme = %6.6lx\n",clct0_vme);
      fprintf(pfile,"clct1_vme = %6.6lx\n",clct1_vme);
      fprintf(pfile,"clctm_vme = %6.6lx\n",rd_data);
      //}

    //Decompose readout CLCT
    clct0_vpf   = (clct0_vme >>  0) & 0x0001;   //Valid pattern flag
    clct0_nhit  = (clct0_vme >>  1) & 0x0007;   //Hits on pattern
    clct0_pat   = (clct0_vme >>  4) & 0x0007;   //Pattern number
    clct0_hsds  = (clct0_vme >>  7) & 0x0001;   //1=1/2-strip, 0=distrip
    clct0_bend  = (clct0_vme >>  8) & 0x0001;   //Bend direction
    clct0_key   = (clct0_vme >>  9) & 0x001F;   //First 1/2-strip ID number
    clct0_cfeb  = (clct0_vme >> 14) & 0x0007;   //First CFEB ID
    clct0_bxn   = (clct0_vme >> 17) & 0x0003;   //Bunch crossing number
    clct0_sync  = (clct0_vme >> 19) & 0x0001;   //Sync error
    clct0_bx0   = (clct0_vme >> 20) & 0x0001;   //Bx0

    clct1_vpf   = (clct1_vme >>  0) & 0x0001;   //Valid pattern flag
    clct1_nhit  = (clct1_vme >>  1) & 0x0007;   //Hits on pattern
    clct1_pat   = (clct1_vme >>  4) & 0x0007;   //Pattern number
    clct1_hsds  = (clct1_vme >>  7) & 0x0001;   //1=1/2-strip, 0=distrip
    clct1_bend  = (clct1_vme >>  8) & 0x0001;   //Bend direction
    clct1_key   = (clct1_vme >>  9) & 0x001F;   //First 1/2-strip ID number
    clct1_cfeb  = (clct1_vme >> 14) & 0x0007;   //First CFEB ID
    clct1_bxn   = (clct1_vme >> 17) & 0x0003;   //Bunch crossing number
    clct1_sync  = (clct1_vme >> 19) & 0x0001;   //Sync error
    clct1_bx0   = (clct1_vme >> 20) & 0x0001;   //Bx0

    //if(!iloop) {
    //if(kill_loop == 0) {
      fprintf(pfile,"CLCT0: vpf= %4ld nhit= %4ld pat= %4ld hsds= %4ld bend= %4ld key= %4ld cfeb= %4ld\n",
              clct0_vpf,clct0_nhit,clct0_pat,clct0_hsds,clct0_bend,clct0_key,clct0_cfeb);

      fprintf(pfile,"CLCT1: vpf= %4ld nhit= %4ld pat= %4ld hsds= %4ld bend= %4ld key= %4ld cfeb= %4ld\n",
              clct1_vpf,clct1_nhit,clct1_pat,clct1_hsds,clct1_bend,clct1_key,clct1_cfeb);
      //}



    /***** Readout raw hits *****/

    //Read back embedded scope data
    scp_arm = false;
    scp_readout = true;
    if(rdscope) scope(scp_arm,scp_readout);

    //Get DMB RAM word count and busy bit
    adr = dmb_wdcnt_adr ;
    tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);
    rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
    dmb_wdcnt = rd_data & 0x0FFF;
    dmb_busy = (rd_data >> 14) & 0x0001;

    printf("   word count = %4ld\n",dmb_wdcnt);
    printf("   busy       = %4ld\n",dmb_busy);

    if((dmb_busy) || (dmb_wdcnt <= 0)) {
      if(dmb_busy) printf("Can not read RAM: dmb reports busy\n");
      if(dmb_wdcnt <= 0) printf("Can not read RAM: dmb reports word count <=0\n");
    } else {
      //Write RAM read address to TMB
      for(i=0;i<dmb_wdcnt;i++) {
        adr = dmb_ram_adr ;
        wr_data = i & 0xFFFF;
        sndbuf[0] = (wr_data & 0xff00)>>8 ;
        sndbuf[1] = wr_data & 0x00ff ;
        tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);

        //Read RAM data from TMB
        adr = dmb_rdata_adr ;
        tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read lsbs
        rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
        dmb_rdata = rd_data;
        adr = dmb_wdcnt_adr ;
        tmb_vme(VME_READ,adr,sndbuf,rcvbuf,NOW);     //read msbs
        rd_data = ((rcvbuf[0]&0xff) << 8) | (rcvbuf[1]&0xff) ;
        dmb_rdata = dmb_rdata | ((rd_data << 2) & 0x30000);     //rdata msbs

        if(disp_dump) printf("Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
        fprintf(pfile,"Adr=%4ld, Data=%5lx\n",i,dmb_rdata);
      }

      //Clear RAM address for next event
      adr = dmb_ram_adr ;
      wr_data = 0x2000; //reset RAM write address
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
      wr_data = 0x0000; //unreset
      sndbuf[0] = (wr_data & 0xff00)>>8 ;
      sndbuf[1] = wr_data & 0x00ff ;
      tmb_vme(VME_WRITE,adr,sndbuf,rcvbuf,NOW);
    }

  } //ends while(iloop) {

  /***** Exit: Close VME Interface *****/
  theController->end() ;
  fclose(pfile);
  //fclose(pfile2);
  //return 0;

} //main


void TMB::fifomode() {

}


void TMB::init_alct(int choice)
{
 //start(1);
 if ( choice ==1 ){
   printf(" Closing ALCT-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x59;
 }
 if ( choice ==2 ){
   printf(" Opening ALCT-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x58;
 }
 tmb_vme(VME_WRITE,0x32,sndbuf,rcvbuf,NOW);
 if ( choice ==1 ){
   printf(" Closing CFEB-TMB Input \n") ;
   sndbuf[0]=0x7c;
   sndbuf[1]=0x00;
 }
 if ( choice ==2 ){
   printf(" Opening CFEB-TMB Input \n") ;
   sndbuf[0]=0x7c;
   sndbuf[1]=0x1f;
 }
 tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
 if ( choice ==1 ){
   printf(" Closing CCB-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x3b;
 }
 if ( choice ==2 ){
   printf(" Opening CCB-TMB Input \n") ;
   sndbuf[0]=0x00;
   sndbuf[1]=0x38;
 }
 tmb_vme(VME_WRITE,0x2a,sndbuf,rcvbuf,NOW);
 end() ;
}


void TMB::load_cscid()
{
  
  int i=(theSlot/2)&0x000f;
  printf(" I am in CSC Slot %d \n",theSlot);
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  printf(" read CSC %02x %02x from TMB\n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );

  sndbuf[0]=( (rcvbuf[0]&0x00fe) | ((i>>3)&0x1)) & 0x00ff;
  sndbuf[1]=( (rcvbuf[1]&0x001f) | ((i<<5)&0xe0)) & 0x00ff;
  printf(" write CSC ID %02x %02x to TMB\n",sndbuf[0]&0x00ff,sndbuf[1]&0x00ff);
  tmb_vme(VME_WRITE,0x6e,sndbuf,rcvbuf,NOW);

  tmb_vme(VME_READ,0x6e,sndbuf,rcvbuf,NOW);
  printf(" read %02x %02x CSC from TMB \n",rcvbuf[0]&0x00ff,rcvbuf[1]&0x00ff );
}


void TMB::lvl1_delay(unsigned short int time)
{
  tmb_vme(VME_READ,seq_l1a_adr,sndbuf,rcvbuf,NOW);
  sndbuf[0]=  rcvbuf[0];
  sndbuf[1]=  (time&0x00ff);
  tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW);
}

int TMB::GetWordCount(){
   //
   tmb_vme(VME_READ,dmb_wdcnt_adr,sndbuf,rcvbuf,NOW);
   //
   return ( rcvbuf[1]&0xff | (rcvbuf[0]&0xf)>>8);
   //
}

void TMB::SetALCTPatternTrigger(){
   //
   sndbuf[0] = 0x0;
   sndbuf[1] = 0x2;
   tmb_vme(VME_WRITE,seq_trig_en_adr,sndbuf,rcvbuf,NOW);
   //
}

int TMB::GetALCTWordCount(){
   //
   tmb_vme(VME_READ,alct_fifo_adr,sndbuf,rcvbuf,NOW);
   //
   return ( (rcvbuf[1]>>2)&0x3f | (rcvbuf[0]&0x1f)>>6);
   //
}


void TMB::optimize()
{
  char rcvbuf_read[2];
  char rcvbuf_sav[2];
  char rcvbuf_sav2[2];
  //
  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  int ncounts; // ncounts_sum ;
  //
  int i,j,k,l,m ;
  FILE *pfile3;
  //

  pfile3 = fopen("Optimize.txt","w");

  //start(1) ;
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0xac,sndbuf,rcvbuf,NOW);
  //
  rcvbuf_sav2[0] = rcvbuf[0] ;
  rcvbuf_sav2[1] = rcvbuf[1] ;
  //
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0x70,sndbuf,rcvbuf,NOW);
  //
  rcvbuf_sav[0] = rcvbuf[0] ;
  rcvbuf_sav[1] = rcvbuf[1] ;
  printf("rcvbuf 0: %x  1: %x\n",rcvbuf_sav[0],rcvbuf_sav[1]);
  //
  //for (i=1;i<9;i++) {
  i = 5 ;                                                                                                                 sndbuf[0] = rcvbuf[0]  ;
  if ( i>5 ) sndbuf[1] = (rcvbuf[1]&0xf0) | (i-4)&0xf  ;
  else sndbuf[1] = (rcvbuf[1]&0xf0) | 0x1  ;
  tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf_read,1);
  //for (j=1;j<7;j++) {
  //for (k=1;k<7;k++) {
  //for (l=0;l<4;l++) {
  j=4;
  k=4;                                                                                                                    l=3;
  ncounts = 0;
  sndbuf[0] = (rcvbuf[0]&0x83) | (i&0xf) | ((j&0x7)<<4) ;
  sndbuf[1] = (rcvbuf[1]&0x80) | ((k&0x7)<<2) | ((l&3)<<5) ;
  //tmb_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW);
  // Loop over events
  for (m=0; m<20;  m++) {
  sndbuf[0] = 0x0 ;
  sndbuf[1] = 0x0 ;
  tmb_vme(VME_READ,0x78,sndbuf,rcvbuf_read,1);
  nhits     = (rcvbuf_read[1] & 0x0e)>>1 ;
  pattern   = (rcvbuf_read[1] & 0x70)>>4 ;
  striptype = (rcvbuf_read[1] & 0x80)>>7 ;
  cfebid    = (rcvbuf_read[0] & 0xc0)>>6 ;
  if ( nhits > 4 && striptype == 1 ) ncounts++ ;
  }
  printf ( " i %d j %d k %d l %d ncounts %d \n",i,j,k,l,ncounts ) ;
  fprintf(pfile3,"i %d j %d k %d l %d ncounts %d \n",i,j,k,l,ncounts ) ;
  //}
  //}
    //}
  //}

  sndbuf[0] = rcvbuf_sav[0] ;
  sndbuf[1] = rcvbuf_sav[1] ;
  tmb_vme(VME_WRITE,0x70,sndbuf,rcvbuf,NOW);
  sndbuf[0] = rcvbuf_sav2[0] ;
  sndbuf[1] = rcvbuf_sav2[1] ;
  tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf,NOW);
  end() ;
  fclose(pfile3);
}

void TMB::ForceScopeTrigger(){
  //
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0] | 0x2 ;
  sndbuf[1]=rcvbuf[1];
  tmb_vme(VME_WRITE,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
  sndbuf[0]=rcvbuf[0] ;
  sndbuf[1]=rcvbuf[1];
  tmb_vme(VME_WRITE,scp_ctrl_adr,sndbuf,rcvbuf,NOW);
  //
}

void TMB::read_delays()
{
  printf(" read delay registers \n");
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,NOW);
  printf(" delay register is: %02x%02x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
}


void TMB::reset() {
  sndbuf[0]=0x41;
  sndbuf[1]=0x90;
  tmb_vme(VME_READ,0x70000,sndbuf,rcvbuf,NOW);
  printf("Bootstrap %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff ) ;
}



void TMB::scan_rx_clock() 
{
  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  long int i;
  int j, time_setting ;
  long int ncounts,ncounts_sum ;
  long int cfeb[5][25];
  int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  char rcvbuf_old2[2];
  int time_set;
  int word_count[25];
  int pattern_quality[25];
  int alct_counts[25];

  goto ALCT;

  // CFEB part

  pfile = fopen("tmb_cfeb_scan.txt","w") ;

  //start(1) ;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x42,sndbuf,rcvbuf_old,1);
  printf("read %x%x \n",rcvbuf_old[0],rcvbuf_old[1]) ;
  for (cfeb_loop=0;cfeb_loop<5;cfeb_loop++) {
    printf("-------------------------------------------\n") ;
    printf("             CFEB %d \n",cfeb_loop) ;
    printf("-------------------------------------------\n") ;
    printf("\n") ;
    fprintf(pfile,"\n") ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"             CFEB %d \n",cfeb_loop) ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"\n") ;    
    time_setting = 0;
    ncounts_sum  = 0 ;
    printf("Masking cfeb %d \n",cfeb_loop) ;
    sndbuf[0] = rcvbuf_old[0];
    sndbuf[1] = (rcvbuf_old[1]&0xe0) | (1 << cfeb_loop ) ;
    printf("Setting %x%x \n",sndbuf[0],sndbuf[1]) ;
    tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
    for (j=0; j<25; j++ ) {
      tmb_clk_delays(j,cfeb_loop) ;
      ncounts = 0 ;
      for (i=0; i<100;  i++) {
	sndbuf[0] = 0x0 ;
	sndbuf[1] = 0x0 ;  
	tmb_vme(VME_READ,0x78,sndbuf,rcvbuf,NOW);
	nhits     = (rcvbuf[1] & 0x0e)>>1 ;
	pattern   = (rcvbuf[1] & 0x70)>>4 ;
	striptype = (rcvbuf[1] & 0x80)>>7 ;
	cfebid    = (rcvbuf[0] & 0xc0)>>6 ;
	if ( nhits > 4 && striptype == 1 ) ncounts++ ;
      }
      time_setting = time_setting + j*ncounts ;
      ncounts_sum  = ncounts_sum + ncounts ; 
      printf ( " time %d ncounts %d %d %d \n ",j,ncounts,time_setting,ncounts_sum ) ;
      cfeb[cfeb_loop][j] = ncounts ;
    }

    printf(" %d %d \n",time_setting,ncounts_sum) ;
    
    time_set = time_setting/ncounts_sum ;

    printf ( " Setting time to %d \n ",time_set) ;
    fprintf ( pfile, " Setting time to %d \n ",time_set) ;
    tmb_clk_delays(time_set,cfeb_loop) ;

    for(j=0;j<25;j++)
      printf("CFEB: %d, timebin %d, counts %ld\n",cfeb_loop,j,cfeb[cfeb_loop][j]);

    /*
    for (j=0;j<50;j++) {
      for (i=0;i<25;i++) {
	if ( cfeb[cfeb_loop][i] > 50-j ) {
	  printf ("*") ;
	  fprintf (pfile,"*") ;
	} else {
	  printf ("_") ;
	  fprintf (pfile,"_") ;
	}
      }
      printf("\n") ;
      fprintf(pfile,"\n") ;
    }
    */

  }
  //
  // Setting old values...
  //
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = rcvbuf_old[1];
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
  theController->end() ;
  fclose(pfile) ;

  //ALCT part

  return ;

 ALCT:

  start(1) ;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  //tmb_vme(VME_READ,0x2A,sndbuf,rcvbuf_old,1);
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x1 ;  //Disconnect CCB
  //tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  //tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<25;j++){
    tmb_clk_delays(j,5) ;
    sndbuf[0] = rcvbuf_old2[0];
    sndbuf[1] = (rcvbuf_old2[1]&0xfe) | 0x1 ;
    tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
    sndbuf[0] = rcvbuf_old2[0];
    sndbuf[1] = (rcvbuf_old2[1]&0xfe) | 0x0 ;
    tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
    sndbuf[0] = 0;
    sndbuf[1] = 0;
    tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW); 
    word_count[j] = ((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc) ;
    printf( " Setting %d word_count %d \n ",j,word_count[j] ) ;
    alct_counts[j] = 0 ;
    for (i=0;i<100;i++) {
      sndbuf[0] = 0;
      sndbuf[1] = 0;
      tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW); 
      pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
      if ( pattern_quality[j] == 3 ) (alct_counts[j])++ ;
    }
  }
  sndbuf[0] = rcvbuf_old[0] ;
  sndbuf[1] = rcvbuf_old[1] ;  //Old CCB status
  //tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  for (j=0;j<25;j++) printf(" %d ",word_count[j]);
  printf("\n") ;
  for (j=0;j<25;j++) printf(" %d  ",alct_counts[j]);
  printf("\n") ;
  endDevice() ;

}


void TMB::activecfeb()
{

  tmb_vme(VME_READ,0x68,sndbuf,rcvbuf,NOW);
//  printf("\n   current TMB adr 0x68 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  sndbuf[1]=rcvbuf[1];
  if((rcvbuf[0]&0x02)==0){
    sndbuf[0]=(rcvbuf[0]|0x02);
    printf("   set to ON!\n");
  }
  else{
    sndbuf[0]=(rcvbuf[0]&0xfd);
    printf("   set to OFF!\n");
  }

  printf("     send to TMB adr 0x68: 0x%02x%02x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  tmb_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW);


  tmb_vme(VME_READ,0x72,sndbuf,rcvbuf,NOW);
  printf("\n   current TMB adr 0x72 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
// [12:8] = #FIFO time bins before pretrigger
// [7:3] =  #FIFO time bins to read out
// [2:0] =  FIFO mode:
//           0=NoDump/1=FullDump/2=LocalDump/3=NoDumpShortHdr/4=NoDumpNoHdr

}



void TMB::toggle_l1req()
{

  tmb_vme(VME_READ,0x2c,sndbuf,rcvbuf,NOW);
//  printf("\n   current TMB adr 0x2c setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  sndbuf[0]=rcvbuf[0];
  if((rcvbuf[1]&0x07)==0){
    sndbuf[1]=(rcvbuf[1]|0x04);
    printf("   set TMB sequencer L1req to ON!\n");
  }
  else{
    sndbuf[1]=(rcvbuf[1]&0xf8);
    printf("   set all TMB L1req to OFF!\n");
  }

  printf("     send to TMB adr 0x2c: 0x%02x%02x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  tmb_vme(VME_WRITE,0x2c,sndbuf,rcvbuf,NOW);


  tmb_vme(VME_READ,0x72,sndbuf,rcvbuf,NOW);
  printf("\n   current TMB adr 0x72 setting:  0x%02x%02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
// [12:8] = #FIFO time bins before pretrigger
// [7:3] =  #FIFO time bins to read out
// [2:0] =  FIFO mode:
//           0=NoDump/1=FullDump/2=LocalDump/3=NoDumpShortHdr/4=NoDumpNoHdr

}


void TMB::trgmode_bprsq_dmb()
{
/* Old TMB:
  sndbuf[0]=0x01;
  sndbuf[1]=0x31;
*/
  sndbuf[0]=0x00;
  sndbuf[1]=0x50;
  tmb_vme(VME_WRITE,0x32,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x7c;
  sndbuf[1]=0x1f;
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x00;
  sndbuf[1]=0x0f;  //0b
  tmb_vme(VME_WRITE,0x86,sndbuf,rcvbuf,NOW); 
  sndbuf[0]=0x00;
  sndbuf[1]=0x08; //b9
  tmb_vme(VME_WRITE,0x68,sndbuf,rcvbuf,NOW); 
}




void TMB::firmwareVersion()
{
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x00,sndbuf,rcvbuf,NOW);
  printf(" TMB Version %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x02,sndbuf,rcvbuf,NOW);
  printf(" TMB month/date %02x/%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x04,sndbuf,rcvbuf,NOW);
  printf(" TMB year %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x06,sndbuf,rcvbuf,NOW);
  printf(" TMB revision %02x%02x \n", rcvbuf[0]&0xff, rcvbuf[1]&0xff);
}


void TMB::setLogicAnalyzerToDataStream(bool yesorno) {
  //Enable/Disable Logic Analyzer in data stream
  sndbuf[0]=0x00;
  sndbuf[1] = yesorno ? 0x38 : 0x18;
  tmb_vme(VME_WRITE,0x98,sndbuf,rcvbuf,NOW); // Scope Readout
}


void TMB::tmb_vme(char fcn, char vme,
                  const char *snd,char *rcv, int wrt) {
  start(1);
  do_vme(fcn, vme, snd, rcv, wrt);
}


void TMB::start() {
#ifdef debugV
  std::cout << "starting to talk to TMB, device " << ucla_ldev << std::endl;
#endif
   // send the first signal
  VMEModule::start();
  theController->initDevice(ucla_ldev, 0);
  theController->goToScanLevel();
}


void TMB::start(int idev) {
  if(idev != ucla_ldev) {
    endDevice();
    ucla_ldev = idev;
  }
  theController->start(this);
}


void TMB::end() {
#ifdef debugV
std::cout << "Ending TMB device " << ucla_ldev << std::endl;
#endif
char rcvx[2];
char sndx[2];
  if(ucla_ldev==1){
    tmb_vme(0xff,0x00,sndx,rcvx,1);
    theController->release_plev();
  }else{
    theController->send_last();
  }
  VMEModule::end();
}


int TMB::tmb_get_id(struct tmb_id_regs* tmb_id)
{
  unsigned short int value;

  tmb_get_reg(TMB_ADR_IDREG0, &value);
  tmb_id->fw_type = value & 0x000f;
  tmb_id->fw_version = (value >> 4) & 0x000f;
  tmb_id->brd_geo_addr = (value >> 8) & 0x000f;
  
  tmb_get_reg(TMB_ADR_IDREG1, &value);
  tmb_id->fw_day = value & 0x00ff;
  tmb_id->fw_month = (value >> 8) & 0x00ff;

  tmb_get_reg(TMB_ADR_IDREG2, &value);
  tmb_id->fw_year = value & 0xffff;

  tmb_get_reg(TMB_ADR_IDREG3, &value);
  tmb_id->fpga_type = value & 0xffff;

  return 0;
}

int TMB::tmb_set_jtag_src(unsigned short int jtag_src)
{ 
  unsigned short int value;

  tmb_get_boot_reg(&value);
  printf("0x%04x\n", value);
  value = (value & ~TMB_JTAG_SRC) | ((jtag_src & 0x01) << 7); // JTAG Source selection bit at position 7
  printf("0x%04x\n", value);
  // tmb_set_boot_reg(value);

  return 0;
} 

int TMB::tmb_get_jtag_src(unsigned short int* jtag_src)
{
  unsigned short int value = 0;

  tmb_get_boot_reg(&value);
  *jtag_src = ((value & TMB_JTAG_SRC) > 0) ? JTAG_HARD_SRC : JTAG_SOFT_SRC; 
  
  return 0;
}

int TMB::tmb_set_jtag_chain(unsigned int jchain)
{
   unsigned short int chain;
   unsigned short int jtag_src = 0;
   
   tmb_get_jtag_src(&jtag_src);

   if (jtag_src == JTAG_SOFT_SRC)
     {
	tmb_get_reg(TMB_ADR_USR_JTAG, &chain);
	chain = (chain & 0xff0f) | ((jchain & 0x0f) << 4);
	tmb_set_reg(TMB_ADR_USR_JTAG, chain);
     } 
   else
     {
	tmb_get_boot_reg(&chain);
	chain = (chain & 0xff87) | ( (jchain & 0x0f) << 3);
        tmb_set_boot_reg(chain);
     }

   return 0;
}

int TMB::tmb_set_reg(unsigned int vmereg, unsigned short int value )
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=value & 0xff;
   sndbuf[1]=(value >> 8) & 0xff;
   tmb_vme(VME_WRITE, vmereg, sndbuf, rcvbuf, NOW );
   return 0;	
}

int TMB::tmb_get_reg(unsigned int vmereg, unsigned short int* value )
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=0;
   sndbuf[1]=0;
   tmb_vme(VME_READ, vmereg, sndbuf, rcvbuf, NOW );
   *value = (rcvbuf[1]&0xff) | (rcvbuf[0]<<8);
   return 0;
}  

int TMB::tmb_vme_reg(unsigned int vmereg, unsigned short int* value)
{
   tmb_set_reg(vmereg, *value);
   tmb_get_reg(vmereg, value);
   return 0;
}

int TMB::tmb_jtag_io(unsigned char tms, unsigned char tdi, unsigned char* tdo)
{
  unsigned short int jtag_src = 0;
  unsigned short int value = 0;
  tmb_get_jtag_src(&jtag_src);
  if (jtag_src == JTAG_SOFT_SRC)
    {
	tmb_get_reg(TMB_ADR_USR_JTAG, &value);

        if (tdi > 0) 
          { value |= SOFT_TDI;}
	else 
	  { value &= ~SOFT_TDI & 0xffff;}; 

        if (tms > 0)
	  { value |= SOFT_TMS;}
	else 
	  { value &= ~SOFT_TMS & 0xffff;};

	tmb_set_reg(TMB_ADR_USR_JTAG, value);

	value |= SOFT_TCK;
        tmb_set_reg(TMB_ADR_USR_JTAG, value);

	tmb_get_reg(TMB_ADR_USR_JTAG, &value);
	*tdo = ((value & SOFT_TDO) > 0) ? 1 : 0;
	
	value &= ~SOFT_TCK & 0xffff;
        tmb_set_reg(TMB_ADR_USR_JTAG, value);
    }
  else
    {
        tmb_get_boot_reg(&value);
	if (tdi > 0) 
          { value |= HARD_TDI;}
        else 
          { value &= ~HARD_TDI & 0xffff;} 

        if (tms > 0)
          { value |= HARD_TMS;}
        else 
          { value &= ~HARD_TMS & 0xffff;}

        tmb_set_boot_reg(value);

        value |= HARD_TCK;
        tmb_set_boot_reg(value);

        tmb_get_boot_reg(&value);
        *tdo = ((value & HARD_TDO) > 0) ? 1 : 0;
        
        value &= ~HARD_TCK & 0xffff;
        tmb_set_boot_reg(value);
    };
  return 0;
}

int TMB::tmb_get_boot_reg(unsigned short int* value)
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=0;
   sndbuf[1]=0;
   tmb_vme(VME_READ | VME_BOOT_REG, 0, sndbuf, rcvbuf, NOW );
   *value = (rcvbuf[1]&0xff) | (rcvbuf[0]<<8);
   return 0;
}

int TMB::tmb_set_boot_reg(unsigned short int value)
{
   char sndbuf[2];
   char rcvbuf[2];
   sndbuf[0]=value & 0xff;
   sndbuf[1]=(value >> 8) & 0xff;
   tmb_vme(VME_WRITE | VME_BOOT_REG, 0, sndbuf, rcvbuf, NOW );
   return 0;   
}

int TMB::tmb_hard_reset_alct_fpga()
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  value &= ~ALCT_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  value |= ALCT_HARD_RESET;
  tmb_set_boot_reg(value);
  value &= ~ALCT_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  return 0;
}

int TMB::tmb_hard_reset_tmb_fpga()
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  value &= ~TMB_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  value |= TMB_HARD_RESET;
  tmb_set_boot_reg(value);
  value &= ~TMB_HARD_RESET & 0xffff;
  tmb_set_boot_reg(value);
  return 0;
}

int TMB::tmb_enable_alct_hard_reset(int flag_enable)
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  if(flag_enable>0)
    { value |= TMB_ENABLE_ALCT_RESET;}
  else 
    { value &= ~TMB_ENABLE_ALCT_RESET & 0xffff; }
  tmb_set_boot_reg(value);
  return 0;
}

int TMB::tmb_enable_vme_commands(int flag_enable)
{
  unsigned short int value = 0;
  tmb_get_boot_reg(&value);
  if(flag_enable>0) 
    { value |= TMB_ENABLE_VME;}
  else
    { value &= ~TMB_ENABLE_VME & 0xffff;} 
  tmb_set_boot_reg(value);
  return 0;
}


std::ostream & operator<<(std::ostream & os, TMB & tmb) {
  os << std::dec << "TMB: crate " << tmb.theCrate
     << " slot " << tmb.theSlot << "\n"
     << std::hex << "  cfeb delays (hex) " << tmb.cfeb0delay_ << " " 
     << tmb.cfeb1delay_ << " " << tmb.cfeb2delay_ << " " 
     << tmb.cfeb3delay_ << " " << tmb.cfeb4delay_ << "\n"
     << " rx, tx clock delays " << tmb.alct_rx_clock_delay_ 
     << " " << tmb.alct_tx_clock_delay_ << std::endl
     << "alct delay " << tmb.alct_vpf_delay_ 
     << "window size " << tmb.alct_match_window_size_ << std::dec << std::endl;
  return os;
}


void TMB::tmb_PHOS4_alct(int time) 
{
  //fg a more elegant solution is on its way
  int maxTimeBins(25);
  //
  if (version_=="2004")
    maxTimeBins=13;

  //int nhits;
  //int pattern ;
  //int striptype ;
  //int cfebid ;
  //long int i;
  int j,k; //time_setting ;
  //long int ncounts,ncounts_sum ;
  //long int cfeb[5][maxTimeBins];
  //int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  char rcvbuf_old2[2];
  //int time_set;
  int word_count1[maxTimeBins][maxTimeBins];
  int word_count2[maxTimeBins][maxTimeBins];
  int word_count3[maxTimeBins][maxTimeBins];
  int word_count4[maxTimeBins][maxTimeBins];
  int envelope[maxTimeBins][maxTimeBins];
  
  //int pattern_quality[maxTimeBins];
  int alct_counts[maxTimeBins][maxTimeBins];
  start(1);
  //ALCT part
  std::cout << "first call in PHOs4 alct " << std::endl;
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x2A,sndbuf,rcvbuf_old,1);
  std::cout << "write in PHOS4" << std::endl;
  //
  while(FmState() == 1 ) {
    printf("%c7", '\033');
    printf("%c[01;37m",'\033');
    printf("%c8", '\033');
    printf("Waiting to get out of StopTrigger\n");
    printf("%c8", '\033');
  }
  printf("%c[0m", '\033');
  //     
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x1 ;  //Disconnect CCB
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  std::cout << "going well " << std::endl;  
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       //
       while(FmState() == 1 ) {
	 printf("%c7", '\033');
	 printf("%c[01;37m",'\033');
	 printf("%c8", '\033');
	 printf("Waiting to get out of StopTrigger\n");
	 printf("%c8", '\033');
       }
       printf("%c[0m", '\033');
       //
	tmb_clk_delays(k,5) ;
	tmb_clk_delays(j,6) ;
	sndbuf[0] = 0;
	sndbuf[1] = 0x1 ;
	tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
	sndbuf[0] = 0;
	sndbuf[1] = 0x0 ;
	tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
	sndbuf[0] = 0;
	sndbuf[1] = 0;
	tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW); 
	word_count1[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
	sndbuf[0] = 0;
	sndbuf[1] = 0;
	//sleep(time); //fg wait <time> to collect significant statistics (not relevant)
	tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
	word_count2[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
	
	printf( " Setting %d word_count %d \n ",j,word_count1[j][k]-word_count2[j][k] ) ;
	alct_counts[j][k] = 0 ;
	/*
	  for (i=0;i<100;i++) {
	  sndbuf[0] = 0;	
	  sndbuf[1] = 0;
	  tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW); 
	  pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
	  if ( pattern_quality[j] == 3 ) (alct_counts[j][k])++ ;
	  }
	*/
     }
  }
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = (rcvbuf_old[1]&0xfe) | 0x0 ;  //Connect CCB
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0xA2,sndbuf,rcvbuf_old2,1);
  for (j=0;j<maxTimeBins;j++){
     for (k=0;k<maxTimeBins;k++) {
       //
       while(FmState() == 1 ) {
	 printf("%c7", '\033');
	 printf("%c[01;37m",'\033');
	 printf("%c8", '\033');
	 printf("Waiting to get out of StopTrigger\n");
	 printf("%c8", '\033');
       }
       printf("%c[0m", '\033');
       //
       tmb_clk_delays(k,5) ;
       tmb_clk_delays(j,6) ;
       sndbuf[0] = 0;
       sndbuf[1] = 0x1 ;
       tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
       sndbuf[0] = 0;
       sndbuf[1] = 0x0 ;
       tmb_vme(VME_WRITE,0xA2,sndbuf,rcvbuf,NOW); //Reset ALCT raw hits
       sndbuf[0] = 0;
       sndbuf[1] = 0;
       tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
       word_count3[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
       sndbuf[0] = 0;
       sndbuf[1] = 0;
       sleep(time); //fg wait <time> to collect significant statistics
       tmb_vme(VME_READ,0x3E,sndbuf,rcvbuf,NOW);
       word_count4[j][k] = (((rcvbuf[0]&0x1f)<<8) | (rcvbuf[1]&0xfc))>>2 ;
       //       
	printf( " Setting %d word_count %d \n ",j,word_count3[j][k]-word_count4[j][k] ) ;
	alct_counts[j][k] = 0 ;
	/*
	  for (i=0;i<100;i++) {
	  sndbuf[0] = 0;
	  sndbuf[1] = 0;
	  tmb_vme(VME_READ,0x3A,sndbuf,rcvbuf,NOW);
	  pattern_quality[j] = (rcvbuf[1]&0x6)>>1 ;
	  if ( pattern_quality[j] == 3 ) (alct_counts[j][k])++ ;
	  }
	*/
     }
  }
//
  pfile = fopen("tmb_alct.txt","w") ;
  //
  sndbuf[0] = rcvbuf_old[0] ;
  sndbuf[1] = rcvbuf_old[1] ;  //Old CCB status
  tmb_vme(VME_WRITE,0x2A,sndbuf,rcvbuf,NOW);
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	printf(" %04d ",abs(word_count1[j][k]-word_count2[j][k]));
	fprintf(pfile," %04d ",abs(word_count1[j][k]-word_count2[j][k]));
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
//
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	printf(" %04d ",abs(word_count3[j][k]-word_count4[j][k]));
	fprintf(pfile," %04d ",abs(word_count3[j][k]-word_count4[j][k]));
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
  //
  printf(" Envelope \n") ;
  printf("\n");
  printf("|-----------------> higher bits \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("| \n") ;
  printf("\\/ \n") ;
  printf("lower bits \n") ;
  printf("\n");
  fprintf(pfile," Envelope \n") ;
  fprintf(pfile,"\n");
  fprintf(pfile,"|-----------------> higher bits \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"| \n") ;
  fprintf(pfile,"\\/ \n") ;
  fprintf(pfile,"lower bits \n") ;
  fprintf(pfile,"\n");
//
  for (j=0;j<maxTimeBins;j++) {
     for (k=0;k<maxTimeBins;k++) {
	if ( word_count1[j][k] - word_count2[j][k] != 0 ) envelope[j][k] = 0 ;
	if ( word_count1[j][k] - word_count2[j][k] == 0 ) envelope[j][k] = abs(word_count3[j][k]-word_count4[j][k]) ;
	printf(" %04d ",envelope[j][k]);
	fprintf(pfile," %04d ",envelope[j][k]);
     }
     printf("\n") ;
     fprintf(pfile,"\n") ;
  }
  printf("\n") ;
  fprintf(pfile,"\n") ;
//   
  printf("\n") ;
  fclose(pfile);
//
}

void TMB::tmb_PHOS4_cfeb() 
{
  //fg a more elegant solution is on its way
  int maxTimeBins(25);
  if (version_=="2004")
    maxTimeBins=13;

  int nhits;
  int pattern ;
  int striptype ;
  int cfebid ;
  long int i;
  int j, time_setting ; //k
  long int ncounts,ncounts_sum ;
  long int cfeb[5][maxTimeBins];
  int cfeb_loop;
  FILE* pfile;
  char rcvbuf_old[2];
  //char rcvbuf_old2[2];
  int time_set;
  //int word_count1[maxTimeBins][maxTimeBins];
  //int word_count2[maxTimeBins][maxTimeBins];
  //int word_count3[maxTimeBins][maxTimeBins];
  //int word_count4[maxTimeBins][maxTimeBins];
  //int envelope[maxTimeBins][maxTimeBins];

  //int pattern_quality[maxTimeBins];
  //int alct_counts[maxTimeBins][maxTimeBins];


  // CFEB part

  pfile = fopen("tmb_cfeb_scan.txt","w") ;
  //start(1);
  sndbuf[0] = 0;
  sndbuf[1] = 0;
  tmb_vme(VME_READ,0x42,sndbuf,rcvbuf_old,1);
  printf("read %x%x \n",rcvbuf_old[0],rcvbuf_old[1]) ;
  for (cfeb_loop=0;cfeb_loop<1;cfeb_loop++) {
    printf("-------------------------------------------\n") ;
    printf("             CFEB %d \n",cfeb_loop) ;
    printf("-------------------------------------------\n") ;
    printf("\n") ;
    fprintf(pfile,"\n") ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"             CFEB %d \n",cfeb_loop) ;
    fprintf(pfile,"-------------------------------------------\n") ;
    fprintf(pfile,"\n") ;    
    time_setting = 0;
    ncounts_sum  = 0 ;
    printf("Masking cfeb %d \n",cfeb_loop) ;
    sndbuf[0] = rcvbuf_old[0];
    sndbuf[1] = (rcvbuf_old[1]&0xe0) | (1 << cfeb_loop ) ;
    printf("Setting %x%x \n",sndbuf[0],sndbuf[1]) ;
    tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
    for (j=0; j<maxTimeBins; j++ ) {
      tmb_clk_delays(j,cfeb_loop) ;
      ncounts = 0 ;
      for (i=0; i<10000;  i++) {
	sndbuf[0] = 0x0 ;
	sndbuf[1] = 0x0 ;  
	tmb_vme(VME_READ,0x78,sndbuf,rcvbuf,NOW);
	nhits     = (rcvbuf[1] & 0x0e)>>1 ;
	pattern   = (rcvbuf[1] & 0x70)>>4 ;
	striptype = (rcvbuf[1] & 0x80)>>7 ;
	cfebid    = (rcvbuf[0] & 0xc0)>>6 ;
	if ( nhits > 4 && striptype == 1 ) ncounts++ ;
      }
      time_setting = time_setting + j*ncounts ;
      ncounts_sum  = ncounts_sum + ncounts ; 
      printf ( " time %d ncounts %d %d %d \n ",j,ncounts,time_setting,ncounts_sum ) ;
      cfeb[cfeb_loop][j] = ncounts ;
    }

    printf(" %d %d \n",time_setting,ncounts_sum) ;
    
    time_set = time_setting/ncounts_sum ;

    printf ( " Setting time to %d \n ",time_set) ;
    fprintf ( pfile, " Setting time to %d \n ",time_set) ;
    tmb_clk_delays(time_set,cfeb_loop) ;

    for(j=0;j<maxTimeBins;j++)
      fprintf(pfile,"CFEB: %d, timebin %d, counts %ld\n",cfeb_loop,j,cfeb[cfeb_loop][j]);

    /*
    for (j=0;j<50;j++) {
      for (i=0;i<25;i++) {
	if ( cfeb[cfeb_loop][i] > 50-j ) {
	  printf ("*") ;
	  fprintf (pfile,"*") ;
	} else {
	  printf ("_") ;
	  fprintf (pfile,"_") ;
	}
      }
      printf("\n") ;
      fprintf(pfile,"\n") ;
    }
    */

  }
  //
  // Setting old values...
  //
  sndbuf[0] = rcvbuf_old[0];
  sndbuf[1] = rcvbuf_old[1];
  tmb_vme(VME_WRITE,0x42,sndbuf,rcvbuf,NOW);
  fclose(pfile) ;


  return ;

}


void TMB::tmb_clk_delays(unsigned short int time,int cfeb_id) {
  if(version_ == "2004") {
    new_clk_delays(time, cfeb_id);
  } else {
    old_clk_delays(time, cfeb_id);
  }
}


void TMB::new_clk_delays(unsigned short int time,int cfeb_id)
{
  //int ierr;
int iloop;
 iloop=0;
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,0);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,0);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,0);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,0);
    sndbuf[0]=(time&0x0f)|(rcvbuf[0]&0xf0);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(0x01,0x1A,sndbuf,rcvbuf,0);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,0);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(((time&0x0f)<<4)&0xf0)|(rcvbuf[1]&0x0f);
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(0x01,0x16,sndbuf,rcvbuf,0);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=(time&0x0f)|(rcvbuf[1]&0xf0);
    tmb_vme(0x02,0x16,sndbuf,rcvbuf,0);
  } 
  if ( cfeb_id == 1000 ) {
    tmb_vme(0x01,0x18,sndbuf,rcvbuf,0);
    sndbuf[0]=(((time&0xf)<<4)&0xf0)|(rcvbuf[0]&0x0f);
    sndbuf[1]=rcvbuf[1];
    tmb_vme(0x02,0x18,sndbuf,rcvbuf,0);
    sndbuf[1]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    sndbuf[0]=(time&0x0f)|(((time&0xf)<<4)&0xf0);
    tmb_vme(0x02,0x1A,sndbuf,rcvbuf,0);
  } 

  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,0);
  sndbuf[0]=0x0;
  sndbuf[1]=0x21;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,0);
  sndbuf[0]=0x0;
  sndbuf[1]=0x20;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,0);

  tmb_vme(0x01,0x14,sndbuf,rcvbuf,0);

  printf("______________ check state machine1 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);

  while((rcvbuf[1]&0x40)!=0x00){
    iloop++;
    if(iloop>10){
      printf(" tmb_clk_delays: loop count exceeded so quit \n");
      return;
    }
    tmb_vme(0x01,0x14,sndbuf,rcvbuf,0);
  }

  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1]&0xfe;
  tmb_vme(0x02,0x14,sndbuf,rcvbuf,0);
  tmb_vme(0x01,0x14,sndbuf,rcvbuf,0);

  printf(" check state machine2 %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x80)!=0x80){
    printf(" tmb_clk_delays: something is wrong. Can NOT be verified \n");
    return;
  }

  /* removed for new TMB delay chip
int ierr;
int iloop;
 iloop=0;
  //start(1); 
  printf(" write to delay registers \n");
  if ( cfeb_id == 0 ) {
    tmb_vme(VME_READ,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 2 ) {
    tmb_vme(VME_READ,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 3 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 4 ) {
    tmb_vme(VME_READ,0x1E,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 5 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=time&0x00ff;
    sndbuf[1]=rcvbuf[1];
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 6 ) {
    tmb_vme(VME_READ,0x16,sndbuf,rcvbuf,LATER);
    sndbuf[0]=rcvbuf[0];
    sndbuf[1]=time&0x00ff;
    tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,LATER);
  } 
  if ( cfeb_id == 1000 ) {
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,LATER);
    sndbuf[1]=time&0x00ff;
    sndbuf[0]=time&0x00ff;
    tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,LATER);
  } 

  sndbuf[0]=0x00;
  sndbuf[1]=0x00;
  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,LATER);
  printf(" check state machine %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00){
    printf(" tmb_clk_delays: state machine not ready return \n");
    return;
  }
  sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  sndbuf[0]=0x00;
  sndbuf[1]=0x77;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,LATER);
  // send delay to dynatem 
  sndbuf[0]=0x7f;
  sndbuf[1]=0xff;
  tmb_vme(0x03,0x00,sndbuf,rcvbuf,LATER); 
   sndbuf[0]=0x00;
  sndbuf[1]=0x33;
  tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW);

 
LOOPBACK:
  iloop=iloop+1;
  if(iloop>100){
    printf(" tmb_clk_delays: loop count exceeded so quit \n");
    return;
  }

  tmb_vme(VME_READ,0x14,sndbuf,rcvbuf,NOW);
  printf(" check state machine2  %02x %02x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  if((rcvbuf[1]&0x88)!=0x00)goto LOOPBACK;
  printf(" done so unstart state machine \n");
  */
}


void TMB::executeCommand(std::string command) {
  if(command=="PHOS4 ALCT")  tmb_PHOS4_alct();
  if(command=="PHOS4 CFEB")  tmb_PHOS4_cfeb(); 
  if(command=="Scan RX Clock") scan_rx_clock();
  if(command=="Show Firmware Version") firmwareVersion();
}


