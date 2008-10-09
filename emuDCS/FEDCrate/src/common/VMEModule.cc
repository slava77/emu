//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEModule.cc,v 3.21 2008/10/09 11:21:19 paste Exp $
*
* $Log: VMEModule.cc,v $
* Revision 3.21  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 3.20  2008/10/04 18:44:06  paste
* Fixed bugs in DCC firmware loading, altered locations of files and updated javascript/css to conform to WC3 XHTML standards.
*
* Revision 3.19  2008/10/01 14:10:04  paste
* Fixed phantom reset bug in IRQ threads and shifted IRQ handling functions to VMEController object.
*
* Revision 3.17  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.16  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.15  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
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

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "VMEController.h"
#include "JTAGElement.h"

#define DELAY3 16.384

emu::fed::VMEModule::VMEModule(int mySlot):
	//vmeController_(0),
	slot_(mySlot)
{
	// Initialize mutexes
	pthread_mutex_init(&mutex_, NULL);
	
	vmeAddress_ = slot_ << 19;
}

/*
void emu::fed::VMEModule::setController(VMEController *controller) {
	
	if (controller_ != NULL) {
		std::cout << "WARNING: Trying change the VMEController of " << boardType() << " " << slot_ << std::endl;
	}

	controller_ = controller;
	
}
*/

/*
void emu::fed::VMEModule::start() {
	// vmeadd_=0x00000000|(slot_<<19);
	
	controller_->start(slot_);
	
}



void emu::fed::VMEModule::end() {
	// vmeadd_=0x00000000|(slot_<<19);
	
	controller_->end();
	
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
*/
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
/*
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
*/


void emu::fed::VMEModule::writeCycle(uint32_t myAddress, unsigned int nBits, std::vector<uint16_t> myData)
throw(FEDException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
	unsigned int remainderBits = nBits % 16;


	// Now, I start the sending process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am writing, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;
		
		writeVME(myAddress,myData[iWord] & bitMask);
	}
	pthread_mutex_unlock(&mutex_);
	
	return;
}



std::vector<uint16_t> emu::fed::VMEModule::readCycle(uint32_t myAddress, unsigned int nBits)
throw(FEDException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
	unsigned int remainderBits = nBits % 16;

	// Reserving speeds things up and helps prevent memory fragmentation.
	std::vector<uint16_t> result;
	result.reserve(nWords);
	
	// Now, I start the reading process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am read, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;
		result.push_back(readVME(myAddress) & bitMask);
	}
	pthread_mutex_unlock(&mutex_);
	return result;
}


/*
std::vector<uint16_t> emu::fed::VMEModule::writeRegAdvanced(enum DEVTYPE dev, uint32_t myAddress, unsigned int nBits, uint16_t myData)
throw(FEDException)
{
	std::vector<uint16_t> newData;
	newData.push_back(myData);
	return writeRegAdvanced(dev, myAddress, nBits, newData);
}
*/


void emu::fed::VMEModule::commandCycle(enum DEVTYPE dev, uint16_t myCommand)
throw (FEDException)
{

	// Address encoded in the JTAG channel, and is the same for all elements of the chain.
	// This is part of the definition of JTAG.
	JTAGChain chain = JTAGMap[dev];
	uint32_t myAddress = chain.front()->bitCode;
	
	// The RESET command is very straight forward
	if (dev == RESET || dev == RESET1 || dev == RESET2) {

		// Some fake vectors for sending data.
		std::vector<uint16_t> bogoData0(1,0);
		std::vector<uint16_t> bogoData1(1,1);

		// Send the reset command pattern
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData1);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData1);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);

		return;
	}

	// Now we need to know the total number of bits our command has.
	// It is very important that we calculate this correctly.
	unsigned int nBits = 0;

	// We also need to know what the commands are along the chain.
	std::vector<uint16_t> commands;

	// This is needed for smart pushing back of arbitrarily-sized commands
	unsigned int remainder = 0;
	unsigned int iCommand = 0;
	
	// Traverse the chain.
	for (JTAGChain::iterator iElement = chain.begin(); iElement != chain.end(); iElement++) {

		// If this is the element we are commanding, push back the command.
		// Else, push back a bypass signal.
		
		if ((*iElement)->dev == dev) {
			commands.push_back(myCommand);
		} else {
			commands.push_back((*iElement)->bypassCommand);
		}

		// Push the command into the vector of commands, but make sure you count the bits!
		// Now we need to be smart and connect the commands together as one long
		// set of 16-bit numbers for the CAEN writing to work properly.
		// Only do the magic shifting if this is not the first and the commands are not
		// 16 bits long.
		if (remainder && iCommand) {
			commands[iCommand - 1] |= commands[iCommand] << (16 - remainder);
			commands[iCommand] = commands[iCommand] >> remainder;
		}

		// Add together the total number of bits in the command.
		nBits += (*iElement)->cmdBits;

		// Set the remainder for the next iteration
		remainder = nBits % 16;
		iCommand++;

	}
	
	// The address has encoded in it the number of bits that are bing sent.
	myAddress |= 0x0000001c | ((nBits - 1) << 8);
	
	// Send the command
	writeCycle(myAddress, nBits, commands);
	
	// Nothing to return
	return;
}



