
#ifndef OSUcc

//-----------------------------------------------------------------------
// $Id: MPC.cc,v 2.17 2006/01/14 22:24:50 mey Exp $
// $Log: MPC.cc,v $
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
#include "MPC.h"
#include "VMEController.h"

MPC::MPC(int newCrate, int slot) : VMEModule(newCrate, slot),
  TLK2501TxMode_(0), TransparentModeSources_(0), TMBDelayPattern_(0){
  std::cout << "MPC: module created in crate=" << this->crate() 
       << " slot=" << this->slot() << std::endl;
}


MPC::~MPC(){
  std::cout << "MPC: module removed from crate=" << this->crate() 
       << " slot=" << this->slot() << std::endl;
}


void MPC::start() {
  char tdata[2];
  int n=2;
  char c=0;
  int lev=0x01;
  c=c|lev;
  // next two bits are board type
  c=c|(boardType()<<2);
  tdata[0]=c;
  if(theMode == READ) {
    tdata[1]=0x01;
  } else {
    tdata[1]=0x00;
  }
  int nt1;
     //printf(" first send \n");
  if( (nt1=theController->writen(tdata, n))  != n)
    std::cout << "MPC: str_cli:  writen error on socket" << std::endl;
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
  do_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  printf(" MPC.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}

void MPC::configure() {
}

bool MPC::SelfTest() {
  return 0;
}


void MPC::init() {
  //
  int btd;
  int xfer_done[2];
  char data[2];
  unsigned long int addr;

  std::cout << "MPC: initialize" << std::endl;

  std::cout << "MPC: turn off Resets" <<std::endl;
  addr = theBaseAddress + CSR0;
  data[0]=0x10;
  data[1]=0x4a;
  write(btd,data,addr,2,xfer_done);

  std::cout << "MPC: logic reset" << std::endl;
  data[0]=0x12;
  data[1]=0x4a;
  write(btd,data,addr,2,xfer_done);

  std::cout << "MPC: end logic Reset" << std::endl;
  data[0]=0x10;
  data[1]=0x4a;
  write(btd,data,addr,2,xfer_done);

  //read_csr0();

  std::cout << "MPC: set default serializer TX mode ..." << std::endl;
  setTLK2501TxMode(TLK2501TxMode_);

  // Sorter Mode is the default power-up mode of the MPC.
  std::cout << "MPC: set default MPC operation Mode ..." << std::endl;
  if (TransparentModeSources_)
    setTransparentMode(TransparentModeSources_);
  else
    setSorterMode();

  // The default power-up delays are always 0.
  std::cout << "MPC: setting default TMB-MPC delays ..." << std::endl;
  setDelayFromTMB(TMBDelayPattern_);

  // report firmware version
  firmwareVersion();
}

void MPC::read_fifo(unsigned long int address, char * data) {
  int xfer_done[2];
  int btd; // comes from d360_bt_open(btd_p,devname_p,0);
  //data[0] = 0x00;
  //data[1] = 0x00;
  read(btd, data, theBaseAddress + address,2, xfer_done); 
}

void MPC::read_fifosA() {
  //
  char data[100];
  //
  std::cout.fill('0');
  //
  read_fifo(FIFO_A1a, data);  
  std::cout << "MPC: FIFO-A1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A1b, data);
  std::cout << "MPC: FIFO-A1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2a, data);
  std::cout << "MPC: FIFO-A2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2b, data);
  std::cout << "MPC: FIFO-A2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3a, data);
  std::cout << "MPC: FIFO-A3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3b, data);
  std::cout << "MPC: FIFO-A3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4a, data);
  std::cout << "MPC: FIFO-A4a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4b, data);
  std::cout << "MPC: FIFO-A4b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5a, data);
  std::cout << "MPC: FIFO-A5a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5b, data);
  std::cout << "MPC: FIFO-A5b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6a, data);
  std::cout << "MPC: FIFO-A6a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6b, data);
  std::cout << "MPC: FIFO-A6b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7a, data);
  std::cout << "MPC: FIFO-A7a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7b, data);
  std::cout << "MPC: FIFO-A7b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8a, data);
  std::cout << "MPC: FIFO-A8a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8b, data);
  std::cout << "MPC: FIFO-A8b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9a, data);
  std::cout << "MPC: FIFO-A9a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9b, data);
  std::cout << "MPC: FIFO-A9b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
}

void MPC::read_fifos() {
  //
  // Read FIFO-B until empty
  //
  std::cout << "MPC:  Read FIFO-B" << std::endl;
  char data[100];
  read_fifo(CSR1, data);
  printf(" %x %x \n",data[0],data[1]);
  read_fifo(CSR3, data); // Need this to actually read FIFO
  read_status();
  //std::cout.fill('0');
  //std::cout << "FIFO status " << std::hex << data << std::endl;
  //std::cout.fill(' ');
  //bool full_fifoa=(data[1]&0x0001);
  //bool empty_fifoa=(data[1]&0x0002)>>1;
  //bool full_fifob=(data[1]&0x0004)>>2;
  bool empty_fifob=(data[1]&0x0008)>>3;
  //
  unsigned long Lct0,Lct1, Lct2;
  Lct0=0;Lct1=0;Lct2=0;
  //
  //
  if(empty_fifob) {
    std::cout << "MPC: FIFO-B is empty!" << std::endl;
    return;
  } else {
    std::cout << "MPC: 1st Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << std::hex;
    std::cout.fill('0');
    std::cout << "MPC: FIFO-B1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = (Lct0<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    //
    if ( (((Lct0>>28)&0xf)==0xc) && ((Lct0>>12)&0xf)==0xd ) Lct0 = 0x0; // Kill TMB default data
    //
    FIFOBLct0.push_back(Lct0);
    std::cout << "MPC: FIFO-B1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)  << std::endl;
    std::cout << "MPC: LCT0     = 0x" << std::setw(8) << Lct0 << std::endl ;
    //
    std::cout << "MPC: 2nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = (Lct1<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    if ( (((Lct1>>28)&0xf)==0xc) && ((Lct1>>12)&0xf)==0xd ) Lct1 = 0x0; // Kill TMB default data
    std::cout << "MPC: FIFO-B2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    FIFOBLct1.push_back(Lct1);
    std::cout << "MPC: LCT1     = 0x" << std::setw(8) << Lct1 << std::endl ;
    //
    std::cout << "MPC: 3nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = (Lct2<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    if ( (((Lct2>>28)&0xf)==0xc) && ((Lct2>>12)&0xf)==0xd ) Lct2 = 0x0; // Kill TMB default data
    std::cout << "MPC: FIFO-B3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) <<(data[1]&0x00ff) << std::endl;
    FIFOBLct2.push_back(Lct2);
    std::cout << "MPC: LCT2     = 0x" << std::setw(8) << Lct2 << std::endl ;
    std::cout.fill(' ');
    std::cout << std::dec;    
    //
    read_fifos();
    //
  }
  //
}


void MPC::read_csr0() {
  char data[100];
  read_fifo(CSR0,data);
  std::cout.fill('0');
  std::cout << "MPC: data read from CSR0: 0x" << std::hex 
       << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)
       << std::dec << std::endl;
  std::cout.fill(' ');
}

void MPC::SoftReset() {
  //
  char data[2];
  int btd, xfer_done[2];
  //
  int addr = theBaseAddress + CSR0;
  //
  read(btd,data,addr,2,xfer_done);
  printf("%x %x \n",data[0],data[1]);
  //
  // reset FPGA logic
  //
  data[1] = (data[1]&0xfd) ;
  write(btd,data,addr,2,xfer_done);
  printf("%x %x \n",data[0],data[1]);
  //
  data[1] = (data[1]&0xfd) | 0x2 ;
  write(btd,data,addr,2,xfer_done);
  printf("%x %x \n",data[0],data[1]);
  //
  data[1] = (data[1]&0xfd) ;
  write(btd,data,addr,2,xfer_done);
  printf("%x %x \n",data[0],data[1]);
  //
}

void MPC::read_status() {
  //
  //Check FIFO Status:
  char data[100];
  //read_fifo(STATUS, data);
  read_fifo(CSR3, data);
  std::cout.fill('0');
  std::cout << "MPC: FIFO status = 0x" << std::hex << std::setw(2) << (data[0]&0x00ff)
       << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
  std::cout.fill(' ');
  bool full_fifoa=(data[1]&0x0001);
  bool empty_fifoa=(data[1]&0x0002)>>1;
  bool full_fifob=(data[1]&0x0004)>>2;
  bool empty_fifob=(data[1]&0x0008)>>3;

  if (full_fifoa>0)  std::cout << "MPC: FIFO_A is Full"  << std::endl;
  if (empty_fifoa>0) std::cout << "MPC: FIFO_A is Empty" << std::endl;
  if (full_fifob>0)  std::cout << "MPC: FIFO_B is Full"  << std::endl;
  if (empty_fifob>0) std::cout << "MPC: FIFO_B is Empty" << std::endl;;
}


//fg deprecated member ... replaced by MPC::firmWare(). 
//fg void MPC::read_date() {
//fg   char data[100];
//fg   read_fifo(DATE, data);
//fg   std::cout.fill('0');
//fg   std::cout << hex << "MPC: date code read = 0x" << setw(2) << (data[0]&0x00ff) 
//fg        << setw(2) << (data[1]&0x00ff) << dec << endl;
//fg   std::cout.fill(' ');
//fg   std::cout << "MPC: month="<< (((data[0]&0x0001)<<3)|((data[1]&0x00e0)>>5))
//fg        << " day=" << (data[1]&0x001f)
//fg        << " year=" << (2000+((data[0]&0x000e)>>1)) << endl;
//fg }


int MPC::read(int btd,char *buf_p,
    unsigned long int add,
    int xfer_len,
    int *xfer_done_p) {

  theMode = READ;
  theController->start(this);
  writeToAddress(add, buf_p, xfer_len);

  int ncnt=0;
  int nleft=xfer_len;
  while(nleft>0){
    int max=xfer_len;
    if(max>nleft)max=nleft;
    int n=theController->readline(&buf_p[ncnt],max);
//         printf(" ncnt %d %02x %02x \n",ncnt,buf_p[0]&0xff,buf_p[1]&0xff);
    ncnt+=n;
    nleft-=n;
  }
  theController->end();
  return ncnt;
}


int MPC::write(int btd,char *buf_p,
	       unsigned long int add,
	       int xfer_len,
	       int *xfer_done_p) {
  
  theMode = WRITE;
  theController->start(this);
  writeToAddress(add, buf_p, xfer_len);
  *xfer_done_p = xfer_len;
  theController->end();
  return xfer_len;
}


void MPC::writeToAddress(unsigned long int add,
     char * bufp, int xfer_len) {

  char * tdata = new char[xfer_len+8];

  int cnt = 0;
  tdata[0]=(add>>24)&0xff;
  tdata[1]=(add>>16)&0xff;
  tdata[2]=(add>>8)&0xff;
  tdata[3]=(add)&0xff;
  cnt+=4;
    //printf(" address filled:  add %08x  xfer_len %d \n",add,xfer_len);
  for(int i=0;i<xfer_len;i++){
    //printf(" cnt %d i %d \n",cnt,i);
    tdata[cnt]=bufp[i];
    ++cnt;
    //printf(" cnt %d \n");
  }
    //printf(" call writenn %d \n",cnt);
  theController->writenn(tdata,cnt);
  delete[] tdata;
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
  unsigned long int addr;

  // make sure we are in framed mode
  addr = theBaseAddress + CSR2;
  //fg read(btd,data,addr,2,xfer_done);
  //fg data[0]=data[0]|0x01;
  data[0]=0x01;
  data[1]=0x00;
  write(btd,data,addr,2,xfer_done);

  // brute force set to 0xC210, will change to masking soon, very soon ...
  addr = theBaseAddress + CSR0;
  data[0]=0x10;
  data[1]=0xC2;
  write(btd,data,addr,2,xfer_done);

  std::cout << "MPC: PRBS mode enabled" << std::endl;
}

void MPC::disablePRBS(){
  int btd, xfer_done[2];
  char data[2];
  unsigned long int addr;

  // brute force set back to 0x5B10, will change to masking soon, very soon ...
  addr = theBaseAddress + CSR0;
  data[0]=0x10;
  data[1]=0x5B;
  write(btd,data,addr,2,xfer_done);

  std::cout << "MPC: PRBS mode disabled" << std::endl;
}

void MPC::initTestLinks(){
// initialise FIFOs ready to load test data with injectSP functs
//nb! need to time in SP after this funct but before inject data

//initialise!!! toggle CSR0 bits.
 std::cout << "Initialising peripheral crate links for load FIFOs" << std::endl;

  int btd, xfer_done[2];
  char data[2];
  unsigned long int addr;


  addr = theBaseAddress + CSR0;
  data[0]=0x11;
  data[1]=0x4E;
  write(btd,data,addr,2,xfer_done);
  
  data[0]=0x13;
  data[1]=0x4E;
  write(btd,data,addr,2,xfer_done);
 
  data[0]=0x11;
  data[1]=0x4E;
  write(btd,data,addr,2,xfer_done);
//done init
}

void MPC::injectSP(){

  int btd, xfer_done[2];
  char data[2];
  unsigned long int addr;

  int ITR=255;

 this->read_status();
 
   std::cout << "..now filling FIFO-A" <<std::endl;
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
	     addr=theBaseAddress+FIFO_A1a;
	     for (int fifo_a=1; fifo_a<=9; fifo_a++) 
             {
	       for (int LCT=0; LCT<2; LCT++)  //dammit, no i think this should just be a loop over frames!!
//2 sorts of LCTs
	         {
		     data[0]= mframe1&0x00FF;
		     data[1]=(mframe1&0xFF00)>>8;
		     this->write(btd,data,addr,2,xfer_done);

//2 frames?:ie 2x16 bits =tot 32 bits..

		     data[0]= mframe2&0x00FF;
		     data[1]=(mframe2&0xFF00)>>8;
		     this->write(btd,data,addr,2,xfer_done);

		     addr+=0x00002;

	         }
	     } 
	   }
        

	// Append 0's to the last one..
	addr=theBaseAddress+FIFO_A1a;
	for (int fifo_a=1; fifo_a<=9; fifo_a++) {
	    for (int LCT=0; LCT<2; LCT++) {

		     data[0]=0;
		     data[1]=0;
		     this->write(btd,data,addr,2,xfer_done);
		     data[0]=0;
		     data[1]=0;
		     this->write(btd,data,addr,2,xfer_done);
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
  unsigned long int addr;
  int readWord;//number of words read by fscanf
  
  std::cout<<"...inject test pattern funct"<<std::endl;
  std::cout<<"data file passed is: "<<injectDataFileName<<std::endl;
  //let's try to open the file:: 
  
  
  
  std::cout<<"opening file..."<<std::endl;
  FILE* myFile = fopen(injectDataFileName , "r" );
  if (myFile==NULL){
    std::cout<<"problem opening data file, exiting.."<<std::endl;
    exit(0);
  }
  
  
  
  this->read_status();
  
  std::cout<<" will use LCT data from file "<<injectDataFileName<<std::endl;
  
  std::cout << "..now filling FIFO-A, with your data, alternating frame 1, 2.." <<std::endl;
  //   int ITR=2;mframe2 = ITR;
  


  // read the first data word and turn it into and integer:
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr1);
  std::cout<<"first frame 1 :: (first word in file) is "<<std::hex<<dataWordInt_fr1<<std::endl;
  if (readWord<1){
    std::cout<<"problem reading first word in file ..exiting..."<<std::endl;
    exit(0);
  }
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr2);
  std::cout<<"first frame 2 :: (second word in file) is "<<std::hex<<dataWordInt_fr2<<std::endl;    

  int EVNT=0;//event counter
  while (dataWordInt_fr1 !=0 && dataWordInt_fr2 !=0)//keep on looping while we have valid words
    {
      
      EVNT++;
      //             mframe1 = (0x1 << 15) | (0xF << 11) | dataWordInt_fr1;
      mframe1 =  dataWordInt_fr1;
      mframe2 =  dataWordInt_fr2;
      
      // Load 2 LCTs to each of 8/9!! buffers in FIFO-A corresponding to 9TMBs
      addr=theBaseAddress+FIFO_A1a;
      for (int fifo_a=1; fifo_a<=9; fifo_a++) //hmm why do we stop at 8??  ..9TMBs and FIFO-A9..?
	{
	  	  
	  for (int LCT=0; LCT<2; LCT++)
	    //2 sorts of LCTs
	    {
	      data[0]= mframe1&0x00FF;
	      data[1]=(mframe1&0xFF00)>>8;
	      this->write(btd,data,addr,2,xfer_done);
	      
	      //std::cout<<"event: "<<EVNT<<"...filling fifoA with "<<std::hex<<mframe1<<std::endl;
	      
	      //2 frames?:ie 2x16 bits =tot 32 bits..
	      
	      data[0]= mframe2&0x00FF;
	      data[1]=(mframe2&0xFF00)>>8;
	      this->write(btd,data,addr,2,xfer_done);
	      
	      addr+=0x00002;
	      
	    }//end for loop, 2LCT
	}
      
      //try to read next 2 data words from file:
      fscanf(myFile,"%s",DataWord);
      sscanf(DataWord,"%x",&dataWordInt_fr1);
      readWord= fscanf(myFile,"%s",DataWord);
      sscanf(DataWord,"%x",&dataWordInt_fr2);
      
      if (readWord<1){
	std::cout<<"reached end of file... .."<<EVNT<<" eventsx2 frames loaded.."<<std::endl;
	dataWordInt_fr1=0;
	dataWordInt_fr2=0;
      }//end if	     
      
    }//end while we have valid words
  
  
  // Append 0's to the last one..
  addr=theBaseAddress+FIFO_A1a;
  for (int fifo_a=1; fifo_a<=9; fifo_a++) {
    for (int LCT=0; LCT<2; LCT++) {
      
      data[0]=0;
      data[1]=0;
      this->write(btd,data,addr,2,xfer_done);
      data[0]=0;
      data[1]=0;
      this->write(btd,data,addr,2,xfer_done);
      addr+=0x00002;
    }
  }
  
  this->read_status();
  return;
}




