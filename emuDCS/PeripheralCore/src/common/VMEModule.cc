#ifndef OSUcc
//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 2.14 2006/03/10 13:13:13 mey Exp $
// $Log: VMEModule.cc,v $
// Revision 2.14  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.11  2006/01/31 08:52:29  mey
// UPdate
//
// Revision 2.10  2006/01/23 15:01:34  mey
// Update
//
// Revision 2.9  2006/01/23 15:00:10  mey
// Update
//
// Revision 2.8  2006/01/14 22:25:09  mey
// UPdate
//
// Revision 2.7  2006/01/12 11:48:17  mey
// Update
//
// Revision 2.6  2006/01/12 11:32:50  mey
// Update
//
// Revision 2.5  2005/12/08 12:00:32  mey
// Update
//
// Revision 2.4  2005/12/06 15:32:09  mey
// Fix bug
//
// Revision 2.3  2005/12/02 18:12:19  mey
// get rid of D360
//
// Revision 2.2  2005/11/21 19:42:28  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(int newcrate, int newslot): 
  theSlot(newslot), cnt(0)
{
  theCrate = Singleton<CrateSetup>::instance()->crate(newcrate);
  #ifdef debugV
    cout << "creating VMEModule in crate " << theCrate->number() << endl;
  #endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
  char sendline[2];
  int n=2;
  char c=0;
  unsigned int lev=0x01;
  c=c|lev;
  // next two bits are board type
  c=c|(boardType()<<2);
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif
  sendline[0]=c;
  sendline[1]=theSlot;
  theController->writen(sendline, n);
}


void VMEModule::end() {
  char sendline[2];
  int n=2;
  sendline[0]=0x02;
  writen(sendline, n);
}

 
void VMEModule::endDevice() {
  theController->end();
}


void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
  //
  char tmp;
  //
  theController->start(this);
  Tdata[cnt]=fcn;
  Tdata[cnt+1]=vme;
  Tdata[cnt+2]=snd[0];
  Tdata[cnt+3]=snd[1];
  //(fcn==VME_WRITE){
    // Jinghua Liu, add byte swap for MPC. Dec.7,2005
    //(boardType()==MPC_ENUM) {
  //Tdata[cnt+2]=snd[0];
  //Tdata[cnt+3]=snd[1];
  //}
  //else {
  //Tdata[cnt+2]=snd[0];
  //Tdata[cnt+3]=snd[1];
  //}
  //}
  cnt += 4;
  assert(cnt < TDATASIZE);
  ///@@ might be a bug with ORed codes in fcn
  if(fcn!=VME_READ && when==LATER)return;
  theController->writenn(Tdata,cnt);
  cnt=0;
  if( fcn==VME_READ || fcn==5 ){
    theController->readn(rcv);
    //if(boardType()==MPC_ENUM) {
      //       tmp=rcv[0];
      //       rcv[0]=rcv[1];
      //       rcv[1]=tmp;
    //}
  }
  //if(boardType()==MPC_ENUM) theController->end();
  //
}

int VMEModule::readn(char *line) {
  theController->start(this);
  return theController->readn(line);
}


int VMEModule::writen(register const char *ptr, register int nbytes) {
  theController->start(this);
  return theController->writen(ptr, nbytes);
}


int VMEModule::readline(char * line, int maxbytes) {
  theController->start(this);
  return theController->readline(line, maxbytes);
}


void VMEModule::devdo(DEVTYPE dev,int ncmd, const char *cmd,int nbuf,
                     char *inbuf,char *outbuf,int irdsnd) {
  theController->start(this);
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan(reg, snd, cnt, rcv, ird);
}


void VMEModule::InitJTAG(int port) {
  theController->start(this);
  theController->InitJTAG(port);
}


void VMEModule::CloseJTAG() {
  theController->start(this);
  theController->CloseJTAG();
  theController->end();
}


VMEController* VMEModule::getTheController(){
  return theController;
}

#else

//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 2.14 2006/03/10 13:13:13 mey Exp $
// $Log: VMEModule.cc,v $
// Revision 2.14  2006/03/10 13:13:13  mey
// Jinghua's changes
//
// Revision 2.11  2006/01/31 08:52:29  mey
// UPdate
//
// Revision 2.10  2006/01/23 15:01:34  mey
// Update
//
// Revision 2.9  2006/01/23 15:00:10  mey
// Update
//
// Revision 2.8  2006/01/14 22:25:09  mey
// UPdate
//
// Revision 2.7  2006/01/12 11:48:17  mey
// Update
//
// Revision 2.6  2006/01/12 11:32:50  mey
// Update
//
// Revision 2.5  2005/12/08 12:00:32  mey
// Update
//
// Revision 2.4  2005/12/06 15:32:09  mey
// Fix bug
//
// Revision 2.3  2005/12/02 18:12:19  mey
// get rid of D360
//
// Revision 2.2  2005/11/21 19:42:28  mey
// Update
//
// Revision 2.1  2005/11/02 16:16:43  mey
// Update for new controller
//
// Revision 1.23  2004/07/22 18:52:38  tfcvs
// added accessor functions for DCS integration
//
//
//----------------------------------------------------------------------
#include "VMEModule.h"
#include "VMEController.h"
#include "Crate.h"
#include "CrateSetup.h"
#include "Singleton.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h> // read and write

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif



VMEModule::VMEModule(int newcrate, int newslot): 
  theSlot(newslot)
{
  theCrate = Singleton<CrateSetup>::instance()->crate(newcrate);
  #ifdef debugV
    cout << "creating VMEModule in crate " << theCrate->number() << endl;
  #endif 
  theController = theCrate->vmeController();
  theCrate->addModule(this);
}


void VMEModule::start() {
  PRINTSTRING(OVAL: start() from VMEModule have been called...);
#ifdef debugV
  cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
#endif  
  theController->start( theSlot, boardType() );
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}

void VMEModule::do_vme(char fcn, char vme, 
                       const char *snd,char *rcv, int when) {
   theController->start( theSlot, boardType() );
   theController->do_vme(fcn, vme, snd, rcv, when);
}

void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  //printf("VMEModule::devdo\n");
  theController->start( theSlot, boardType() );
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}

void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start( theSlot, boardType() );
  if(boardType()==TMB_ENUM)
    theController->scan_alct(reg, snd, cnt, rcv, ird);
  else
    theController->scan(reg, snd, cnt, rcv, ird);
}

void VMEModule::InitJTAG(int port) {
  theController->start( theSlot, boardType() );
  theController->InitJTAG(port);
}

void VMEModule::CloseJTAG() {
  theController->start( theSlot, boardType() );
  theController->CloseJTAG();
  theController->end();
}


VMEController* VMEModule::getTheController(){
  return theController;
}

#endif
