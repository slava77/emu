//-----------------------------------------------------------------------
// $Id: MPC.cc,v 2.25 2006/07/06 07:31:48 mey Exp $
// $Log: MPC.cc,v $
// Revision 2.25  2006/07/06 07:31:48  mey
// MPC firmware loading added
//
// Revision 2.24  2006/07/05 09:29:18  mey
// Update
//
// Revision 2.23  2006/06/16 13:05:24  mey
// Got rid of Compiler switches
//
// Revision 2.22  2006/05/12 08:03:06  mey
// Update
//
// Revision 2.21  2006/02/16 09:41:47  mey
// Fixed byte swap
//
// Revision 2.20  2006/02/15 22:39:57  mey
// UPdate
//
// Revision 2.19  2006/02/15 10:48:43  mey
// Fixed byte swapping
//
// Revision 2.18  2006/02/13 19:51:01  mey
// Fix bugs
//
// Revision 2.17  2006/01/14 22:24:50  mey
// UPdate
//
// Revision 2.16  2006/01/13 10:11:51  mey
// Update
//
// Revision 2.15  2006/01/12 23:44:43  mey
// Update
//
// Revision 2.14  2006/01/12 22:36:13  mey
// UPdate
//
// Revision 2.13  2006/01/12 12:28:50  mey
// UPdate
//
// Revision 2.12  2006/01/11 16:58:25  mey
// Update
//
// Revision 2.11  2006/01/11 13:47:47  mey
// Update
//
// Revision 2.10  2005/12/08 12:00:07  mey
// Update
//
// Revision 2.9  2005/11/25 23:42:46  mey
// Update
//
// Revision 2.8  2005/11/22 15:15:01  mey
// Update
//
// Revision 2.7  2005/11/21 17:38:28  mey
// Update
//
// Revision 2.6  2005/11/02 16:16:14  mey
// Update for new controller
//
// Revision 2.5  2005/08/23 19:27:17  mey
// Update MPC injector
//
// Revision 2.4  2005/08/22 07:55:45  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.3  2005/08/17 12:27:22  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 19:22:35  geurts
// additions for MPC/SP connectivity tests (Dan Holmes)
//
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include <iomanip> 
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "MPC.h"
#include "VMEController.h"

MPC::MPC(int newCrate, int slot) : VMEModule(newCrate, slot),
				   TLK2501TxMode_(0), TransparentModeSources_(0), TMBDelayPattern_(0){
  std::cout << "MPC: module created in crate=" << this->crate() 
	    << " slot=" << this->slot() << std::endl;
  MyOutput_ = &std::cout ;
}


MPC::~MPC(){
  (*MyOutput_) << "MPC: module removed from crate=" << this->crate() 
       << " slot=" << this->slot() << std::endl;
}


void MPC::init() {
}

bool MPC::SelfTest() {
  //
  return 0;
  //
}

void MPC::configure() {
  int btd;
  int xfer_done[2];
  char data[2];
  char addr;
  
  (*MyOutput_) << "MPC: initialize" << std::endl;

  ReadRegister(CSR0);

  (*MyOutput_) << "MPC: turn off Resets" <<std::endl;
  addr = CSR0;
  data[1]=0x10;
  data[0]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  ReadRegister(CSR0);

  (*MyOutput_) << "MPC: logic reset" << std::endl;
  data[1]=0x12;
  data[0]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  ReadRegister(CSR0);

  (*MyOutput_) << "MPC: end logic Reset" << std::endl;
  data[1]=0x10;
  data[0]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  ReadRegister(CSR0);

  //read_csr0();

  (*MyOutput_) << "MPC: set default serializer TX mode ..." << std::endl;
  setTLK2501TxMode(TLK2501TxMode_);

  // Sorter Mode is the default power-up mode of the MPC.
  (*MyOutput_) << "MPC: set default MPC operation Mode ..." << std::endl;
  if (TransparentModeSources_)
    setTransparentMode(TransparentModeSources_);
  else
    setSorterMode();

  // The default power-up delays are always 0.
  (*MyOutput_) << "MPC: setting default TMB-MPC delays ..." << std::endl;
  setDelayFromTMB(TMBDelayPattern_);

  // report firmware version
  firmwareVersion();
}

void MPC::read_fifo(char address, char * data) {
  int xfer_done[2];
  int btd; // comes from d360_bt_open(btd_p,devname_p,0);
  //data[0] = 0x00;
  //data[1] = 0x00;
  do_vme(1, address, NULL, data, 1); 
}

void MPC::read_fifosA() {
  //
  char data[100];
  //
  std::cout.fill('0');
  //
  read_fifo(FIFO_A1a, data);  
  (*MyOutput_) << "MPC: FIFO-A1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A1b, data);
  (*MyOutput_) << "MPC: FIFO-A1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2a, data);
  (*MyOutput_) << "MPC: FIFO-A2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2b, data);
  (*MyOutput_) << "MPC: FIFO-A2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3a, data);
  (*MyOutput_) << "MPC: FIFO-A3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3b, data);
  (*MyOutput_) << "MPC: FIFO-A3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4a, data);
  (*MyOutput_) << "MPC: FIFO-A4a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4b, data);
  (*MyOutput_) << "MPC: FIFO-A4b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5a, data);
  (*MyOutput_) << "MPC: FIFO-A5a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5b, data);
  (*MyOutput_) << "MPC: FIFO-A5b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6a, data);
  (*MyOutput_) << "MPC: FIFO-A6a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6b, data);
  (*MyOutput_) << "MPC: FIFO-A6b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7a, data);
  (*MyOutput_) << "MPC: FIFO-A7a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7b, data);
  (*MyOutput_) << "MPC: FIFO-A7b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8a, data);
  (*MyOutput_) << "MPC: FIFO-A8a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8b, data);
  (*MyOutput_) << "MPC: FIFO-A8b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9a, data);
  (*MyOutput_) << "MPC: FIFO-A9a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9b, data);
  (*MyOutput_) << "MPC: FIFO-A9b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
}

