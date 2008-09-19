//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEModule.cc,v 3.15 2008/09/19 16:53:52 paste Exp $
*
* $Log: VMEModule.cc,v $
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
	slot_(mySlot),
	controller_(NULL)
{
	
	vmeAddress_ = slot_ << 19;

	// In order to have a more accurate count in the delay sequence,
	// I need to calculate bogomips myself.
	// I wish there were a better way to do this, but these machines
	// do not run in real time.
	unsigned long int loopsThatWasteTheComputersTime = 1000000;
	timeval startTime;
	timeval endTime;
	gettimeofday(&startTime,NULL);
	
	while (loopsThatWasteTheComputersTime > 0) {
		--loopsThatWasteTheComputersTime;
	}
	
	gettimeofday(&endTime,NULL);
	uint64_t usecsWasted = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
	bogoMips_ = ((long double) 1000000. / (long double) usecsWasted);
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
	unsigned int remainderBits = nBits % 16;

	// Now, I start the sending process...
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am writing, be sure to use a bitmask.
		int16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;
		writeVME(myAddress,myData[iWord] & bitMask);
	}
	return;
}



std::vector<int16_t> emu::fed::VMEModule::readCycle(int32_t myAddress, unsigned int nBits)
throw(FEDException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
	unsigned int remainderBits = nBits % 16;

	// Reserving speeds things up and helps prevent memory fragmentation.
	std::vector<int16_t> result;
	result.reserve(nWords);
	
	// Now, I start the reading process...
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am writing, be sure to use a bitmask.
		int16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;
		result.push_back(readVME(myAddress) & bitMask);
	}
	return result;
}


/*
std::vector<int16_t> emu::fed::VMEModule::writeRegAdvanced(enum DEVTYPE dev, int32_t myAddress, unsigned int nBits, int16_t myData)
throw(FEDException)
{
	std::vector<int16_t> newData;
	newData.push_back(myData);
	return writeRegAdvanced(dev, myAddress, nBits, newData);
}
*/


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
	// I have to be careful:  if there are multiple JTAG devices in the chain,
	// I have to send the bypass command as the first command in the sequence, then
	// I send the additional commands tagged on.
	unsigned int nBits = 8;
	if (element->previousDevice != NONE) {
		myCommand = (myCommand << 8) & 0xff00;
		myCommand |= (JTAGMap[element->previousDevice]->bypassCommand & 0x00ff);
		nBits += 8;
	}
	std::vector<int16_t> bogoData(1,myCommand);
	
	// Send the command
	writeCycle(myAddress, nBits , bogoData);
	
	// Nothing to return
	return;
}



std::vector<int16_t> emu::fed::VMEModule::jtagReadWrite(enum DEVTYPE dev, unsigned int nBits, std::vector<int16_t> myData, bool writeOnly)
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

		// If this is the last thing I am writing, be sure to use a bitmask.
		int16_t bitMask = 0xffff;
		
		// Check to see if this is the first word and flag appropriately.
		int32_t address = myAddress & 0xfffff0ff;
		if (iWord == 0) address |= 0x4; // first
		if (iWord == nWords - 1) address |= 0x8; // last
		if ((iWord == nWords - 1) && remainderBits) {
			address |= ((remainderBits - 1) << 8); // remainder, if any.
			bitMask = (1 << remainderBits) - 1;
		}
		else address |= 0x0f00;

		
		// Do the write command first, as this is required for every read-back.
		writeVME(address,myData[iWord] & bitMask);
	
		// Read now and store it for later.
		if (!writeOnly) {
			address = myAddress | 0x14;
			int16_t tempResult = readVME(address);
		
			// Do some bit manipulations if this is the remainder.
			if (iWord == nWords - 1 && remainderBits) {
				tempResult = tempResult >> (16 - remainderBits);
			}

			// Problem:  There are extraBits garbage bits at the beginning of this read.
			// Everything needs to be shifted right extraBits times.
			// This means I have to manipulate previous entries in the vector.
			if (extraBits) {

				// Make sure the bits I shift off the end carry to the previous value.
				int16_t carryValue = tempResult << (16 - extraBits);
				if (iWord > 0) result[iWord - 1] |= carryValue;

				// Then shift the garbage off.
				tempResult = tempResult >> extraBits;
			}
		
			result.push_back(tempResult);
		}
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
	int16_t data;
	//int64_t *data;

	// Read and return error code
	//std::cerr << std::hex << "New Read  BHandle_(" << BHandle_ << ") Address(" << Address << ") " << std::flush;
	CVErrorCodes err = CAENVME_ReadCycle(BHandle_, Address, &data, AM, DW);
	//std::cerr << std::hex << "data(" << data << ")" << std::flush << std::endl;

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			std::cerr << error.str() << std::endl;
			std::cerr << "    sleeping it off..." << std::endl;
			sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}

	// 16-bit value to return.
	// PGK TESTING
	return data;
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
	//std::cerr << std::hex << "New Write BHandle_(" << BHandle_ << ") Address(" << Address << ") data(" << data << ")" << std::flush << std::endl;
	CVErrorCodes err = cvSuccess;
