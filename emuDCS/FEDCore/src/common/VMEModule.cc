//#define CAEN_DEBUG 1
/*****************************************************************************\
* $Id: VMEModule.cc,v 1.6 2009/07/01 14:17:19 paste Exp $
\*****************************************************************************/
#include "emu/fed/VMEModule.h"

#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
//#include "emu/fed/VMEController.h"

emu::fed::VMEModule::VMEModule(const unsigned int &mySlot):
slot_(mySlot)
{
	// Initialize mutexes
	pthread_mutex_init(&mutex_, NULL);

	vmeAddress_ = slot_ << 19;
}



void emu::fed::VMEModule::writeCycle(const uint32_t &myAddress, const unsigned int &nBits, const std::vector<uint16_t> &data, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	const unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
	const unsigned int remainderBits = nBits % 16;


	// Now, I start the sending process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am writing, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;

		try {
			writeVME(myAddress, data[iWord] & bitMask, debug);
		} catch (emu::fed::exception::CAENException &e) {
			pthread_mutex_unlock(&mutex_);
			std::ostringstream error;
			error << "Exception in writeCycle(myAddress=" << myAddress << ", nBits=" << nBits << ", data=" << &data << ")";
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}
	}
	pthread_mutex_unlock(&mutex_);

	return;
}



std::vector<uint16_t> emu::fed::VMEModule::readCycle(const uint32_t &myAddress, const unsigned int &nBits, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	const unsigned int nWords = (nBits == 0) ? 0 : (nBits - 1)/16 + 1;
	const unsigned int remainderBits = nBits % 16;

	// Reserving speeds things up and helps prevent memory fragmentation.
	std::vector<uint16_t> result;
	result.reserve(nWords);

	// Now, I start the reading process...
	pthread_mutex_lock(&mutex_);
	for (unsigned int iWord = 0; iWord < nWords; iWord++) {
		// If this is the last thing I am read, be sure to use a bitmask.
		uint16_t bitMask = 0xffff;
		if (iWord == nWords - 1 && remainderBits) bitMask = (1 << remainderBits) - 1;
		try {
			result.push_back(readVME(myAddress, debug) & bitMask);
		} catch (emu::fed::exception::CAENException &e) {
			pthread_mutex_unlock(&mutex_);
			std::ostringstream error;
			error << "Exception in readCycle(myAddress=" << myAddress << ", nBits=" << nBits << ")";
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}
	}
	pthread_mutex_unlock(&mutex_);
	return result;
}



void emu::fed::VMEModule::commandCycle(const enum DEVTYPE &dev, const uint16_t &myCommand, const bool &debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// Address encoded in the JTAG channel, and is the same for all elements of the chain.
	// This is part of the definition of JTAG.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		throw e2;
	}

	JTAGChain chain = JTAGMap[dev];
	uint32_t myAddress = chain.front()->bitCode;

	try {
		// The RESET command is very straight forward
		if (dev == RESET) {

			// Some fake vectors for sending data.
			const std::vector<uint16_t> bogoData0(1,0);
			const std::vector<uint16_t> bogoData1(1,1);
			const std::vector<uint16_t> bogoData2(1,2);
			const std::vector<uint16_t> bogoData3(1,3);

			// The number of bits in the command
			// Note:  the RESET path is always the first element.
			const unsigned int nBits = chain.front()->cmdBits;

			// Send the reset command pattern
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData1, debug);
			writeCycle(myAddress, 2, bogoData1, debug);
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData0, debug);

			// Send each bit in turn.
			// The 2nd bit of the data is the value of the bit,
			// the 1st bit ends the write.
			for (unsigned int iBit = 0; iBit < nBits; iBit++) {

				if (iBit == nBits - 1) { // last bit
					// Check if the bit is high/low
					if (myCommand & (1 << iBit)) writeCycle(myAddress, 2, bogoData3, debug);
					else writeCycle(myAddress, 2, bogoData1, debug);
				} else { // not last bit
					// Check if the bit is high/low
					if (myCommand & (1 << iBit)) writeCycle(myAddress, 2, bogoData2, debug);
					else writeCycle(myAddress, 2, bogoData0, debug);
				}
			}

			// End the reset command
			writeCycle(myAddress, 2, bogoData1, debug);
			writeCycle(myAddress, 2, bogoData0, debug);

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
		writeCycle(myAddress, nBits, commands, debug);

	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in commandCycle(dev=" << dev << ", myCommand=" << myCommand << ")";
		XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
		throw e2;
	}

	// Nothing to return
	return;
}