std::vector<uint16_t> emu::fed::VMEModule::jtagWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<uint16_t> myData, bool noRead)
throw (FEDException)
{
	// Get the chain.  Very important to know.
	JTAGChain chain = JTAGMap[dev];

	// Set up the return value.
	std::vector<uint16_t> result;
	result.reserve(nBits/16 + 1);

	// The address is encoded in the JTAG channel.  The address is the same for all
	// elements in the chain (part of the definition of JTAG).
	uint32_t myAddress = chain.front()->bitCode;

	// The number of bits you have to send increases by one for each JTAG element
	// in the chain _after_ this element.  Count those now.
	// This is where reading and writing differ.
	unsigned int extraBits = 0;
	for (JTAGChain::reverse_iterator iElement = chain.rbegin(); iElement != chain.rend(); iElement++) {
		if ((*iElement)->dev != dev) {
			extraBits++;
		} else {
			break;
		}
	}
	
	// The RESET command writes things bit-by-bit.
	if (dev == RESET || dev == RESET2) {

		// Some fake vectors for sending data.
		std::vector<uint16_t> bogoData0(1,0);
		std::vector<uint16_t> bogoData1(1,1);
		std::vector<uint16_t> bogoData2(1,2);
		std::vector<uint16_t> bogoData3(1,3);
		
		// Send the write command pattern
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData1);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
		writeCycle(myAddress, chain.front()->cmdBits, bogoData0);

		// Send each bit in turn.
		// The 2nd bit of the data is the value of the bit,
		// the 1st bit ends the write.
		for (unsigned int iBit = 0; iBit < nBits + extraBits; iBit++) {

			// Discover which word/bit I am on.
			unsigned int iWord = iBit/16;
			unsigned int remainderBits = iBit % 16;
			
			if (iBit == nBits + extraBits - 1) { // last bit
				// Check if the bit is high/low
				if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, chain.front()->cmdBits, bogoData3);
				else writeCycle(myAddress, chain.front()->cmdBits, bogoData1);
			} else { // not last bit
				// Check if the bit is high/low
				if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, chain.front()->cmdBits, bogoData2);
				else writeCycle(myAddress, chain.front()->cmdBits, bogoData0);
			}
		}
		
		// Return nothing.
		return result;
	}
	
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits + extraBits == 0) ? 0 : (nBits + extraBits - 1)/16 + 1;
	unsigned int remainderBits = (nBits + extraBits) % 16;

	// We have to push extra bits through, so be sure that we have enough in myData
	// to actually push through.
	while (myData.size() < nWords) {
		myData.push_back(0xffff);
	}
	
	// Now, I start the sending process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {

		// If this is the last thing I am writing, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;
		
		// Check to see if this is the first word and flag appropriately.
		uint32_t address = myAddress & 0xfffff0ff;
		if (iWord == 0) address |= 0x4; // first
		if (iWord == nWords - 1) address |= 0x8; // last
		if ((iWord == nWords - 1) && remainderBits) {
			address |= ((remainderBits - 1) << 8); // remainder, if any.
			// For writing, the mask is on the write, so we want to mask out the HIGH bits.
			bitMask = (1 << remainderBits) - 1;
		}
		else address |= 0x0f00;

		
		// Do the write command first, as this is required for every read-back.
		writeVME(address,myData[iWord] & bitMask);
	
		// Read now and store it for later.
		if (!noRead) {
			address = myAddress | 0x14;
			uint16_t tempResult = readVME(address);
		
			// Do some bit manipulations if this is the remainder.
			if (iWord == nWords - 1 && remainderBits) {
				tempResult = tempResult >> (16 - remainderBits);
			}

			// Problem:  There are extraBits garbage bits at the beginning of this read.
			// Everything needs to be shifted right extraBits times.
			// This means I have to manipulate previous entries in the vector.
			if (extraBits) {

				// Make sure the bits I shift off the end carry to the previous value.
				if (iWord > 0) {
					uint16_t carryValue = tempResult << (16 - extraBits);
					result[iWord - 1] |= carryValue;
				}

				// Then shift the garbage off.
				tempResult = tempResult >> extraBits;
			}
		
			result.push_back(tempResult);
		}
	}
	pthread_mutex_unlock(&mutex_);
	
	return result;

}