#ifndef CAEN_DEBUG
	err = CAENVME_WriteCycle(BHandle_, Address, &data, AM, DW);
#endif
	if (err != cvSuccess) {
		std::ostringstream error;
		error << "error " << err << ": " << CAENVME_DecodeError(err);
		if (err == cvBusError) {
			std::cerr << error.str() << std::endl;
			std::cerr << "    sleeping it off..." << std::endl;
			sleep((unsigned int) 1);
		} else {
			XCEPT_RAISE(FEDException, error.str());
			//std::cerr << error.str() << std::endl;
		}
	}
	
	return;
}



void emu::fed::VMEModule::loadPROMAdvanced(enum DEVTYPE dev, char *fileName, std::string startString, std::string stopString)
throw (FEDException)
{
	
	// The element in the chain that I am using
	JTAGElement *element = JTAGMap[dev];
	
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
			}
			continue;
		}
		
		// End immidately on a particular command
		// (useful for before loading a custom value into the usercode)
		if (stopString != "") {
			if (myLine.find(stopString) != std::string::npos) {
				inFile.close();
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
			std::vector<int8_t> bogoData;
			bogoData.reserve(nBytes);
			for (unsigned long int iByte = 0; iByte < nBytes; iByte++) {
				int8_t byte;
				sscanf(value.substr(iByte * 2, 2).c_str(), "%2hhx", &byte);
				// Be careful!  This is in reverse order!
				bogoData.push_back(byte);
				//std::cerr << "bogoData " << iByte << " " << (int) bogoData[iByte] << std::endl;
			}
			
			// Make the data into a vector we can use.
			std::vector<int16_t> myData;
			// Remember that the vector we made above is made in reverse endianness!
			// I would like to iterate backwards here, but I have to use two elements
			// of the old vector at a time to make the new vector.
			for (unsigned int iDatum = 0; iDatum < nBytes; iDatum +=2) {
				int16_t newDatum = bogoData[nBytes - iDatum - 1];
				if ( (iDatum + 1) < nBytes) {
					newDatum |= (bogoData[nBytes - iDatum - 2] << 8);
				} else {
					newDatum &= 0xff00;
				}
				myData.push_back(newDatum);
				//std::cerr << "myData " << iDatum << " " << (int) myData[iDatum] << std::endl;
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
				//std::vector<int16_t> result = jtagReadWrite(dev, nBits, myData);
				// You can do something with the result here if you want.
				
				// The "true" at the end means write-only and don't read back.
				jtagReadWrite(dev, nBits, myData, true);
				
				continue;
			}
			
			// That's it.  Now we do sleeping
		} else if (command == "RUNTEST") {
			
			// The next variable is the time to sleep.
			unsigned long int time;
			myLineStream >> time;
			
			// Use magic bogoDelay.  We don't really need it exact, just close enough.
			// Oh, and at least 16 microseconds.
			//if (time < 16) time = 16;
			
			timeval startTime;
			timeval endTime;
			gettimeofday(&startTime,NULL);
			
			bogoDelay(time);
			
			gettimeofday(&endTime,NULL);
			unsigned long int diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
			std::clog << "--bogoDelay time: " << diffTime << " microseconds" << std::endl;
			
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
			
			if (dev == RESET || dev == RESET2) {
				
				int32_t myAddress = element->bitCode;
				
				// Real data to send.
				std::vector<int16_t> bogoBits(1,1);
				
				// Send it 5 times.
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				writeCycle(myAddress, element->cmdBits, bogoBits);
				
				continue;
				
			} else {
				
				int32_t myAddress = element->bitCode | 0x00000018;
				
				// Fake data to send.
				std::vector<int16_t> bogoBits(1,0xFFFF);
				
				writeCycle(myAddress, element->cmdBits, bogoBits);
			}
			continue;
			
		}
	}
	
	inFile.close();
}



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

