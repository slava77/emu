//----------------------------------------------------------------------
// $Id: VMEModule.cc,v 3.1 2007/07/23 05:03:31 gilmore Exp $
// $Log: VMEModule.cc,v $
// Revision 3.1  2007/07/23 05:03:31  gilmore
// major structural chages to improve multi-crate functionality
//
// Revision 3.0  2006/07/20 21:16:11  geurts
// *** empty log message ***
//
// Revision 1.3  2006/01/27 16:04:50  gilmore
// *** empty log message ***
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

extern unsigned long vmeadd;
extern int delay_type;

#ifndef debugV //silent mode
#define PRINT(x) 
#define PRINTSTRING(x)  
#else //verbose mode
#define PRINT(x) cout << #x << ":\t" << x << endl; 
#define PRINTSTRING(x) cout << #x << endl; 
#endif

VMEModule::VMEModule(int crate,int slot):
	theSlot(slot),theController(NULL)
{
	#ifdef debugV
	cout << "creating VMEModule" << endl;
	#endif 
	//printf("--Construction complete, controller address %08x\n",this->theController);
}

VMEModule::VMEModule(int slot):
	theSlot(slot),theController(NULL)
{
	#ifdef debugV
	cout << "creating VMEModule" << endl;
	#endif 
	//printf("--Construction complete, controller address %08x\n",this->theController);
}

void VMEModule::setController(VMEController *controller) {
	if (theController != NULL) {
		cout << "WARNING: Trying change the VMEController of " << boardType() << " " << theSlot << endl;
	}
	theController = controller;
}

void VMEModule::start() {
	PRINTSTRING(OVAL: start() from VMEModule have been called...);
	#ifdef debugV
	cout << "starting VMEModule for slot " << dec << theSlot << " boardType " << boardType() << " line " << (int) c << endl;
	#endif  
	vmeadd=0x00000000|(theSlot<<19);
}


void VMEModule::end() {
}

 
void VMEModule::endDevice() {
  theController->end();
}

 
int VMEModule::CAEN_read(unsigned long Address,unsigned short int *data){
  theController->start(this);
  return theController->CAEN_read(Address,data);
}

int VMEModule::CAEN_write(unsigned long Address,unsigned short int *data){
  theController->start(this);
  return theController->CAEN_write(Address,data);
}

void VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  theController->start(this);
  theController->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan(reg, snd, cnt, rcv, ird);
}


void VMEModule::scan_reset(int reg,const char *snd,int cnt,char *rcv,int ird) {
  theController->start(this);
  theController->scan_reset(reg, snd, cnt, rcv, ird);
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

void VMEModule::flush_vme()
{
  theController->start(this);
  theController->flush_vme();
}

void VMEModule::handshake_vme()
{
  theController->start(this);
  theController->handshake_vme();
}

void VMEModule::sleep_vme(const char *outbuf)
{
  theController->start(this);
  theController->sleep_vme(outbuf);
}
void VMEModule::sleep_vme2(unsigned short int time)
{
  theController->start(this);
  theController->sleep_vme2(time);
}

void VMEModule::long_sleep_vme2(float time)
{
  theController->start(this);
  theController->long_sleep_vme2(time);
}

void VMEModule::send_last()
{
  theController->start(this);
  theController->send_last();
}


void VMEModule::vmepara(const char *cmd,const char *snd,char *rcv)
{
  theController->start(this);
  theController->vmepara(cmd,snd,rcv);
}

void VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{
  theController->start(this);
  theController->vmeser(cmd,snd,rcv);
}

void VMEModule::dcc(const char *cmd,char *rcv)
{
  theController->start(this);
  theController->dcc(cmd,rcv);
}

void VMEModule::vme_adc(int ichp,int ichn,char *rcv)
{
  theController->start(this);
  theController->vme_adc(ichp,ichn,rcv);
}


