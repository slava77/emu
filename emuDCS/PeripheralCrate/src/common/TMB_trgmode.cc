//-----------------------------------------------------------------------
// $Id: TMB_trgmode.cc,v 2.6 2006/03/03 07:59:20 mey Exp $
// $Log: TMB_trgmode.cc,v $
// Revision 2.6  2006/03/03 07:59:20  mey
// Update
//
// Revision 2.5  2006/01/23 14:21:59  mey
// Update
//
// Revision 2.4  2005/09/22 12:54:52  mey
// Update
//
// Revision 2.3  2005/09/05 06:49:37  mey
// Fixed address
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:06  geurts
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

//choice = 1 CLCT
//choice = 2 ALCT
//choice = 3 SCINT
//choice = 4 DMB

//cable = 1 blue
//cable = 2 black

void TMB::trgmode(int choice)
{
  //int ierr;

  // Read address back
  tmb_vme(VME_READ, rpc_cfg_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1] & 0xf0 | rpc_exists_ & 0x0f ;
  // Change address
  tmb_vme(VME_WRITE, rpc_cfg_adr ,sndbuf,rcvbuf, NOW); 

  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_WRITE, vme_ratctrl_adr ,sndbuf,rcvbuf, NOW); 
  
  // Read address back
  tmb_vme(VME_READ, vme_loopbk_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=rcvbuf[1] & 0xfb | (ALCT_input_ << 2) ;
  printf(" ALCT_input %d \n",ALCT_input_);
  printf(" Setting ALCT input to %x %x \n",sndbuf[0],sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE, vme_loopbk_adr ,sndbuf,rcvbuf, NOW); 

  // Read address back
  tmb_vme(VME_READ, ccb_trig_adr ,sndbuf,rcvbuf, NOW); 
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=0;
  printf(" Disabling L1a_request %x %x \n",sndbuf[0],sndbuf[1]);
  // Change address
  tmb_vme(VME_WRITE, ccb_trig_adr ,sndbuf,rcvbuf, NOW); 


  // Read address back
  tmb_vme(VME_READ, seq_fifo_adr ,sndbuf,rcvbuf, NOW);
  printf(" ^^^ fifo_pretrig_ %d \n",fifo_pretrig_);
  printf(" ^^^ fifo_mode_ %d \n",fifo_mode_);
  printf(" ^^^ fifo_tbins_ %d \n",fifo_tbins_);
  sndbuf[0]= (fifo_pretrig_ & 0x1f) ;
  sndbuf[1]= (fifo_mode_ & 0x7) | ((fifo_tbins_ & 0x1f) << 3); 
  printf(" -------------- Reading 0x72 %x %x \n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  printf(" -------------- Writing 0x72 %x %x \n",sndbuf[0]&0xff,sndbuf[1]&0xff);
  // Change address
  tmb_vme(VME_WRITE, seq_fifo_adr ,sndbuf,rcvbuf, NOW); 
  
  //disable CCB input during VME programming
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,  ccb_cfg_adr,sndbuf,rcvbuf, NOW); //Read CCB Config
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=(rcvbuf[1] | 0x01);
  //Write back with ignore CCB
  tmb_vme(VME_WRITE, ccb_cfg_adr,sndbuf,rcvbuf, NOW); 

  //sndbuf[0]=0x03;
  //sndbuf[1]=0x9C;
  //tmb_vme(VME_WRITE, lhc_cycle_adr,sndbuf,rcvbuf,NOW); // Maximum bxn
  sndbuf[0]=0x7c;
  sndbuf[1]=0x1f;
  tmb_vme(VME_WRITE, cfeb_inj_adr,sndbuf,rcvbuf,NOW); //Mask CFEBs
  sndbuf[0]=0x01;
  sndbuf[1]=0x1b;
  //tmb_vme(VME_WRITE,0x86,sndbuf,rcvbuf,NOW); // don't touch
  if ( choice ==1 ){
    sndbuf[0]=0x00;
    sndbuf[1]=0x01;
  }
  if ( choice ==2 ){
    sndbuf[0]=0x00;
    sndbuf[1]=0x02; 
  }
  if ( choice ==3 ){
    sndbuf[0]=0x00;
    sndbuf[1]=0x20; 
  }
  if ( choice ==4 ){
    sndbuf[0]=0x00;
    sndbuf[1]=0x10; 
  }
  printf("TRGMODE %x %x %x" , 0x68, sndbuf[0], sndbuf[1]); 
  tmb_vme(VME_WRITE,seq_trig_en_adr,sndbuf,rcvbuf,NOW); // Sequencer Trigger Source

//ALCT match window size and pulse delay settings ...
  sndbuf[0] = mpc_tx_delay_ & 0xf;
  sndbuf[1] = alct_match_window_size_ * 16 + alct_vpf_delay_;
  printf("TRGMODE %x %x %x \n" , 0xb2, sndbuf[0], sndbuf[1]);
  tmb_vme(VME_WRITE,tmbtim_adr,sndbuf,rcvbuf,NOW); // ALCT delay
  //
  // l1a and bxn offsets 
  //
  printf(" **********  Setting bxn to %d l1a to %d \n",bxn_offset_,l1a_offset_);
  sndbuf[0] = (bxn_offset_>>4)&0xff ;
  sndbuf[1] = (l1a_offset_&0xf) | ((bxn_offset_&0xf)<<4);
  tmb_vme(VME_WRITE,seq_offset_adr,sndbuf,rcvbuf,NOW); // Sequencer Counter Offset
  //
  sndbuf[0]=0x21;
  sndbuf[1]=0x41;
  //tmb_vme(VME_WRITE,0xac,sndbuf,rcvbuf,NOW); // Trigger Modifier

  
//Pattern Thresholds (di/half/valid)
  //sndbuf[0]=0x4d;  // 3/3/3
  //sndbuf[1]=0xb5;
  //sndbuf[0]=0x45;  // 3/3/1
  //sndbuf[1]=0xb5;
  //sndbuf[0]=0x53;   // 7/4/4
  //sndbuf[1]=0xc5;
  //sndbuf[0]=0x47;   // 7/4/1
  //sndbuf[1]=0xc5;
  //sndbuf[0]=0x52;   // 4/4/4
  //sndbuf[1]=0x45;
  sndbuf[0]=0x46;   // 4/4/1 this is the best overall on 28-june-03
  sndbuf[1]=0x45;
  //sndbuf[0]=0x4F;   // 7/3/3 new
  //sndbuf[1]=0xD5;
  //sndbuf[0]=0x4D;   // 7/2/2 new
  //sndbuf[1]=0xA5;
  //sndbuf[0]=0x53;   // 7/2/4 new
  //sndbuf[1]=0xA5;
  //
  printf("TRGMODE %x %x %x" , seq_clct_adr, sndbuf[0], sndbuf[1]);
  tmb_vme(VME_WRITE,seq_clct_adr,sndbuf,rcvbuf,NOW); // Sequencer CLCT Conf.
  //
  if ( alct_clear_ == 1 ) {
    DisableALCTInputs();
  }

  setLogicAnalyzerToDataStream(false);

  //
  // Change PHOS4 setting
  //
  //
   printf (" Setting PHOS4 and slot depending settings on TMB slot .... %2d \n", theSlot);

  //L1A window size and pulse delay Chamber 1
   sndbuf[0]=l1a_window_size_ & 0x0f;
   sndbuf[1]=l1adelay_ & 0xff;
   printf("TRGMODE %x %x %x" , seq_l1a_adr, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,seq_l1a_adr,sndbuf,rcvbuf,NOW); // L1A delay

   // Read Trigger conf
   tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
   //
   printf("**********MPC delay %d \n",mpc_delay_);
   //
   printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
   sndbuf[0] = (rcvbuf[0] & 0xfe | (mpc_delay_ & 0x8)>>3) & 0xff;
   sndbuf[1] = (rcvbuf[1] & 0x1f | (mpc_delay_ & 0x7)<<5) & 0xff;
   printf("Setting address 0x86 to %x %x\n",sndbuf[0]&0xff,sndbuf[1]&0xff);
   tmb_vme(VME_WRITE,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Write Trigger conf

   sleep(1);

   std::cout << "Setting up delay chips for TMB version " << version_ << std::endl;
   if(version_ == "2004") {
     setupNewDelayChips();
   } else {
     setupOldDelayChips();
   }

  //enable CCB input after VME programming
  sndbuf[0]=0;
  sndbuf[1]=0;
  tmb_vme(VME_READ,0x2a,sndbuf,rcvbuf,NOW); //Read CCB Config
  sndbuf[0]=rcvbuf[0];
  sndbuf[1]=(rcvbuf[1] & 0xfe);
  //
  printf("TRGMODE %x %x %x" , 0x2a, sndbuf[0], sndbuf[1]);
  //
  tmb_vme(VME_WRITE,0x2a,sndbuf,rcvbuf,NOW); //Write back, enable CCB

  tmb_vme(VME_READ,tmb_trig_adr,sndbuf,rcvbuf,NOW); // Trigger conf
  //
  printf("Reading address 0x86 to %x %x\n",rcvbuf[0]&0xff,rcvbuf[1]&0xff);
  
  ::sleep(1);
  //
  if ( disableCLCTInputs_ ) DisableCLCTInputs();
  //
}


void TMB::setupNewDelayChips() {
   std::cout << "setting up new TMB delay chips" << std::endl;
   sndbuf[0]=((cfeb0delay_<<4)&0xF0);
   sndbuf[1]=0x00;
   tmb_vme(0x02,0x18,sndbuf,rcvbuf,1);  // CFEB0 DDD setting
   sndbuf[0]=((cfeb4delay_<<4)&0xF0)|(cfeb3delay_&0x0F);
   sndbuf[1]=((cfeb2delay_<<4)&0xF0)|(cfeb1delay_&0x0F);
   sndbuf[0]=((cfeb0delay_<<4)&0xF0);
   sndbuf[1]=0x00;
   tmb_vme(0x02,0x18,sndbuf,rcvbuf,1);  // CFEB0 DDD setting
   sndbuf[0]=((cfeb4delay_<<4)&0xF0)|(cfeb3delay_&0x0F);
   sndbuf[1]=((cfeb2delay_<<4)&0xF0)|(cfeb1delay_&0x0F);
   tmb_vme(0x02,0x1A,sndbuf,rcvbuf,1);  // CFEB1-4 DDD setting
   //
   sndbuf[0]=0x05;
   sndbuf[1]=((alct_rx_clock_delay_<<4)&0xF0)|(alct_tx_clock_delay_&0x0F);
   tmb_vme(0x02,0x16,sndbuf,rcvbuf,1);
   sndbuf[0]=0x00;
   sndbuf[1]=0x20;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
   sndbuf[0]=0x00;
   sndbuf[1]=0x21;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
   sndbuf[0]=0x00;
   sndbuf[1]=0x20;
   tmb_vme(0x02,0x14,sndbuf,rcvbuf,1); // PHOS4 state machine
}


void TMB::setupOldDelayChips() {
   sndbuf[0]=cfeb0delay_;
   sndbuf[1]=0x00;
   printf("TRGMODE %x %x %x" , 0x1a, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1A,sndbuf,rcvbuf,NOW);

   sndbuf[0]=cfeb2delay_;
   sndbuf[1]=cfeb1delay_;
   printf("TRGMODE %x %x %x" , 0x1c, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1C,sndbuf,rcvbuf,NOW);

   sndbuf[0]=cfeb4delay_;
   sndbuf[1]=cfeb3delay_;
   printf("TRGMODE %x %x %x" , 0x1e, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x1E,sndbuf,rcvbuf,NOW);

   //sndbuf[0]=0x00;  // Orig BeamTest setting
   //sndbuf[1]=0x0a;
   sndbuf[0] = alct_rx_clock_delay_;
   sndbuf[1] = alct_tx_clock_delay_;
   printf("TRGMODE %x %x %x" , 0x16, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x16,sndbuf,rcvbuf,NOW);

   sndbuf[0]=0x00;
   sndbuf[1]=0x07;
   printf("TRGMODE %x %x %x" , 0x18, sndbuf[0], sndbuf[1]);
   tmb_vme(VME_WRITE,0x18,sndbuf,rcvbuf,NOW);
   sndbuf[0]=0x88;
   sndbuf[1]=0x77;
   tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW); // PHOS4 state machine
   sndbuf[0]=0x88;
   sndbuf[1]=0x33;
   tmb_vme(VME_WRITE,0x14,sndbuf,rcvbuf,NOW); // PHOS4 state machine
   ::sleep(5);
}
