/*****************************************************************************\
* $Id: DCC.cc,v 1.5 2009/05/29 11:23:18 paste Exp $
\*****************************************************************************/
#include "emu/fed/DCC.h"

#include <vector>
#include <sstream>
//#include <cmath>

#include "emu/fed/JTAGElement.h"
#include "emu/fed/FIFO.h"

emu::fed::DCC::DCC(int slot):
VMEModule(slot),
fifoVector_(10, new FIFO()),
fifoinuse_(0x3fe),
softsw_(0),
fmm_id_(0),
slink1_id_(0),
slink2_id_(0)
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
	JTAGElement *elementRESET = new JTAGElement("RESET", RESET, 12, PROM_BYPASS, 16, 0x0000fffe, false);
	chainRESET.push_back(elementRESET);

	JTAGMap[RESET] = chainRESET;
}



emu::fed::DCC::~DCC()
{
	// std::cout << "Killing DCC" << std::endl;
}



emu::fed::FIFO *emu::fed::DCC::getFIFO(size_t fifoNumber)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fifoNumber >= fifoVector_.size()) {
		std::ostringstream error;
		error << "FIFO vector overflow, fifoNumber=" << fifoNumber;
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	return fifoVector_[fifoNumber];
}



void emu::fed::DCC::addFIFO(emu::fed::FIFO *fifo, size_t fifoNumber)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fifoNumber > 9) {
		std::ostringstream error;
		error << "FIFO vector overflow, fifoNumber=" << fifoNumber;
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	fifoVector_[fifoNumber] = fifo;
	
	// Set the FIFO in use appropriately
	if (!fifo->isUsed()) fifoinuse_ &= ~(1 << fifoNumber);
	else fifoinuse_ |= (1 << fifoNumber);
}



void emu::fed::DCC::setFIFOs(std::vector<emu::fed::FIFO *> fifoVector)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fifoVector.size() > 10) {
		std::ostringstream error;
		error << "FIFO vector overflow, new fifoVector.size()=" << fifoVector.size();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot " << slot() << " board DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	} else if (fifoVector.size() < 10) {
		// Resize the new vector just in case
		fifoVector.resize(10, new FIFO());
	}
	fifoVector_ = fifoVector;
	
	// Set the FIFO in use appropriately
	fifoinuse_ = 0;
	for (size_t iFIFO = 0; iFIFO < fifoVector.size(); iFIFO++) {
		if (fifoVector[iFIFO]->isUsed()) fifoinuse_ |= (1 << iFIFO);
	}
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeFIFOInUse(uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1,value & 0x07FF);
		writeRegister(MCTRL, 0x03, 16, myData);
		
		// Set the used bit on the owned FIFOs for convenience
		reloadFIFOUsedBits(value & 0x03FF);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint16_t emu::fed::DCC::readRate(unsigned int fifo)
throw (emu::fed::exception::DCCException)
{
	if (fifo > 11) {
		std::ostringstream error;
		error << "there are only 12 FIFOs to check [0-11]";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	try {
		return readRegister(MCTRL, 0x10 + fifo, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint32_t emu::fed::DCC::readIDCode(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	uint16_t command = 0;
	if (dev == MPROM) {
		command = (uint16_t) (MPROM_IDCODE_H << 8) | (MPROM_IDCODE_L);
	} else if (dev == INPROM) {
		command = PROM_IDCODE & 0xff;
	} else {
		std::ostringstream error;
		error << "Must supply a PROM device as an argument";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		std::vector<uint16_t> result = readRegister(dev,command,32);
		return result[0] | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



uint32_t emu::fed::DCC::readUserCode(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	uint16_t command = 0;
	if (dev == MPROM) {
		command = (uint16_t) (MPROM_USERCODE_H << 8) | (MPROM_USERCODE_L);
	} else if (dev == INPROM) {
		command = PROM_USERCODE & 0xff;
	} else {
		std::ostringstream error;
		error << "Must supply a PROM device as an argument";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

	try {
		std::vector<uint16_t> result = readRegister(dev,command,32);
		return result[0] | (result[1] << 16);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::resetPROM(enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	if (dev != INPROM && dev != MPROM && dev != RESET) {
		std::ostringstream error;
		error << "Must supply a PROM device as an argument";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
			XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
			std::ostringstream tag;
			tag << "slot:" << slot() << ",board:DCC";
			e2.setProperty("tag", tag.str());
			throw e2;
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
			XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
			std::ostringstream tag;
			tag << "slot:" << slot() << ",board:DCC";
			e2.setProperty("tag", tag.str());
			throw e2;
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
			XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
			std::ostringstream tag;
			tag << "slot:" << slot() << ",board:DCC";
			e2.setProperty("tag", tag.str());
			throw e2;
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
			XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
			std::ostringstream tag;
			tag << "slot:" << slot() << ",board:DCC";
			e2.setProperty("tag", tag.str());
			throw e2;
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::reloadFIFOUsedBits(uint16_t fifoInUse)
{
	for (size_t iFIFO = 0; iFIFO < fifoVector_.size(); iFIFO++) {
		fifoVector_[iFIFO]->used_ = (fifoInUse & (1 << iFIFO));
	}
}



std::vector<uint16_t> emu::fed::DCC::readRegister(enum DEVTYPE dev, uint16_t myRegister, unsigned int nBits, bool debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The information about the element being written is stored in the chain.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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

			return readCycle(myAddress,nBits,debug);

		} else {
			// Everything else is a JTAG command, and may or may not
			// be part of a chain.

			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister,debug);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagRead(dev, nBits, debug);

			// Finally, set the bypass.  All bypass commands in the chain are equal.
			// That is part of the definition of JTAG.
			commandCycle(dev, chain.front()->bypassCommand,debug);

			return result;

		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in readRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ")";
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}

}



std::vector<uint16_t> emu::fed::DCC::writeRegister(enum DEVTYPE dev, uint16_t myRegister, unsigned int nBits, std::vector<uint16_t> myData, bool debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The information about the element being written
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
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

			writeCycle(myAddress, nBits, myData, debug);

			// This sort of write does not read back, so return an empty vector.
			std::vector<uint16_t> bogoBits;
			return bogoBits;

			// Everything else is a JTAG command?
		} else {

			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister, debug);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagWrite(dev, nBits, myData, false, debug);

			// Finally, set the bypass
			commandCycle(dev, chain.front()->bypassCommand, debug);

			return result; // The value that used to be in the register.

		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in writeRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ", myData=" << &myData << ")";
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "slot:" << slot() << ",board:DCC";
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}
