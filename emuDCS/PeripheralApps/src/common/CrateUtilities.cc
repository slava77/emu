#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <sstream>
#include <algorithm>
//
#include "emu/pc/CrateUtilities.h"
#include "emu/pc/MPC.h"
#include "emu/pc/TMB.h"
#include "emu/pc/ALCTController.h"
#include "emu/pc/DAQMB.h"
#include "emu/pc/CCB.h"
#include "emu/pc/VMEController.h"
//
namespace emu {
  namespace pc {
//
CrateUtilities::CrateUtilities() : MpcTMBTestResult(-1), myCrate_(0)
{
  //
  debug_ = false;
  if(debug_) std::cout << "CrateUtilities" << std::endl ;
  //
  MyOutput_ = &std::cout ;
  //
}
//
//
CrateUtilities::~CrateUtilities(){
  //
  if(debug_) std::cout << "Destructor" << std::endl ;
  //
}
//
void CrateUtilities::MpcTMBTest(int Nloop) {
  MpcTMBTest(Nloop, 0, 0);  //default is no scan for "safe window"
  return;
}
//
void CrateUtilities::MpcTMBTest(int Nloop, int min_delay, int max_delay){
  //
  // Allows for scan over "safe window"
  //
     int NFound[255] = {};
  //
  if(min_delay==0 && max_delay==0) 
        myCrate_->mpc()->setDelay(0x30);  /* same as the default value in the firmware */
  int tmb_mask = myCrate_->mpc()->ReadMask();
  for (int delay=min_delay; delay<=max_delay; delay++)
  {
    //
    myCrate_->mpc()->SoftReset();
    //
    if (delay) {
      std::cout << "Delay value = " << delay << std::endl;
      myCrate_->mpc()->setDelay(delay);
    }
    //
    myCrate_->mpc()->read_fifos();   //read FIFOB to clear it
    //
    std::vector <TMB*> myTmbs = myCrate_->tmbs();

    int sequential_events_with_fifob_empty=0;
    //
    std::vector <unsigned long int> InjectedCSCId;
    //
    int NFrames = 5;
    int nloop;
    for(nloop = 0; nloop<Nloop; nloop++) {
      //
      std::cout << "Begin Loop " << nloop << std::endl;
      //
      //
      // clear the vectors of read out LCTs
      (myCrate_->mpc())->ResetFIFOBLct();
      //
      for (unsigned i=0; i<myTmbs.size(); i++) {
	 int TMBslot = myTmbs[i]->slot();
         int tmb_num = TMBslot/2;
         if(tmb_num>5) tmb_num--;
         int tmbmaskbit = tmb_mask & (1<<(9-tmb_num));
         if(tmbmaskbit==0)
	 {  myTmbs[i]->ResetInjectedLCT(); // clear the vectors of input LCTs
	    myTmbs[i]->InjectMPCData(NFrames,0,0); // inject Random Data
	   //      myTmbs[i]->ReadBackMpcRAM(NFrames)
         }
      }
      //
      //  myCrate_->ccb()->FireCCBMpcInjector();
      myCrate_->ccb()->injectTMBPattern();
      //
      ::usleep(50);
      myCrate_->mpc()->read_fifos();
      //
      if (((myCrate_->mpc())->GetFIFOBLct0()).size() == 0 ) {
	std::cout << "No data in FIFO B for event " << nloop << std::endl;
	sequential_events_with_fifob_empty++;
	//
	if (sequential_events_with_fifob_empty > 9) {
	  std::cout << "Number of events in a row with FIFO-B empty = " 
		    << std::dec << sequential_events_with_fifob_empty << std::endl;
	  std::cout << "...quitting..." << std::endl;
	  break;
	} else {
	  continue;
	}
      } else {
	sequential_events_with_fifob_empty = 0;
      }
      //
      int N_LCTs_found_this_pass = 0;
      //
      for (int frame = 0; frame < NFrames ; frame++ ) {
	//
	unsigned long int MPCLct0= ((myCrate_->mpc())->GetFIFOBLct0())[frame] ;
	unsigned long int MPCLct1= ((myCrate_->mpc())->GetFIFOBLct1())[frame] ;
	unsigned long int MPCLct2= ((myCrate_->mpc())->GetFIFOBLct2())[frame] ;
	//
	// Copy LCT data and sort according to the MPC algorithm...
	//
	std::vector <unsigned long int> SortingLCT;
	//
	for (unsigned i=0; i<myTmbs.size(); i++) {
	  int TMBslot = myTmbs[i]->slot();
          int tmb_num = TMBslot/2;
          if(tmb_num>5) tmb_num--;
          int tmbmaskbit = tmb_mask & (1<<(9-tmb_num));
	  if (debug_)
	    std::cout << "slot = " << std::dec << TMBslot << "..." << std::endl;
	  if ( tmbmaskbit==0 && (myTmbs[i]->GetInjectedLct0()).size()>0 ) {
	    //
	    // MPC algorithm picks the LCT according to:
            // 1) the highest quality
	    // 2) if two LCTs have the same quality, it picks the one from the highest number slot
	    //
	    // Can mimic this by inserting the CSC ID value (bits 12-15 in the LCT)
	    // behind the quality bits and sorting by largest value...
	    //
	    unsigned long int original_lct0_value = (myTmbs[i]->GetInjectedLct0())[frame];
	    //
	    if (debug_)
	      std::cout << "LCT0:  original value = " << std::hex << original_lct0_value << std::endl;
	    //
	    // extract the csc_id from the LCT
	    unsigned long int csc_id = (original_lct0_value & 0x0000f000) >> 12;
	    unsigned long int lct0_sorting_value = 
	      ( (original_lct0_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	      ( (original_lct0_value & 0x07ff0000) >>  4) | //move information over to make space for csc_id
	      ( (csc_id              & 0xf)        << 23);  //insert the csc_id
	    //
	    if (debug_) {
	      std::cout << "csc_id = " << std::hex << csc_id << std::endl;
	      std::cout << "LCT0:  shifted cscid  = " << std::hex << lct0_sorting_value << std::endl;
	    }
	    //
	    SortingLCT.push_back(lct0_sorting_value);
	    //
	    unsigned long int original_lct1_value = (myTmbs[i]->GetInjectedLct1())[frame];
	    //
	    if (debug_)
	      std::cout << "LCT1:  original value = " << std::hex << original_lct1_value << std::endl;
	    //
	    // extract the csc_id from the LCT
	    csc_id = (original_lct1_value & 0x0000f000) >> 12;
	    unsigned long int lct1_sorting_value = 
	      ( (original_lct1_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	      ( (original_lct1_value & 0x07ff0000) >>  4) | //move information over to make space for csc_id
	      ( (csc_id              & 0xf)        << 23);  //insert the csc_id
	    //
	    if (debug_) {
	      std::cout << "csc_id = " << std::hex << csc_id << std::endl;
	      std::cout << "LCT1:  shifted cscid  = " << std::hex << lct1_sorting_value << std::endl;
	    }	    
	    //
	    SortingLCT.push_back(lct1_sorting_value);
	  }
	}
	std::sort(SortingLCT.begin(),SortingLCT.end(), std::greater<int>() );
	//
	// remove slot from sorting vector to print out value:
	std::vector <unsigned long int> InjectedLCT;
	InjectedCSCId.clear();
	//
	for (unsigned vec=0; vec<SortingLCT.size(); vec++) {
	  //
	  unsigned long int original_value = SortingLCT[vec];
	  //
	  // extract the csc_id from the LCT
	  unsigned long int csc_id = (original_value & 0x07800000) >> 23;
	  unsigned long int return_value = 
	    ( (original_value & 0xf8000fff)      ) | //clear out information to make space for csc_id
	    ( (original_value & 0x007ff000) <<  4) | //move information over to make space for csc_id
	    ( (csc_id              & 0xf)   << 12);  //insert the csc_id
	    //
	    InjectedLCT.push_back(return_value);
	    InjectedCSCId.push_back(csc_id);
	}
	//
	std::cout << "MPC data in event " << frame << ":" << std::hex 
		  << " " << MPCLct0 << " " << MPCLct1 << " " << MPCLct2 
		  << std::endl ;
	//
	std::cout << "Data sorted in code: ";
	for (unsigned vec=0; vec<InjectedLCT.size(); vec++) 
	  std::cout << std::hex << InjectedLCT[vec] << " "  ;
	std::cout << std::endl ;
	//
	if (debug_) {
	  std::cout << "Data sorted w/cscid: ";
	  for (unsigned vec=0; vec<SortingLCT.size(); vec++) 
	    std::cout << std::hex << SortingLCT[vec] << " "  ;
	  std::cout << std::endl;
	}
	//
	if ( (InjectedLCT[0]|0x800) == (MPCLct0|0x800) ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 0" << std::endl;
	}
	//
	if ( (InjectedLCT[1]|0x800) == (MPCLct1|0x800) ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 1" << std::endl;
	}
	//
	if ( (InjectedLCT.size()<3) || (InjectedLCT[2]|0x800) == (MPCLct2|0x800) ) {
	  NFound[delay] ++;
	  N_LCTs_found_this_pass++;
	} else {
	  std::cout << "LCT source TMB  = ";
	  for (unsigned vec=0; vec<InjectedCSCId.size(); vec++) 
	    std::cout << std::dec << std::setw(9) << InjectedCSCId[vec];
	  std::cout << std::endl ;
	  std::cout << "FAIL on LCT 2" << std::endl;
	  //
	}
	//
	//	for (unsigned i=0; i<myTmbs.size(); i++) {
	//	  if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct0 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct0 )  {
	//	      std::cout << " LCT0 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct1 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct1 )  {
	//	      std::cout << " LCT1 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	    if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct2 ||
	//		 ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct2 )  {
	//	      std::cout << " LCT2 ";
	//	      NFound[delay]++;
	//	      N_LCTs_found_this_pass++;
	//	    }
	//	  }
	//	}
	//	std::cout << std::endl;
      }
      //
      std::cout << "N_LCTs found this pass = " << std::dec << N_LCTs_found_this_pass << std::endl;
      std::cout << "N_LCTs found for delay " << std::dec << delay << " = " << NFound[delay] << std::endl;
      //
      if ( NFound[delay] == (nloop+1)*3*NFrames ) {
	//
	if (delay == 0) MpcTMBTestResult = 1;
	//
      } else {
	//
	if (delay == 0) MpcTMBTestResult = 0;
	break;
      }
    }
    //
    if (delay == 0 && MpcTMBTestResult == 1) {
      (*MyOutput_) << "TMB-MPC Crate Test PASS, N_LCT = " << std::dec << NFound[delay] << std::endl;
      std::cout << "TMB-MPC Crate Test PASS, N_LCT = " << std::dec << NFound[delay] << std::endl;
    } else if (delay == 0 && MpcTMBTestResult == 0) {
      (*MyOutput_) << "TMB-MPC Crate Test FAIL " << ", nloop=" << nloop << std::endl;
      std::cout << "TMB-MPC Crate Test FAIL " << ", nloop=" << nloop << std::endl;
    }
    //
    //    std::cout << "Broke out, try next delay..." << std::endl;
  }
  //
  if (min_delay>0 || max_delay>0) {
    (*MyOutput_) << "MPC safe-window scan:" << std::endl;
    for (int delay=min_delay; delay<=max_delay; delay++) {
      (*MyOutput_) << "N_LCT[ " << delay << "] = " << NFound[delay] << std::endl;
    }
  }
  //
  return;
}

  } // namespace emu::pc
 } // namespace emu