void MPC::setTLK2501TxMode(int mode){
  /// set the TLK2501 serializer Tx mode
  if (mode==1)
    std::cout << "MPC: serializer in FRAMED mode" << std::endl;
  else if (mode==0)
    std::cout << "MPC: serializer in CONTINUOUS mode" << std::endl;
  else
    std::cerr << "MPC: -WARNING- serializer in UNKNOWN mode ("<<mode<<")"<< std::endl;

  //Note: CSR2 has only bit-0 as a relevant bit. As soon as there are more
  //      bits this should be specific to that bit using a mask.

  int addr = theBaseAddress + CSR2; //addr=0x000ac;
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
  write(btd,data,addr,2,xfer_done);
}


void MPC::firmwareVersion(){
  /// report the firmware version
  int btd, xfer_done[2];
  char data[2];
  read(btd, data, theBaseAddress + CSR1,2, xfer_done);
  
  int versionWord = (data[0]<<8) + (data[1]&0xFF);
  int day   =  versionWord & 0x1F;
  int month = (versionWord >> 5   ) & 0xF;
  int year  = (versionWord >>(5+4)) + 2000;
  std::cout << "MPC: firmware version: " << std::dec
       << day << "-" << month << "-" << year << std::endl;
}


void MPC::setSorterMode(){
  /// Switches the MPC to Sorter Mode while keeping the original sources intact.
  std::cout << "MPC: switching to Sorter Mode" << std::endl;
  int addr = theBaseAddress + CSR4;
  int btd;
  int xfer_done[2];
  char data[2];
  read(btd,data,addr,2,xfer_done);  
  //fg data[0]=data[0]&0xfe;
  //fg data[1]=data[1];
  //fg andersom, slimpy.
  data[0]=data[0];
  data[1]=data[1]&0xfe;
  write(btd,data,addr,2,xfer_done);
}