std::vector<uint16_t> emu::fed::VMEModule::jtagRead(enum DEVTYPE dev, unsigned int nBits)
throw (FEDException)
{
	// Get the chain.  Very important to know.
	JTAGChain chain = JTAGMap[dev];
	
	// Set up the return value.
	std::vector<uint16_t> result;
	result.reserve(nBits/16 + 1);
	
	// The address is encoded in the JTAG channel.  The address is the same for all
	// elements in the chain (part of the definition of JTAG).
	uint32_t myAddress = chain.front()->bitCode;
	
	// The number of bits you have to send increases by one for each JTAG element
	// in the chain _before_ this element.  Count those now.
	unsigned int extraBits = 0;
	for (JTAGChain::iterator iElement = chain.begin(); iElement != chain.end(); iElement++) {
		if ((*iElement)->dev != dev) {
			extraBits++;
		} else {
			break;
		}
	}
	
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits + extraBits == 0) ? 0 : (nBits + extraBits - 1)/16 + 1;
	unsigned int remainderBits = (nBits + extraBits) % 16;
	
	// Now, I start the sending process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		
		// If this is the last thing I am reading, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;

		// Check to see if this is the first word and flag appropriately.
		uint32_t address = myAddress & 0xfffff0ff;
		if (iWord == 0) address |= 0x4; // first
		if (iWord == nWords - 1) address |= 0x8; // last
		if ((iWord == nWords - 1) && remainderBits) {
			address |= ((remainderBits - 1) << 8); // remainder, if any.
			// The bit mask is to mask in the HIGH remainderBits bits, not the LOW bits.
			bitMask = (1 << remainderBits) - 1;
			bitMask = bitMask << (16 - remainderBits);
		}
		else address |= 0x0f00;
		
		
		// Do the write command first, as this is required for every read-back.
		writeVME(address, 0xffff);
		
		// Read now and store it for later.
		address = myAddress | 0x14;
		uint16_t tempResult = readVME(address) & bitMask;
		//std::cerr << "tempResult is " << std::hex << tempResult << " mask " << bitMask << std::endl;

		// Do some bit manipulations if this is the remainder.
		if ((iWord == nWords - 1) && remainderBits) {
			tempResult = tempResult >> (16 - remainderBits);
		}

		// Problem:  There are extraBits garbage bits at the beginning of this read.
		// Everything needs to be shifted right extraBits times.
		// This means I have to manipulate previous entries in the vector.
		if (extraBits) {

			// Make sure the bits I shift off the end carry to the previous value.
			if (iWord > 0) {
				uint16_t carryValue = tempResult << (16 - extraBits);
				result[iWord - 1] |= carryValue;
			}

			// Then shift the garbage off.
			tempResult = tempResult >> extraBits;
		}

		//std::cerr << "tempResult finally is " << std::hex << tempResult << std::endl;

		result.push_back(tempResult);
	}
	pthread_mutex_unlock(&mutex_);
	
	return result;
	
}



uint16_t emu::fed::VMEModule::readVME(uint32_t Address)
throw (FEDException)
{
	// The address always has the board slot encoded.
	Address |= vmeAddress_;
	
	// The address modifier for talking to other boards
	CVAddressModifier AM = cvA24_U_DATA;

	// 16-bit data width
	CVDataWidth DW = cvD16;

	// 16-bit buffer to fill durring reads
	uint16_t data;
	//uint64_t *data;

	// Read and return error code
#ifdef CAEN_DEBUG
	std::cerr << std::hex << "New Read  BHandle_(" << BHandle_ << ") Address(" << Address << ") " << std::flush;
#endif
	CVErrorCodes err = CAENVME_ReadCycle(BHandle_, Address, &data, AM, DW);
#ifdef CAEN_DEBUG
	std::cerr << std::hex << "data(" << data << ")" << std::flush << std::endl;
#endif

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}

	// 16-bit value to return.
	// PGK TESTING
	return data;
}



void emu::fed::VMEModule::writeVME(uint32_t Address, uint16_t data)
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
	CVErrorCodes err = cvSuccess;
