/*****************************************************************************\
* $Id: DCC.cc,v 1.1 2009/03/05 16:02:14 paste Exp $
*
* $Log: DCC.cc,v $
* Revision 1.1  2009/03/05 16:02:14  paste
* * Shuffled FEDCrate libraries to new locations
* * Updated libraries for XDAQ7
* * Added RPM building and installing
* * Various bug fixes
*
* Revision 3.28  2009/01/29 15:31:23  paste
* Massive update to properly throw and catch exceptions, improve documentation, deploy new namespaces, and prepare for Sentinel messaging.
*
* Revision 3.27  2008/10/22 20:23:58  paste
* Fixes for random FED software crashes attempted.  DCC communication and display reverted to ancient (pointer-based communication) version at the request of Jianhui.
*
* Revision 3.26  2008/10/09 11:21:19  paste
* Attempt to fix DCC MPROM load.  Added debugging for "Global SOAP death" bug.  Changed the debugging interpretation of certain DCC registers.  Added inline SVG to EmuFCrateManager page for future GUI use.
*
* Revision 3.25  2008/09/24 18:38:38  paste
* Completed new VME communication protocols.
*
* Revision 3.24  2008/09/22 14:31:54  paste
* /tmp/cvsY7EjxV
*
* Revision 3.23  2008/09/19 16:53:52  paste
* Hybridized version of new and old software.  New VME read/write functions in place for all DCC communication, some DDU communication.  New XML files required.
*
* Revision 3.22  2008/09/07 22:25:36  paste
* Second attempt at updating the low-level communication routines to dodge common-buffer bugs.
*
* Revision 3.21  2008/09/03 17:52:58  paste
* Rebuilt the VMEController and VMEModule classes from the EMULIB_V6_4 tagged versions and backported important changes in attempt to fix "high-bits" bug.
*
* Revision 3.20  2008/09/01 23:46:24  paste
* Trying to fix what I broke...
*
* Revision 3.19  2008/08/31 21:18:27  paste
* Moved buffers from VMEController class to VMEModule class for more rebust communication.
*
* Revision 3.18  2008/08/25 12:25:49  paste
* Major updates to VMEController/VMEModule handling of CAEN instructions.  Also, added version file for future RPMs.
*
* Revision 3.17  2008/08/19 14:51:02  paste
* Update to make VMEModules more independent of VMEControllers.
*
* Revision 3.16  2008/08/15 16:14:51  paste
* Fixed threads (hopefully).
*
* Revision 3.15  2008/08/15 08:35:51  paste
* Massive update to finalize namespace introduction and to clean up stale log messages in the code.
*
*
\*****************************************************************************/
#include "emu/fed/DCC.h"

#include <vector>
#include <sstream>
//#include <cmath>

#include "emu/fed/JTAGElement.h"

emu::fed::DCC::DCC(int slot):
VMEModule(slot),
fifoinuse_(0x3fe),
softsw_(0)
{
	// Build the JTAG chains

	// MPROM is one element
	JTAGChain chainMPROM;
	JTAGElement *elementMPROM = new JTAGElement("MPROM", MPROM, 2, MPROM_BYPASS_L | (MPROM_BYPASS_H << 8), 16, 0x00002000, false);
	chainMPROM.push_back(elementMPROM);
	JTAGMap[MPROM] = chainMPROM;

	// INPROM is one element
	JTAGChain chainINPROM;
	JTAGElement *elementINPROM = new JTAGElement("INPROM", INPROM, 3, PROM_BYPASS, 8, 0x00003000, false);
	chainINPROM.push_back(elementINPROM);
	JTAGMap[INPROM] = chainINPROM;

	// MCTRL is one element
	JTAGChain chainMCTRL;
	JTAGElement *elementMCTRL = new JTAGElement("MCTRL", MCTRL, 11, PROM_BYPASS, 10, 0x00000000, true);
	chainMCTRL.push_back(elementMCTRL);
	JTAGMap[MCTRL] = chainMCTRL;

	// The RESET path is one element.
	JTAGChain chainRESET;
	JTAGElement *elementRESET = new JTAGElement("RESET", RESET, 12, PROM_BYPASS, 8, 0x0000fffe, false);
	chainRESET.push_back(elementRESET);

	JTAGMap[RESET] = chainRESET;
}


emu::fed::DCC::~DCC()
{
	// std::cout << "Killing DCC" << std::endl;
}



