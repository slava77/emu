//-----------------------------------------------------------------------
// $Id: CalibDAQ.cc,v 2.14 2006/04/20 15:33:54 mey Exp $
// $Log: CalibDAQ.cc,v $
// Revision 2.14  2006/04/20 15:33:54  mey
// Update
//
// Revision 2.13  2006/04/19 15:19:06  mey
// Update
//
// Revision 2.12  2006/04/18 08:17:29  mey
// UPdate
//
// Revision 2.11  2006/04/11 15:27:42  mey
// Update
//
// Revision 2.10  2006/03/30 13:55:38  mey
// Update
//
// Revision 2.9  2006/03/27 09:51:42  mey
// UPdate
//
// Revision 2.8  2006/03/27 09:22:36  mey
// Update
//
// Revision 2.7  2006/03/24 16:40:36  mey
// Update
//
// Revision 2.6  2006/03/24 14:35:04  mey
// Update
//
// Revision 2.5  2006/03/07 09:22:58  mey
// Update
//
// Revision 2.4  2006/02/20 13:31:14  mey
// Update
//
// Revision 2.3  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.2  2006/01/18 19:38:16  mey
// Fixed bugs
//
// Revision 2.1  2006/01/11 17:00:01  mey
// Update
//
// Revision 2.0  2005/06/06 10:01:03  geurts
// calibration routines by Alex Tumanov and Jason Gilmore
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <unistd.h> // for sleep
#include <string>
#include "CalibDAQ.h"
#include "Crate.h"
#include "DAQMB.h"
#include "CCB.h"
#include "JTAG_constants.h"
#include "ChamberUtilities.h"

void CalibDAQ::loadConstants() {

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned i = 0; i < myCrates.size(); ++i) {
    loadConstants(myCrates[i]);
  }

}