void MPC::setTransparentMode(unsigned int pattern){
  /// Sets the Transparent Mode according to the source pattern.

  //fg At this point the routine expects the full CSR4 pattern.
  //fg This should change to the source pattern only followed by a
  //fg bit shift adding the bit-1=1 ... Should change once we feel
  //fg comfortable enough about it.

  std::cout << "MPC: switching to Transparent Mode. Source pattern = 0x" 
       << std::hex << pattern << std::dec << std::endl;
  int btd, xfer_done[2];
  char data[2];
  int addr(theBaseAddress + CSR4);

  read(btd,data,addr,2,xfer_done);  

  // make sure that the last bit is actually 1, otherwise there is no transparentMode
  if ( !(pattern & 0x01)){
    std::cout << "MPC: WARNING - last bit in source pattern (" << (unsigned short)pattern 
	 << ") assumes Sorter Mode. Using Transparent instead" << std::endl;
    pattern |=0x01;
  }

  // upload the pattern
  //fg data[0]=pattern;
  //fg data[1]=0;
  //fg andersom, slimpy
  data[1]=pattern>>8;   // MSB
  data[0]=pattern&0xff; // LSB
  write(btd,data,addr,2,xfer_done);
}


void MPC::setTransparentMode(){
  /// Switches to Transparent Mode using whatever orginal source pattern
  /// was previously loaded.
  std::cout << "MPC: switching to Transparent Mode. No new source pattern loaded" << std::endl;
  int btd;
  int xfer_done[2];
  char data[2];
  int addr = theBaseAddress + CSR4; 
  read(btd,data,addr,2,xfer_done);
  //fg data[0] |= 0x01;
  //fg data[1] = 0;
  data[0] =0;
  data[1] |= 0x01;
  write(btd,data,addr,2,xfer_done);
}