void MPC::read_fifos() {
  //
  // Read FIFO-B until empty
  //
  (*MyOutput_) << "MPC:  Read FIFO-B" << std::endl;
  char data[100];
  //read_fifo(STATUS, data);
  read_fifo(CSR3, data);
  std::cout.fill('0');
  (*MyOutput_) << "FIFO status " << std::hex << data << std::endl;
  std::cout.fill(' ');
  //bool full_fifoa=(data[1]&0x0001);
  //bool empty_fifoa=(data[1]&0x0002)>>1;
  //bool full_fifob=(data[1]&0x0004)>>2;
  bool empty_fifob=(data[1]&0x0008)>>3;
  //
  unsigned long Lct0,Lct1, Lct2;
  Lct0=0;Lct1=0;Lct2=0;
  //
  if(empty_fifob) {
    (*MyOutput_) << "MPC: FIFO-B is empty!" << std::endl;
    return;
  } else {
    (*MyOutput_) << "MPC: 1st Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    (*MyOutput_) << std::hex;
    std::cout.fill('0');
    (*MyOutput_) << "MPC: FIFO-B1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = (Lct0<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    FIFOBLct0.push_back(Lct0);
    (*MyOutput_) << "MPC: FIFO-B1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)  << std::endl;
    (*MyOutput_) << "MPC: LCT0     = 0x" << std::setw(8) << Lct0 << std::endl ;
    //
    (*MyOutput_) << "MPC: 2nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    (*MyOutput_) << "MPC: FIFO-B2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = (Lct1<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    (*MyOutput_) << "MPC: FIFO-B2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    FIFOBLct1.push_back(Lct1);
    (*MyOutput_) << "MPC: LCT1     = 0x" << std::setw(8) << Lct1 << std::endl ;
    //
    (*MyOutput_) << "MPC: 3nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    (*MyOutput_) << "MPC: FIFO-B3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = (Lct2<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    (*MyOutput_) << "MPC: FIFO-B3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) <<(data[1]&0x00ff) << std::endl;
    FIFOBLct2.push_back(Lct2);
    (*MyOutput_) << "MPC: LCT2     = 0x" << std::setw(8) << Lct2 << std::endl ;
    std::cout.fill(' ');
    (*MyOutput_) << std::dec;    
    //
    read_fifos();
    //
  }
  //
  read_fifo(CSR3, data);
  (*MyOutput_) << "FIFO status = " << std::hex << data << std::endl;
  //
}

void MPC::read_csr0() {
  char data[100];
  read_fifo(CSR0,data);
  (*MyOutput_).fill('0');
  (*MyOutput_) << "MPC: data read from CSR0: 0x" << std::hex 
       << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)
       << std::dec << std::endl;
  (*MyOutput_).fill(' ');
}

void MPC::SoftReset() {
  //
  char data[2];
  int btd, xfer_done[2];
  //
  char addr =  CSR0;
  //
  do_vme(1, addr, NULL, data, 1);
  printf("%x %x \n",data[0],data[1]);
  //
  // reset FPGA logic
  //
  data[0] = (data[0]&0xfd) ;
  do_vme(2, addr, data, NULL, 1);
  printf("%x %x \n",data[0],data[1]);
  //
  data[0] = (data[0]&0xfd) | 0x2 ;
  do_vme(2, addr, data, NULL, 1);
  printf("%x %x \n",data[0],data[1]);
  //
  data[0] = (data[0]&0xfd) ;
  do_vme(2, addr, data, NULL, 1);
  printf("%x %x \n",data[0],data[1]);
  //
}

void MPC::read_status() {
  //Check FIFO Status:
  char data[100];
  //read_fifo(STATUS, data);
  read_fifo(CSR3, data);
  std::cout.fill('0');
  (*MyOutput_) << "MPC: FIFO status = 0x" << std::hex << std::setw(2) << (data[0]&0x00ff)
       << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
  std::cout.fill(' ');
  bool full_fifoa=(data[1]&0x0001);
  bool empty_fifoa=(data[1]&0x0002)>>1;
  bool full_fifob=(data[1]&0x0004)>>2;
  bool empty_fifob=(data[1]&0x0008)>>3;

  if (full_fifoa>0)  (*MyOutput_) << "MPC: FIFO_A is Full"  << std::endl;
  if (empty_fifoa>0) (*MyOutput_) << "MPC: FIFO_A is Empty" << std::endl;
  if (full_fifob>0)  (*MyOutput_) << "MPC: FIFO_B is Full"  << std::endl;
  if (empty_fifob>0) (*MyOutput_) << "MPC: FIFO_B is Empty" << std::endl;;
}


void MPC::executeCommand(std::string command) {
  if(command=="Init")   init();
  if(command=="Read FIFOs") read_fifos();
  if(command=="Read CSR0") read_csr0();
  //if(command=="Read Date") read_date();
  if(command=="Read Date") firmwareVersion();
}

void MPC::enablePRBS(){
  int btd, xfer_done[2];
  char data[2];
  char addr;

  // make sure we are in framed mode
  addr = CSR2;
  //fg read(btd,data,addr,2,xfer_done);
  //fg data[0]=data[0]|0x01;
  data[0]=0x00;
  data[1]=0x01;
  do_vme(2, addr, data, NULL, 1);

  // brute force set to 0xC210, will change to masking soon, very soon ...
  addr = CSR0;
  data[0]=0xCA;
  data[1]=0x10;
  do_vme(2, addr, data, NULL, 1);

  (*MyOutput_) << "MPC: PRBS mode enabled" << std::endl;
}

void MPC::disablePRBS(){
  int btd, xfer_done[2];
  char data[2];
  char addr;

  // brute force set back to 0x5B10, will change to masking soon, very soon ...
  addr = CSR0;
  data[0]=0x5B;
  data[1]=0x10;
  do_vme(2, addr, data, NULL, 1);

  (*MyOutput_) << "MPC: PRBS mode disabled" << std::endl;
}

void MPC::initTestLinks(){
// initialise FIFOs ready to load test data with injectSP functs
//nb! need to time in SP after this funct but before inject data

//initialise!!! toggle CSR0 bits.
 (*MyOutput_) << "Initialising peripheral crate links for load FIFOs" << std::endl;

  int btd, xfer_done[2];
  char data[2];
  char addr;


  addr = CSR0;
  data[1]=0x11;
  data[0]=0x4E;
  do_vme(2, addr, data, NULL, 1);
  
  data[1]=0x13;
  data[0]=0x4E;
  do_vme(2, addr, data, NULL, 1);
 
  data[1]=0x11;
  data[0]=0x4E;
  do_vme(2, addr, data, NULL, 1);
//done init
}

void MPC::injectSP(){

  int btd, xfer_done[2];
  char data[2];
  char addr;

  int ITR=255;

 this->read_status();
 
   (*MyOutput_) << "..now filling FIFO-A" <<std::endl;
	   // Fill FIFOA with 255 events
	   for (int EVNT=0; EVNT<255; EVNT++)
           {
             unsigned long mframe1, mframe2;

//             mframe1 = (0x1 << 15) | (0xF << 11) | EVNT;
 	mframe1 =  (0xF << 12) | EVNT;
//	     ITR--;
	ITR=3;
	     mframe2 = (0xF << 12) | ITR;

	     // Load 9x2 LCTs to MPC Input "FIFOA"
	     //9 buffers in FIFO-A corresponding to 9 TMBs
	     addr=FIFO_A1a;
	     for (int fifo_a=1; fifo_a<=9; fifo_a++) 
             {
	       for (int LCT=0; LCT<2; LCT++)  //dammit, no i think this should just be a loop over frames!!
//2 sorts of LCTs
	         {
		     data[1]= mframe1&0x00FF;
		     data[0]=(mframe1&0xFF00)>>8;
		     this->do_vme(2, addr, data, NULL, 1);

//2 frames?:ie 2x16 bits =tot 32 bits..

		     data[1]= mframe2&0x00FF;
		     data[0]=(mframe2&0xFF00)>>8;
		     this->do_vme(2, addr, data, NULL, 1);

		     addr+=0x00002;

	         }
	     } 
	   }
        

	// Append 0's to the last one..
	addr=FIFO_A1a;
	for (int fifo_a=1; fifo_a<=9; fifo_a++) {
	    for (int LCT=0; LCT<2; LCT++) {

		     data[0]=0;
		     data[1]=0;
		     this->do_vme(2, addr, data, NULL, 1);
		     data[0]=0;
		     data[1]=0;
		     this->do_vme(2, addr, data, NULL, 1);
		     addr+=0x00002;
	    }
    }
 
this->read_status();

}


void MPC::injectSP(char *injectDataFileName){


// member function as above but here you have passed a file 
// containing the LCTs you want to inject.
// data file should have a set of 16 bit (4 hex character) words 
  
  
  int btd, xfer_done[2];
  char data[2];
  char DataWord[4];//4 character hex string=16 bit word 
  int dataWordInt_fr1;// the above string "DataWord" converted -->int.::frame 1s
  int dataWordInt_fr2;// the above string "DataWord" converted -->int.::frame 2s
  unsigned long mframe1, mframe2;
  char addr;
  int readWord;//number of words read by fscanf
  
  (*MyOutput_)<<"...inject test pattern funct"<<std::endl;
  (*MyOutput_)<<"data file passed is: "<<injectDataFileName<<std::endl;
  //let's try to open the file:: 
  
  
  
  (*MyOutput_)<<"opening file..."<<std::endl;
  FILE* myFile = fopen(injectDataFileName , "r" );
  if (myFile==NULL){
    (*MyOutput_)<<"problem opening data file, exiting.."<<std::endl;
    exit(0);
  }
  
  
  
  this->read_status();
  
  (*MyOutput_)<<" will use LCT data from file "<<injectDataFileName<<std::endl;
  
  (*MyOutput_) << "..now filling FIFO-A, with your data, alternating frame 1, 2.." <<std::endl;
  //   int ITR=2;mframe2 = ITR;
  


  // read the first data word and turn it into and integer:
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr1);
  (*MyOutput_)<<"first frame 1 :: (first word in file) is "<<std::hex<<dataWordInt_fr1<<std::endl;
  if (readWord<1){
    (*MyOutput_)<<"problem reading first word in file ..exiting..."<<std::endl;
    exit(0);
  }
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr2);
  (*MyOutput_)<<"first frame 2 :: (second word in file) is "<<std::hex<<dataWordInt_fr2<<std::endl;    

  int EVNT=0;//event counter
  while (dataWordInt_fr1 !=0 && dataWordInt_fr2 !=0)//keep on looping while we have valid words
    {
      
      EVNT++;
      //             mframe1 = (0x1 << 15) | (0xF << 11) | dataWordInt_fr1;
      mframe1 =  dataWordInt_fr1;
      mframe2 =  dataWordInt_fr2;
      
      // Load 2 LCTs to each of 8/9!! buffers in FIFO-A corresponding to 9TMBs
      addr=FIFO_A1a;
      for (int fifo_a=1; fifo_a<=9; fifo_a++) //hmm why do we stop at 8??  ..9TMBs and FIFO-A9..?
	{
	  	  
	  for (int LCT=0; LCT<2; LCT++)
	    //2 sorts of LCTs
	    {
	      data[1]= mframe1&0x00FF;
	      data[0]=(mframe1&0xFF00)>>8;
	      this->do_vme(2, addr, data, NULL, 1);
	      
	      //(*MyOutput_)<<"event: "<<EVNT<<"...filling fifoA with "<<std::hex<<mframe1<<std::endl;
	      
	      //2 frames?:ie 2x16 bits =tot 32 bits..
	      
	      data[0]= mframe2&0x00FF;
	      data[1]=(mframe2&0xFF00)>>8;
	      this->do_vme(2, addr, data, NULL, 1);
	      
	      addr+=0x00002;
	      
	    }//end for loop, 2LCT
	}
      
      //try to read next 2 data words from file:
      fscanf(myFile,"%s",DataWord);
      sscanf(DataWord,"%x",&dataWordInt_fr1);
      readWord= fscanf(myFile,"%s",DataWord);
      sscanf(DataWord,"%x",&dataWordInt_fr2);
      
      if (readWord<1){
	(*MyOutput_)<<"reached end of file... .."<<EVNT<<" eventsx2 frames loaded.."<<std::endl;
	dataWordInt_fr1=0;
	dataWordInt_fr2=0;
      }//end if	     
      
    }//end while we have valid words
  
  
  // Append 0's to the last one..
  addr=FIFO_A1a;
  for (int fifo_a=1; fifo_a<=9; fifo_a++) {
    for (int LCT=0; LCT<2; LCT++) {
      
      data[0]=0;
      data[1]=0;
      this->do_vme(2, addr, data, NULL, 1);
      data[0]=0;
      data[1]=0;
      this->do_vme(2, addr, data, NULL, 1);
      addr+=0x00002;
    }
  }
  
  this->read_status();
  return;
}