#ifdef CAEN_DEBUG
	std::cerr << std::hex << "New Write BHandle_(" << BHandle_ << ") Address(" << Address << ") data(" << data << ")" << std::flush << std::endl;
#else
	err = CAENVME_WriteCycle(BHandle_, Address, &data, AM, DW);
#endif
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			//std::cerr << error.str() << std::endl;
			//std::cerr << "    sleeping it off..." << std::endl;
			//sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	return;
}



void emu::fed::VMEModule::loadPROM(enum DEVTYPE dev, char *fileName, std::string startString, std::string stopString)
throw (FEDException)
{
	
	// The element in the chain that I am using
	JTAGChain chain = JTAGMap[dev];
	JTAGElement *element = NULL;
	for (JTAGChain::iterator iElement = chain.begin(); iElement != chain.end(); iElement++) {
		if ((*iElement)->dev == dev) {
			element = (*iElement);
			break;
		}
	}
	
	// Now we open the file and being parsing.
	std::ifstream inFile(fileName, std::ifstream::in);
	
	// Can't have bogus files
	if (!inFile.is_open()) {
		std::stringstream error;
		error << "Cannot open file " << fileName;
		XCEPT_RAISE(FEDException, error.str());
	}
	
	// Now start parsing the file.  Read lines until we have an eof.
	while (!inFile.eof()) {
		
		// Each line is a command (or comment)
		std::string myLine;
		getline(inFile, myLine);
		
		// Start after a particular command
		// (useful for after loading a custom value into the usercode)
		if (startString != "") {
			if (myLine.find(startString) != std::string::npos) {
				startString = "";
				//std::cerr << "START!" << std::flush << std::endl;
			}
			continue;
		}
		
		// End immidately on a particular command
		// (useful for before loading a custom value into the usercode)
		if (stopString != "") {
			if (myLine.find(stopString) != std::string::npos) {
				inFile.close();
				//std::cerr << "STOP!" << std::flush << std::endl;
				return;
			}
		}
		
		// Automatically reject comments.
		if (myLine.substr(0,2) == "//" || myLine.substr(0,1) == "!") {
			std::cerr << myLine << std::flush << std::endl;
			continue;
		}
		
		// IMPORTANT:  Concatonate lines until there is a semicolon
		while (myLine.rfind(';') == std::string::npos) {
			
			// There may be nothing more to read.  In that case, quit (no error)
			if (inFile.eof()) {
				std::cout << "Warning: reached end-of-file and discarding the line " << myLine << std::endl;
				inFile.close();
				return;
			}
			
			// Get the next line and staple it on.
			std::string nextLine;
			getline(inFile, nextLine);
			myLine += nextLine;
		}
		
		std::cerr << myLine << std::flush << std::endl;
		
		// Wipe out that troublesome semicolon now.
		myLine = myLine.substr(0,myLine.find(';'));
		
		// Make the line into a stream for easier parsing.
		std::istringstream myLineStream(myLine);
		
		// The first thing in the string is the command.
		std::string command;
		myLineStream >> command;
		
		// Switch on the command
		// SDR is "send data", SIR is "send instruction"
		if (command == "SDR" || command == "SIR") {

			//std::cerr << "Found command " << command << std::endl;
			
			// The next thing is the decimal number of bits.
			unsigned long int nBits = 0;
			myLineStream >> std::dec >> nBits;

			//std::cerr << "Found nBits " << nBits << std::endl;
			
			// It will be imporant to know how many words and bytes this is.
			// Use the magic ceiling...
			//unsigned long int nWords = (nBits == 0) ? 0 : (nBits - 1) / 16 + 1;
			unsigned long int nBytes = (nBits == 0) ? 0 : (nBits - 1) / 8 + 1;
			
			// The next instruction should be "TDI".  If not, it's not useful to us.
			std::string nextCommand;
			myLineStream >> nextCommand;

			//std::cerr << "Found nextCommand " << nextCommand << std::endl;
			
			if (nextCommand != "TDI") {
				continue;
			}
			
			// Read in the crap after TDI as a string.  We have to parse it as hex.
			std::string value;
			myLineStream >> value;

			//std::cerr << "Found value " << value << std::endl;
			
			// If we know what we are doing, there are parentheses at the beginning and end.
			if (value[0] != '(' || value[value.length() - 1] != ')') {
				std::cout << "Warning:  " << command << " value " << value << " not enclosed in parentheses.  Ignoring." << std::endl;
				continue;
			} else {
				value = value.substr(1,value.length() - 2);
			}
			
			// Now we parse the hex digits one byte at a time, as all instructions have
			// byte-sized values.
			std::vector<uint8_t> bogoData;
			bogoData.reserve(nBytes);
			for (unsigned long int iByte = 0; iByte < nBytes; iByte++) {
				uint8_t byte;
				sscanf(value.substr(iByte * 2, 2).c_str(), "%2hhx", &byte);
				// Be careful!  This is in reverse order!
				bogoData.push_back(byte & 0xff);
				//std::cerr << "bogoData " << iByte << " " << (int) bogoData[iByte] << std::endl;
			}
			
			// Make the data into a vector we can use.
			std::vector<uint16_t> myData;
			myData.reserve(nBytes/2);
			// Remember that the vector we made above is made in reverse endianness!
			// I would like to iterate backwards here, but I have to use two elements
			// of the old vector at a time to make the new vector.
			for (unsigned int iDatum = 0; iDatum < nBytes; iDatum +=2) {
				uint16_t newDatum = bogoData[nBytes - iDatum - 1] & 0x00ff;
				if ( (iDatum + 1) < nBytes) {
					newDatum |= ((bogoData[nBytes - iDatum - 2] & 0x00ff) << 8);
				}
				myData.push_back(newDatum & 0xffff);
				//std::cerr << "myData " << (iDatum/2) << " " << (int) myData[iDatum/2] << std::endl;
			}
			
			// Send instructions, but only if it makes sense to.
			if (command == "SIR") {
				/*
				if (nBits != element->cmdBits) {
					std::stringstream error;
					error << "SIR command " << value << " with nBits " << nBits << " does not match the number of bits in the command bus of dev " << element->name << " (" << element->cmdBits << ")";
					XCEPT_RAISE(FEDException, error.str());
				}
				*/
				// The number of bits matches that of the command bus.
				//std::clog << "Attempting commandCycle with dev " << dev << " data " << myData[0] << std::endl;
				commandCycle(dev, myData[0]);
				
				continue;
				
			// Send data
			} else {
				
				// It's just a JTAG read/write.
				//std::vector<uint16_t> result = jtagReadWrite(dev, nBits, myData);
				// You can do something with the result here if you want.
				
				// The "true" at the end means write-only and don't read back.
				jtagWrite(dev, nBits, myData, true);
				
				continue;
			}
			
			// That's it.  Now we do sleeping
		} else if (command == "RUNTEST") {
			
			// The next variable is the time to sleep.
			unsigned long int time;
			myLineStream >> time;
			
#ifdef CAEN_DEBUG
			timeval startTime;
			timeval endTime;
			gettimeofday(&startTime,NULL);
#endif
			
			// Only use usleep if the number of microseconds is greater than 50.
			// Below that, we don't have the kind of resolution we need to be accurate,
			// and it's not like it matters much anyway.
			if (time >= 50) {
				usleep(time);
			}
			
#ifdef CAEN_DEBUG
			gettimeofday(&endTime,NULL);
			unsigned long int diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
			std::clog << "--usleep time: " << diffTime << " microseconds" << std::endl;
#endif
			
			continue;
			
			// Finally, a reset-idle command
		} else if (command == "STATE") {
			
			// The only state we care about is when we have
			// STATE RESET IDLE;
			// So check for these two.
			std::string value;
			myLineStream >> value;
			
			if (value != "RESET") continue;
			
			myLineStream >> value;
			
			if (value != "IDLE") continue;
			
			// Now we reset idle, clearly.
			
			if (dev == RESET || dev == RESET1 || dev == RESET2) {
				
				uint32_t myAddress = element->bitCode;
				
				// Real data to send.
				std::vector<uint16_t> bogoBits(1,1);
				
				// Send it 5 times.
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				
				continue;
				
			} else {
				
				uint32_t myAddress = element->bitCode | 0x00000018;
				
				// Fake data to send.
				std::vector<uint16_t> bogoBits(1,0x0);
				
				writeCycle(myAddress, element->cmdBits, bogoBits);
			}
			continue;
			
		}
	}
	
	inFile.close();
}


/*
void emu::fed::VMEModule::bogoDelay(uint64_t time) {
	// I hate myself for doing this, but it looks like I have no choice.
	// These machines do not run in real-time, so I need a busyloop to wait
	// a certain number of microseconds.

	uint64_t bogoLoops = (uint64_t) (time * bogoMips_);
	while (bogoLoops > 0) {
		--bogoLoops;
	}
	return;
}
*/