std::vector<uint16_t> emu::fed::VMEModule::jtagWrite(const enum DEVTYPE &dev, const unsigned int &nBits, const std::vector<uint16_t> &data, const bool &noRead, const bool &debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{
	std::vector<uint16_t> myData = data;
	
	// Get the chain.  Very important to know.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		throw e2;
	}

	JTAGChain chain = JTAGMap[dev];

	// Set up the return value.
	std::vector<uint16_t> result;
	result.reserve(nBits/16 + 1);

	// The address is encoded in the JTAG channel.  The address is the same for all
	// elements in the chain (part of the definition of JTAG).
	const uint32_t myAddress = chain.front()->bitCode;

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
	if (dev == RESET) {

		try {

			// Some fake vectors for sending data.
			const std::vector<uint16_t> bogoData0(1,0);
			const std::vector<uint16_t> bogoData1(1,1);
			const std::vector<uint16_t> bogoData2(1,2);
			const std::vector<uint16_t> bogoData3(1,3);

			// Send the write command pattern
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData1, debug);
			writeCycle(myAddress, 2, bogoData0, debug);
			writeCycle(myAddress, 2, bogoData0, debug);

			// Send each bit in turn.
			// The 2nd bit of the data is the value of the bit,
			// the 1st bit ends the write.
			for (unsigned int iBit = 0; iBit < nBits + extraBits; iBit++) {

				// Discover which word/bit I am on.
				unsigned int iWord = iBit/16;
				unsigned int remainderBits = iBit % 16;

				if (iBit == nBits + extraBits - 1) { // last bit
					// Check if the bit is high/low
					if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, 2, bogoData3, debug);
					else writeCycle(myAddress, 2, bogoData1, debug);
				} else { // not last bit
					// Check if the bit is high/low
					if (myData[iWord] & (1 << remainderBits)) writeCycle(myAddress, 2, bogoData2, debug);
					else writeCycle(myAddress, 2, bogoData0, debug);
				}
			}

			// End the reset command
			writeCycle(myAddress, 2, bogoData1, debug);
			writeCycle(myAddress, 2, bogoData0, debug);

		} catch (emu::fed::exception::CAENException &e) {
			std::ostringstream error;
			error << "Exception in jtagWrite(dev=" << dev << ", nBits=" << nBits << ", myData=" << &myData << ", noRead=" << noRead << ")";
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}

		// Return nothing.
		return result;
	}

	// What I really need is the number of words and remainder bits.
	// These are incomplete words, a sort of ceiling function for unsigned ints
	const unsigned int nWords = (nBits + extraBits == 0) ? 0 : (nBits + extraBits - 1)/16 + 1;
	const unsigned int remainderBits = (nBits + extraBits) % 16;

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
		try {
			writeVME(address, myData[iWord] & bitMask, debug);

			// Read now and store it for later.
			if (!noRead) {
				address = myAddress | 0x14;
				uint16_t tempResult = readVME(address, debug);

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
						const uint16_t carryValue = tempResult << (16 - extraBits);
						result[iWord - 1] |= carryValue;
					}

					// Then shift the garbage off.
					tempResult = tempResult >> extraBits;
				}

				result.push_back(tempResult);
			}
		} catch (emu::fed::exception::CAENException &e) {
			pthread_mutex_unlock(&mutex_);
			std::ostringstream error;
			error << "Exception in jtagWrite(dev=" << dev << ", nBits=" << nBits << ", myData=" << &myData << ", noRead=" << noRead << ")";
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}
	}
	pthread_mutex_unlock(&mutex_);

	return result;

}