void MPC::setTLK2501TxMode(int mode){
  /// set the TLK2501 serializer Tx mode
  if (mode==1)
    (*MyOutput_) << "MPC: serializer in FRAMED mode" << std::endl;
  else if (mode==0)
    (*MyOutput_) << "MPC: serializer in CONTINUOUS mode" << std::endl;
  else
    std::cerr << "MPC: -WARNING- serializer in UNKNOWN mode ("<<mode<<")"<< std::endl;

  //Note: CSR2 has only bit-0 as a relevant bit. As soon as there are more
  //      bits this should be specific to that bit using a mask.

  char addr =  CSR2; //addr=0x000ac;
  int btd;
  int xfer_done[2];
  char data[2];
  //fg was this really the right way ... or, maybe, 
  //fg it should have been the other way around -- oh-oh.
  //fg anyways, nobody but Lev uses it ...
  //fg data[0]=mode;
  //fg data[1]=0x00;
  data[0]=0x00;
  data[1]=mode;
  do_vme(2, addr, data, NULL, 1);
}


void MPC::WriteRegister(int reg, int value){
  //
  sndbuf[0] = (value>>8)&0xff;
  sndbuf[1] = value&0xff;
  //
  do_vme(VME_WRITE,reg,sndbuf,rcvbuf,NOW);
  //
}