void MPC::setDelayFromTMB(unsigned char delays){
  /// Add single BX delays to each of the TMBs based on the delayPattern
   std::cout << "MPC: setting TMB-MPC delays. Delay pattern = 0x"
	<< std::hex << (unsigned short)delays << std::dec << std::endl;
  int btd;
  int xfer_done[2];
  char data[2];
  int addr = theBaseAddress + CSR5;
  //fg data[0] = delays;
  //fg data[1] = 0;
  data[0] = 0;
  data[1] = delays;
  write(btd,data,addr,2,xfer_done);
}


void MPC::interconnectTest(){
  char data[2];
  int btd, xfer_done[2];

  // reset FPGA logic
  int addr = theBaseAddress + CSR0;
  data[0] = 0x00;
  data[1] = 0x12;
  //data[0] = 0x12;
  //data[1] = 0x00;
  write(btd,data,addr,2,xfer_done);
  read(btd, data, theBaseAddress + CSR0,2, xfer_done);
  std::cout << "MPC: interconnectTest  0x" << std::hex << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;

  // release reset, put TLK2501 transmitters in the test mode
  addr = theBaseAddress + CSR0;
  data[0] = 0xc2;
  data[1] = 0x10;
  //data[0] = 0x10;
  //data[1] = 0xc2;
  write(btd,data,addr,2,xfer_done);
  read(btd, data, theBaseAddress + CSR0,2, xfer_done);
  std::cout << "MPC: interconnectTest  0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
}