void emu::fed::DCC::configure()
throw (emu::fed::exception::DCCException)
{
	try {
		writeFIFOInUse(fifoinuse_);
		writeSoftwareSwitch(softsw_);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readStatusHigh()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x02, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readStatusLow()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x01, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readFIFOInUse()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x06, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::writeFIFOInUse(uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1,value & 0x07FF);
		writeRegister(MCTRL, 0x03, 16, myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readRate(unsigned int fifo)
throw (emu::fed::exception::DCCException)
{
	if (fifo > 11) XCEPT_RAISE(emu::fed::exception::DCCException, "there are only 12 FIFOs to check [0-11]");
	try {
		return readRegister(MCTRL, 0x10 + fifo, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readSoftwareSwitch()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x1f, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::writeSoftwareSwitch(uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x07, 16, myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readFMM()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x1e, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::writeFMM(uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x08, 16, myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readTTCCommand()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL,0x05,16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::writeTTCCommand(uint8_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		// The first two bits are special.
		std::vector<uint16_t> myData(1, 0xff00 | ((value << 2) & 0xfc));
		writeRegister(MCTRL,0x00,16,myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::resetBX()
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, 0x02);
		writeRegister(MCTRL,0x00,16,myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::resetEvents()
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, 0x01);
		writeRegister(MCTRL,0x00,16,myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::writeFakeL1A(uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL,0x04,16,myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint32_t emu::fed::DCC::readIDCode(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	uint16_t command = 0;
	if (dev == MPROM) {
		command = ((MPROM_IDCODE_H << 8) & 0xff00) | (MPROM_IDCODE_L & 0xff);
	} else if (dev == INPROM) {
		command = PROM_IDCODE & 0xff;
	} else {
		XCEPT_RAISE(emu::fed::exception::DCCException, "must supply a PROM device as an argument");
	}
	
	try {
		std::vector<uint16_t> result = readRegister(dev,command,32);
		return result[0] | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint32_t emu::fed::DCC::readUserCode(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	uint16_t command = 0;
	if (dev == MPROM) {
		command = ((MPROM_USERCODE_H << 8) & 0xff00) | (MPROM_USERCODE_L & 0xff);
	} else if (dev == INPROM) {
		command = PROM_USERCODE & 0xff;
	} else {
		XCEPT_RAISE(emu::fed::exception::DCCException, "must supply a PROM device as an argument");
	}
	
	try {
		std::vector<uint16_t> result = readRegister(dev,command,32);
		return result[0] | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::resetPROM(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	if (dev != INPROM && dev != MPROM && dev != RESET) {
		XCEPT_RAISE(emu::fed::exception::DCCException, "must supply a PROM device as an argument");
	}
	
	try {
		commandCycle(dev, 0x00EE);
		uint16_t bpCommand = 0;
		if (dev == RESET) bpCommand = 0xFFFF;
		else bpCommand = PROM_BYPASS;
		commandCycle(dev, bpCommand);
		sleep(1);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



unsigned int emu::fed::DCC::getDDUSlotFromFIFO(unsigned int fifo)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (slot() == 8) {
		switch (fifo) {
		case 1: return 3;
		case 2: return 13;
		case 3: return 4;
		case 4: return 12;
		case 5: return 5;
		case 7: return 11;
		case 8: return 6;
		case 9: return 10;
		case 10: return 7;
		case 11: return 9;
		default:
			std::ostringstream error;
			error << "FIFO " << fifo << " does not correspond to a DDU for DCC in slot " << slot();
			XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
			break;
		}
	} else if (slot() == 17) {
		switch (fifo) {
		case 5: return 14;
		case 7: return 20;
		case 8: return 15;
		case 9: return 19;
		case 10: return 16;
		case 11: return 18;
		default:
			std::ostringstream error;
			error << "FIFO " << fifo << " does not correspond to a DDU for DCC in slot " << slot();
			XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
	}
}



unsigned int emu::fed::DCC::getFIFOFromDDUSlot(unsigned int mySlot)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (slot() == 8) {
		switch (mySlot) {
		case 3: return 1;
		case 4: return 3;
		case 5: return 5;
		case 6: return 8;
		case 7: return 10;
		case 9: return 11;
		case 10: return 9;
		case 11: return 7;
		case 12: return 4;
		case 13: return 2;
		default:
			std::ostringstream error;
			error << "DDU in slot " << mySlot << " does not input to DCC in slot " << slot();
			XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
			break;
		}
	} else if (slot() == 17) {
		switch (mySlot) {
		case 14: return 5;
		case 15: return 8;
		case 16: return 10;
		case 18: return 11;
		case 19: return 9;
		case 20: return 7;
		default:
			std::ostringstream error;
			error << "DDU in slot " << mySlot << " does not input to DCC in slot " << slot();
			XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
	}
}



unsigned int emu::fed::DCC::getSLinkFromFIFO(unsigned int fifo)
throw (emu::fed::exception::OutOfBoundsException)
{
	switch (fifo) {
	case 0: return 1;
	case 6: return 2;
	default:
		std::ostringstream error;
		error << "FIFO " << fifo << " does not correspond to an SLink output";
		XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
		break;
	}
}



unsigned int emu::fed::DCC::getFIFOFromSLink(unsigned int slink)
throw (emu::fed::exception::OutOfBoundsException)
{
	switch (slink) {
	case 1: return 0;
	case 2: return 6;
	default:
		std::ostringstream error;
		error << "SLink " << slink << " does not correspond to an output FIFO";
		XCEPT_RAISE(emu::fed::exception::OutOfBoundsException, error.str());
		break;
	}
}



uint16_t emu::fed::DCC::readDDURate(unsigned int mySlot)
throw (emu::fed::exception::DCCException)
{
	try {
		return readRate(getFIFOFromDDUSlot(mySlot));
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to read data rate for DDU slot " << mySlot;
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



uint16_t emu::fed::DCC::readSLinkRate(unsigned int slink)
throw (emu::fed::exception::DCCException)
{
	try {
		return readRate(getFIFOFromSLink(slink));
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to read data rate for SLink " << slink;
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::crateHardReset()
throw (emu::fed::exception::DCCException)
{
	try {
		uint16_t switchCache = readSoftwareSwitch();
		writeSoftwareSwitch(0x1000);
		writeTTCCommand(0x34);
		sleep((unsigned int) 2);
		writeSoftwareSwitch(switchCache);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



void emu::fed::DCC::crateResync()
throw (emu::fed::exception::DCCException)
{
	try {
		uint16_t switchCache = readSoftwareSwitch();
		writeSoftwareSwitch(0x1000);
		writeTTCCommand(0x3);
		sleep((unsigned int) 1);
		writeSoftwareSwitch(switchCache);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_RETHROW(emu::fed::exception::DCCException, error.str(), e);
	}
}



std::vector<uint16_t> emu::fed::DCC::readRegister(enum DEVTYPE dev, char myRegister, unsigned int nBits)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{
	// The information about the element being written is stored in the chain.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_RAISE(emu::fed::exception::DevTypeException, error.str());
	}
	JTAGChain chain = JTAGMap[dev];
	// The first element in the chain will give us all the information about the
	// class of VME communication we need to use.
	
	//std::clog << "Attempting to read from " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << std::endl;
	try {
		if (chain.front()->directVME) {
			// Direct VME reads are always one element, and are not JTAG commands.
			
			// The address of the read is stored in the chain.
			uint32_t myAddress = (myRegister << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;
			
			return readCycle(myAddress,nBits);
			
		} else {
			// Everything else is a JTAG command, and may or may not
			// be part of a chain.
			
			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister);
			
			// Shove in (and read out)
			std::vector<uint16_t> result = jtagRead(dev, nBits);
			
			// Finally, set the bypass.  All bypass commands in the chain are equal.
			// That is part of the definition of JTAG.
			commandCycle(dev, chain.front()->bypassCommand);
			
			return result;
			
		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in readRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ")";
		XCEPT_RETHROW(emu::fed::exception::CAENException, error.str(), e);
	}
	
}



std::vector<uint16_t> emu::fed::DCC::writeRegister(enum DEVTYPE dev, char myRegister, unsigned int nBits, std::vector<uint16_t> myData)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{
	
	// The information about the element being written
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_RAISE(emu::fed::exception::DevTypeException, error.str());
	}
	JTAGChain chain = JTAGMap[dev];
	
	//std::cout << "Attempting to write to " << element->name << " register " << std::hex << (unsigned int) myRegister << " bits " << std::dec << nBits << " values (low to high) ";
	//for (std::vector<uint16_t>::iterator iData = myData.begin(); iData != myData.end(); iData++) {
	//std::cout << std::showbase << std::hex << (*iData) << std::dec << " ";
	//}
	//std::cout << std::endl;

	try {
		// Direct VME writes are different
		if (chain.front()->directVME) {
			
			// The address for MCTRL is special, as it also contains the command code.
			uint32_t myAddress = (myRegister << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;
			
			writeCycle(myAddress, nBits, myData);
			
			// This sort of write does not read back, so return an empty vector.
			std::vector<uint16_t> bogoBits;
			return bogoBits;
			
			// Everything else is a JTAG command?
		} else {
			
			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister);
			
			// Shove in (and read out)
			std::vector<uint16_t> result = jtagWrite(dev, nBits, myData);
			
			// Finally, set the bypass
			commandCycle(dev, chain.front()->bypassCommand);
			
			return result; // The value that used to be in the register.
			
		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in writeRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ", myData=" << &myData << ")";
		XCEPT_RETHROW(emu::fed::exception::CAENException, error.str(), e);
	}
}