int MPC::ReadRegister(int reg){
  //
  int btd;
  char data[2];
  unsigned long int addr;
  //
  // make sure we are in framed mode
  do_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  printf(" MPC.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}
//
void MPC::firmwareVersion(){
  /// report the firmware version
  int btd, xfer_done[2];
  char data[2];
  do_vme(1,CSR1,NULL, data, 1);
  
  int versionWord = (data[0]<<8) + (data[1]&0xFF);
  std::cout << std::hex << versionWord << std::endl;
  int day   =  versionWord & 0x1F;
  int month = (versionWord >> 5   ) & 0xF;
  int year  = (versionWord >>(5+4)) + 2000;
  (*MyOutput_) << "MPC: firmware version: " << std::dec
       << day << "-" << month << "-" << year << std::endl;
}


void MPC::setSorterMode(){
  /// Switches the MPC to Sorter Mode while keeping the original sources intact.
  (*MyOutput_) << "MPC: switching to Sorter Mode" << std::endl;
  char addr =  CSR4;
  int btd;
  int xfer_done[2];
  char data[2];
  do_vme(1,addr, NULL,data,1);  
  //fg data[0]=data[0]&0xfe;
  //fg data[1]=data[1];
  //fg andersom, slimpy.
  data[0]=data[0];
  data[1]=data[1]&0xfe;
  do_vme(2, addr, data, NULL, 1);
}


void MPC::setTransparentMode(unsigned int pattern){
  /// Sets the Transparent Mode according to the source pattern.

  //fg At this point the routine expects the full CSR4 pattern.
  //fg This should change to the source pattern only followed by a
  //fg bit shift adding the bit-1=1 ... Should change once we feel
  //fg comfortable enough about it.

  (*MyOutput_) << "MPC: switching to Transparent Mode. Source pattern = 0x" 
       << std::hex << pattern << std::dec << std::endl;
  int btd, xfer_done[2];
  char data[2];
  char addr=CSR4;

  do_vme(1, addr, NULL, data, 1);

  // make sure that the last bit is actually 1, otherwise there is no transparentMode
  if ( !(pattern & 0x01)){
    (*MyOutput_) << "MPC: WARNING - last bit in source pattern (" << (unsigned short)pattern 
	 << ") assumes Sorter Mode. Using Transparent instead" << std::endl;
    pattern |=0x01;
  }

  // upload the pattern
  //fg data[0]=pattern;
  //fg data[1]=0;
  //fg andersom, slimpy
  data[1]=pattern>>8;   // MSB
  data[0]=pattern&0xff; // LSB
  do_vme(2, addr, data, NULL, 1);
}


void MPC::setTransparentMode(){
  /// Switches to Transparent Mode using whatever orginal source pattern
  /// was previously loaded.
  (*MyOutput_) << "MPC: switching to Transparent Mode. No new source pattern loaded" << std::endl;
  int btd;
  int xfer_done[2];
  char data[2];
  char addr = CSR4; 
  do_vme(1, addr, NULL, data, 1);

  //fg data[0] |= 0x01;
  //fg data[1] = 0;
  data[0] =0;
  data[1] |= 0x01;
  do_vme(2, addr, data, NULL, 1);
}


void MPC::setDelayFromTMB(unsigned char delays){
  /// Add single BX delays to each of the TMBs based on the delayPattern
   (*MyOutput_) << "MPC: setting TMB-MPC delays. Delay pattern = 0x"
	<< std::hex << (unsigned short)delays << std::dec << std::endl;
  int btd;
  int xfer_done[2];
  char data[2];
  char addr = CSR5;
  //fg data[0] = delays;
  //fg data[1] = 0;
  data[0] = 0;
  data[1] = delays;
  do_vme(2, addr, data, NULL, 1);
}


void MPC::interconnectTest(){
  char data[2];
  int btd, xfer_done[2];

  // reset FPGA logic
  char addr = CSR0;
  data[0] = 0x00;
  data[1] = 0x12;
  //data[0] = 0x12;
  //data[1] = 0x00;
  do_vme(2, addr, data, NULL, 1);
  do_vme(1, addr, NULL, data, 1);
  (*MyOutput_) << "MPC: interconnectTest  0x" << std::hex << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;

  // release reset, put TLK2501 transmitters in the test mode
  addr =  CSR0;
  data[0] = 0xc2;
  data[1] = 0x10;
  //data[0] = 0x10;
  //data[1] = 0xc2;
  do_vme(2, addr, data, NULL, 1);
  do_vme(1, addr, NULL, data, 1);
  (*MyOutput_) << "MPC: interconnectTest  0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
}

void MPC::start() {
#ifdef debugV
  (*MyOutput_) << "starting to talk to TMB, device " << ucla_ldev << std::endl;
#endif
   // send the first signal
  theController->SetupJtagBaseAddress(0x0);
  VMEModule::start();
  theController->initDevice(1, 0);
  theController->goToScanLevel();
}


int MPC::SVFLoad(int *jch, const char *fn, int db )
{
  int MAXBUFSIZE=8200;
  unsigned char snd[MAXBUFSIZE], rcv[MAXBUFSIZE], expect[MAXBUFSIZE],rmask[MAXBUFSIZE],smask[MAXBUFSIZE],cmpbuf[MAXBUFSIZE];
  unsigned char sndbuf[MAXBUFSIZE],rcvbuf[MAXBUFSIZE], realsnd[MAXBUFSIZE];
  unsigned char sndhdr[MAXBUFSIZE],sndtdr[MAXBUFSIZE], sndhir[MAXBUFSIZE], sndtir[MAXBUFSIZE];
  unsigned char hdrsmask[MAXBUFSIZE],tdrsmask[MAXBUFSIZE], hirsmask[MAXBUFSIZE], tirsmask[MAXBUFSIZE];
  int rcvword;
  FILE *dwnfp, *ftmptdi, *ftmpsmask;
  char buf[MAXBUFSIZE], buf2[256];
  //  char buf[8192],buf2[256];
  char *Word[256],*lastn;
  const char *downfile;
  char fStop;
  int jchan;
  unsigned char sndvalue;
  fpos_t ftdi_pos, fsmask_pos;
  unsigned char send_tmp;//, rcv_tmp;
  int i,j,Count,nbytes,nbits,nframes,step_mode,pause;
  int hdrbits = 0, tdrbits = 0, hirbits = 0, tirbits = 0;
  int hdrbytes = 0, tdrbytes = 0, hirbytes = 0, tirbytes = 0; 
  int nowrit, cmpflag, errcntr;
  static int count;
  // MvdM struct JTAG_BBitStruct   driver_data;
  // int jtag_chain[4] = {1, 0, 5, 4};
  int jtag_chain_tmb[6] = {7, 6, 9, 8, 3, 1};
  // === SIR Go through SelectDRScan->SelectIRScan->CaptureIR->ShiftIR  
  //char tms_pre_sir[4]={ 1, 1, 0, 0 }; 
  char tdi_pre_sir[4]={ 0, 0, 0, 0 };
  // === SDR Go through SelectDRScan->CaptureDR->ShiftDR
  // char tms_pre_sdr[3]={ 1, 0, 0 };
  char tdi_pre_sdr[3]={ 0, 0, 0 };
  // === SDR,SIR Go to RunTestIdle after scan
  // char tms_post[4]={ 0, 1, 1, 0 };
  char tdi_post[4]={ 0, 0, 0, 0 };
  int total_packages ;
  int send_packages ;
  
  total_packages = 0 ;
  send_packages = 0 ;
  jchan = *jch;
  downfile = fn;
  errcntr = 0;
  if (downfile==NULL)    downfile="default.svf";
  
  dwnfp    = fopen(downfile,"r");
  while (fgets(buf,256,dwnfp) != NULL) 
    {
      memcpy(buf,buf,256);
      Parse(buf, &Count, &(Word[0]));
      if( strcmp(Word[0],"SDR")==0 ) total_packages++ ;
    }
  fclose(dwnfp) ;
  
  printf("=== Programming Design with %s through JTAG chain %d\n",downfile, jchan);  
  printf("=== Have to send %d DATA packages \n",total_packages) ;
  dwnfp    = fopen(downfile,"r");
  
  if (dwnfp == NULL)
    {
      perror(downfile);
      
      return -1;
    }
  
  this->start(); 
  
  count=0; 
  nowrit=1;
  step_mode=0;
  while (fgets(buf,256,dwnfp) != NULL)  
    {
      if((buf[0]=='/'&&buf[1]=='/')||buf[0]=='!')
	{
	  if (db>4)          printf("%s",buf);
	}
      else 
	{
	  if(strrchr(buf,';')==0)
	    {
	      lastn=strrchr(buf,'\r');
	      if(lastn!=0)lastn[0]='\0';
	      lastn=strrchr(buf,'\n');
	      if(lastn!=0)lastn[0]='\0';
	      memcpy(buf2,buf,256);
	      Parse(buf2, &Count, &(Word[0]));
	      if(( strcmp(Word[0],"SDR")==0) || (strcmp(Word[0],"SIR")==0))
		{
		  sscanf(Word[1],"%d",&nbits);
		  if (nbits>MAXBUFSIZE) // === Handle Big Bitstreams
		    {
		      (*MyOutput_) << "EMUjtag. nbits larger than buffer size" << std::endl;
		    }
		  else do  // == Handle Normal Bitstreams
		    {
		      lastn=strrchr(buf,'\r');
		      if(lastn!=0)lastn[0]='\0';
		      lastn=strrchr(buf,'\n');
		      if(lastn!=0)lastn[0]='\0';
		      if (fgets(buf2,256,dwnfp) != NULL)
			{
			  strcat(buf,buf2);
			}
		      else 
			{
			  if (db)              printf("End of File encountered.  Quiting\n");
			  return -1;
			}
		    }
		  while (strrchr(buf,';')==0);
		}
	    } 
	  bzero(snd, sizeof(snd));
	  bzero(cmpbuf, sizeof(cmpbuf));
	  bzero(sndbuf, sizeof(sndbuf));
	  bzero(rcvbuf, sizeof(rcvbuf));
	  
	  /*
	    for(i=0;i<MAXBUFSIZE;i++)
	    {
	    snd[i]=0;
	    cmpbuf[i]=0;
	    sndbuf[i]=0;
	    rcvbuf[i]=0;
	    }
	  */
	  Parse(buf, &Count, &(Word[0]));
	  count=count+1;
	  cmpflag=0;
	  // ==================  Parsing commands from SVF file ====================
	  // === Handling HDR ===
	  if(strcmp(Word[0],"HDR")==0)
	    {
	      sscanf(Word[1],"%d",&hdrbits);
	      hdrbytes=(hdrbits)?(hdrbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Header Data\n", hdrbits);
	    // if (db>3)          printf("HDR: Num of bits - %d, num of bytes - %d\n",hdrbits,hdrbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&sndhdr[j]);
			// printf("%2X",sndhdr[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&hdrsmask[j]);
		      }
		  }
     	 	if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<hdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    }
	  
	  // === Handling HIR ===
	  else if(strcmp(Word[0],"HIR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&hirbits);
	      hirbytes=(hirbits)?(hirbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Header Data\n", hirbits);
	      // if (db>3)          printf("HIR: Num of bits - %d, num of bytes - %d\n",hirbits,hirbytes);
	      for(i=2;i<Count;i+=2)
		{
		  if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<hirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&sndhir[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
		  if(strcmp(Word[i],"SMASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
		      {
      		  	sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&hirsmask[j]);
		      }
		    }
		  if(strcmp(Word[i],"TDO")==0)
		    {
		      //if (db>2)             cmpflag=1;
		      cmpflag=1;
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&expect[j]);
			}
		    }
		  if(strcmp(Word[i],"MASK")==0)
		    {
		      for(j=0;j<hirbytes;j++)
			{
			  sscanf(&Word[i+1][2*(hirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
			}
		    }
		}
	    }	
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TDR")==0)
	    {
	      // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	      sscanf(Word[1],"%d",&tdrbits);
	      tdrbytes=(tdrbits)?(tdrbits-1)/8+1:0;
	      if (db)	  
		printf("Sending %d bits of Tailer Data\n", tdrbits);
	      // if (db>3)          printf("TDR: Num of bits - %d, num of bytes - %d\n",tdrbits,tdrbytes);
	      for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&sndtdr[j]);
			// printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&tdrsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tdrbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tdrbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }	
		  }
	      }
	    }
	  
	  // === Handling TDR ===
	  else if(strcmp(Word[0],"TIR")==0)
	  {
	    // for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&tirbits);
	    tirbytes=(tirbits)?(tirbits-1)/8+1:0;
	    if (db)	  
	      printf("Sending %d bits of Tailer Data\n", tdrbits);
	    // if (db>3)          printf("TIR: Num of bits - %d, num of bytes - %d\n",tirbits,tirbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&sndtir[j]);
			    // printf("%2X",sndhir[j]);
		      }
		    // printf("\n%d\n",nbytes);
    		  }
      		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
				  sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&tirsmask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
		    //if (db>2)             cmpflag=1;
		    cmpflag=1;
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&expect[j]);
		      }
		  }
      		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<tirbytes;j++)
		      {
			sscanf(&Word[i+1][2*(tirbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	  }
	  // === Handling SDR ===
	  else if(strcmp(Word[0],"SDR")==0)
	    {
	      //std::cout << "SDR" << std::endl;
	      for(i=0;i<3;i++)sndbuf[i]=tdi_pre_sdr[i];
	      // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits Data\n", nbits);
	    // if (db>3)          printf("SDR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
	      if(strcmp(Word[i],"TDI")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      //                printf("%2X",snd[j]);
		    }
		  //                printf("\n%d\n",nbytes);
		}
	      if(strcmp(Word[i],"SMASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		    }
		}
	      if(strcmp(Word[i],"TDO")==0)
		{
		  //if (db>2)             cmpflag=1;
		  cmpflag=1;
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
		    }
		}
	      if(strcmp(Word[i],"MASK")==0)
		{
		  for(j=0;j<nbytes;j++)
		    {
		      sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		    }
		}
	      }
	    for(i=0;i<nbytes;i++)
	      {
	      send_tmp = snd[i]&smask[i];
	      for(j=0;j<8;j++)
		{
		  if ((i*8+j)< nbits) 
		    { 
		      sndbuf[i*8+j+3]=send_tmp&0x01; 
		    }
		  send_tmp = send_tmp >> 1;
		}
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+3]=tdi_post[i];         
	    nframes=nbits+7;
          // Put send SDR here
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	      realsnd[i] = 0;
	    if (hdrbytes>0) {
	      for (i=0;i<hdrbytes;i++)
		realsnd[i]=sndhdr[i];
	    }
	    for (i=0;i<nbits;i++)
	    realsnd[(i+hdrbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hdrbits)%8);
	    if (tdrbytes>0) {
	      for (i=0;i<tdrbits;i++)
		realsnd[(i+hdrbits+nbits)/8] |= (sndtdr[i/8] >> (i%8)) << ((i+hdrbits+nbits)%8);
	    }
	    
	    if (db>6) {	printf("SDR Send Data:\n");
	    for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++)
	    printf("%02X",realsnd[i]);
	    printf("\n");
	    }
	    send_packages++ ;
	    printf("%c[0m", '\033');
	    printf("%c[1m", '\033');
	    //printf("%c[2;50H", '\033');
	    if ( send_packages == 1 )   {
	      printf("%c7", '\033');
	    }
	  printf("%c8", '\033'); 
	  printf(" Sending %d/%d ",send_packages,total_packages) ;
	  printf("%c8", '\033'); 
	  printf("%c[0m", '\033');
	  if ( send_packages == total_packages ) printf("\n") ;
	  //
	  this->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, 2); 
	  this->scan(DATA_REG, (char*)realsnd, hdrbits+nbits+tdrbits, (char*)rcv, 2); 
	  //
	  if (cmpflag==1)
	    {     
	      /*
		for(i=0;i<nbytes;i++)
		{
		rcv_tmp = 0;
		for(j=0;j<8;j++)
		{
		if ((i*8+j) < nbits)
		{
		rcv_tmp |= ((rcvbuf[i*8+j+3]<<7) &0x80); 
		}
		rcv_tmp = rcv_tmp >> (j<7);
		
		}	
		rcv[nbytes-1-i] = rcv_tmp;		
		}
	      */
	      if(db>6){	
		printf("SDR Sent Data:\n");
		for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) 
		  printf("%02X",realsnd[i]);
		printf("\n");
		//
		printf("SDR Readback Data:\n");
		for (i=0; i< ((hdrbits+nbits+tdrbits)); i++) 
		  printf("%02X",rcv[i]);
		printf("\n");
	      }	
	      
	      for(i=0;i<nbytes;i++)
		{
		  rcvword = rcv[i+(hdrbits/8)]+(((int)rcv[i+1+(hdrbits/8)])<<8);
		  rcvword = rcvword>>(hdrbits%8);
		  rcvword = rcv[i];
		  // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		  if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    {
		      //printf("1.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
		      errcntr++;
		    }
		}	
	    }
	  /*         if (cmpflag==1)
		     {
		     for(i=3;i<nbits+3;i++) printf("%d",rcvbuf[i]);
		     printf("\n");
		     }
	  */       
          }
        // === Handling SIR ===
        else if(strcmp(Word[0],"SIR")==0)
          {
	    for(i=0;i<4;i++)sndbuf[i]=tdi_pre_sir[i];
	    // cmpflag=1;    //disable the comparison for no TDO SDR
	    sscanf(Word[1],"%d",&nbits);
	    nbytes=(nbits)?(nbits-1)/8+1:0;
	    if (db)	  printf("Sending %d bits of Command\n",nbits);
	    // if (db>3)          printf("SIR: Num of bits - %d, num of bytes - %d\n",nbits,nbytes);
	    for(i=2;i<Count;i+=2)
	      {
		if(strcmp(Word[i],"TDI")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&snd[j]);
		      }
		  }
		if(strcmp(Word[i],"SMASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&smask[j]);
		      }
		  }
		if(strcmp(Word[i],"TDO")==0)
		  {
			cmpflag=1;
			// if (db>2)              cmpflag=1;
			for(j=0;j<nbytes;j++)
			  {
			    sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&expect[j]);
			  }
		  }
		if(strcmp(Word[i],"MASK")==0)
		  {
		    for(j=0;j<nbytes;j++)
		      {
			sscanf(&Word[i+1][2*(nbytes-j-1)+1],"%2X",(int *)&rmask[j]);
		      }
		  }
	      }
	    for(i=0;i<nbytes;i++)
	      {
		send_tmp = snd[i]&smask[i];
		// printf("\n%d - ", send_tmp);
		for(j=0;j<8;j++)
		  {
		    if ((i*8+j)< nbits) 
		      {
			sndbuf[i*8+j+4]=send_tmp&0x01;
			// printf("%d", sndbuf[i*8+j+4]);
		      }
		    send_tmp = send_tmp >> 1;
		  }
	      }
	    for(i=0;i<4;i++)sndbuf[nbits+4]=tdi_post[i];
	    nframes=nbits+8;
	    // Put send SIR here
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      realsnd[i] = 0;
	    if (hirbytes>0) {
	      for (i=0;i<hirbytes;i++)
		realsnd[i]=sndhir[i];
	    }
	    for (i=0;i<nbits;i++)
	      realsnd[(i+hirbits)/8] |= (snd[i/8] >> (i%8)) << ((i+hirbits)%8);
	    if (tirbytes>0) {
	      for (i=0;i<tirbits;i++)
		realsnd[(i+hirbits+nbits)/8] |= (sndtir[i/8] >> (i%8)) << ((i+hirbits+nbits)%8);
	    }
	    //
	    this->scan(INSTR_REG, (char*)realsnd, hirbits+nbits+tirbits, (char*)rcv, 2); 
	    //	   
	    if(db>6){ 	printf("SIR Send Data:\n");
	    for (i=0; i< ((hirbits+nbits+tirbits-1)/8+1);  i++)
	      printf("%02X",realsnd[i]);
	    printf("\n");
	    for (i=0; i< ((hirbits+nbits+tirbits));  i++)
	      printf("%02X ",rcv[i]);
	    printf("\n");
	    }
	    //
	    if (cmpflag==1)
	      {
		/*               for(i=0;i<nbytes;i++)
				 {
				 rcv_tmp = 0;
				 for(j=0;j<8;j++)
				 {
				 if ((i*8+j) < nbits)
				 {
				 rcv_tmp |= ((rcvbuf[i*8+j+4]<<7) &0x80);
				 }
				 rcv_tmp = rcv_tmp >> (j<7);
				 
				 }
				 rcv[nbytes-1-i] = rcv_tmp;
				 }
		*/
		if (db>4){                printf("SIR Readback Data:\n");
                //for(i=0;i<nbytes;i++) printf("%02X",rcv[i]);
		for (i=0; i< ((hdrbits+nbits+tdrbits-1)/8+1); i++) printf("%02X",rcv[i]);
                printf("\n");
		}
		
                for(i=0;i<nbytes;i++)
		  {
		    rcvword = rcv[i+(hirbits/8)<<3]+(((int)rcv[i+1+(hirbits/8)])<<2)+(((int)rcv[i+2+(hirbits/8)])<<1)+(((int)rcv[i+3+(hirbits/8)]));
		    printf("%02x %02x\n",rcv[i+(hirbits/8)],(((int)rcv[i+1+(hirbits/8)])<<8));
		    printf("hirbits=%d %02x\n",hirbits,rcvword);
		    //rcvword = rcv[i];
		    // if (((rcv[nbytes-1-i]^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		    if ((((rcvword&0xFF)^expect[i]) & (rmask[i]))!=0 && cmpflag==1)
		      {
			printf("2.read back wrong, at i %02d  rdbk %02X  expect %02X  rmask %02X\n",i,rcv[i]&0xFF,expect[i]&0xFF,rmask[i]&0xFF);
                	errcntr++;
		      }
		  }
	      }
	    /*
	      if (cmpflag==1)
	      {
	      for(i=4;i<nbits+4;i++) printf("%d",rcvbuf[i]);
	      printf("\n");
	      }
	    */   
          }
	  // === Handling RUNTEST ===
	  else if(strcmp(Word[0],"RUNTEST")==0)
	    {
	      sscanf(Word[1],"%d",&pause);
	      //printf("RUNTEST:  %d\n",pause);
	      usleep(pause+100);
	      // InsertDelayJTAG(pause,MYMICROSECONDS);
	    }
	  // === Handling STATE ===
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET")==0))
	    {
	      if(strcmp(Word[2],"IDLE;")==0)
		{
		  cout << "STATE: goto reset idle state" << std::endl;
		  //RestoreIdle();
		} 
	    }
	  else if((strcmp(Word[0],"STATE")==0)&&(strcmp(Word[1],"RESET;")==0))
	    {
	      cout << "STATE: goto reset state" << std::endl;
	      //RestoreReset();
	    }
	  else if(strcmp(Word[0],"TRST")==0)
	    {
	      //          printf("TRST\n");
	    }
	  // === Handling ENDIR ===
	  else if(strcmp(Word[0],"ENDIR")==0)
	    {
	      //          printf("ENDIR\n");
	    }
	  // === Handling ENDDR ===
	  else if(strcmp(Word[0],"ENDDR")==0)
	    {
	    //	   printf("ENDDR\n");
	    }
	}
    }
  this->endDevice();
  fclose(dwnfp);
  return errcntr; 
}


// ====
// SVF File Parser module
// ====
void MPC::Parse(char *buf,int *Count,char **Word)
{

  //std::cout << buf << std::endl;

  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  {
    while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
    if ((*buf != '\n') && (*buf != '\0'))  {
      Word[(*Count)++] = buf;

    }
    while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) {
      buf++;
    }
  }
  buf = '\0';
}