#else

//-----------------------------------------------------------------------
// $Id: MPC.cc,v 2.17 2006/01/14 22:24:50 mey Exp $
// $Log: MPC.cc,v $
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
#include "MPC.h"
#include "VMEController.h"

MPC::MPC(int newCrate, int slot) : VMEModule(newCrate, slot),
				   TLK2501TxMode_(0), TransparentModeSources_(0), TMBDelayPattern_(0){
  std::cout << "MPC: module created in crate=" << this->crate() 
	    << " slot=" << this->slot() << std::endl;
  MyOutput_ = &std::cout ;
}


MPC::~MPC(){
  std::cout << "MPC: module removed from crate=" << this->crate() 
       << " slot=" << this->slot() << std::endl;
}


void MPC::configure() {
}

bool MPC::SelfTest() {
  //
  return 0;
  //
}

void MPC::init() {
  int btd;
  int xfer_done[2];
  char data[2];
  char addr;

  std::cout << "MPC: initialize" << std::endl;

  std::cout << "MPC: turn off Resets" <<std::endl;
  addr = CSR0;
  data[0]=0x10;
  data[1]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  std::cout << "MPC: logic reset" << std::endl;
  data[0]=0x12;
  data[1]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  std::cout << "MPC: end logic Reset" << std::endl;
  data[0]=0x10;
  data[1]=0x4a;
  do_vme(2, addr, data, NULL, 1);

  //read_csr0();

  std::cout << "MPC: set default serializer TX mode ..." << std::endl;
  setTLK2501TxMode(TLK2501TxMode_);

  // Sorter Mode is the default power-up mode of the MPC.
  std::cout << "MPC: set default MPC operation Mode ..." << std::endl;
  if (TransparentModeSources_)
    setTransparentMode(TransparentModeSources_);
  else
    setSorterMode();

  // The default power-up delays are always 0.
  std::cout << "MPC: setting default TMB-MPC delays ..." << std::endl;
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
  std::cout << "MPC: FIFO-A1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A1b, data);
  std::cout << "MPC: FIFO-A1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2a, data);
  std::cout << "MPC: FIFO-A2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A2b, data);
  std::cout << "MPC: FIFO-A2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3a, data);
  std::cout << "MPC: FIFO-A3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A3b, data);
  std::cout << "MPC: FIFO-A3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4a, data);
  std::cout << "MPC: FIFO-A4a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A4b, data);
  std::cout << "MPC: FIFO-A4b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5a, data);
  std::cout << "MPC: FIFO-A5a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A5b, data);
  std::cout << "MPC: FIFO-A5b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6a, data);
  std::cout << "MPC: FIFO-A6a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A6b, data);
  std::cout << "MPC: FIFO-A6b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7a, data);
  std::cout << "MPC: FIFO-A7a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A7b, data);
  std::cout << "MPC: FIFO-A7b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8a, data);
  std::cout << "MPC: FIFO-A8a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A8b, data);
  std::cout << "MPC: FIFO-A8b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9a, data);
  std::cout << "MPC: FIFO-A9a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
  read_fifo(FIFO_A9b, data);
  std::cout << "MPC: FIFO-A9b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
  //
}

