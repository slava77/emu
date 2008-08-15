/*****************************************************************************\
* $Id: VMEModule.cc,v 3.6 2008/08/15 08:35:51 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.6  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "VMEModule.h"

//#include <cmath>
#include <string>
//#include <stdio.h>
#include <iostream>
//#include <unistd.h>

#include "VMEController.h"

emu::fed::VMEModule::VMEModule(int myCrate, int mySlot):
	vmeController_(0),
	slot_(mySlot)
{
	std::cerr << "Deprecated constructor for VMEModule.  Use VMEModule(int), not VMEModule(int,int)." << std::endl;

}

emu::fed::VMEModule::VMEModule(int mySlot):
	vmeController_(0),
	slot_(mySlot)
{
}

void emu::fed::VMEModule::setController(VMEController *controller) {
	if (vmeController_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of " << boardType() << " " << slot_ << std::endl;
	}
	vmeController_ = controller;
}

void emu::fed::VMEModule::start() {
	// vmeadd=0x00000000|(slot_<<19);
	vmeController_->start(slot_);
}


void emu::fed::VMEModule::end() {
}

 
void emu::fed::VMEModule::endDevice() {
  vmeController_->end();
}

 
int emu::fed::VMEModule::CAEN_read(unsigned long Address,unsigned short int *data){
  vmeController_->start(slot_);
  return vmeController_->CAEN_read(Address,data);
}

int emu::fed::VMEModule::CAEN_write(unsigned long Address,unsigned short int *data){
  vmeController_->start(slot_);
  return vmeController_->CAEN_write(Address,data);
}

void emu::fed::VMEModule::devdo(DEVTYPE dev,int ncmd,const char *cmd,int nbuf,
                     const char *inbuf,char *outbuf,int irdsnd) {
  vmeController_->start(slot_);
  vmeController_->devdo(dev, ncmd, cmd, nbuf, inbuf, outbuf, irdsnd);
}


void emu::fed::VMEModule::scan(int reg,const char *snd,int cnt,char *rcv,int ird) {
  vmeController_->start(slot_);
  vmeController_->scan(reg, snd, cnt, rcv, ird);
}


void emu::fed::VMEModule::scan_reset(int reg,const char *snd,int cnt,char *rcv,int ird) {
  vmeController_->start(slot_);
  vmeController_->scan_reset(reg, snd, cnt, rcv, ird);
}

void emu::fed::VMEModule::InitJTAG(int port) {
  vmeController_->start(slot_);
  vmeController_->InitJTAG(port);
}


void emu::fed::VMEModule::CloseJTAG() {
  vmeController_->start(slot_);
  vmeController_->CloseJTAG();
  vmeController_->end();
}

void emu::fed::VMEModule::flush_vme()
{
  vmeController_->start(slot_);
  vmeController_->flush_vme();
}

void emu::fed::VMEModule::handshake_vme()
{
  vmeController_->start(slot_);
  vmeController_->handshake_vme();
}

void emu::fed::VMEModule::sleep_vme(const char *outbuf)
{
  vmeController_->start(slot_);
  vmeController_->sleep_vme(outbuf);
}
void emu::fed::VMEModule::sleep_vme2(unsigned short int time)
{
  vmeController_->start(slot_);
  vmeController_->sleep_vme2(time);
}

void emu::fed::VMEModule::long_sleep_vme2(float time)
{
  vmeController_->start(slot_);
  vmeController_->long_sleep_vme2(time);
}

void emu::fed::VMEModule::send_last()
{
  vmeController_->start(slot_);
  vmeController_->send_last();
}


void emu::fed::VMEModule::vmepara(const char *cmd,const char *snd,char *rcv)
{
  vmeController_->start(slot_);
  vmeController_->vmepara(cmd,snd,rcv);
}

void emu::fed::VMEModule::vmeser(const char *cmd,const char *snd,char *rcv)
{
  vmeController_->start(slot_);
  vmeController_->vmeser(cmd,snd,rcv);
}

void emu::fed::VMEModule::dcc(const char *cmd,char *rcv)
{
  vmeController_->start(slot_);
  vmeController_->dcc(cmd,rcv);
}

void emu::fed::VMEModule::vme_adc(int ichp,int ichn,char *rcv)
{
  vmeController_->start(slot_);
  vmeController_->vme_adc(ichp,ichn,rcv);
}