std::vector<uint16_t> emu::fed::VMEModule::jtagRead(const enum DEVTYPE &dev, const unsigned int &nBits, const bool &debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{
	// Get the chain.  Very important to know.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		throw e2;
	}

	JTAGChain chain = JTAGMap[dev];

	// Set up the return value.
	std::vector<uint16_t> result;
	result.reserve(nBits/16 + 1);

	// The address is encoded in the JTAG channel.  The address is the same for all
	// elements in the chain (part of the definition of JTAG).
	const uint32_t myAddress = chain.front()->bitCode;

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
	const unsigned int nWords = (nBits + extraBits == 0) ? 0 : (nBits + extraBits - 1)/16 + 1;
	const unsigned int remainderBits = (nBits + extraBits) % 16;

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
		uint16_t tempResult = 0;
		try {
			writeVME(address, 0xffff, debug);

			// Read now and store it for later.
			address = myAddress | 0x14;
			tempResult = readVME(address, debug) & bitMask;
			//std::cerr << "tempResult is " << std::hex << tempResult << " mask " << bitMask << std::endl;
		} catch (emu::fed::exception::CAENException &e) {
			pthread_mutex_unlock(&mutex_);
			std::ostringstream error;
			error << "Exception in jtagRead(dev=" << dev << ", nBits=" << nBits << ")";
			XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
			throw e2;
		}

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
				const uint16_t carryValue = tempResult << (16 - extraBits);
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



uint16_t emu::fed::VMEModule::readVME(const uint32_t &Address, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	// The address always has the board slot encoded.
	uint32_t myAddress = Address | vmeAddress_;

	// The address modifier for talking to other boards
	CVAddressModifier AM = cvA24_U_DATA;

	// 16-bit data width
	CVDataWidth DW = cvD16;

	// 16-bit buffer to fill durring reads
	uint16_t data;
	//uint64_t *data;

	// Read and return error code
	if (debug) std::cerr << std::hex << "Read BHandle_(" << BHandle_ << ") Address(" << myAddress << ") " << std::flush;
	CVErrorCodes err = CAENVME_ReadCycle(BHandle_, myAddress, &data, AM, DW);
	if (debug) std::cerr << std::hex << "data(" << data << ") err(" << err << ")" << std::flush << std::endl;

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in readVME(Address=" << myAddress << "): " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

	return data;
}



void emu::fed::VMEModule::writeVME(const uint32_t &Address, const uint16_t &data, const bool &debug)
throw (emu::fed::exception::CAENException)
{
	uint16_t myData = data;
	
	// The address always has the board slot encoded.
	uint32_t myAddress = Address | vmeAddress_;

	// The address modifier for talking to other boards
	CVAddressModifier AM = cvA24_U_DATA;

	// 16-bit data width
	CVDataWidth DW = cvD16;

	// Write and return error code
	if (debug) std::cerr << std::hex << "Write BHandle_(" << BHandle_ << ") Address(" << myAddress << ") data(" << data << ")" << std::flush;
	CVErrorCodes err = CAENVME_WriteCycle(BHandle_, myAddress, &myData, AM, DW);
	if (debug) std::cerr << std::hex << " err(" << err << ")" << std::endl << std::flush;

	if (err != cvSuccess) {
		std::ostringstream error;
		error << "Exception in writeVME(Address=" << myAddress << ", data=" << data << "): " << CAENVME_DecodeError(err);
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

	return;
}



int emu::fed::VMEModule::loadPROM(const enum DEVTYPE &dev, const char *fileName, const std::string &startString, const std::string &stopString, const bool &debug)
throw (emu::fed::exception::FileException, emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The element in the chain that I am using
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

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
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		throw e2;
	}

	try {
		// Now start parsing the file.  Read lines until we have an eof.
		std::string myStartString = startString;
		while (!inFile.eof()) {

			// Each line is a command (or comment)
			std::string myLine;
			getline(inFile, myLine);

			// Start after a particular command
			// (useful for after loading a custom value into the usercode)
			if (myStartString != "") {
				if (myLine.find(myStartString) != std::string::npos) {
					myStartString = "";
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
					return 0;
				}
			}

			// Automatically reject comments.
			if (myLine.substr(0,2) == "//" || myLine.substr(0,1) == "!") {
				//std::cerr << myLine << std::flush << std::endl;
				continue;
			}

			// IMPORTANT:  Concatonate lines until there is a semicolon
			while (myLine.rfind(';') == std::string::npos) {

				// There may be nothing more to read.  In that case, quit (no error)
				if (inFile.eof()) {
					//std::cout << "Warning: reached end-of-file and discarding the line " << myLine << std::endl;
					inFile.close();
					return 1;
				}

				// Get the next line and staple it on.
				std::string nextLine;
				getline(inFile, nextLine);
				myLine += nextLine;
			}

			if (debug) std::cerr << myLine << std::flush << std::endl;

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
				const unsigned long int nBytes = (nBits == 0) ? 0 : (nBits - 1) / 8 + 1;

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
					//std::cout << "Warning:  " << command << " value " << value << " not enclosed in parentheses.  Ignoring." << std::endl;
					continue;
				} else {
					value = value.substr(1,value.length() - 2);
				}

				// Now we parse the hex digits one byte at a time, as all instructions have
				// byte-sized values.
				std::vector<uint8_t> bogoData;
				bogoData.reserve(nBytes);
				for (unsigned long int iByte = 0; iByte < nBytes; iByte++) {
					unsigned int byte;
					//std::istringstream parser(value.substr(iByte * 2, 2));
					//parser >> std::hex >> byte;
					sscanf(value.substr(iByte * 2, 2).c_str(), "%2x", &byte);
					// Be careful!  This is in reverse order!
					bogoData.push_back((uint8_t) (byte & 0xff));
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
						XCEPT_RAISE(emu::fed::exception::FEDException, error.str());
					}
					*/
					// The number of bits matches that of the command bus.
					//std::clog << "Attempting commandCycle with dev " << dev << " data " << myData[0] << std::endl;
					commandCycle(dev, myData[0], debug);

					continue;

				// Send data
				} else {

					// It's just a JTAG read/write.
					//std::vector<uint16_t> result = jtagReadWrite(dev, nBits, myData);
					// You can do something with the result here if you want.

					// The "true" at the end means write-only and don't read back.
					jtagWrite(dev, nBits, myData, true, debug);

					continue;
				}

				// That's it.  Now we do sleeping
			} else if (command == "RUNTEST") {

				// The next variable is the time to sleep.
				unsigned long int time;
				myLineStream >> time;

				timeval startTime;
				timeval endTime;
				if (debug) gettimeofday(&startTime,NULL);

				// Only use usleep if the number of microseconds is greater than 50.
				// Below that, we don't have the kind of resolution we need to be accurate,
				// and it's not like it matters much anyway.
				if (time >= 50) {
					usleep(time);
				}

				if (debug) {
					gettimeofday(&endTime,NULL);
					const unsigned long int diffTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
					std::cerr << "--usleep time: " << std::dec << diffTime << " microseconds" << std::endl;
				}

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

				if (dev == RESET) {

					const uint32_t myAddress = element->bitCode;

					// Real data to send.
					const std::vector<uint16_t> bogoBits(1,1);

					// Send it 5 times.
					writeCycle(myAddress, 2, bogoBits, debug);
					writeCycle(myAddress, 2, bogoBits, debug);
					writeCycle(myAddress, 2, bogoBits, debug);
					writeCycle(myAddress, 2, bogoBits, debug);
					writeCycle(myAddress, 2, bogoBits, debug);

					continue;

				} else {

					const uint32_t myAddress = element->bitCode | 0x00000018;

					// Fake data to send.
					const std::vector<uint16_t> bogoBits(1,0x0);

					writeCycle(myAddress, element->cmdBits, bogoBits, debug);
				}
				continue;

			}
		}

	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in loadPROM(dev=" << dev << ", fileName=" << fileName << ", startString=" << startString << ", stopString=" << stopString << ")";
		XCEPT_DECLARE_NESTED(emu::fed::exception::CAENException, e2, error.str(), e);
		throw e2;
	}

	inFile.close();
	return 0;
}


