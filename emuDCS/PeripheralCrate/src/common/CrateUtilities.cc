#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
//
#include "CrateUtilities.h"
#include "MPC.h"
#include "TMB.h"
#include "CCB.h"
//
using namespace std;
//
CrateUtilities::CrateUtilities() : myCrate_(0), MpcTMBTestResult(-1)
{
  //
  std::cout << "CrateUtilities" << endl ;
  //
}
//
//
CrateUtilities::~CrateUtilities(){
  //
  std::cout << "Destructor" << std::endl ;
  //
}
//
void CrateUtilities::MpcTMBTest(int Nloop){
  //
  int NFound = 0;
  //
  for(int nloop = 0; nloop<Nloop; nloop++) {
  //
  myCrate_->mpc()->SoftReset();
  myCrate_->mpc()->configure();
  myCrate_->mpc()->read_csr0();
  myCrate_->mpc()->read_fifos();
  myCrate_->mpc()->read_csr0();
  //
  std::vector <TMB*> myTmbs = myCrate_->tmbs();
  //
  for (int i=0; i<myTmbs.size(); i++) {
    //
    (myTmbs[i]->ResetInjectedLCT());
    //
  }
  //
  (myCrate_->mpc())->ResetFIFOBLct();
  //
  for (int i=0; i<myTmbs.size(); i++) {
    //
    myTmbs[i]->InjectMPCData(myTmbs.size(),0,0); //Random Data
    //
  }
  //
  myCrate_->ccb()->FireCCBMpcInjector();
  //
  myCrate_->mpc()->read_fifos();
  //
  if (((myCrate_->mpc())->GetFIFOBLct0()).size() == 0 ) {
    std::cout << "No data" << std::endl;
    return;
  }
  //
  int NInjected = 0;
  //
  int NFrames = myTmbs.size();
  if (NFrames > 5 ) NFrames = 5;
  //
  for( int frame = 0; frame < NFrames ; frame++ ) {
    //
    std::cout << " Frame " << frame << " " << std::hex << std::setw(8) <<
      " " << ((myCrate_->mpc())->GetFIFOBLct0())[frame] <<
      " " << ((myCrate_->mpc())->GetFIFOBLct1())[frame] <<
      " " << ((myCrate_->mpc())->GetFIFOBLct2())[frame] 
	      << std::endl ;
    //
    unsigned long int MPCLct0= ((myCrate_->mpc())->GetFIFOBLct0())[frame] ;
    unsigned long int MPCLct1= ((myCrate_->mpc())->GetFIFOBLct1())[frame] ;
    unsigned long int MPCLct2= ((myCrate_->mpc())->GetFIFOBLct2())[frame] ;
    //
    // Copy LCT data and sort...
    //
    std::vector <unsigned long int> InjectedLCT;
    //
    for (int i=0; i<myTmbs.size(); i++) {
      if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	InjectedLCT.push_back((myTmbs[i]->GetInjectedLct0())[frame]);
	InjectedLCT.push_back((myTmbs[i]->GetInjectedLct1())[frame]);
      }
    }
    std::sort(InjectedLCT.begin(),InjectedLCT.end(), std::greater<int>() );
    //
    std::cout << "Sorted : " ;
    for ( int vec=0; vec<InjectedLCT.size(); vec++) std::cout << InjectedLCT[vec] << " "  ;
    //
    std::cout << std::endl ;
    //
    for (int i=0; i<myTmbs.size(); i++) {
      if ( (myTmbs[i]->GetInjectedLct0()).size() ) {
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct0 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct0 )  {
	  std::cout << "Found0 " << std::endl;
	  NFound++;
	}
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct1 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct1 )  {
	  std::cout << "Found1 " << std::endl;
	  NFound++;
	}
	if ( ((myTmbs[i]->GetInjectedLct0())[frame]) == MPCLct2 ||
	     ((myTmbs[i]->GetInjectedLct1())[frame]) == MPCLct2 )  {
	  std::cout << "Found2 " << std::endl;
	  NFound++;
	}
      }
    }
  }
  //
  std::cout << "NFound " << std::dec << NFound%15 << " " << NFound << std::endl;
  if ( NFound == (nloop+1)*3*NFrames ) {
    std::cout << "Passed" << std::endl;
    MpcTMBTestResult = 1;
  } else {
    std::cout << "Failed " << " nloop=" << nloop << std::endl;
    MpcTMBTestResult = 0;
    break;
  }
  //
}
}