void MPC::read_fifos() {
  //
  // Read FIFO-B until empty
  //
  std::cout << "MPC:  Read FIFO-B" << std::endl;
  char data[100];
  //read_fifo(STATUS, data);
  read_fifo(CSR3, data);
  std::cout.fill('0');
  std::cout << "FIFO status " << std::hex << data << std::endl;
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
    std::cout << "MPC: FIFO-B is empty!" << std::endl;
    return;
  } else {
    std::cout << "MPC: 1st Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << std::hex;
    std::cout.fill('0');
    std::cout << "MPC: FIFO-B1a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B1, data);
    Lct0 = (Lct0<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    FIFOBLct0.push_back(Lct0);
    std::cout << "MPC: FIFO-B1b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)  << std::endl;
    std::cout << "MPC: LCT0     = 0x" << std::setw(8) << Lct0 << std::endl ;
    //
    std::cout << "MPC: 2nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B2a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B2, data);
    Lct1 = (Lct1<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B2b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    FIFOBLct1.push_back(Lct1);
    std::cout << "MPC: LCT1     = 0x" << std::setw(8) << Lct1 << std::endl ;
    //
    std::cout << "MPC: 3nd Best Muon FIFO" << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B3a = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;
    read_fifo(FIFO_B3, data);
    Lct2 = (Lct2<<16) | ((data[0]&0x00ff) << 8) | (data[1]&0x00ff) ;
    std::cout << "MPC: FIFO-B3b = 0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) <<(data[1]&0x00ff) << std::endl;
    FIFOBLct2.push_back(Lct2);
    std::cout << "MPC: LCT2     = 0x" << std::setw(8) << Lct2 << std::endl ;
    std::cout.fill(' ');
    std::cout << std::dec;    
    //
    read_fifos();
    //
  }
  //
  read_fifo(CSR3, data);
  std::cout << "FIFO status = " << std::hex << data << std::endl;
  //
}

void MPC::read_csr0() {
  char data[100];
  read_fifo(CSR0,data);
  std::cout.fill('0');
  std::cout << "MPC: data read from CSR0: 0x" << std::hex 
       << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff)
       << std::dec << std::endl;
  std::cout.fill(' ');
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
  data[1] = (data[1]&0xfd) ;
  do_vme(2, addr, data, NULL, 1);
  printf("%x %x \n",data[0],data[1]);
  //
  data[1] = (data[1]&0xfd) | 0x2 ;
  do_vme(2, addr, data, NULL, 1);
  printf("%x %x \n",data[0],data[1]);
  //
  data[1] = (data[1]&0xfd) ;
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
  std::cout << "MPC: FIFO status = 0x" << std::hex << std::setw(2) << (data[0]&0x00ff)
       << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
  std::cout.fill(' ');
  bool full_fifoa=(data[1]&0x0001);
  bool empty_fifoa=(data[1]&0x0002)>>1;
  bool full_fifob=(data[1]&0x0004)>>2;
  bool empty_fifob=(data[1]&0x0008)>>3;

  if (full_fifoa>0)  std::cout << "MPC: FIFO_A is Full"  << std::endl;
  if (empty_fifoa>0) std::cout << "MPC: FIFO_A is Empty" << std::endl;
  if (full_fifob>0)  std::cout << "MPC: FIFO_B is Full"  << std::endl;
  if (empty_fifob>0) std::cout << "MPC: FIFO_B is Empty" << std::endl;;
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
  data[0]=0x01;
  data[1]=0x00;
  do_vme(2, addr, data, NULL, 1);

  // brute force set to 0xC210, will change to masking soon, very soon ...
  addr = CSR0;
  data[0]=0x10;
  data[1]=0xC2;
  do_vme(2, addr, data, NULL, 1);

  std::cout << "MPC: PRBS mode enabled" << std::endl;
}