void CalibDAQ::loadConstants(Crate * crate) {
  CCB * ccb = crate->ccb();
  ccb->configure();
  ::sleep(1);

  std::vector<DAQMB*> myDmbs = theSelector.daqmbs(crate);
  for(unsigned i =0; i < myDmbs.size(); ++i) {
    myDmbs[i]->restoreCFEBIdle();
    myDmbs[i]->restoreMotherboardIdle();
    myDmbs[i]->configure();
  }

}
//
void CalibDAQ::rateTest() {
  //
  int chip,ch,brd, nstrip;
  int counter = 0;
  float dac;

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned j = 0; j < myCrates.size(); ++j) {
    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);

    for (nstrip=0;nstrip<16;nstrip++) {  
      for (int j=0; j<10; j++) {
	dac=0.2+0.2*j;
	for(unsigned i =0; i < myDmbs.size(); ++i) {
	  myDmbs[i]->set_cal_dac(dac,dac);
	}
	for(unsigned i =0; i < myDmbs.size(); ++i) {    
	  for(brd=0;brd<5;brd++){
	    for(chip=0;chip<6;chip++){
	      for(ch=0;ch<16;ch++){
		myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	      }
	      myDmbs[i]->shift_array[brd][chip][nstrip]=EXT_CAP;
	    }
	  }
	  myDmbs[i]->buck_shift();
	}
	
	for (int tries=0;tries<20; tries++){
          counter++;
	  std::cout << "dac = " << dac <<
	    "  strip = " << nstrip << 
	    "  try = " << tries << 
	    "  event  = " << counter << std::endl;
	  ccb->pulse(1, 0xff);//pulse all dmbs in this crate
	  ::usleep(100);
	}

      } //end of loop by strips
    } //endof loop by amplitudes
  }//end of loop by crates
}
//
void CalibDAQ::pulseAllWires(){
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    (myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
    //
    usleep(100);
    //
    std::vector<ChamberUtilities> utils = (myCrates[j]->chamberUtilsMatch()) ;
    //
    for (int i = 0; i < utils.size() ; i++ ) {
      //
      utils[i].PulseAllWires();
      //
    }
    //
  }
  //
}
//
void CalibDAQ::pulseComparatorPulse(){
  //
  for (int thresh=0; thresh<35; thresh++) {
    for (int i=0;i<16;i++) {  
      pulseAllDMBs(25, i, 0.15, 1000,thresh*0.003+0.013);
    }
  }
  //
  for (int thresh=0; thresh<35; thresh++) {
    for (int i=0;i<16;i++) {  
      pulseAllDMBs(25, i, 0.35, 1000,thresh*0.003+0.049);
    }
  }
  //
}
//
void CalibDAQ::pulseComparator(){
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    (myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
    //
    usleep(100);
    //
    std::vector<ChamberUtilities> utils = (myCrates[j]->chamberUtilsMatch()) ;
    //
    int CLCTInputList[5] = {0x1,0x2,0x4,0x8,0x10};
    //
    for (int thresh=0; thresh<35; thresh++) {
      for (int input=0; input<5; input++) {
	for (int strip=0; strip<32; strip++) {
	  for (int i = 0; i < utils.size() ; i++ ) {	    
	    utils[i].GetDMB()->set_cal_dac(0.15,0.15);
	    utils[i].GetDMB()->set_comp_thresh(0.013+thresh*0.003);
	    utils[i].LoadCFEB(strip,CLCTInputList[input],true);
	  }
	  std::cout << "Pulsing now" << std::endl;
	  utils[0].GetCCB()->inject(25,0xff);
	  //
	  for (int i = 0; i < utils.size() ; i++ ) {	    
	    utils[i].GetTMB()->DecodeCLCT();
	    utils[i].GetTMB()->GetCounters();
	    utils[i].GetTMB()->PrintCounters();
	    utils[i].GetDMB()->PrintCounters();
	  }
	  //
	}
      }
    }
    //
    for (int thresh=0; thresh<35; thresh++) {
      for (int input=0; input<5; input++) {
	for (int strip=0; strip<32; strip++) {      	  
	  for (int i = 0; i < utils.size() ; i++ ) {
	    utils[i].GetDMB()->set_cal_dac(0.35,0.35);
	    utils[i].GetDMB()->set_comp_thresh(0.049+thresh*0.003);
	    utils[i].LoadCFEB(strip,CLCTInputList[input],true);
	  }
	  std::cout << "Pulsing now" << std::endl;
	  utils[0].GetCCB()->inject(25,0xff);
	}
      }
    }
    //
  }
  //
}
//
void CalibDAQ::pulseRandomWires(){
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    //(myCrates[j]->chamberUtils())[0].CCBStartTrigger();
    //
    //sleep(1);
    //
    std::vector<ChamberUtilities> utils = (myCrates[j]->chamberUtilsMatch()) ;
    //
    for (int i = 0; i < utils.size() ; i++ ) {
      //
      utils[i].PulseRandomALCT();
      //
    }
    //
  }
  //
}
//
void CalibDAQ::pedestalCFEB() { 
  //
  int ntim = 1;
  float dac = 0.0;
  int nsleep(100);
  int counter = 0;
  int nevents = 1000;
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for (int events = 0; events<nevents; events++) {
    //for (int i=0;i<1;i++) {  
      pulseAllDMBs(ntim, -1, dac, nsleep);  
      counter++;
      std::cout << "dac = " << dac <<
	//"  strip = " << i <<
	"  event  = " << counter << std::endl;
      //}
  }
}  
//
//
void CalibDAQ::pulseAllDMBs(int ntim, int nstrip, float dac, int nsleep, float thresh) { 
//injects identical pulse to all dmbs (EXT capacitors)
//in all crates one crate at a time          
  //
  int chip,ch,brd;
  //
  std::vector<Crate*> myCrates = theSelector.crates();
  //
  for(unsigned j = 0; j < myCrates.size(); ++j) {
    //
    (myCrates[j]->chamberUtilsMatch())[0].CCBStartTrigger();
    usleep(10);    usleep(10);
    //
    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);
    std::vector<TMB*> myTmbs   = theSelector.tmbs(myCrates[j]);
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->EnableInternalL1aSequencer();
      myTmbs[i]->DisableCLCTInputs();
      std::cout << "Disabling inputs for slot " << myTmbs[i]->slot() << std::endl;
      myTmbs[i]->DisableALCTInputs();
    }
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      //
      // set amplitude
      //
      myDmbs[i]->set_cal_dac(dac,dac);
      myDmbs[i]->set_comp_thresh(thresh);
      //
      //myDmbs[i]->set_comp_thresh(0.4);
      //
      // set external pulser for strip # nstrip on all 6 chips
      for(brd=0;brd<5;brd++){
	for(chip=0;chip<6;chip++){
	  for(ch=0;ch<16;ch++){
	    myDmbs[i]->shift_array[brd][chip][ch]=NORM_RUN;
	  }
	  if ( nstrip != -1 ) myDmbs[i]->shift_array[brd][chip][nstrip]=SMALL_CAP;
	}
      }
      //
      myDmbs[i]->buck_shift();
      //
      //set timing
      //ntim is the same as pulse_delay initially set in xml configuration file
      //
      myDmbs[i]->set_cal_tim_pulse(ntim);   
      //
    }
    //
    ::usleep(nsleep);
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->EnableCLCTInputs(0x1f);
    }
    //
    ccb->pulse(1, 0xff);//pulse all dmbs in this crate
    //
    for (unsigned i=0; i<myTmbs.size(); i++) {
      myTmbs[i]->DisableCLCTInputs();
    }
    ::usleep(nsleep);
    //
    for(unsigned i =0; i < myDmbs.size(); ++i) {
      myDmbs[i]->PrintCounters();
    }
    //
    for(unsigned i =0; i < myTmbs.size(); ++i) {
      myTmbs[i]->GetCounters();
      myTmbs[i]->PrintCounters();
      myTmbs[i]->ResetCounters();
    }
    //
  }
}
//
void CalibDAQ::injectAllDMBs(int ntim) { //injects identical pulse to all dmbs

  std::vector<Crate*> myCrates = theSelector.crates();

  for(unsigned j = 0; j < myCrates.size(); ++j) {

    CCB * ccb = myCrates[j]->ccb();
    std::vector<DAQMB*> myDmbs = theSelector.daqmbs(myCrates[j]);
    for(unsigned i =0; i < myDmbs.size(); ++i) {

      // set dac
      myDmbs[i]->set_cal_dac(1.0,1.0);

      // shift patterns for half strip 16 on all planes in all cfebs
      for(int k=0;k<6;k++) {
        for (int l=0;l<5;l++){
          myDmbs[i]->halfset(l,k,16);
        }
      }

      myDmbs[i]->buck_shift();

      // set timing
      // ntim is the same as inject_delay and it overrides the initial setting in the xml file
      myDmbs[i]->set_cal_tim_inject(ntim);  // 0-22 are valid times
    }

    ::usleep(1000);
    std::cout << "injecting one time" << std::endl;
    ccb->inject(1, 0xff);//inject all dmbs in this crate
  }
}




