/*****************************************************************************\
* $Id: DCC.cc,v 1.14 2012/06/22 15:22:44 cvuosalo Exp $
\*****************************************************************************/
#include "emu/fed/DCC.h"

#include <vector>
#include <sstream>
//#include <cmath>

#include "emu/fed/JTAGElement.h"
#include "emu/fed/FIFO.h"

emu::fed::DCC::DCC(const unsigned int slot, const bool fake):
VMEModule(slot, fake),
fifoinuse_(0x3fe),
softsw_(0),
fmm_id_(0),
slink1_id_(0),
slink2_id_(0)
{
	// Build the JTAG chains

	// MPROM is one element
	JTAGChain chainMPROM;
	chainMPROM.push_back(new JTAGElement("MPROM", MPROM, 2, MPROM_BYPASS_L | (MPROM_BYPASS_H << 8), 16, 0x00002000, false));
	JTAGMap[MPROM] = chainMPROM;

	// INPROM is one element
	JTAGChain chainINPROM;
	chainINPROM.push_back(new JTAGElement("INPROM", INPROM, 3, PROM_BYPASS, 8, 0x00003000, false));
	JTAGMap[INPROM] = chainINPROM;

	// MCTRL is one element
	JTAGChain chainMCTRL;
	chainMCTRL.push_back(new JTAGElement("MCTRL", MCTRL, 11, PROM_BYPASS, 10, 0x00000000, true));
	JTAGMap[MCTRL] = chainMCTRL;

	// The RESET path is one element.
	JTAGChain chainRESET;
	chainRESET.push_back(new JTAGElement("RESET", RESET, 12, PROM_BYPASS, 16, 0x0000fffe, false));
	JTAGMap[RESET] = chainRESET;
}



emu::fed::DCC::~DCC()
{
	for (size_t iFIFO = 0; iFIFO < fifoVector_.size(); iFIFO++) {
		delete fifoVector_[iFIFO];
	}
}



const emu::fed::FIFO *emu::fed::DCC::getFIFO(const unsigned int fifoNumber) const
throw (emu::fed::exception::OutOfBoundsException)
{
	for (std::vector<FIFO *>::const_iterator iFIFO = fifoVector_.begin(); iFIFO != fifoVector_.end(); iFIFO++) {
		if ((*iFIFO)->number() == fifoNumber) return (*iFIFO);
	}

	return new FIFO(fifoNumber);
}



void emu::fed::DCC::addFIFO(emu::fed::FIFO *fifo)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fifoVector_.size() == 10) {
		std::ostringstream error;
		error << "The physical limitations of the DCC mean only 10 FIFOs can be added.";
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	fifoVector_.push_back(fifo);

	// Set the FIFO in use appropriately
	if (!fifo->isUsed()) fifoinuse_ &= ~(1 << fifo->number());
	else fifoinuse_ |= (1 << fifo->number());
}