void MPC::disablePRBS(){
  int btd, xfer_done[2];
  char data[2];
  char addr;

  // brute force set back to 0x5B10, will change to masking soon, very soon ...
  addr = CSR0;
  data[0]=0x10;
  data[1]=0x5B;
  do_vme(2, addr, data, NULL, 1);

  std::cout << "MPC: PRBS mode disabled" << std::endl;
}

void MPC::initTestLinks(){
// initialise FIFOs ready to load test data with injectSP functs
//nb! need to time in SP after this funct but before inject data

//initialise!!! toggle CSR0 bits.
 std::cout << "Initialising peripheral crate links for load FIFOs" << std::endl;

  int btd, xfer_done[2];
  char data[2];
  char addr;


  addr = CSR0;
  data[0]=0x11;
  data[1]=0x4E;
  do_vme(2, addr, data, NULL, 1);
  
  data[0]=0x13;
  data[1]=0x4E;
  do_vme(2, addr, data, NULL, 1);
 
  data[0]=0x11;
  data[1]=0x4E;
  do_vme(2, addr, data, NULL, 1);
//done init
}

void MPC::injectSP(){

  int btd, xfer_done[2];
  char data[2];
  char addr;

  int ITR=255;

 this->read_status();
 
   std::cout << "..now filling FIFO-A" <<std::endl;
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
		     data[0]= mframe1&0x00FF;
		     data[1]=(mframe1&0xFF00)>>8;
		     this->do_vme(2, addr, data, NULL, 1);

//2 frames?:ie 2x16 bits =tot 32 bits..

		     data[0]= mframe2&0x00FF;
		     data[1]=(mframe2&0xFF00)>>8;
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
  
  std::cout<<"...inject test pattern funct"<<std::endl;
  std::cout<<"data file passed is: "<<injectDataFileName<<std::endl;
  //let's try to open the file:: 
  
  
  
  std::cout<<"opening file..."<<std::endl;
  FILE* myFile = fopen(injectDataFileName , "r" );
  if (myFile==NULL){
    std::cout<<"problem opening data file, exiting.."<<std::endl;
    exit(0);
  }
  
  
  
  this->read_status();
  
  std::cout<<" will use LCT data from file "<<injectDataFileName<<std::endl;
  
  std::cout << "..now filling FIFO-A, with your data, alternating frame 1, 2.." <<std::endl;
  //   int ITR=2;mframe2 = ITR;
  


  // read the first data word and turn it into and integer:
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr1);
  std::cout<<"first frame 1 :: (first word in file) is "<<std::hex<<dataWordInt_fr1<<std::endl;
  if (readWord<1){
    std::cout<<"problem reading first word in file ..exiting..."<<std::endl;
    exit(0);
  }
  readWord=fscanf( myFile,"%s",DataWord);
  sscanf(DataWord,"%x",&dataWordInt_fr2);
  std::cout<<"first frame 2 :: (second word in file) is "<<std::hex<<dataWordInt_fr2<<std::endl;    

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
	      data[0]= mframe1&0x00FF;
	      data[1]=(mframe1&0xFF00)>>8;
	      this->do_vme(2, addr, data, NULL, 1);
	      
	      //std::cout<<"event: "<<EVNT<<"...filling fifoA with "<<std::hex<<mframe1<<std::endl;
	      
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
	std::cout<<"reached end of file... .."<<EVNT<<" eventsx2 frames loaded.."<<std::endl;
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
    std::cout << "MPC: serializer in FRAMED mode" << std::endl;
  else if (mode==0)
    std::cout << "MPC: serializer in CONTINUOUS mode" << std::endl;
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
  do_vme(VME_READ,reg,sndbuf,rcvbuf,NOW);
  //
  int value = ((rcvbuf[0]&0xff)<<8)|(rcvbuf[1]&0xff);
  //
  printf(" MPC.reg=%x %x %x %x\n", reg, rcvbuf[0]&0xff, rcvbuf[1]&0xff,value&0xffff);
  //
  return value;
  //
}


