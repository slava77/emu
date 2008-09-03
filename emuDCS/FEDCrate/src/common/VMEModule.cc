/*****************************************************************************\
* $Id: VMEModule.cc,v 3.13 2008/09/03 17:52:59 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.13  2008/09/03 17:52:59  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.12  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
* Revision 3.11  2008/09/01 11:30:32  paste
* Added features to DDU, IRQThreads corresponding to new DDU firmware.
*
* Revision 3.10  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.9  2008/08/30 14:49:04  paste
* Attempts to make VME work under the new design model where VMEModules take over for the VMEController.
*
* Revision 3.8  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.7  2008/08/19 14:51:03  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEModule.h"

//#include <cmath>
#include <string>
#include <sstream>
//#include <stdio.h>
#include <iostream>
//#include <unistd.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "VMEController.h"

#define DELAY3 16.384

emu::fed::VMEModule::VMEModule(int mySlot):
	//vmeController_(0),
	slot_(mySlot),
	controller_(NULL)
{
	//vmeadd_ = slot_ << 19;
}


void emu::fed::VMEModule::setController(VMEController *controller) {
	if (controller_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of " << boardType() << " " << theSlot << std::endl;
	}
	controller_ = controller;
}



void emu::fed::VMEModule::start() {
	// vmeadd_=0x00000000|(slot_<<19);
	controller_->start(slot_);
}



void emu::fed::VMEModule::endDevice() {
	controller_->end();
}



int emu::fed::VMEModule::CAEN_read(unsigned long Address,unsigned short int *data){
	controller_->start(slot_);
	return controller_->CAEN_read(Address,data);
}

int emu::fed::VMEModule::CAEN_write(unsigned long Address,unsigned short int *data){
	controller_->start(slot_);
	return controller_->CAEN_write(Address,data);
}

void emu::fed::VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,const char *inbuf,char *outbuf,int irdsnd) {
	controller_->start(slot_);
	controller_->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void emu::fed::VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
	controller_->start(slot_);
	controller_->scan(reg, snd, cnt, rcv, ird);
}


void emu::fed::VMEModule::scan_reset(int reg,const char *snd,int cnt,char *rcv,int ird) {
	controller_->start(slot_);
	controller_->scan_reset(reg, snd, cnt, rcv, ird);
}

void emu::fed::VMEModule::InitJTAG(int port) {
	controller_->start(slot_);
	controller_->InitJTAG(port);
}


void emu::fed::VMEModule::CloseJTAG() {
	controller_->start(slot_);
	controller_->CloseJTAG();
	controller_->end();
}

void emu::fed::VMEModule::flush_vme()
{
	controller_->start(slot_);
	controller_->flush_vme();
}

void emu::fed::VMEModule::handshake_vme()
{
	controller_->start(slot_);
	controller_->handshake_vme();
}

void emu::fed::VMEModule::sleep_vme(const char *outbuf)
{
	controller_->start(slot_);
	controller_->sleep_vme(outbuf);
}
/*
void emu::fed::VMEModule::sleep_vme2(unsigned short int time)
{
	controller_->start(slot_);
	controller_->sleep_vme2(time);
}
*/
/*
void emu::fed::VMEModule::long_sleep_vme2(float time)
{
	controller_->start(slot_);
	controller_->long_sleep_vme2(time);
}
*/
void emu::fed::VMEModule::send_last()
{
	controller_->start(slot_);
	controller_->send_last();
}


void emu::fed::VMEModule::vmepara(const char *cmd,const char *snd,char *rcv)
{
	controller_->start(slot_);
	controller_->vmepara(cmd,snd,rcv);
}

void emu::fed::VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{
	controller_->start(slot_);
	controller_->vmeser(cmd,snd,rcv);
}

void emu::fed::VMEModule::dcc(const char *cmd,char *rcv)
{
	controller_->start(slot_);
	controller_->dcc(cmd,rcv);
}

void emu::fed::VMEModule::vme_adc(int ichp,int ichn,char *rcv)
{
	controller_->start(slot_);
	controller_->vme_adc(ichp,ichn,rcv);
}