void emu::fed::DCC::setFIFOs(const std::vector<emu::fed::FIFO *> &fifoVector)
throw (emu::fed::exception::OutOfBoundsException)
{
	if (fifoVector.size() > 10) {
		std::ostringstream error;
		error << "The physical limitations of the DCC mean only 10 fibers can be added.";
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
	for (size_t iFIFO = 0; iFIFO < fifoVector_.size(); iFIFO++) {
		delete fifoVector_[iFIFO];
	}
	fifoVector_ = fifoVector;

	// Set the FIFO in use appropriately
	reloadFIFOInUse();
}



void emu::fed::DCC::reloadFIFOInUse()
{
	fifoinuse_ = 0;
	for (std::vector<FIFO *>::const_iterator iFIFO = fifoVector_.begin(); iFIFO != fifoVector_.end(); ++iFIFO) {
		if ((*iFIFO)->isUsed()) fifoinuse_ |= (1 << (*iFIFO)->number());
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readStatusHigh()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x02, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readStatusLow()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x01, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint8_t emu::fed::DCC::readL1AHigh()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x09, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readFIFOStatus()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x0A, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readFIFOInUse()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x06, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeFIFOInUse(const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value & 0x07FF);
		writeRegister(MCTRL, 0x03, 16, myData);

		// Set the used bit on the owned FIFOs for convenience
		reloadFIFOUsedBits(value & 0x03FF);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readRate(const unsigned int fifo)
throw (emu::fed::exception::DCCException)
{
	if (fifo > 11) {
		std::ostringstream error;
		error << "there are only 12 FIFOs to check [0-11]";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readSoftwareSwitch()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x1f, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeSoftwareSwitch(const uint16_t value)
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readFMM(const enum DEVICE dev)
throw (emu::fed::exception::DCCException)
{
	try {
		uint16_t returnMe = readRegister(MCTRL, 0x1e, 16)[0];
		if (dev == All) return returnMe;
		else if (dev == TTS) return returnMe & 0x1f;
		else if (dev == SLinkA) return (returnMe >> 5) & 0x1f;
		else if (dev == SLinkB) return (returnMe >> 10) & 0x1f;
		else return 0;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeFMM(const enum DEVICE dev, const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		if (dev == All) {
			std::vector<uint16_t> myData(1, value);
			writeRegister(MCTRL, 0x08, 16, myData);
		} else {
			uint16_t oldData = readFMM(All);
			if (dev == TTS) {
				oldData &= 0xffe0;
				oldData |= value & 0x1f;
			} else if (dev == SLinkA) {
				oldData &= 0xfe1f;
				oldData |= (value & 0x1f) << 5;
			} else if (dev == SLinkB) {
				oldData &= 0x73ff;
				oldData |= (value & 0x1f) << 10;
			}
			std::vector<uint16_t> myData(1, oldData);
			writeRegister(MCTRL, 0x08, 16, myData);
		}
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readTTCCommand()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x05, 16)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeTTCCommand(const uint8_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		// The first two bits are special.
		std::vector<uint16_t> myData(1, 0xff00 | ((value << 2) & 0xfc));
		writeRegister(MCTRL, 0x00, 16, myData);
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::writeFakeL1A(const uint16_t value)
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

/* stan added routines Feb 9, 2012 */

void emu::fed::DCC::writeDisableOutOfSyncOnL1AMismatch(const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x0b, 16, myData, true);
		std::cout << " disable out of sync fmm " << std::endl;
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}


void emu::fed::DCC::writeEnableOutOfSyncOnL1AMismatch(const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x0c, 16, myData, true);
		std::cout << "enable out of sync fmm " << std::endl; 
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

const uint16_t emu::fed::DCC::readNumberOfL1AMismatches()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x0d, 16, true)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

const uint16_t emu::fed::DCC::readOutofSyncEnableDisable()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x0b, 16, true)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



/* end stan added routines Fedb 9, 2012 */


/* start CRC error on OOS Fed 23, 2012 */

void emu::fed::DCC::writeDisableCRCErrOnL1AMismatch(const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x0f, 16, myData, true);
		std::cout << " disable CRC error " << std::endl;
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}


void emu::fed::DCC::writeEnableCRCErrOnL1AMismatch(const uint16_t value)
throw (emu::fed::exception::DCCException)
{
	try {
		std::vector<uint16_t> myData(1, value);
		writeRegister(MCTRL, 0x0e, 16, myData, true);
		std::cout << "enable CRC error " << std::endl; 
		return;
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}

const uint16_t emu::fed::DCC::readCRCErrEnableDisable()
throw (emu::fed::exception::DCCException)
{
	try {
		return readRegister(MCTRL, 0x0b, 16, true)[0];
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



/* end CRC error on OOS Fed 23, 2012 */


const uint32_t emu::fed::DCC::readIDCode(const enum DEVTYPE dev)
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
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint32_t emu::fed::DCC::readUserCode(const enum DEVTYPE dev)
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
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::resetPROM(const enum DEVTYPE dev)
throw (emu::fed::exception::DCCException)
{
	if (dev != INPROM && dev != MPROM && dev != RESET) {
		std::ostringstream error;
		error << "Must supply a PROM device as an argument";
		XCEPT_DECLARE(emu::fed::exception::DCCException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const unsigned int emu::fed::DCC::getDDUSlotFromFIFO(const unsigned int fifo) const
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
			tag << "FMM " << fmm_id_;
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
			tag << "FMM " << fmm_id_;
			e2.setProperty("tag", tag.str());
			throw e2;
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const unsigned int emu::fed::DCC::getFIFOFromDDUSlot(const unsigned int mySlot) const
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
			tag << "FMM " << fmm_id_;
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
			tag << "FMM " << fmm_id_;
			e2.setProperty("tag", tag.str());
			throw e2;
			break;
		}
	} else {
		std::ostringstream error;
		error << "FIFO mapping is not known for a DCC in slot " << slot();
		XCEPT_DECLARE(emu::fed::exception::OutOfBoundsException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const unsigned int emu::fed::DCC::getSLinkFromFIFO(const unsigned int fifo) const
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
		break;
	}
}



const unsigned int emu::fed::DCC::getFIFOFromSLink(const unsigned int slink) const
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
		break;
	}
}



const uint16_t emu::fed::DCC::readDDURate(const unsigned int mySlot)
throw (emu::fed::exception::DCCException)
{
	try {
		return readRate(getFIFOFromDDUSlot(mySlot));
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to read data rate for DDU slot " << mySlot;
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



const uint16_t emu::fed::DCC::readSLinkRate(const unsigned int slink)
throw (emu::fed::exception::DCCException)
{
	try {
		return readRate(getFIFOFromSLink(slink));
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Unable to read data rate for SLink " << slink;
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::crateResync(bool ignoreBackPress) // Param defaults to false
throw (emu::fed::exception::DCCException)
{
	try {
		uint16_t switchCache = readSoftwareSwitch();
		uint16_t newSwStat = 0x1000;	// Means allow TTC command
		if (ignoreBackPress)
			newSwStat = 0x7000;
			// Adds ignore backpressure SLink 0 & 1, bits 13 & 14
		writeSoftwareSwitch(newSwStat);
		writeTTCCommand(0x3);
		sleep((unsigned int) 1);
		writeSoftwareSwitch(switchCache);
	} catch (emu::fed::exception::Exception &e) {
		std::ostringstream error;
		error << "Exception communicating with DCC";
		XCEPT_DECLARE_NESTED(emu::fed::exception::DCCException, e2, error.str(), e);
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}



void emu::fed::DCC::reloadFIFOUsedBits(const uint16_t fifoInUse)
{
	for (size_t iFIFO = 0; iFIFO < fifoVector_.size(); iFIFO++) {
		fifoVector_[iFIFO]->setUsed(fifoInUse & (1 << iFIFO));
	}
}



const std::vector<uint16_t> emu::fed::DCC::readRegister(const enum DEVTYPE dev, const uint16_t myRegister, const unsigned int nBits, const bool debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The information about the element being written is stored in the chain.
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
			const uint32_t myAddress = (myRegister << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;

			return readCycle(myAddress, nBits, debug);

		} else {
			// Everything else is a JTAG command, and may or may not
			// be part of a chain.

			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister,debug);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagRead(dev, nBits, debug);

			// Finally, set the bypass.  All bypass commands in the chain are equal.
			// That is part of the definition of JTAG.
			commandCycle(dev, chain.front()->bypassCommand, debug);

			return result;

		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in readRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ")";
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}

}



const std::vector<uint16_t> emu::fed::DCC::writeRegister(const enum DEVTYPE dev, const uint16_t myRegister, const unsigned int nBits, const std::vector<uint16_t> &data, const bool debug)
throw (emu::fed::exception::CAENException, emu::fed::exception::DevTypeException)
{

	// The information about the element being written
	if (JTAGMap.find(dev) == JTAGMap.end()) {
		std::ostringstream error;
		error << "JTAGChain not defined for dev=" << dev;
		XCEPT_DECLARE(emu::fed::exception::DevTypeException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
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
			const uint32_t myAddress = (myRegister << 2) | chain.front()->bitCode;
			//std::cout << "address " << std::hex << myAddress << std::dec << std::endl;

			writeCycle(myAddress, nBits, data, debug);

			// This sort of write does not read back, so return an empty vector.
			std::vector<uint16_t> bogoBits;
			return bogoBits;

			// Everything else is a JTAG command?
		} else {

			// Open the appropriate register with an initialization command.
			commandCycle(dev, myRegister, debug);

			// Shove in (and read out)
			std::vector<uint16_t> result = jtagWrite(dev, nBits, data, false, debug);

			// Finally, set the bypass
			commandCycle(dev, chain.front()->bypassCommand, debug);

			return result; // The value that used to be in the register.

		}
	} catch (emu::fed::exception::CAENException &e) {
		std::ostringstream error;
		error << "Exception in writeRegister(dev=" << dev << ", myRegister=" << myRegister << ", nBits=" << nBits << ", data=" << &data << ")";
		XCEPT_DECLARE(emu::fed::exception::CAENException, e2, error.str());
		std::ostringstream tag;
		tag << "FMM " << fmm_id_;
		e2.setProperty("tag", tag.str());
		throw e2;
	}
}