void MPC::firmwareVersion(){
  /// report the firmware version
  int btd, xfer_done[2];
  char data[2];
  do_vme(1,CSR1,NULL, data, 1);
  
  int versionWord = (data[0]<<8) + (data[1]&0xFF);
  int day   =  versionWord & 0x1F;
  int month = (versionWord >> 5   ) & 0xF;
  int year  = (versionWord >>(5+4)) + 2000;
  (*MyOutput_) << "MPC: firmware version: " << std::dec
       << day << "-" << month << "-" << year << std::endl;
}


void MPC::setSorterMode(){
  /// Switches the MPC to Sorter Mode while keeping the original sources intact.
  std::cout << "MPC: switching to Sorter Mode" << std::endl;
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

  std::cout << "MPC: switching to Transparent Mode. Source pattern = 0x" 
       << std::hex << pattern << std::dec << std::endl;
  int btd, xfer_done[2];
  char data[2];
  char addr=CSR4;

  do_vme(1, addr, NULL, data, 1);

  // make sure that the last bit is actually 1, otherwise there is no transparentMode
  if ( !(pattern & 0x01)){
    std::cout << "MPC: WARNING - last bit in source pattern (" << (unsigned short)pattern 
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
  std::cout << "MPC: switching to Transparent Mode. No new source pattern loaded" << std::endl;
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
   std::cout << "MPC: setting TMB-MPC delays. Delay pattern = 0x"
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
  std::cout << "MPC: interconnectTest  0x" << std::hex << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::endl;

  // release reset, put TLK2501 transmitters in the test mode
  addr =  CSR0;
  data[0] = 0xc2;
  data[1] = 0x10;
  //data[0] = 0x10;
  //data[1] = 0xc2;
  do_vme(2, addr, data, NULL, 1);
  do_vme(1, addr, NULL, data, 1);
  std::cout << "MPC: interconnectTest  0x" << std::setw(2) << (data[0]&0x00ff) << std::setw(2) << (data[1]&0x00ff) << std::dec << std::endl;
}

#endif
