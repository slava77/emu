/*****************************************************************************\
* $Id: VMEModule.cc,v 3.14 2008/09/07 22:25:36 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.14  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
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
#include "JTAGElement.h"

#define DELAY3 16.384

emu::fed::VMEModule::VMEModule(int mySlot):
	//vmeController_(0),
	slot_(mySlot),
	controller_(NULL)
{
	vmeAddress_ = slot_ << 19;
}


void emu::fed::VMEModule::setController(VMEController *controller) {
	if (controller_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of " << boardType() << " " << slot_ << std::endl;
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



void emu::fed::VMEModule::writeCycle(int32_t myAddress, unsigned int nBits, std::vector<int16_t> myData)
throw(FEDException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;

	// Now, I start the sending process...
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		writeVME(myAddress,myData[iWord]);
	}
	return;
}



std::vector<int16_t> emu::fed::VMEModule::readCycle(int32_t myAddress, unsigned int nBits)
throw(FEDException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;

	// Reserving speeds things up and helps prevent memory fragmentation.
	std::vector<int16_t> result;
	result.reserve(nWords);
	
	// Now, I start the sending process...
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		result.push_back(readVME(myAddress));
	}
	return result;
}



std::vector<int16_t> emu::fed::VMEModule::writeRegAdvanced(enum DEVTYPE dev, int32_t myAddress, unsigned int nBits, int16_t myData)
throw(FEDException)
{
	std::vector<int16_t> newData;
	newData.push_back(myData);
	return writeRegAdvanced(dev, myAddress, nBits, newData);
}



void emu::fed::VMEModule::commandCycle(enum DEVTYPE dev, int16_t myCommand)
throw (FEDException)
{
	
	// Nab the JTAG element information
	JTAGElement *element = JTAGMap[dev];

	// Address encoded in the JTAG channel
	int32_t myAddress = element->bitCode;
	
	// The RESET command is very straight forward
	if (dev == RESET || dev == RESET2) {
		// Special reset address
		

		// Some fake vectors for sending data.
		std::vector<int16_t> bogoData0(1,0);
		std::vector<int16_t> bogoData1(1,1);
		
		// Send the reset command pattern
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData1);
		writeCycle(myAddress, element->cmdBits, bogoData1);
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData0);

		return;
	}
	
	myAddress |= 0x0000001c | ((element->cmdBits - 1) << 8);
	
	// Make me a vector for writing
	std::vector<int16_t> bogoData(1,myCommand);
	
	// Send the command
	writeCycle(myAddress, element->cmdBits, bogoData);
	
	// Nothing to return
	return;
}



std::vector<int16_t> emu::fed::VMEModule::jtagReadWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<int16_t> myData)
throw (FEDException)
{
	JTAGElement *element = JTAGMap[dev];

	// Set up the return value.
	std::vector<int16_t> result;

	// The address is encoded in the JTAG channel
	int32_t myAddress = element->bitCode;
	
	// The number of bits you have to send increases by one for each JTAG element
	// in the chain.  Count those now.
	unsigned int extraBits = 0;
	enum DEVTYPE previousDevice = element->previousDevice;
	while (previousDevice != NONE) {
		extraBits++;
		previousDevice = JTAGMap[previousDevice]->previousDevice;
	}
	
	// The RESET command writes things bit-by-bit.
	if (dev == RESET || dev == RESET2) {

		// Some fake vectors for sending data.
		std::vector<int16_t> bogoData0(1,0);
		std::vector<int16_t> bogoData1(1,1);
		std::vector<int16_t> bogoData2(1,2);
		std::vector<int16_t> bogoData3(1,3);
		
		// Send the write command pattern
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData1);
		writeCycle(myAddress, element->cmdBits, bogoData0);
		writeCycle(myAddress, element->cmdBits, bogoData0);

		// Send each bit in turn.
		// The 2nd bit of the data is the value of the bit,
		// the 1st bit ends the write.
		for (unsigned int iBit = 0; iBit < nBits + extraBits; iBit++) {

			// Discover which word/bit I am on.
			unsigned int iWord = iBit/16;
			unsigned int remainderBits = iBit % 16;
			
			if (iBit == nBits + extraBits - 1) { // last bit
				// Check if the bit is high/low
				if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, element->cmdBits, bogoData3);
				else writeCycle(myAddress, element->cmdBits, bogoData1);
			} else { // not last bit
				// Check if the bit is high/low
				if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, element->cmdBits, bogoData2);
				else writeCycle(myAddress, element->cmdBits, bogoData0);
			}
		}
		
		// Return nothing.
		return result;
	}
	
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits + extraBits == 0) ? 0 : (nBits + extraBits - 1)/16 + 1;
	unsigned int remainderBits = (nBits + extraBits) % 16;
	
	// Now, I start the sending process...
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
	
		// Check to see if this is the first word and flag appropriately.
		int32_t address = myAddress & 0xfffff0ff;
		if (iWord == 0) address |= 0x4; // first
		if (iWord == nWords - 1) address |= 0x8; // last
		if (iWord == nWords - 1 && remainderBits) address |= ((remainderBits - 1) << 8); // remainder, if any.
		else address |= 0x0f00;
	
		// Do the write command first, as this is required for every read-back.
		writeVME(address,myData[iWord]);
	
		// Read now and store it for later.
		address = myAddress | 0x14;
		int16_t tempResult = readVME(address);
	
		// Do some bit manipulations if this is the remainder.
		if (iWord == nWords - 1 && remainderBits) {
			tempResult = tempResult >> (16 - remainderBits);
		}

		// Problem:  There are extraBits garbage bits at the beginning of this read.
		// Everything needs to be shifted right extraBits times.
		// This means I have to manipulate previous entries in the vector.
		if (iWord > 0 && extraBits) {

			// Make sure the bits I shift off the end carry to the previous value.
			int16_t carryValue = tempResult << (16 - extraBits);
			result[iWord - 1] |= carryValue;

			// Then shift the garbage off.
			tempResult = tempResult >> extraBits;
		}
	
		result.push_back(tempResult);
	}
	
	return result;

}


int16_t emu::fed::VMEModule::readVME(uint32_t Address)
throw (FEDException)
{
	// The address always has the board slot encoded.
	Address |= vmeAddress_;
	
	// The address modifier for talking to other boards
	CVAddressModifier AM = cvA24_U_DATA;

	// 16-bit data width
	CVDataWidth DW = cvD16;

	// 16-bit buffer to fill durring reads
	int16_t *data;

	// Read and return error code
	CVErrorCodes err = CAENVME_ReadCycle(BHandle_, Address, data, AM, DW);

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			std::cerr << error.str() << std::endl;
			std::cerr << "    sleeping it off..." << std::endl;
			sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
		}
	}

	// 16-bit value to return.
	// PGK TESTING
	return *data;
}



void emu::fed::VMEModule::writeVME(uint32_t Address, int16_t data)
throw (FEDException)
{
	// The address always has the board slot encoded.
	Address |= vmeAddress_;
	
	// The address modifier for talking to other boards
	CVAddressModifier AM = cvA24_U_DATA;
	
	// 16-bit data width
	CVDataWidth DW = cvD16;
	
	// Write and return error code
	// PGK TESTING using non-char pointer...
	CVErrorCodes err = CAENVME_WriteCycle(BHandle_, Address, &data, AM, DW);
	
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			std::cerr << error.str() << std::endl;
			std::cerr << "    sleeping it off..." << std::endl;
			sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
		}
	}
	
	return;
}
