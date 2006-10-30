#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
#include <sstream>
//
#include "CrateUtilities.h"
#include "MPC.h"
#include "TMB.h"
#include "CCB.h"
#include "VMEController.h"
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
void CrateUtilities::DumpTstoreTables(){
  //
  for( unsigned int periph_entry=0; 
       periph_entry<periph.size(); periph_entry++){
    std::cout << periph[periph_entry] << std::endl;
  }
  //
}
//
void CrateUtilities::CreateTstoreTables(){
  //
  //
  CCB * myCCB = myCrate_->ccb();
  MPC * myMPC = myCrate_->mpc();
  //
  std::ostringstream output;
  //
  output.str("");
  output << myCCB->GetBxOrbit();
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->GetCCBmode();
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->slot();
  periph.push_back(output.str());
  //
  output.str("");
  output << "2004" ;
  periph.push_back(output.str());
  //
  output.str("");
  output << "21" ;
  periph.push_back(output.str());
  //
  output.str("");
  output << "20/06/2004" ;
  periph.push_back(output.str());
  //
  output.str("");
  output << "v5" ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCrate_->number() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCrate_->GetLabel() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->Getl1adelay() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myMPC->GetBoardID() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myMPC->slot() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << ((myCrate_->number())+1) ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCrate_->vmeController()->port() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myMPC->GetSerializerMode() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->GetSPS25ns() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myMPC->GetTransparentMode() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->GetTTCmode() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->GetTTCrxCoarseDelay() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCCB->GetTTCrxID() ;
  periph.push_back(output.str());
  //
  output.str("");
  output << myCrate_->vmeController()->GetVMEAddress() ;
  periph.push_back(output.str());
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
